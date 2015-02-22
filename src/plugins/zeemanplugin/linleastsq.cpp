#include "linleastsq.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
using namespace boost::numeric::ublas;


//! Constructor
LinLeastSq::LinLeastSq() : _order(0)  {
}


//! Destructor
LinLeastSq::~LinLeastSq() {
}


//! Set max order of fitted polynomial (0<= order <= 10)
void LinLeastSq::setOrder(int order) {
    if(order < 0 || order > 10)
        return;
    if(order != _order)
        invalidate();
    _order = order;
}


//! Get max order of fitted polynomial
int LinLeastSq::order() const {
    return _order;
}


//! Define the experimental data to be fitted to
void LinLeastSq::setExpData(const QPolygonF & points) {
    _points = points;
    invalidate();
}

//! Add another data point to be fitted to
void LinLeastSq::appendPoint(const QPointF &point) {
    _points.append(point);
    invalidate();
}

//! Returns fitted parameters in order (beta_0, ..., beta_[order])
QVector<double> LinLeastSq::parameters() const {
    calculate();
    return _beta;
}

//! Use fitted polynomial to calculate theoretical value at given x
double LinLeastSq::operator()(double x) const {
    calculate();
    double y = 0, px = 1;
    for(int i = 0; i < _beta.size(); i++) {
        y += px * _beta[i];
        px *= x;
    }
    return y;
}


//! Invalidate cached fitting results
void LinLeastSq::invalidate() const {
    _beta.clear();
}


//! Calculate least squares coefficients if needed
void LinLeastSq::calculate() const {
    if(!_beta.isEmpty())
        return;

    _beta.fill(0, _order + 1);

    if(_points.isEmpty())
        return;


    // prepare input for boost
    int N = _order + 1; // matrix order = polynomial order + 1
    matrix<double> Amat = zero_matrix<double>(N); // Vandermonde matrix
    vector<double> yvec = zero_vector<double>(N); // the other side of the equation

    for(int i = 0; i < _points.size(); i++) {
        int j = 0;
        double x = _points.at(i).x();
        double xp = 1.; // j-th power of x[i]
        while (j < 2*N - 1) {

            for(int col = 0; col < N; col++) {
                int row = j - col;
                if(row < 0 || row >= N)
                    continue;
                Amat(row, col) += xp; // I'm not sure about the order of arguments but Amat is symmetric
            }
            if(j < N)
                yvec(j) += _points.at(i).y() * xp;

            xp *= x;
            j++;
        }
    }

    // Now let's try to solve the equation. In degenerate case, we decrease the order
    // of output polynomial. We start with (matrix) order N.

    while( N>0 ) {
        // take a submatrix
        matrix<double> A = subrange(Amat, 0, N, 0, N);
        // create a permutation matrix for the LU-factorization
        permutation_matrix<std::size_t> pm(A.size1());

        // perform LU-factorization
        if(lu_factorize(A,pm) == 0) { // 0 = success
         // create identity matrix of "inverse"

         // backsubstitute
         vector<double> solution(subrange(yvec, 0, N));
         lu_substitute(A, pm, solution);

         for(int i = 0; i < N; i++)
             _beta[i] = solution(i);

         return;
        }
     N--;
    }
}
