#include "data1d.h"

Data1D::Data1D() {
    _cachedBoundingRect = 0;
    _cachedSortedData = 0;
}


Data1D::Data1D(QVector<double>x, QVector<double>y) {
    _cachedBoundingRect = 0;
    _cachedSortedData = 0;
    for(int i = 0; i < qMin(x.size(), y.size()); i++)
        append(x[i], y[i]);
}


Data1D::~Data1D() {
    delete _cachedBoundingRect;
    delete _cachedSortedData;
}


void Data1D::clear() {
    _data.clear();
    invalidateCache();
}

void Data1D::append(double x, double y) {
    _data.push_back(Point2D(x, y));
    invalidateCache();
}

Point2D Data1D::point(int n) const {
    return _data.at(n);
}

double Data1D::interpolatedValue(double x) const {
    // TODO
    return 0;
}


QRectF Data1D::boundingRect() const {
    if(!_cachedBoundingRect) {
        double minX = INFINITY, minY = INFINITY;
        double maxX = -INFINITY, maxY = -INFINITY;

        for(int i = 0; i < _data.size(); i++) {
            if(minX > _data[i].x()) minX = _data[i].x();
            if(minY > _data[i].y()) minY = _data[i].y();
            if(maxX < _data[i].x()) maxX = _data[i].x();
            if(maxY < _data[i].y()) maxY = _data[i].y();
        }

        _cachedBoundingRect = new QRectF(minX, minY, maxX - minX, maxY - minY);
    }
    return *_cachedBoundingRect;
}


void Data1D::invalidateCache() {
    delete _cachedBoundingRect;
    _cachedBoundingRect = 0;
    delete _cachedSortedData;
    _cachedSortedData = 0;
}
