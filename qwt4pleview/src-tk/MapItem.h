#ifndef _MAPITEM_H
#define _MAPITEM_H

#include <QPainter>
#include <QPicture>

#include "qwt_plot_item.h"
#include "qwt_scale_map.h"
#include "offlinerenderitem.h"
#include "data2d.h"
#include "colormap.h"

class MapItem : public OfflineRenderItem {
    Q_OBJECT

protected:
  class MapRenderThread;

public:
  MapItem(MapRenderThread * thread = 0);
  ~MapItem();


  virtual QRectF boundingRect() const override;
  GridData2D * data();

  ColorMap colorMap() { return d_colormap; }


  enum { Rtti_MapItem = 502 };

  int rtti() const {
      return Rtti_MapItem; // just a random unique number
  }


public slots:
  void setColorMap(const ColorMap & map);
  void setData(const GridData2D * data);

signals:
  void colorMapChanged(const ColorMap &colorMap);

protected:
  double d_radius;
  GridData2D * d_data;
  ColorMap d_colormap;

  MapRenderThread * mapThread;
};



class MapItem::MapRenderThread : public RenderThread {
    Q_OBJECT

public:
    MapRenderThread(GridData2D * data = 0) : d_data(data) { }
    ~MapRenderThread();
    void setData(GridData2D * data);

    QPixmap render(QRectF area, QSize resultSize) override;

public slots:
    void setColorMap(const ColorMap &colormap);

protected:
  QMutex dataMutex;
  GridData2D * d_data;
  ColorMap d_colormap;

  volatile bool dataAbort;
};

#endif
