#include "curvegroup.h"
#include "plotcurveext.h"
#include "qwt_symbol.h"
#include "xml.h"
#include <iostream>

CurveGroup::CurveGroup()
{
    _plot = 0;
}


CurveGroup::~CurveGroup() {
    clearCurves();
}


void CurveGroup::clearCurves() {
    detachCurves();
    foreach(QwtPlotCurve * curve, _curves) {
        delete curve;
    }
    _curves.clear();
}

void CurveGroup::updateCurve(int index, const QColor &color) {
    if(_curves.size() > index)
        _curves[index]->setPen(QPen(color));
}

void CurveGroup::updateCurve(int index, const QPolygonF &data,
                             const QVector<double> &intensities) {
    QwtPlotCurve * curve = _curves.value(index, 0);
    if(curve) {
        curve->setData(data);
        if(!intensities.isEmpty() && curve->rtti() == PlotCurveExt::Rtti_CurveExt) {
            PlotCurveExt * curveext = static_cast<PlotCurveExt*>(curve);
                            curveext->setIntensities(intensities);
        }
    }
}


void CurveGroup::attachCurves(QwtPlot * plot) {
    foreach(QwtPlotCurve * curve, _curves) {
        curve->attach(plot);
    }

   _plot = plot;
}


void CurveGroup::detachCurves() {
    foreach(QwtPlotCurve * curve, _curves) {
        curve->detach();
    }

    _plot = 0;
}


void CurveGroup::addCurve(const QColor &color, const QPolygonF &data,
                          const QVector<double> &intensities) {
    QwtPlotCurve * curve;
    if(intensities.isEmpty()) {
        curve = new QwtPlotCurve();
        curve->setPen(QPen(color));
    }
    else {
        PlotCurveExt * extcurve = new PlotCurveExt();
        extcurve->setIntensities(intensities);
        curve = extcurve;
        QwtSymbol symbol(QwtSymbol::Ellipse, QBrush(color), Qt::NoPen, QSize(5,5));
        curve->setPen(Qt::NoPen);
        curve->setSymbol(symbol);
    }
    curve->setData(data);
    curve->setItemAttribute(QwtPlotItem::AutoScale, false);
    _curves.append(curve);
    if(_plot)
        curve->attach(_plot);
}


void CurveGroup::setCurveSymbol(int index, const QwtSymbol &symbol) {
    QwtPlotCurve * curve = _curves.value(index, 0);
    if(curve) {
        curve->setSymbol(symbol);
        curve->setPen(Qt::NoPen);
    }
}


QPolygonF CurveGroup::curveData(int index) {
    QwtPlotCurve * curve = _curves.value(index, 0);
    if(curve == 0)
        return QPolygonF();
    QPolygonF result;
    for(int i = 0; i < curve->dataSize(); i++)
        result.append(QPointF(curve->x(i), curve->y(i)));
    return result;
}


void writeCurveXml(QXmlStreamWriter * writer, QwtPlotCurve * curve) {
    PlotCurveExt * curveext = 0;
    QString type;
    switch(curve->rtti()) {
    case QwtPlotItem::Rtti_PlotCurve:
        type = QString("normal");
        break;
    case PlotCurveExt::Rtti_CurveExt:
        type = QString("ext");
        curveext = static_cast<PlotCurveExt*>(curve);
        break;
    default:
        type = QString("unknown");
    }

    writer->writeStartElement("curve");      
    writeXmlAttribute(writer, "type", type);

//    if(curve->pen().style() == Qt::NoPen)
//        writeXmlAttribute(writer, "pen", "none");
//    else
//        writeXmlAttribute(writer, "pencolor", curve->pen().color().name());
//
//    if

    for(int i = 0; i < curve->dataSize(); i++) {
        writer->writeStartElement("point");
        writeXmlAttribute(writer, "x", curve->y(i));
        writeXmlAttribute(writer, "y", curve->y(i));
        if(curveext)
            writeXmlAttribute(writer, "z", curveext->intensity(i));
        writer->writeEndElement();
    }

    writer->writeEndElement();
}


QwtPlotCurve * readCurveXml(QXmlStreamReader * reader) {
    QString type("unknown");
    readXmlAttribute(reader, "type", &type);

    QPolygonF points;
    QVector<double> intensities;
    double x, y, z;

    while(!seekChildElement(reader, "point").isEmpty()) {
        x = y = z = 0.;
        readXmlAttribute(reader, "x", &x);
        readXmlAttribute(reader, "y", &y);
        readXmlAttribute(reader, "z", &z);
        points.append(QPointF(x,y));
        intensities.append(z);
        seekEndElement(reader);
    }

    QwtPlotCurve * curve = 0;
    if(type == "normal")
        curve = new QwtPlotCurve();
    else if(type == "ext") {
        PlotCurveExt * curveext = new PlotCurveExt();
        curveext->setIntensities(intensities);
        curve = curveext;
    }

   if(curve) {
       curve->setItemAttribute(QwtPlotItem::AutoScale, false);
       curve->setData(points);
   }

   return curve;
}


void CurveGroup::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    for(int i = 0; i < _curves.size(); i++) {
        writeCurveXml(writer, _curves[i]);
    }
    writer->writeEndElement();
}


void CurveGroup::unserializeFromXml(QXmlStreamReader *reader) {
    while(!seekChildElement(reader, "curve").isEmpty()) {
        QwtPlotCurve * curve = readCurveXml(reader);
        if(curve)
            _curves.append(curve);
    }
}
