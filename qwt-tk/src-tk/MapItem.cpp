#include <iostream>

// #include "voro++.cc"

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


Data2D * MapItem::data() {
    return d_data;
}


void MapItem::setData(const GridData2D * data) {
    d_data = data->clone();
    mapThread->setColorMap(d_colormap);
    mapThread->setData(d_data);
    thread->render(boundingRect(), QSize(200, 200));
    clearCache(); // there is no race condition here as queued slots execute in the same thread
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


bool MapItem::MapRenderThread::draw(QPainter * painter, QRectF area) {
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(Qt::NoPen);

    if(d_data != 0 && d_data->cols() * d_data->rows() > 3e6) {
        painter->setPen(Qt::red);
        painter->fillRect(area, Qt::yellow);
        painter->drawLine(area.topLeft(), area.bottomRight());
        painter->drawLine(area.bottomLeft(), area.topRight());
        return true;
    }

    if(!recache())
        return false;

    QVector<double> xs = d_data->xValues();
    QVector<double> ys = d_data->yValues();

    int cnt = 0;
    for(int ix = 0; ix < xs.size(); ix++) {
        for(int iy = 0; iy < ys.size(); iy++) {
            double x1 = 0.5 * (xs.value(ix-1, xs[ix]) + xs[ix]);
            double x2 = 0.5 * (xs.value(ix+1, xs[ix]) + xs[ix]);
            double y1 = 0.5 * (ys.value(iy-1, ys[iy]) + ys[iy]);
            double y2 = 0.5 * (ys.value(iy+1, ys[iy]) + ys[iy]);
            painter->fillRect(QRectF(x1,y2, x2-x1, y1-y2), _cacheC.value(cnt++));
      }
      if(restart || abort)
          return false;
    }

    return true;
}


bool MapItem::MapRenderThread::recache() {
    QMutexLocker locker(&dataMutex);

    if(d_data == 0)
        return false;

    if(colorsCached)
        return true; // nothing to do

    int counter = 0;
    _cacheC.resize(d_data->size());

    for(int ix = 0; ix < d_data->cols(); ix++) {
        for(int iy = 0; iy < d_data->rows(); iy++) {
            _cacheC[counter++] = d_colormap.color(d_data->valueAtIndex(ix, iy));
      }

     if(dataAbort)
        return false;
    }
    colorsCached = true;
    return true;
}


void MapItem::MapRenderThread::setColorMap(const ColorMap &colormap)
{
    dataAbort = true; // tell the thread to pause and release dataMutex
    dataMutex.lock();
    dataAbort = false;
    d_colormap = colormap;
    colorsCached = false;
    dataMutex.unlock();
    render();
}


void MapItem::MapRenderThread::setData(GridData2D * data)
{
    dataAbort = true; // tell the thread to pause and release dataMutex
    dataMutex.lock();
    dataAbort = false;
    if(d_data != 0)
        delete d_data;
    d_data = data->clone();
    colorsCached = false;
    dataMutex.unlock();
    render();
}
