#include "advancedplot.h"

AdvancedPlot::AdvancedPlot()
{
    replot_requested = false;

    connect(this, SIGNAL(internalWakeup()), this, SLOT(wakeUp()), Qt::QueuedConnection);
}


void AdvancedPlot::replot() {
    replot_requested = true;
    emit internalWakeup();
}


void AdvancedPlot::wakeUp() {
    if(replot_requested) {
        replot_requested = false;
        QwtPlot::replot();
    }
}


void AdvancedPlot::setHScale(const QwtScaleDiv &scaleDiv) {
    if(!scaleDiv.isEmpty())
        this->setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);
    else
        this->setAxisAutoScale(QwtPlot::xBottom);
    replot();
}


void AdvancedPlot::setVScale(const QwtScaleDiv &scaleDiv) {
    if(!scaleDiv.isEmpty())
        this->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
    else
        this->setAxisAutoScale(QwtPlot::yLeft);
    replot();
}
