#include <cstdlib>
#include <cmath>
#include <iostream>
#include <QWidget>
#include <QProgressDialog>

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<double>::infinity()
#endif

#include "data2d.h"

Data2D::Data2D() {
      invalidateCache();
}

Data2D::~Data2D() {
}

void UnalignedData2D::append(const Point3D & point) {
    _data.push_back(point);
    dataChanged();
}

void UnalignedData2D::append(double x, double y, double z) {
    append(Point3D(x, y, z));
}

void UnalignedData2D::clear() {
    _data.clear();
    dataChanged();
}

void Data2D::dataChanged() {
    invalidateCache();
}

void Data2D::invalidateCache() {
  _boundingCacheValid = false;
}

void Data2D::cacheBoundingCube() const {
  if (_boundingCacheValid)
    return;
  _minX = _minY = _minZ = INFINITY;
  _maxX = _maxY = _maxZ = -INFINITY;
  int licznik = 0;
  Iterator it = this->begin();
  while(it != this->end()) {
    double z = it.value();
    QPointF p = it.position();
    if(_minX > p.x()) _minX = p.x();
    if(_minY > p.y()) _minY = p.y();
    if(_minZ > z) _minZ = z;
    if(_maxX < p.x()) _maxX = p.x();
    if(_maxY < p.y()) _maxY = p.y();
    if(_maxZ < z) _maxZ = z;
    ++it;
  }
  _boundingCacheValid = true;
}

double Data2D::minX() const {
  cacheBoundingCube();
  return _minX;
}

double Data2D::minY() const {
  cacheBoundingCube();
  return _minY;
}

double Data2D::minZ() const {
  cacheBoundingCube();  
  return _minZ;
}

double Data2D::maxX() const {
  cacheBoundingCube();
  return _maxX;
}

double Data2D::maxY() const {
  cacheBoundingCube();
  return _maxY;
}

double Data2D::maxZ() const {
  cacheBoundingCube();
  return _maxZ;
}





int UnalignedData2D::size() const {
    return _data.size();
}

UnalignedData2D * UnalignedData2D::clone() const {
    UnalignedData2D * ret = new UnalignedData2D();
    ret->_data = _data; /* deep copy */
    return ret;
}


GridData2D::GridData2D() {
    /* example */
    int nx = 0;
    int ny = 0;
}

GridData2D::GridData2D(QVector<double> &ys, QVector<double> &xs, QVector<double> &zs) {
    if(xs.size() * ys.size() == zs.size()) {
      _x = xs;
      _y = ys;
      _values = zs;
  } else {
      /* error! */
  }
}

int GridData2D::size() const {
    return _values.size();
}

GridData2D * GridData2D::clone() const {
    GridData2D * ret = new GridData2D(*this);
    ret->_values = _values; /* deep copy */
    ret->_x = _x;
    ret->_y = _y;
    return ret;
}

GridData2D & GridData2D::operator=(const GridData2D &other) {
    _x = other._x;
    _y = other._y;
    _values = other._values;
    dataChanged();
    return *this;
}


Data2D::Iterator GridData2D::begin() const {
    return Iterator(new IteratorData(this, 0, 0));
}

Data2D::Iterator GridData2D::end() const {
    if(cols() == 0 || rows() == 0)
        return begin();
    return Iterator(new IteratorData(this, cols()-1, rows()-1));
}

double GridData2D::IteratorData::value() const {
    return _data->valueAtIndex(_nx, _ny);
}

QPointF GridData2D::IteratorData::position() const {
    /* _n = nx*rows() + ny */
    return QPointF(_data->_x[_nx], _data->_y[_ny]);
}

