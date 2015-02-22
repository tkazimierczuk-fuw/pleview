#ifndef SECTIONEDCURVE_H
#define SECTIONEDCURVE_H

#include "qwt_plot_curve.h"

class SectionedCurve : public QwtPlotCurve
{
public:
    void setRange(const QVector<int> &pointPairs) {
        this->pointPairs = pointPairs;
    }

protected:
    virtual void drawCurve(QPainter *p, int style,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        int from, int to) const;


private:
    QVector<int> pointPairs;
};

#endif // SECTIONEDCURVE_H
