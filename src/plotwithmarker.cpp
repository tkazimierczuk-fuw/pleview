#include <iostream>
#include "plotwithmarker.h"
#include "qwt_scale_div.h"
#include "qwt_picker_machine.h"

PlotWithMarker::PlotWithMarker(int direction, PlotRangesManager * plotRangesManager) : _direction(direction)
{
    _selected = false;
    this->setMinimumHeight(100);
    //this->setMargin(10);
    this->setFocusPolicy(Qt::ClickFocus);

    marker = new DoubleMarker();
    marker->attach(this);
    marker->setItemAttribute(QwtPlotItem::AutoScale, false);

    curve = new QwtPlotCurve();
    curve->setPen(QPen(QColor(0,128,0)));
    curve->attach(this);

    picker = new QwtPlotPicker(this->canvas());
    picker->setStateMachine(new QwtPickerDragPointMachine());
    zoomer = new QwtPlotPicker(this->canvas());
    zoomer->setStateMachine(new QwtPickerDragRectMachine ());
    unzoomer = new QwtPlotPicker(this->canvas());
    unzoomer->setStateMachine(new QwtPickerClickPointMachine());
    zoomer->setRubberBand(QwtPicker::RectRubberBand);
    connect(picker, SIGNAL(selected(QPointF)), this, SLOT(midClick(QPointF)));
    connect(picker, SIGNAL(moved(QPointF)), this, SLOT(midClick(QPointF)));

    if(direction != 0) {
        connect(zoomer, SIGNAL(selected(QRectF)), plotRangesManager, SLOT(setXZRanges(QRectF)));
        connect(unzoomer, SIGNAL(selected(QPointF)), plotRangesManager, SLOT(unzoomXZ()));
        connect(plotRangesManager, SIGNAL(xPlotRangesChanged(QwtScaleDiv)), this, SLOT(setHScale(QwtScaleDiv)));
        connect(plotRangesManager, SIGNAL(zxPlotRangesChanged(QwtScaleDiv)), this, SLOT(setVScale(QwtScaleDiv)));
    }
    else {
        connect(zoomer, SIGNAL(selected(QRectF)), plotRangesManager, SLOT(setYZRanges(QRectF)));
        connect(unzoomer, SIGNAL(selected(QPointF)), plotRangesManager, SLOT(unzoomYZ()));
        connect(plotRangesManager, SIGNAL(yPlotRangesChanged(QwtScaleDiv)), this, SLOT(setHScale(QwtScaleDiv)));
        connect(plotRangesManager, SIGNAL(zyPlotRangesChanged(QwtScaleDiv)), this, SLOT(setVScale(QwtScaleDiv)));
    }

    setZoomMode();
}


void PlotWithMarker::updateMarker(int direction, double pos0, double pos1, double pos2) {
    if(direction != _direction) {
        marker->setXValues(pos1, pos2);
        marker->setLabel(QString::number(pos0));
    }
    this->replot();
}


void PlotWithMarker::updateCrossSection(int direction, const QVector<double> &x, const QVector<double> &y) {
    if(direction == _direction) {
        curve->setSamples(x, y);
    }
    this->replot();
}


void PlotWithMarker::setZoomMode() {
    zoomer->setEnabled(true);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
    unzoomer->setEnabled(true);
    unzoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
    picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::MidButton);
}

void PlotWithMarker::setMarkerMode() {
    zoomer->setEnabled(false);
    unzoomer->setEnabled(false);
    picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
}


void PlotWithMarker::attachPlugin(PlotAddon * plugin) {
    if(_direction)
        plugin->attach(this, PlotAddon::xCrossSection);
    else
        plugin->attach(this, PlotAddon::yCrossSection);
}



void PlotWithMarker::midClick(QPointF pos) {
    emit(markerMoved(_direction, pos.x()));
}


void PlotWithMarker::paintEvent(QPaintEvent * event) {
    QwtPlot::paintEvent(event);

    if(_selected) {
        QRect rect = this->contentsRect();
        rect.adjust(5, 5, -5, -5);
        QPainter painter(this);
        painter.drawRect(rect);
        painter.end();
    }
}


void PlotWithMarker::focusInEvent(QFocusEvent * event) {
    this->select(true);
}


void PlotWithMarker::select(bool state) {
    if(state ^ _selected) { /* TODO: check ^ semantics */
        _selected = state;
        emit(selected(state));
        update();
    }
}
