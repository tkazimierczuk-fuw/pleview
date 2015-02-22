#include <QtCore>
#include "plotrangesmanager.h"
#include "qwt_scale_engine.h"
#include <iostream>

PlotRangesManager::PlotRangesManager()
{
    timecounter = 0;
}


PlotRangesManager::~PlotRangesManager() {

}


void PlotRangesManager::setXYRanges(const QRectF & rect) {
    timecounter++;    
    setRange(X, rect.left(), rect.right());
    setRange(Y, rect.top(), rect.bottom());
}


void PlotRangesManager::setXZRanges(const QRectF & rect) {
    timecounter++;
    setRange(X, rect.left(), rect.right());
    setRange(Zx, rect.top(), rect.bottom());
}


void PlotRangesManager::setYZRanges(const QRectF & rect) {
    timecounter++;
    setRange(Y, rect.left(), rect.right());
    setRange(Zy, rect.top(), rect.bottom());
}


void PlotRangesManager::unzoomXY() {
    unzoom(X, Y);
}


void PlotRangesManager::unzoomXZ() {
    unzoom(X, Zx);
}


void PlotRangesManager::unzoomYZ() {
    unzoom(Y, Zy);
}


void PlotRangesManager::setRange(PlotAxis restrictionType, double minValue, double maxValue) {
    RangeChange rc;
    rc.time = timecounter;
    rc.restrictionType = restrictionType;
    QPair<double, double> prevRange = currentRange(restrictionType);
    rc.prevMin = prevRange.first;
    rc.prevMax = prevRange.second;
    rc.nextMin = minValue;
    rc.nextMax = maxValue;
    stack.push(rc);
    emitSignal(restrictionType, minValue, maxValue);
}


void PlotRangesManager::unzoom(PlotAxis axis1, PlotAxis axis2) {
    for(int n = stack.size() - 1;  n >= 0; n--) {
        if(stack[n].restrictionType == axis1 || stack[n].restrictionType == axis2) {
            emitSignal(stack[n].restrictionType, stack[n].prevMin, stack[n].prevMax);
            int time = stack[n].time;
            stack.remove(n);
            if(n > 0 && stack[n-1].time == time && (stack[n-1].restrictionType == axis1 || stack[n-1].restrictionType == axis2)) {
                emitSignal(stack[n-1].restrictionType, stack[n-1].prevMin, stack[n-1].prevMax);
                stack.remove(n-1);
            }
            break;
        }
    }
}


void PlotRangesManager::unzoomAll(PlotAxis axis) {
    for(int n = stack.size() - 1;  n >= 0; n--) {
        if(stack[n].restrictionType == axis) {
            stack.remove(n);
        }
    }
    emitSignal(axis, qSNaN(), qSNaN());
}


QPair<double, double> PlotRangesManager::currentRange(PlotAxis axis) const {
    QPair<double, double> range;
    range.first = range.second = qSNaN();
    for(int n = stack.size() - 1; n >= 0; n--)
        if(stack[n].restrictionType == axis) {
            range.first = stack[n].nextMin;
            range.second = stack[n].nextMax;
            return range;
        }
    return range;
}


bool PlotRangesManager::isAutoScaled(PlotAxis axis) const {
    return qIsNaN(currentRange(axis).first);
}



void PlotRangesManager::emitSignal(PlotAxis axis, double minValue, double maxValue) {
    QwtLinearScaleEngine scaleEngine;
    QwtScaleDiv scaleDiv;
    if(!qIsNaN(minValue) && !qIsNaN(minValue))
        scaleDiv = scaleEngine.divideScale(minValue, maxValue, 5, 10);
    switch(axis) {
        case X:     emit(xPlotRangesChanged(scaleDiv));
                    break;
        case Y:     emit(yPlotRangesChanged(scaleDiv));
                    break;
        case Zx:    emit(zxPlotRangesChanged(scaleDiv));
                    break;
        case Zy:    emit(zyPlotRangesChanged(scaleDiv));
                    break;
    default: // should not happen
                    break;
    }
}
