#include "data.h"


Transform3D::Transform3D() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++)
            _rotation[i][j] = 0;
        _rotation[i][i] = 1;
        _tr[i] = 0;
    }
}

Point3D Transform3D::map(const Point3D &base) const {
    double result[3], source[3];
    result[0] = _tr[0];   result[1] = _tr[1];   result[2] = _tr[2];
    source[0] = base.x(); source[1] = base.y(); source[2] = base.z();
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            result[i] +=_rotation[i][j] * source[j];
    return Point3D(result[0], result[1], result[2]);
}


Data::~Data() {
}


BinarySearchResult BinarySearchResult::search(double value, const QVector<double> &vec) {
    if(vec.isEmpty() || vec.first() == vec.last())
        return 0;

    bool backward = (vec.first() > vec.last());
    if((!backward && value < vec.first()) || (backward && value > vec.first()) )
        return BinarySearchResult(0);
    if((!backward && value > vec.last()) || (backward && value < vec.last()) )
        return BinarySearchResult(vec.size()-1);

    int left = 0, right = vec.size() - 1;
    while (right > left) { // searched value is in range [left..right]
        int mid = (left + right) / 2;
        if((!backward && value <= vec[mid]) || (backward && value >= vec[mid]))
            right = mid;
        else
            left = mid + 1;
    }

    if(right == 0 || vec[right] == vec[right-1] || vec[right] == value) // degenerate cases
        return BinarySearchResult(right);

    else
        return BinarySearchResult(right-1, right, (vec[right-1]-value) / (vec[right-1]-vec[right]));
}
