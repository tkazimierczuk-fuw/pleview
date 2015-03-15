#include "voronoi_plot_item.h"
#include "qwt_scale_map.h"
#include "vorowrapper.h"
#include <iostream>

VoronoiPlotItem::VoronoiPlotItem()
{
    _autoBoundingRect = true;
    _autorange = true;
    setItemAttribute(AutoScale);
}

QRectF VoronoiPlotItem::boundingRect() const {
    return _boundingRect.normalized();
}


void VoronoiPlotItem::setBoundingRect(QRectF rect)
{
    _boundingRect = rect;
    _autoBoundingRect = false;
    findVoronoi();
    itemChanged();
}



void VoronoiPlotItem::setPoints(const QPolygonF &points) {
    _points = points;
    findVoronoi();
    itemChanged();
}



void VoronoiPlotItem::setPoints(const QVector<double> &xs, const QVector<double> &ys) {
    QPolygonF poly;
    for(int i = 0; i < qMin(xs.size(), ys.size()); i++)
        poly.append(QPointF(xs[i], ys[i]));
    setPoints(poly);
}


QPolygonF VoronoiPlotItem::points() const {
    return _points;
}




void VoronoiPlotItem::findColorMapLimits()
{
    if(!_autorange)
        return;

    double min = qInf(), max = -qInf();

    foreach(double d, _values) {
        if(d < min) min = d;
        if(d > max) max = d;
    }
    if(qIsFinite(min) && qIsFinite(max))
        _colorMap.setRange(min, max);
}


void VoronoiPlotItem::findVoronoi()
{
    VoroWrapper vw;

    if(!_autoBoundingRect)
        vw.setBoundary(_boundingRect);
    vw.setPoints(_points);
    _diagram = vw.diagram();

    if(_autoBoundingRect)
        _boundingRect = vw.boundary();
}


void VoronoiPlotItem::setValues(QVector<double> values) {
    _values = values;
    findColorMapLimits();
    itemChanged();
}


void VoronoiPlotItem::setColorMap(ColorMap colorMap) {
    _colorMap = colorMap;
    findColorMapLimits();
    itemChanged();
}

ColorMap VoronoiPlotItem::colorMap() const {
    return _colorMap;
}


void VoronoiPlotItem::setColorAutoscale(bool on) {
    _autorange = on;
    if(on)
        findColorMapLimits();
    itemChanged();
}

bool VoronoiPlotItem::colorAutoscale() const {
    return _autorange;
}



void VoronoiPlotItem::draw(QPainter *painter) const {
    //painter->setPen(Qt::NoPen);

    for(int i = 0; i < qMin(_diagram.size(), _values.size()); i++) {
        painter->setBrush(QBrush(_colorMap.color(_values[i])));
        painter->drawPolygon(_diagram[i]);
    }
    painter->setBrush(Qt::NoBrush);
    for(int i = qMin(_diagram.size(), _values.size()); i < _diagram.size(); i++) {
        painter->drawPolygon(_diagram[i]);
    }

}


void VoronoiPlotItem::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const {

    painter->save();
    painter->translate(xMap.transform(0), yMap.transform(0));
    painter->scale(xMap.transform(1) - xMap.transform(0), yMap.transform(1) - yMap.transform(0));

    draw(painter);
    painter->restore();
}
