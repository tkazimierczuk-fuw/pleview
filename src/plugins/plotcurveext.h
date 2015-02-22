#ifndef PLOTCURVEEXT_H
#define PLOTCURVEEXT_H

#include "qwt_plot_curve.h"
#include "qwt_scale_map.h"
#include "qwt_symbol.h"
#include <QVector>


/**
  \brief PlotCurveExt is a QwtPlotCurve which allowes to vary symbol size
  independently for all data points.

  Size of each symbol is proportional to its intensity value (supplied by
  user by calling setIntensities(QVector<double>).
  */
class PlotCurveExt : public QwtPlotCurve
{
public:
    void setIntensities(const QVector<double> &intensities);
    double intensity(int index) { return _intensities.value(index); }

    enum { Rtti_CurveExt = 141 } ;
    int rtti() const { return Rtti_CurveExt; }

protected:
    virtual void drawSymbols(QPainter *p, const QwtSymbol &,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        int from, int to) const;

private:
    QVector<double> _intensities;
};

#endif // PLOTCURVEEXT_H
