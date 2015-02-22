#ifndef DATA_H
#define DATA_H

#include <QIODevice>
#include <QWidget>
#include <iostream>

#ifndef INFINITY
#include <limits>
#define INFINITY std::numeric_limits<double>::infinity()
#endif

enum Dimension {x = 1, y = 2, z = 4};



class Point1D {
public:
    Point1D(double x = 0) : _x(x) { }
    double x() const { return _x; }
    void setX(double x) { _x = x; }

private:
  double _x;
};

class Point2D : public Point1D {
public:
    Point2D(double x = 0, double y = 0) : Point1D(x), _y(y) { }
    double y() const { return _y; }
    void setY(double y) { _y = y; }

private:
    double _y;
};

class Point3D : public Point2D {
public:
    Point3D(double x = 0, double y = 0, double z = 0) : Point2D(x, y), _z(z) { }
    double z() const { return _z; }
    void setZ(double z) { _z = z; }

private:
    double _z;
};



class Transform3D {
public:
    Transform3D();
    Point3D map(const Point3D &base) const;
//    void transpose(Dimension norm);

private:
    double _rotation[3][3];
    double _tr[3];
};



class Data {
public:
    virtual ~Data();
};


class DataReaderInterface
{
public:
    virtual ~DataReaderInterface() {}
    virtual Data * read(QIODevice * device, QWidget *parent = 0) = 0;
};



/** This class represents a result of binary search of value x in sorted array A.
 * Possible results include following cases of x present in A (and we get its index)
 * and x not present in A. In the latter case we get information about position where x
 * should be (i.e. two closest neighbors and the relative x's position between them).
 */
class BinarySearchResult {
public:
    //! Constructor with single index (either success or value beyond array range)
    BinarySearchResult(int index) : _idx0(index), _idx1(index), _f(0) {}

    //! Constructor with two indexes (value not found in the array)
    BinarySearchResult(int index0, int index1, double ratio) {
        if(ratio < 0.5) {
            _idx0 = index0; _idx1 = index1; _f = ratio;
        } else  {
            _idx0 = index1; _idx1 = index0; _f = 1 - ratio;
        }
    }

    //! Index of closest value to the searched one
    int closest() {
        return _idx0;
    }

    //! Index of second closest value to the search one (=closest() in degenerate cases)
    int secondClosest() {
        return _idx1;
    }

    //! Where x should be relative to two closest indexes (e.g. 0.1 - almost at closest() index)
    double ratio() {
        return _f;
    }

    //! Search a value x in sorted vector A
    static BinarySearchResult search(double x, const QVector<double> &A);

protected:
    int _idx0, _idx1;
    double _f;
};



#endif // DATA_H
