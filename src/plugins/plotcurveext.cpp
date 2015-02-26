#include <QtGui>

#include "plotcurveext.h"
#include "qwt_painter.h"
#include "qwt_symbol.h"
#include "qwt_scale_map.h"
#include "qwt_point_mapper.h"

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

    QwtPointMapper mapper;
    mapper.setFlag( QwtPointMapper::RoundPoints, QwtPainter::roundingAlignment( painter ) );
    mapper.setFlag( QwtPointMapper::WeedOutPoints, testPaintAttribute( QwtPlotCurve::FilterPoints ) );
//    mapper.setBoundingRect( canvasRect );

    const int chunkSize = 500;

    const double rx = symbol.size().width();
    const double ry = symbol.size().height();

    for ( int i = from; i <= to; i += chunkSize )
    {
        const int n = qMin( chunkSize, to - i + 1 );

        const QPolygonF points = mapper.toPointsF( xMap, yMap,  data(), i, i + n - 1 );

        int ip = i;
        foreach(QPointF pt, points) {
            double f = _intensities.value(ip, 1.);
            painter->drawEllipse(pt, f*rx, f*ry);
            ip++;
        }
    }
}




void PlotCurveExt::setIntensities(const QVector<double> &intensities) {
    _intensities = intensities;
    itemChanged();
}
