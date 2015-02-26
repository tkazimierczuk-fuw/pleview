#include "sectionedcurve.h"

void SectionedCurve::drawCurve(QPainter *painter, int style, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect, int from, int to) const {
    switch (style)
    {
       case Lines:
            for(int i = 0; i+1 < pointPairs.size(); i+=2)
                drawLines(painter, xMap, yMap, canvasRect, pointPairs[i], pointPairs[i+1]);
            break;
        case Sticks:
            drawSticks(painter, xMap, yMap, canvasRect, from, to);
            break;
        case Steps:
            drawSteps(painter, xMap, yMap, canvasRect, from, to);
            break;
        case Dots:
            drawDots(painter, xMap, yMap, canvasRect, from, to);
            break;
        case NoCurve:
        default:
            break;
    }
}
