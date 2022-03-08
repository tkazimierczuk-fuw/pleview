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
    Q_OBJECT

public:
  ColorMap(double min = 0, double max = 1, const QColor &firstStop = Qt::black, const QColor &lastStop = Qt::white);
  ColorMap(const ColorMap &other);
  virtual ~ColorMap() = default;

  void addColorStop (double value, const QColor &);
  QList<double> colorStops () const;


  void setRange(double min, double max);
  double min() const { return d_min; }
  void setMin(double value) { setRange(value, max()); }
  double max() const { return d_max; }
  void setMax(double value) { setRange(min(), value); }

  QMap<double, QRgb> rawMap() const { return d_stops; }
  void setRawMap(const QMap<double, QRgb> &map) { d_stops = map; emit(valueChanged()); }
  
  virtual QRgb rgb(double value) const;
  QColor color (double value) const;
  QColor colorAtFraction(double frac) const { return color(fractionToValue(frac)); }
  
  void serializeToXml(QXmlStreamWriter * writer, const QString & tagName = "colormap") const;
  void unserializeFromXml(QXmlStreamReader *reader);

  ColorMap & operator=(const ColorMap &other);

  // renders a colorscale to an image file
  void exportScaleToPng(QString filename);

  double valueToFraction(double value) const { return (value-d_min)/(d_max-d_min); }
  double fractionToValue(double fraction) const { return d_min+(d_max-d_min)*fraction; }

  bool operator==(const ColorMap &other) const {
      return (d_stops == other.d_stops) && (d_min == other.d_min) && (d_max == other.d_max);
  }

  bool operator!=(const ColorMap &other) const {
      return !( (*this) == other );
  }


  bool autoscaling() const { return autoscale; }
  void setAutoscaling(bool on) { if(autoscale != on) { autoscale = on; emit(valueChanged());} }

  //! Updates scale limits (min, max) if autoscaling is on.
  void updateLimits(const QVector<double> &values);

signals:
  void valueChanged(); //! Emitted when the ColorMap is changed in any way

private:
  QMap<double, QRgb> d_stops;
  double d_min, d_max;
  bool autoscale;
};

#endif