QPolygonF GridData2D::IteratorData::cell() const {
    QPolygonF ret;

    double x1 = (_nx==0) ? _data->_x[_nx] : _data->_x[_nx - 1];
    x1 = 0.5 * (x1 + _data->_x[_nx]);
    double x2 = (_nx==_data->cols()-1) ? _data->_x[_nx] : _data->_x[_nx + 1];
    x2 = 0.5 * (x2 + _data->_x[_nx]);
    double y1 = (_ny==0) ? _data->_y[_ny] : _data->_y[_ny - 1];
    y1 = 0.5 * (y1 + _data->_y[_ny]);
    double y2 = (_ny==_data->rows()-1) ? _data->_y[_ny] : _data->_y[_ny + 1];
    y2 = 0.5 * (y2 + _data->_y[_ny]);

    ret.append(QPointF(x1, y1));
    ret.append(QPointF(x1, y2));
    ret.append(QPointF(x2, y2));
    ret.append(QPointF(x2, y1));
    return ret;
}


Data2D::Iterator UnalignedData2D::begin() const {
    return Iterator(new IteratorData(this, 0));
}


Data2D::Iterator UnalignedData2D::end() const {
    return Iterator(new IteratorData(this, size()));
}


double UnalignedData2D::IteratorData::value() const {
    return _data->_data[_n].z();
}


QPointF UnalignedData2D::IteratorData::position() const {
    return QPointF(_data->_data[_n].x(), _data->_data[_n].y());
}


bool PleDataReader::readLine(QIODevice *device, double *x, double *y, double *z) {
    if(device->atEnd())
        return false;
    QString line = device->readLine();
    QStringList tokens = line.split(QRegExp("\\s+"));
    /* TODO: check for y */
    if(tokens.size() < 2)
        return false;
    bool ok1, ok2;
    double _x = tokens[0].toDouble(&ok1);
    double _z = tokens[1].toDouble(&ok2);
    if(ok1 && ok2) {
        *x = _x;
        *z = _z;
        return true;
    }
    else return false;
}


Data * PleDataReader::read(QIODevice * device, QWidget *parent) {
    QProgressDialog progress("Opening file...", "Cancel",
                                                   0, device->size(), parent);
    /* TODO: who deletes progress dialog? */
    progress.show();
    int pos0 = device->pos();
    QVector<double> xs, ys, values;
    double x, y, value;

    y = 0;
    // discard initial comments
    while(!readLine(device, &x, &y, &value) && !device->atEnd())
        ;
    progress.setValue(device->pos());

    ys.append(y);
    y += 1;
    xs.append(x);
    values.append(value);

    double x0 = x;

    // read the first spectrum
    while(readLine(device, &x, &y, &value) && x!=x0) {
        xs.append(x);
        values.append(value);
    }

    if(x == x0) {
        values.append(value);
    }

    // estimate total size
    int pos1 = device->pos();
    int estimate = (device->size() / (pos1 - pos0)) * values.size();
    estimate += 2 * values.size(); // it's better to over-estimate than under-estimate
    values.reserve(estimate);

    bool prev = false;
    // read all other spectra
    while(!device->atEnd()) {
        bool next = readLine(device, &x, &y, &value);
        if((x==x0) || (!prev && next)) { // beginning of a new spectrum
            ys.append(y);
            y += 1;
            progress.setValue(device->pos());
            if(progress.wasCanceled()) {
                progress.close();
                return 0;
            }
        }

        if(next)
            values.append(value);

        prev = next;
    }

    if(values.size() != xs.size() * ys.size())
        return 0; // error

    return new GridData2D(ys, xs, values);
}


QVector<double> GridData2D::xValues() const {
    return _x;
}

QVector<double> GridData2D::yValues() const {
    return _y;
}

bool UnalignedData2D::IteratorData::operator==(const Data2D::IteratorData &other) const {
    const UnalignedData2D::IteratorData * it = dynamic_cast<const UnalignedData2D::IteratorData*>(&other);
    return it != 0 && _n == it->_n;
}

