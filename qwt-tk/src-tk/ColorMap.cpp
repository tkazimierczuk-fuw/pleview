#include <iostream>

#include "ColorMap.h"
#include "ColorMapSlider.h"

ColorMap::ColorMap(double min, double max, const QColor &firstStop, const QColor &lastStop) {
  d_min = min;
  d_max = max;
  d_stops.insert(0, firstStop.rgba());
  d_stops.insert(1, lastStop.rgba());
}

ColorMap::ColorMap(const ColorMap &other) {
    d_min = other.d_min;
    d_max = other.d_max;
    d_stops = other.d_stops;
}


ColorMap::~ColorMap() {
}


void ColorMap::setRange(double min, double max) {
    d_min = min;
    d_max = max;
}

/**
  Set a color at given fraction of working range (0 < value < 1).
  */
void  ColorMap::addColorStop (double value, const QColor &color) {
  d_stops.insert(value, color.rgba());
}

/**
  Return list of points (fractions of working range) at which colors are set by user.
  The list starts with 0 and ends with 1.
  */
QList<double> ColorMap::colorStops () const {
  return d_stops.keys();
}



/**
  Map number into a color by linear interpolation of RGBA values.
  */
QRgb ColorMap::rgb(double value) const {
    if(d_stops.isEmpty())
        return qRgba(0, 0, 0, 0);

    double fraction = (value-d_min) / (d_max-d_min);    
    QMap<double, QRgb>::const_iterator it = d_stops.begin();       

    if(fraction <= it.key())
        return it.value();

    while(it != d_stops.end()) {
        if(fraction == it.key())
            return it.value();
        else if(fraction < it.key()) {
            double pos1 = it.key();
            QRgb color1 = it.value();

            it--;
            double pos0 = it.key();
            QRgb color0 = it.value();

            double dist = pos1 - pos0;
            double f2 = (fraction - pos0) / dist;
            double f1 = (pos1 - fraction) / dist;
            double red = f1 * qRed(color0) + f2 * qRed(color1);
            double green = f1 * qGreen(color0) + f2 * qGreen(color1);
            double blue = f1 * qBlue(color0) + f2 * qBlue(color1);
            double alpha = f1 * qAlpha(color0) + f2 * qAlpha(color1);
            return qRgba((int) red, (int) green, (int) blue, (int) alpha);
        }
        it++;
    }

    it--; // go back to last element
    return it.value();
}

/**
  @see rgb(double)
  */
QColor ColorMap::color(double value) const {
  return rgb(value);
}

/**
  Writes XML representation of the class in following format:
  <colormap min=... max=...>
    <step pos=... color=... alpha=...>
    <step pos=... color=... alpha=...>
    <step pos=... color=... alpha=...>
  </colormap>
  */
void ColorMap::serializeToXml(QXmlStreamWriter * writer, const QString & tagName) const {
  writer->writeStartElement(tagName);
  writeXmlAttribute(writer, "min", d_min);
  writeXmlAttribute(writer, "max", d_max);

  QMap<double, QRgb>::const_iterator i = d_stops.constBegin();
  while (i != d_stops.constEnd()) {
      writer->writeStartElement("step");
      writeXmlAttribute(writer, "pos", i.key());
      writeXmlAttribute(writer, "color", QColor(i.value()).name());
      writeXmlAttribute(writer, "alpha", qAlpha(i.value()));
      writer->writeEndElement();
      ++i;
  }
  writer->writeEndElement();
}

/**
  @see serializeToXml(QXmlStreamWriter, const QString)
  */
void ColorMap::unserializeFromXml(QXmlStreamReader * reader) {
  d_min = 0.49;
  d_max = 0.51;
  readXmlAttribute(reader, "min", &d_min);
  readXmlAttribute(reader, "max", &d_max);

  d_stops.clear();

  while(!seekChildElement(reader, "step").isEmpty()) {
      double pos = 0;
      readXmlAttribute(reader, "pos", &pos);
      QString name;
      readXmlAttribute(reader, "color", &name);
      int alpha = 0;
      readXmlAttribute(reader, "alpha", &alpha);
      QColor color(name);
      color.setAlpha(alpha);
      addColorStop(pos, color);
      seekEndElement(reader);
  }
}


ColorMap & ColorMap::operator=(const ColorMap &other) {
    if (this != &other) { // protect against invalid self-assignment
        d_stops = other.d_stops;
        d_min = other.d_min;
        d_max = other.d_max;
    }
    return *this;
}
