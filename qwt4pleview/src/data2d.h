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
#include <QPixmap>

#include "qwt_global.h"
#include "xml.h"
#include "data.h"
#include "colormap.h"


// This is a stupid workaround to force CMake to run MOC on this file
class Dummy : public QObject {
    Q_OBJECT
};


class QWT_EXPORT GridData2D : public SaveableModel {
    P_SAVEABLE_GADGET
public:
    GridData2D();
    GridData2D(const QVector<double> &ys, const QVector<double> &xs, const QVector<double> &zs);

    Q_PROPERTY(QVector<double> xs READ xValues WRITE setXValues STORED true)
    Q_PROPERTY(QVector<double> ys READ yValues WRITE setYValues STORED true)
    Q_PROPERTY(QVector<double> values READ rawData WRITE setRawData STORED true)

    double minX() const { return _minX; }
    double minY() const { return _minY; }
    double minZ() const { return _minZ; }
    double maxX() const { return _maxX; }
    double maxY() const { return _maxY; }
    double maxZ() const { return _maxZ; }


    int size() const;
    inline int cols() const { return _x.size(); }
    inline int rows() const { return _y.size(); }

    void setXValues(const QVector<double> &values) { _x = values; dataChanged(); }
    QVector<double> xValues() const;

    void setYValues(const QVector<double> &values) { _y = values; dataChanged(); }
    QVector<double> yValues() const;

    const QVector<double> & rawData() const { return _values; }
    void setRawData(const QVector<double> &values) { _values = values; dataChanged(); }

    inline double valueAtIndex(int nx, int ny) const {
        /* doesn't check boundaries */
        return _values[ny * cols() + nx];
    }

    inline void setValueAtIndex(int nx, int ny, double value) {
        /* doesn't check boundaries */
        _values[ny * cols() + nx] = value;
    }

    //! You should call this method after you change the values by setValueAtIndex or similar functions to recalculate the cache
    void dataChangingFinished();


    // Returns sum of values in a rectangular slice. Indexes should be valid
    double sumInIndexRange(int firstx, int lastx, int firsty, int lasty);

    double valueAtIndexBounded(int nx, int ny) const;

    double valueAt(double x, double y) const;

    double interpolatedValueAt(double x, double y) const;

    virtual std::shared_ptr<GridData2D> clone() const;

    GridData2D & operator=(const GridData2D &other);

    /**
     * Prepare a graphics (map) of given area with given resolution and color scale.
     */
    QPixmap render(QRectF area, QSize resolution, ColorMap cmap) const;

private:   
    //! Function checks if the _x and _y are sorted and if not then re-orders the data. Also caches values like minX and similar.
    void dataChanged();

    // actual data
    QVector<double> _x, _y, _values;

    // cached values calculated in dataChanged()
    QVector<double> _cumulative; // _cumulative[x,y] is a sum of _values[i,j] where with i <= x and j <= y
    mutable double _minX, _maxX, _minY, _maxY, _minZ, _maxZ; // cached value
};


class PleDataReader : public DataReaderInterface
{
public:
    virtual std::unique_ptr<GridData2D> read(QIODevice * device, QWidget *parent = 0) override;

private:
    static bool readLine(QIODevice *device, double *x, double *y, double *value);
};

#endif // DATA2D_H
