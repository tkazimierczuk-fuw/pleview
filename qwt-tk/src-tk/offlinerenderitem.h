#ifndef OFFLINERENDERITEM_H
#define OFFLINERENDERITEM_H

#include <QtCore>
#include <QtGui>
#include "qwt_plot.h"
#include "qwt_plot_item.h"
#include "qwt_scale_map.h"



#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

class QImage;

inline uint qHash(const QRectF &key)
{
    const double fx = 52.4; // any number will work
    return qHash((int) (key.left() + fx * key.right() + fx*fx * key.top() + fx*fx*fx * key.bottom()));
}

inline uint qHash(const QSize &key) {
    return qHash(key.width()) ^ qHash(key.height());
}



class RenderThread : public QThread
{
   Q_OBJECT

public:
    RenderThread(QObject *parent = 0);
    ~RenderThread();

    /** Render an image for given area in given size. In the end, function should
      * emit renderedImage(QImage) signal. When subclassing, function should periodically
      * check values of restart and abort variables.
      */
    void render(QRectF area, QSize resultSize);

    /**
      * Render again with previous area and size.
      */
    void render();

    //! Stop rendering current image
    //void abort();

signals:
    void renderedImage(const QImage &image, const QRectF & area);
    void renderedPixmap(const QPixmap &image, const QRectF & area);

protected:
    void run();

public: // TODO: remove
    virtual bool draw(QPainter * painter, QRectF area);

    volatile bool restart, abort, halt; // reasonable assumption that setting bool value is atomic

    QMutex mutex;

private:
    QWaitCondition condition;
    QRectF area;
    QSize resultSize;
};


/**
  * @class OfflineRenderItem
  *
  * This class represent a plot item that takes some time to render.
  * The CPU-intensive rendering is made through separate thread and
  * the plot is updated when rendering is complete.
  * To subclass the OfflineRenderItem you should probably subclass
  * RenderThread and change initThread() function accordingly.
  */
class OfflineRenderItem : public QObject, public QwtPlotItem
{
    Q_OBJECT

public:
    OfflineRenderItem();
    ~OfflineRenderItem();
    void draw(QPainter *painter, const QwtScaleMap &xMap,
              const QwtScaleMap &yMap, const QRectF &canvasRect) const;
    void setOfflineRendering(bool on);

private slots:
     void updatePixmap(const QImage &image, QRectF area);
     void updatePixmap(const QPixmap &pixmap, QRectF area);


protected:
     void initThread(RenderThread * thread);
     QPixmap chooseBestPixmap(QRectF * area, QSize size) const;
     RenderThread * thread;
     mutable QCache<QPair<QRectF, QSize>, QPixmap> cache;
     void clearCache();
     bool useOfflineRendering;
};


#endif // OFFLINERENDERITEM_H
