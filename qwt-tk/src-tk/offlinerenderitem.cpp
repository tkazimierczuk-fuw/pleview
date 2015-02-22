#include "offlinerenderitem.h"
#include "qwt_plot.h"

#include <iostream>

OfflineRenderItem::OfflineRenderItem() {
     qRegisterMetaType<QImage>("QImage");
     useOfflineRendering = true;
}


OfflineRenderItem::~OfflineRenderItem() {
    delete thread;
}


void OfflineRenderItem::initThread(RenderThread * thread) {
    this->thread = thread;
    thread->start(QThread::LowPriority);
    connect(thread, SIGNAL(renderedImage(const QImage &, QRectF)),
            this, SLOT(updatePixmap(const QImage &, QRectF)), Qt::QueuedConnection);
}


void OfflineRenderItem::draw(QPainter *painter, const QwtScaleMap &xMap,
                             const QwtScaleMap &yMap, const QRectF &canvasRect) const {
    if(!painter->isActive())
        return; // is this check necessary?

    double l2 = xMap.invTransform(canvasRect.left());
    double t2 = yMap.invTransform(canvasRect.top());
    double r2 = xMap.invTransform(canvasRect.right());
    double b2 = yMap.invTransform(canvasRect.bottom());
    QRectF nextArea = QRectF(l2, t2, r2-l2, b2-t2).intersected(boundingRect());

    int l3 = xMap.transform(nextArea.left());
    int t3 = yMap.transform(nextArea.top());
    int r3 = xMap.transform(nextArea.right());
    int b3 = yMap.transform(nextArea.bottom());
    QSize nextSize(abs(r3-l3), abs(b3-t3));


    // however, if we want to do synchronized painting...
    if(!useOfflineRendering && thread) {
        painter->save();
        painter->translate(canvasRect.topLeft());
        painter->scale((canvasRect.right()-canvasRect.left()) / (r2-l2), (canvasRect.bottom()-canvasRect.top()) / (b2-t2));
        painter->translate(-l2,-t2);
        thread->draw(painter, nextArea);
        painter->restore();
        return;
    }


    QPixmap pixmap = chooseBestPixmap(&nextArea, nextSize);
    if(pixmap.isNull())
        return;

    double l = xMap.transform(nextArea.left());
    double t = yMap.transform(nextArea.top());
    double r = xMap.transform(nextArea.right());
    double b = yMap.transform(nextArea.bottom());
    painter->translate(l, t);
    painter->scale(r-l, b-t); // scale(...) allows to invert the image (and drawPixmap(...) does not)
    painter->drawPixmap(0, 0, 1, 1, pixmap);
}


//! Return "average" dpi (
double dpi(const QRectF area, QSize size) {
    if(size.width() == 0 || size.height() == 0)
        return 0;
    else
        return size.width() / area.width() + size.height() / area.height();
}


//! Return a fraction of rect1 covered by rect2
double covered(const QRectF &rect1, const QRectF &rect2) {
    double field1 = rect1.width() * rect1.height();
    QRectF rectU = rect1.intersected(rect2);
    double fieldU = rectU.width() * rectU.height();
    return field1 == 0 ? 0 : fieldU / field1;
}


QPixmap OfflineRenderItem::chooseBestPixmap(QRectF * area, QSize size) const {
    QPixmap * pixmap = cache.object(QPair<QRectF, QSize>(*area, size));
    if (pixmap) {
        return *pixmap;
    }
    else {
        thread->render(*area, size);
        QList<QPair<QRectF, QSize> > list = cache.keys();
        QList<QPair<QRectF, QSize> >::const_iterator it;
        QPair<QRectF, QSize> best_key;
        for(it = list.begin(); it != list.end(); ++it) {
            if(covered(*area, it->first) > covered(*area, best_key.first) ||
               (covered(*area, it->first) == covered(*area, best_key.first)
                && dpi(it->first, it->second) > dpi(best_key.first, best_key.second)))
                    best_key = *it;
        }
        pixmap = cache.object(best_key);
        if(pixmap) {
            *area = best_key.first;
            return *pixmap;
        }
        else
            return QPixmap();
    }
}


void OfflineRenderItem::updatePixmap(const QImage &image, QRectF area) {
    cache.insert(QPair<QRectF, QSize>(area, image.size()), new QPixmap(QPixmap::fromImage(image)));
    if(plot() != 0)
        plot()->replot();
}


RenderThread::RenderThread(QObject *parent)
    : QThread(parent)
{
    restart = false;
    abort = false;
    halt = false;
}


RenderThread::~RenderThread()
{
    mutex.lock();
    abort = true;
    halt = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void RenderThread::render(QRectF area, QSize resultSize) {


    double l2 = area.left();
    double t2 = area.top();
    double r2 = area.right();
    double b2 = area.bottom();

    mutex.lock();

    this->area = area;
    this->resultSize = resultSize;

    restart = true;
    condition.wakeOne();
    mutex.unlock();
}


void RenderThread::render() {
    mutex.lock();
    restart = true;
    condition.wakeOne();
    mutex.unlock();
}


bool RenderThread::draw(QPainter * painter, QRectF area) {
    painter->fillRect(area, Qt::green);
    return true; // return successful exit
}



void RenderThread::run()
{
    mutex.lock();
    forever {
        if (!restart)
            condition.wait(&mutex);

        QImage image(resultSize, QImage::Format_ARGB32);
        image.fill(0); // do we need it?
        QRectF area = this->area;
        QPainter painter;
        painter.begin(&image);
        painter.scale(resultSize.width() / area.width(),
                      resultSize.height() / area.height());
        painter.translate(-area.topLeft());
        restart = false;
        abort = false;
        mutex.unlock();

        bool success = draw(&painter, area);
        if(painter.isActive())
            painter.end();

        if (halt)
            return;

        mutex.lock();
        if(success && !abort) {
            emit renderedImage(image, area);
        }
        // mutex stays locked when stepping to the next iteration
    }
}


void OfflineRenderItem::setOfflineRendering(bool on) {
    useOfflineRendering = on;
}


void OfflineRenderItem::clearCache() {
    cache.clear();
}
