#ifndef DATA2D_H
#define DATA2D_H

#include <vector>
#include <iostream>
#include <QRectF>
#include <QVector>
#include <QPointF>
#include <QPolygonF>
#include <QStringList>
#include <QTransform>
#include <QWidget>
#include <QtCore>

#include "qwt_global.h"
#include "xml.h"
#include "data.h"

class QWT_EXPORT Data2D : public Data
{
public:
    Data2D();
    virtual ~Data2D();

    virtual int size() const = 0;

    class Iterator;

    virtual double minX() const;
    virtual double minY() const;
    virtual double minZ() const;
    virtual double maxX() const;
    virtual double maxY() const;
    virtual double maxZ() const;

    virtual Iterator begin() const = 0;
    virtual Iterator end() const = 0;

    virtual Data2D * clone() const = 0;

protected:
    class IteratorData;
    virtual void dataChanged();

private:           
    void invalidateCache();
    void cacheBoundingCube() const;
    mutable bool _boundingCacheValid;
    mutable double _minX, _maxX, _minY, _maxY, _minZ, _maxZ; // boundingCache
};



class Data2D::IteratorData {
public:
    virtual bool operator==(const IteratorData &other) const = 0;
    virtual void next() = 0;
    virtual double value() const = 0;
    virtual QPolygonF cell() const = 0;
    virtual QPointF position() const = 0;
    virtual IteratorData * clone() const = 0;
};

class Data2D::Iterator {
public:
    Iterator(IteratorData * itdat) : _itdat(itdat) {}
    ~Iterator() { delete _itdat; }
    Iterator(const Iterator &other) {_itdat = other._itdat->clone(); }
    void operator++() { _itdat->next();}
    void operator++(int unused) { _itdat->next();}
    bool operator==(const Iterator &other) const {return *_itdat == *(other._itdat);}
    bool operator!=(const Iterator &other) const { return !(*this == other); }
    double value() const {return _itdat->value(); }
    QPointF position() const {return _itdat->position(); }
    QPolygonF cell() const {return _itdat->cell(); }
private:
    IteratorData * _itdat;
};



class QWT_EXPORT GridData2D : public Data2D {
public:
    GridData2D();
    GridData2D(QVector<double> &ys, QVector<double> &xs, QVector<double> &zs);

    int size() const;
    virtual Data2D::Iterator begin() const;
    virtual Data2D::Iterator end() const;
    inline int cols() const { return _x.size(); }
    inline int rows() const { return _y.size(); }

    void setXValues(const QVector<double> &values) { _x = values; }
    QVector<double> xValues() const;
    void setYValues(const QVector<double> &values) { _y = values; }
    QVector<double> yValues() const;
    QVector<double> rawData() const { return _values; }
    inline double valueAtIndex(int nx, int ny) const {
        /* doesn't check boundaries */
        return _values[ny * cols() + nx];
    }

    inline void setValueAtIndex(int nx, int ny, double value) {
        /* doesn't check boundaries */
        _values[ny * cols() + nx] = value;
    }

    double valueAtIndexBounded(int nx, int ny) const;

    double valueAt(double x, double y);

    double interpolatedValueAt(double x, double y);

    virtual GridData2D * clone() const;

    GridData2D & operator=(const GridData2D &other);

    class IteratorData;

    /**
     * Pickle all properties to XML stream. Do not write EndElement.
     */
    virtual void serializeToXml(QXmlStreamWriter * writer, const QString & tagName = "data") const;

    /**
     Update properties retrieved from XML stream. The previous
     token had type StartElement. The function should read
     all data till its EndElement is read.
     */
    virtual void unserializeFromXml(QXmlStreamReader * reader);

private:
    QVector<double> _x, _y, _values;
};


class GridData2D::IteratorData : public Data2D::IteratorData {
public:
    IteratorData(const GridData2D * data, int nx, int ny) : _data(data), _nx(nx), _ny(ny) {}
    virtual bool operator==(const Data2D::IteratorData &other) const;
    virtual void next() { if(++_nx == _data->cols()) {_nx = 0; _ny++;} }
    virtual double value() const;
    virtual QPolygonF cell() const;
    virtual QPointF position() const;
    virtual IteratorData * clone() const { return new IteratorData(_data, _nx, _ny); }

private:
    const GridData2D * _data;
    int _nx, _ny;
};


class PleDataReader : public DataReaderInterface
{
public:
    virtual Data * read(QIODevice * device, QWidget *parent = 0);

private:
    static bool readLine(QIODevice *device, double *x, double *y, double *value);
};

#endif // DATA2D_H
