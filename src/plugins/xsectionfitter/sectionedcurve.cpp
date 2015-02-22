#include "sectionedcurve.h"

void SectionedCurve::drawCurve(QPainter *painter, int style, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const {
    switch (style)
    {
       case Lines:
            for(int i = 0; i+1 < pointPairs.size(); i+=2)
                drawLines(painter, xMap, yMap, pointPairs[i], pointPairs[i+1]);
            break;
        case Sticks:
            drawSticks(painter, xMap, yMap, from, to);
            break;
        case Steps:
            drawSteps(painter, xMap, yMap, from, to);
            break;
        case Dots:
            drawDots(painter, xMap, yMap, from, to);
            break;
        case NoCurve:
        default:
            break;
    }
}
