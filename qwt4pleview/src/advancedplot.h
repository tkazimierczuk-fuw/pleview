#ifndef ADVANCEDPLOT_H
#define ADVANCEDPLOT_H

#include "qwt_plot.h"
#include "qwt_plot_zoomer.h"
#include "qwt_scale_div.h"


/**
 * @brief The AdvancedPlot class is QwtPlot + some commonly used features like zoomer.
 *
 * Additional features of AdvancedPlot include:
 * - QwtZoomer
 * - queued replot
 * - dialog windows for axes configuration
 */

class AdvancedPlot : public QwtPlot
{
    Q_OBJECT

public:
    AdvancedPlot();
    virtual void replot();

    const QAction * autoscaleAction(Qt::Orientation orientation) const;
    const QAction * fullscaleAction(Qt::Orientations orientation) const;

    //! Set action name according to given axis name (e.g. "x")
    void setAxisName(Qt::Orientation orientation, const QString &name);

    void setZoomerActive(bool active);


signals:
    //! Private signal which results in calling a replot
    void internalWakeup();

protected slots:
    void setHScale(const QwtScaleDiv &scaleDiv);
    void setVScale(const QwtScaleDiv &scaleDiv);

    void setVAxisAutoscale(bool);
    void setHAxisAutoscale(bool);


    void doVAxisFullscale();
    void doHAxisFullscale();


    void displayVAxisCustomContextMenu(const QPoint & pos);
    void displayHAxisCustomContextMenu(const QPoint & pos);
    void displayAxisCustomContextMenu(const QPoint & pos, int axisid);

private slots:
    void wakeUp();

private:
    //! Called exclusively as part of the constructor
    void createActions();


    bool replot_requested;
    QwtPlotZoomer * zoomer;
    QAction *hAutoscaleAction, *vAutoscaleAction, *hFullscaleAction, *vFullscaleAction, *hvFullscaleAction;
};

#endif // ADVANCEDPLOT_H
