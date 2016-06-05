#ifndef DOUBLEMARKER_H
#define DOUBLEMARKER_H

#include <QtGui>
#include "qwt_plot_item.h"

class DoubleMarker : public QwtPlotItem
{
public:
    DoubleMarker(Qt::Orientation orientation = Qt::Vertical);
    ~DoubleMarker();

    void setBrush(const QBrush &b);
    const QBrush &brush() const;

    void setLabel(const QwtText&);
    QwtText label() const;

    void setLabelAlignment(Qt::Alignment);
    Qt::Alignment labelAlignment() const;

    void setLabelOrientation(Qt::Orientation);
    Qt::Orientation labelOrientation() const;

    void setSpacing(int);
    int spacing() const;

    virtual void draw(QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &) const;

    void setXValues(double, double);

    enum { Rtti_XHair = 501 };

    int rtti() const {
        return (int) Rtti_XHair; // magic number identifying plot item type
    }

protected:
    void drawAt(QPainter *,const QRectF &, int, int) const;

private:
    void drawLabel(QPainter *, const QRectF &, double) const;

    class PrivateData;
    PrivateData *d_data;
};

#endif // DOUBLEMARKER_H
