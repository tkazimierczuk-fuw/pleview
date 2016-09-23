#ifndef DATA1D_H
#define DATA1D_H

#include <vector>
#include <QRectF>
#include <QVector>

#include "data.h"

class Data1D
{
public:
    Data1D();
    Data1D(QVector<double> x, QVector<double> y);
    virtual ~Data1D();

    void clear();
    void append(double x, double y);
    Point2D point(int n) const;
    double interpolatedValue(double x) const;
    QRectF boundingRect() const;

    int size() const {
        return _data.size();
    }


private:
    void invalidateCache();
    std::vector<Point2D> _data;
    mutable QRectF *_cachedBoundingRect;
    mutable std::vector<Point2D> *_cachedSortedData;
};

#endif // DATA1D_H