bool GridData2D::IteratorData::operator==(const Data2D::IteratorData &other) const {
    const GridData2D::IteratorData * it = dynamic_cast<const GridData2D::IteratorData*>(&other);
    return it != 0 && _nx == it->_nx && _ny == it->_ny;
}


void GridData2D::serializeToXml(QXmlStreamWriter * writer, const QString & tagName) const {
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "xsize", _x.size());
    writeXmlAttribute(writer, "ysize", _y.size());

    QByteArray array = QByteArray((char*) _x.data(), _x.size() * sizeof(double));
    writer->writeTextElement("xs", array.toBase64());

    array = QByteArray((char*) _y.data(), _y.size() * sizeof(double));
    writer->writeTextElement("ys", array.toBase64());

    array = QByteArray((char*) _values.data(), _values.size() * sizeof(double));
    writer->writeTextElement("values", array.toBase64());

    writer->writeEndElement();
}


// Helper function for unserializeFromXml(QXmlStreamReader*)
void read_vector_xml(QXmlStreamReader * reader, QVector<double> *v, int decl_size) {
    // reader.tokenType() == StartElement
    while(!reader->isEndElement() && !reader->isCharacters())
        reader->readNext();
    if(reader->isCharacters()) {
        QByteArray array = QByteArray::fromBase64(reader->text().toString().toLatin1());
        if(array.size() / sizeof(double) < decl_size)
            decl_size = array.size() / sizeof(double);
        if(decl_size >= 0) {
            v->resize(decl_size);
            for(int i = 0; i < decl_size; i++)
              (*v)[i] = ((double*) array.data())[i];
        }
        seekEndElement(reader);   //reader->skipCurrentElement();
    }
}

    /**
     Update properties retrieved from XML stream. The previous
     token had type StartElement. The function should read
     all data till its EndElement is read.
     */
void GridData2D::unserializeFromXml(QXmlStreamReader * reader) {
    int xsize = _x.size(), ysize = _y.size();
    readXmlAttribute(reader, "xsize", &xsize);
    readXmlAttribute(reader, "ysize", &ysize);

    QString child;
    do {
        child = seekChildElement(reader, "xs", "ys", "values");

        if(child == "xs")
            read_vector_xml(reader, &_x, xsize);
        else if(child == "ys")
                read_vector_xml(reader, &_y, ysize);
        else if(child == "values")
                read_vector_xml(reader, &_values, xsize * ysize);
    } while (!child.isEmpty());
}


double GridData2D::valueAtIndexBounded(int nx, int ny) const {
    if(nx < 0)
        nx = 0;

    if(nx >= cols())
        nx = cols() - 1;

    if(ny < 0)
        ny = 0;

    if(ny >= rows())
        ny = rows() - 1;

    return valueAtIndex(nx, ny);
}


double GridData2D::interpolatedValueAt(double x, double y) {
    if(size() == 0)
        return 0;// or better NaN?

    BinarySearchResult idxX = BinarySearchResult::search(x, _x);
    BinarySearchResult idxY = BinarySearchResult::search(y, _y);

    double interpolated = 0;
    interpolated += (1-idxX.ratio()) * (1-idxY.ratio()) * valueAtIndex(idxX.closest(), idxY.closest());
    interpolated += idxX.ratio() * (1-idxY.ratio()) * valueAtIndex(idxX.secondClosest(), idxY.closest());
    interpolated += (1-idxX.ratio()) * idxY.ratio() * valueAtIndex(idxX.closest(), idxY.secondClosest());
    interpolated += idxX.ratio() * idxY.ratio() * valueAtIndex(idxX.secondClosest(), idxY.secondClosest());
    return interpolated;
}


double GridData2D::valueAt(double x, double y) {
    if(size() == 0)
        return 0;// or better NaN?

    BinarySearchResult idxX = BinarySearchResult::search(x, _x);
    BinarySearchResult idxY = BinarySearchResult::search(x, _x);

    return valueAtIndex(idxX.closest(), idxY.closest());
}
