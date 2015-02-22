#ifndef COLOR_MAP_H_
#define COLOR_MAP_H_

#include <QtCore>
#include <QColor>
#include "qwt.h"

#include "xml.h"

/**
  ColorMap is a class used to map real values into colors, e.g. in 2D plots.
  Usage scenario:
  1. Set a working range by setRange(double, double)
  2. Set limiting colors (i.e. for min and max values) by resetColorInterval(...)
  3. Set additional color stops by addColorStop(double, QColor)
  4. Map real number to color by color(double) or rgb(double)
*/
class ColorMap : public QObject, public Model {

public:
  ColorMap(double min = 0, double max = 1, const QColor &firstStop = Qt::black, const QColor &lastStop = Qt::white);
  ColorMap(const ColorMap &other);
  virtual ~ColorMap();

  void addColorStop (double value, const QColor &);
  QList<double> colorStops () const;


  void setRange(double min, double max);
  double min() const { return d_min; }
  void setMin(double value) { setRange(value, max()); }
  double max() const { return d_max; }
  void setMax(double value) { setRange(min(), value); }

  QMap<double, QRgb> rawMap() const { return d_stops; }
  void setRawMap(const QMap<double, QRgb> &map) { d_stops = map; }
  
  virtual QRgb rgb(double value) const;
  QColor color (double value) const;
  
  void serializeToXml(QXmlStreamWriter * writer, const QString & tagName = "colormap") const;
  void unserializeFromXml(QXmlStreamReader *reader);

  ColorMap & operator=(const ColorMap &other);



  double valueToFraction(double value) { return (value-d_min)/(d_max-d_min); }
  double fractionToValue(double fraction) { return d_min+(d_max-d_min)*fraction; }

private:
  QMap<double, QRgb> d_stops;
  double d_min, d_max;
};

#endif
