#include "data.h"


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
