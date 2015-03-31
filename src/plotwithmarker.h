#ifndef PLOTWITHMARKER_H
#define PLOTWITHMARKER_H

#include "advancedplot.h"
#include "doublemarker.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_zoomer.h"
#include "plotaddons.h"
#include "plotrangesmanager.h"

/**
  * @class PlotWithMarker
  *
  * @brief A plot with vertical marker
  *
  * This class is a plot widget intended to display a cross-section
  * through 2D data. It wraps two basic elements: a curve and
  * a vertical marker denoting place of the other cross-section.
  *
  */
class PlotWithMarker : public AdvancedPlot
{
    Q_OBJECT

public:
    PlotWithMarker(int direction, PlotRangesManager * plotRangesManager);

public slots:
    void updateMarker(int direction, double pos0, double pos1, double pos2);
    void updateCrossSection(int direction, const QVector<double> &x, const QVector<double> &y);
    void select(bool state = true);
    void setZoomMode();
    void setMarkerMode();
    void attachPlugin(PlotAddon * plugin);

protected:
    void paintEvent(QPaintEvent *);
    void focusInEvent(QFocusEvent * event);

private slots:
    void midClick(QPointF pos);

    //! Slot is triggered by event of holding down Shift and Ctrl. The argument is the mouse position in coordinates of canvas()
    void keyMidClick(QPoint pos);

signals:
    void markerMoved(int direction, double pos);
    void selected(bool);

private:
    DoubleMarker * marker;
    QwtPlotCurve *curve;
    QwtPlotPicker *picker, *zoomer, *unzoomer;
    bool _selected;
    int _direction;
};


#endif // PLOTWITHMARKER_H
