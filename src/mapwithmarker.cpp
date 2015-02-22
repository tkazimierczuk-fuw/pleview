#include <QSvgGenerator>
#include <QPrinter>
#include <QPrintDialog>

#include "model.h"
#include "mapwithmarker.h"
#include "dataexport.h"
#include "qwt_scale_div.h"
#include "qwt_picker_machine.h"

MapWithMarker::MapWithMarker(PlotRangesManager * plotRangesManager)
{
    mapItem = new MapItem();
    mapItem->attach(this);

    vMarker = new DoubleMarker();
    vMarker->attach(this);
    hMarker = new DoubleMarker(Qt::Horizontal);
    hMarker->attach(this);

    picker = new QwtPlotPicker(this->canvas());
    picker->setStateMachine(new QwtPickerDragPointMachine());
    zoomer = new QwtPlotPicker(this->canvas());
    zoomer->setStateMachine(new QwtPickerDragRectMachine());
    unzoomer = new QwtPlotPicker(this->canvas());
    unzoomer->setStateMachine(new QwtPickerClickPointMachine());
    zoomer->setRubberBand(QwtPicker::RectRubberBand);
    connect(picker, SIGNAL(selected(QPointF)), this, SLOT(midClick(QPointF)));
    connect(picker, SIGNAL(moved(QPointF)), this, SLOT(midClick(QPointF)));
    connect(zoomer, SIGNAL(selected(QRectF)), plotRangesManager, SLOT(setXYRanges(QRectF)));
    connect(unzoomer, SIGNAL(selected(QPointF)), plotRangesManager, SLOT(unzoomXY()));
    connect(plotRangesManager, SIGNAL(xPlotRangesChanged(QwtScaleDiv)), this, SLOT(setHScale(QwtScaleDiv)));
    connect(plotRangesManager, SIGNAL(yPlotRangesChanged(QwtScaleDiv)), this, SLOT(setVScale(QwtScaleDiv)));

    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
    unzoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
    picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::MidButton);

    picker1 = new QwtPlotPicker(this->canvas());
    picker1->setStateMachine(new QwtPickerDragPointMachine());
    picker1->setMousePattern(QwtEventPattern::MouseSelect1,
                             Qt::LeftButton, Qt::ControlModifier);

    picker2 = new QwtPlotPicker(this->canvas());
    picker2->setStateMachine(new QwtPickerDragPointMachine());
    picker2->setMousePattern(QwtEventPattern::MouseSelect1,
                             Qt::LeftButton, Qt::ShiftModifier);
}


void MapWithMarker::setMarker(int direction, double pos0, double pos1, double pos2)
{
    if(direction == Engine::X)
        vMarker->setXValues(pos1, pos2);
    else
        hMarker->setXValues(pos1, pos2);
    replot();
}


void MapWithMarker::midClick(QPointF pos) {
    emit(markerMoved(1, pos.x()));
    emit(markerMoved(0, pos.y()));
}


void MapWithMarker::setData(const GridData2D * data) {
    mapItem->setData(data->clone());
    this->replot();
}


void MapWithMarker::setColor(const ColorMap &colorMap) {
    mapItem->setColorMap(colorMap);
    this->replot();
}


void MapWithMarker::attachPlugin(PlotAddon * plugin) {
    plugin->attach(this, PlotAddon::Map);
}


QString MapWithMarker::button1String() {
    return QString("Ctrl+LeftMouseButton");
}


QString MapWithMarker::button2String() {
    return QString("Shift+LeftMouseButton");
}


void MapWithMarker::print() {
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    dialog->setOption(QAbstractPrintDialog::PrintToFile);
    if (dialog->exec() != QDialog::Accepted)
        return;

    QRect rect = printer.pageRect();
    QPoint pageCenter = QPoint(rect.width()/2, rect.height()/2);
    int preferredHeight = height() * rect.width() / width();
    if(rect.height() > preferredHeight)
        rect.setHeight(preferredHeight);
    rect.moveCenter(pageCenter);

    QPainter painter;
    painter.begin(&printer);
    QwtPlot::drawCanvas(&painter);
    painter.end();
}


void MapWithMarker::exportImage() {
    DataExport::exportBitmap(this);
}

void MapWithMarker::generateSvg(QIODevice *device) {
    QSvgGenerator generator;
    generator.setOutputDevice(device);
    QPainter painter;
    painter.begin(&generator);
    QwtPlot::drawCanvas(&painter); //print(&painter, QRect(QPoint(0,0), this->size()));
    painter.end();
}
