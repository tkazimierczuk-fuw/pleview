#include "doublemapitem.h"
#include <QColor>


DoubleMapItem::DoubleMapItem(DoubleMapRenderThread * t) : MapItem(t), _doubleMapThread(t) {
}


DoubleMapItem * DoubleMapItem::createDoubleMapItem() {
    return new DoubleMapItem(new DoubleMapRenderThread());
}


void DoubleMapItem::setColor(int i, const QColor &color) {
    _doubleMapThread->setColor(i, color);
}


void DoubleMapItem::setSecondData(const GridData2D *data) {
    static_cast<DoubleMapRenderThread*>(mapThread)->setSecondData(data);
    thread->render(boundingRect(), QSize(200, 200));
    clearCache(); // there is no race condition here as queued slots execute in the same thread
}


DoubleMapItem::DoubleMapRenderThread::DoubleMapRenderThread(GridData2D * data) : MapRenderThread(data), d_data2(0) {
    _color1 = qRgb(255, 55, 0);
    _color2 = qRgb(0, 200, 255);
}


void DoubleMapItem::DoubleMapRenderThread::setSecondData(const GridData2D *data) {
    dataAbort = true; // tell the thread to pause and release dataMutex
    dataMutex.lock();
    dataAbort = false;
    if(d_data2 != 0)
        delete d_data2;
    d_data2 = data->clone();
    dataMutex.unlock();
    offlineRender();
}


void DoubleMapItem::DoubleMapRenderThread::setColor(int i, const QColor &color) {
    dataAbort = true; // tell the thread to pause and release dataMutex
    dataMutex.lock();
    dataAbort = false;

    if(i == 0)
        _color1 = color.rgb();
    else
        _color2 = color.rgb();

    dataMutex.unlock();
    offlineRender();
}



//bool DoubleMapItem::DoubleMapRenderThread::recache() {
//    QMutexLocker locker(&dataMutex);

//    if(d_data == 0 || d_data2 == 0)
//        return false;

//    if(colorsCached)
//        return true; // nothing to do

//    int counter = 0;
//    _cacheC.resize(d_data->size());

//    for(int ix = 0; ix < d_data->cols(); ix++) {
//        for(int iy = 0; iy < d_data->rows(); iy++) {
//            int v = qGray(d_colormap.color(d_data->valueAtIndex(ix, iy)).rgb());
//            int w = qGray(d_colormap.color(d_data2->valueAtIndexBounded(ix, iy)).rgb());
//            _cacheC[counter++] = QColor::fromRgb((v*qRed(_color1) + w*qRed(_color2))/255,
//                                                 (v*qGreen(_color1) + w*qGreen(_color2))/255,
//                                                 (v*qBlue(_color1) + w*qBlue(_color2))/255);
//      }

//     if(dataAbort)
//        return false;
//    }
//    colorsCached = true;
//    return true;
//}
