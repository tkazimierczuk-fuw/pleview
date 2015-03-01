#include <cstdlib>
#include <cmath>
#include <iostream>
#include <QWidget>
#include <QProgressDialog>
#include <QImage>

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<double>::infinity()
#endif

#include "data2d.h"


// finds the minimum and maximum of a given vector
void findLimits(const QVector<double> &vec, double &minValue, double &maxValue) {
    if(vec.isEmpty()) {
        minValue = maxValue = 0;
        return;
    }
    minValue = INFINITY;
    maxValue = -INFINITY;
    for(int i = 0; i < vec.size(); i++) {
        if(minValue > vec[i])
            minValue = vec[i];
        if(maxValue < vec[i])
            maxValue = vec[i];
    }
}


void GridData2D::dataChanged() {
    bool alreadySorted = true;

    QVector<QPair<double, int> > mx(_x.size());
    for(int i = 0; i < _x.size(); i++) {
        mx[i].first = _x[i];
        mx[i].second = i;
    }
    std::sort(mx.begin(), mx.end());
    for(int i = 0; i < mx.size(); i++)
        if(mx[i].second != i)
            alreadySorted = false;

    QVector<QPair<double, int> > my(_y.size());
    for(int i = 0; i < _y.size(); i++) {
        my[i].first = _y[i];
        my[i].second = i;
    }
    std::sort(my.begin(), my.end());
    for(int i = 0; i < my.size(); i++)
        if(my[i].second != i)
            alreadySorted = false;

    if(!alreadySorted) {
        for(int i = 0; i < _x.size(); i++)
            _x[i] = mx[i].first;

        for(int j = 0; j < _y.size(); j++)
            _y[j] = my[j].first;

        QVector<double> tmpval(_values.size());
        for(int i = 0; i < _x.size(); i++)
            for(int j = 0; j < _y.size(); j++) {
                tmpval[i + j * _x.size()] = _values[mx[i].second + my[j].second * _x.size()];
            }
        _values = tmpval;
    }

    _cumulative.resize(_values.size());
    if(_cumulative.size() > 0) {
        _cumulative[0] = _values[0]; // 0,0
        for(int i = 1; i < _x.size(); i++) // first row
            _cumulative[i] = _cumulative[i-1] + _values[i];
        for(int j = 1; j < _y.size(); j++) // first column
            _cumulative[j * _x.size()] = _cumulative[(j-1) * _x.size()] + _values[j * _x.size()];
        for(int i = 1; i < _x.size(); i++)
            for(int j = 1; j < _y.size(); j++) // the rest
                _cumulative[i + j * _x.size()] = _cumulative[i + (j-1) * _x.size()] + _cumulative[i-1 + j * _x.size()] - _cumulative[i-1 + (j-1) * _x.size()] + _values[i + j * _x.size()];
    }

    findLimits(_x, _minX, _maxX);
    findLimits(_y, _minY, _maxY);
    findLimits(_values, _minZ, _maxZ);
}


GridData2D::GridData2D() {
    dataChanged();
}


GridData2D::GridData2D(QVector<double> &ys, QVector<double> &xs, QVector<double> &zs) {
    if(xs.size() * ys.size() == zs.size()) {
      _x = xs;
      _y = ys;
      _values = zs;
      dataChanged();
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

    dataChanged();
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





/** A small helper function for GridData2D::image(...). It assumes that 'width' points are uniformly
 *  spread from 'left' to 'right'. For each of such point we find a suitable index of xs vector or
 *  -1 if the point is out of bounds of xs grid. xs vector should be sorted in ascending order.
 */
QVector<int> doMapping(const int &width, const QVector<double> &xs, double left, double right) {
    QVector<int> xmap(width);
    for(int i = 0; i < width; i++) {
        double x = (i+0.5) / width;
        x = left * (1-x) + right * x;
        QVector<double>::const_iterator nx = std::lower_bound(xs.begin(), xs.end(), x);
        if(nx == xs.begin()) {
            double dist = (xs.size()==1) ? 1 : qAbs(xs[0] - xs[1]);
            if(qAbs(xs[0]-x) <= dist/2)
                xmap[i] = 0;
            else
                xmap[i] = -1;
        } else if(nx == xs.end()) {
            double dist = (xs.size()==1) ? 1 : qAbs(xs[xs.size()-1] - xs[xs.size()-2]);
            if(qAbs(xs.last()-x) <= dist/2)
                xmap[i] = xs.size()-1;
            else
                xmap[i] = -1;
        } else {
            if(qAbs((*nx)-x) > qAbs( (*(nx-1))-x ))
                xmap[i] = nx-xs.begin()-1;
            else
                xmap[i] = nx-xs.begin();
        }
    }
    return xmap;
}


void coerce(int &v, int lbound, int ubound) {
    if(v < lbound)
        v = lbound;
    if(v > ubound)
        v = ubound;
}

double GridData2D::sumInIndexRange(int firstx, int lastx, int firsty, int lasty) {
    if(_values.isEmpty())
        return 0;

    coerce(firstx, 0, _x.size()-1);
    coerce(lastx, 0, _x.size()-1);
    coerce(firsty, 0, _y.size()-1);
    coerce(lasty, 0, _y.size()-1);

    double result = _cumulative[lastx + lasty*_x.size()];
    if(firstx > 0)
        result -= _cumulative[firstx-1 + lasty*_x.size()];
    if(firsty > 0)
        result -= _cumulative[lastx + (firsty-1)*_x.size()];
    if(firsty > 0 && firstx > 0)
        result += _cumulative[firstx-1 + (firsty-1)*_x.size()];

    return result;
}


QPixmap GridData2D::render(QRectF area, QSize resolution, ColorMap cmap) const {
    if(_x.isEmpty() || _y.isEmpty())
        return QPixmap();

    int width = resolution.width();
    int height = resolution.height();


    // This will be the image data
    QVector<QRgb> imdata(width*height);

    // First we determine the mapping, i.e. which is corresponding data coordinate for each image pixel
    QVector<int> xmap = doMapping(width, _x, area.left(), area.right());
    QVector<int> ymap = doMapping(height, _y, area.top(), area.bottom());


    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++) {
            if(xmap[i] < 0 || ymap[j] < 0)
                imdata[i + width * j] = qRgba(0,0,0,0);
            else
                imdata[i + width * j] = cmap.rgb(_values[xmap[i] + _x.size() * ymap[j]]);
        }

    QImage im((uchar*) imdata.data(), width, height, QImage::Format_ARGB32);
    return QPixmap::fromImage(im);
}


