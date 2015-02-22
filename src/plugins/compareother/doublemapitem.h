#ifndef DOUBLEMAPITEM_H
#define DOUBLEMAPITEM_H

#include <MapItem.h>
#include "data2d.h"


class DoubleMapItem : public MapItem {
    Q_OBJECT
protected:
    class DoubleMapRenderThread;
    DoubleMapItem(DoubleMapRenderThread * t);

public:
    void setSecondData(const GridData2D * data);

    static DoubleMapItem * createDoubleMapItem();

public slots:
    void setColor(int i, const QColor &color);
    void setColor0(const QColor &color) { setColor(0, color); }
    void setColor1(const QColor &color) { setColor(1, color); }


protected:
    DoubleMapRenderThread * _doubleMapThread;
};


class DoubleMapItem::DoubleMapRenderThread : public MapItem::MapRenderThread {
    Q_OBJECT

public:
    DoubleMapRenderThread(GridData2D * data = 0);
    void setSecondData(const GridData2D * data);
    void setColor(int i, const QColor &color);

protected:
    GridData2D * d_data2;

    virtual bool recache();
    QRgb _color1, _color2;
};


#endif // DOUBLEMAPITEM_H
