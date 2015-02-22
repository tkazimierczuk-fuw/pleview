#include <QtGui>

#include "plotcurveext.h"
#include "qwt_painter.h"
#include "qwt_symbol.h"
#include "qwt_scale_map.h"

/*!
  \brief Draw symbols with variable sizes
  \param painter Painter
  \param symbol Curve symbol
  \param xMap x map
  \param yMap y map
  \param from index of the first point to be painted
  \param to index of the last point to be painted

  @see QwtPlotCurve::drawSymbols()
*/
void PlotCurveExt::drawSymbols(QPainter *painter, const QwtSymbol &symbol,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to) const
{
    painter->setBrush(symbol.brush());
    painter->setPen(Qt::NoPen);

    const QwtMetricsMap &metricsMap = QwtPainter::metricsMap();

    QRect rect;
    rect.setSize(metricsMap.screenToLayout(symbol.size()));
    double ry = rect.height()/2;
    double rx = rect.width()/2;


    for (int i = from; i <= to; i++)
    {
      const int xi = xMap.transform(x(i));
      const int yi = yMap.transform(y(i));
      double f = _intensities.value(i, 1.);
      painter->drawEllipse(QPointF(xi, yi), f*rx, f*ry);
    }
}

void PlotCurveExt::setIntensities(const QVector<double> &intensities) {
    _intensities = intensities;
    itemChanged();
}
