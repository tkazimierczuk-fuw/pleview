#ifndef CURVEGROUP_H
#define CURVEGROUP_H

#include <QtCore>
#include <QPolygonF>
#include "qwt_plot.h"
#include "qwt_symbol.h"
#include "qwt_plot_curve.h"

class CurveGroup
{
public:
    CurveGroup();
    ~CurveGroup();

    void addCurve(const QColor & color, const QPolygonF & data,
                  const QVector<double> & intensities = QVector<double>());
    void updateCurve(int index, const QColor &color);
    void updateCurve(int index, const QPolygonF &data,
                     const QVector<double> &intensities = QVector<double>());
    void clearCurves();
    void setCurveSymbol(int index, const QwtSymbol &symbol);

    QPolygonF curveData(int index);

    void attachCurves(QwtPlot *plot);
    void detachCurves();

    //! @see Model::serializeToXml()
    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;

    //! @see Model::unserializeFromXml()
    void unserializeFromXml(QXmlStreamReader *reader);

private:
    QMap<int, QColor> _colors;

    QVector<QwtPlotCurve*> _curves;

    QwtPlot * _plot;
};


#endif // CURVEGROUP_H
