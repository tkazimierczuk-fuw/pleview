#ifndef ADVANCEDPLOT_H
#define ADVANCEDPLOT_H

#include "qwt_plot.h"
#include "qwt_plot_zoomer.h"
#include "qwt_scale_div.h"

class AdvancedPlot : public QwtPlot
{
    Q_OBJECT

public:
    AdvancedPlot();
    virtual void replot();

signals:
    void internalWakeup();

protected slots:
    void setHScale(const QwtScaleDiv &scaleDiv);
    void setVScale(const QwtScaleDiv &scaleDiv);

private slots:
    void wakeUp();

private:
    bool replot_requested;
    QwtPlotZoomer * zoomer;
};

#endif // ADVANCEDPLOT_H
