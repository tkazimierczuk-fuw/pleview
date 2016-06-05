#include <iostream>

#include "MapItem.h"
#include "timecounter.h"


MapItem::MapItem(MapRenderThread *thread) {
    d_data = 0;
    d_radius = 5;
    setItemAttribute(QwtPlotItem::AutoScale);
    if(!thread)
        thread = new MapRenderThread();
    mapThread = thread;
    mapThread->setColorMap(d_colormap);
    initThread(mapThread);
}


MapItem::~MapItem() {
}

QRectF MapItem::boundingRect() const {
    if(d_data == 0)
        return QRectF();

    double xmin = d_data->minX();
    double ymin = d_data->minY();
    double xmax = d_data->maxX();
    double ymax = d_data->maxY();
    return QRectF(xmin, ymin, xmax - xmin, ymax - ymin);
}


GridData2D *MapItem::data() {
    return d_data;
}


void MapItem::setData(const GridData2D * data) {
    d_data = data->clone();
    mapThread->setColorMap(d_colormap);
    mapThread->setData(d_data);
    thread->render(boundingRect(), QSize(200, 200));
    clearCache(); // there is no race condition here as queued slots execute in the same thread
    itemChanged();
}


void MapItem::setColorMap(const ColorMap & map) {
    d_colormap = map;
    mapThread->setColorMap(d_colormap);
    clearCache(); // there is no race condition here as queued slots execute in the same thread
}


MapItem::MapRenderThread::~MapRenderThread() {
    if(d_data != 0)
        delete d_data;
}




void MapItem::MapRenderThread::setColorMap(const ColorMap &colormap)
{
    dataAbort = true; // tell the thread to pause and release dataMutex
    dataMutex.lock();
    dataAbort = false;
    d_colormap = colormap;
    dataMutex.unlock();
    if(d_data)
        d_colormap.updateLimits(d_data->rawData());
    offlineRender();
}


void MapItem::MapRenderThread::setData(GridData2D * data)
{
    dataAbort = true; // tell the thread to pause and release dataMutex
    dataMutex.lock();
    dataAbort = false;
    if(d_data != 0)
        delete d_data;
    d_data = data->clone();
    dataMutex.unlock();
    d_colormap.updateLimits(d_data->rawData());
    offlineRender();
}

QPixmap MapItem::MapRenderThread::render(QRectF area, QSize resultSize) {
    if(!d_data)
        return QPixmap();
    dataMutex.lock();
    QPixmap pixmap = d_data->render(area, resultSize, d_colormap);
    dataMutex.unlock();
    return pixmap;
}


