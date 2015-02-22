#ifndef PLOTRANGESMANAGER_H
#define PLOTRANGESMANAGER_H

#include <QObject>
#include <QStack>
#include <QPair>
#include "qwt_scale_div.h"

/**
  * @class PlotRangesManager
  *
  * This class holds information about current plot ranges. It is designed to
  * coordinate zooming events on different plots.
  * If a scale is set to auto then respective min and max values are NaN.
  */
class PlotRangesManager : public QObject {
    Q_OBJECT
public:
    PlotRangesManager();
    virtual ~PlotRangesManager();

    enum PlotAxis {X=1, Y=2, Zx=4, Zy=8};
    bool isAutoScaled(PlotAxis axis) const;
    QPair<double, double> currentRange(PlotAxis axis) const;

public slots:
    void setXYRanges(const QRectF & rect);
    void setXZRanges(const QRectF & rect);
    void setYZRanges(const QRectF & rect);
    void setRange(PlotRangesManager::PlotAxis axis, double minValue, double maxValue);
    void unzoomXY();
    void unzoomYZ();
    void unzoomXZ();
    void unzoom(PlotAxis axis1, PlotAxis axis2);
    void unzoomAll(PlotRangesManager::PlotAxis axis);

signals:
    void xPlotRangesChanged(QwtScaleDiv scaleDiv);
    void yPlotRangesChanged(QwtScaleDiv scaleDiv);
    void zxPlotRangesChanged(QwtScaleDiv scaleDiv);
    void zyPlotRangesChanged(QwtScaleDiv scaleDiv);

private:
    void emitSignal(PlotAxis axis, double minValue, double maxValue);
    int timecounter;

    class RangeChange {
    public:
        double prevMin, prevMax, nextMin, nextMax;
        PlotAxis restrictionType;
        int time;
    };

    QStack<RangeChange> stack;
};

#endif // PLOTRANGESMANAGER_H
