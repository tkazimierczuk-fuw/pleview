#include "nonlinleastsq.h"
#include "lmmin.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;
using namespace std;

//! Constructor
NonLinLeastSq::NonLinLeastSq() : _func(0) {
}


//! Destructor
NonLinLeastSq::~NonLinLeastSq() {

}


//! Set data to be fitted
void NonLinLeastSq::setExpData(const QVector<Point3D> &data) {
    _points = data;
}


//! Set data to be fitted (used if y dimension is not relevant)
void NonLinLeastSq::setExpData(QPolygonF &data) {
    _points.reserve(data.size());
    _points.resize(0);
    for(int i = 0; i < data.size(); i++)
        _points.append(Point3D(data[i].x(), 0, data[i].y()));
}


//! Set function to be fitted. Do not delete the parser object while using the fitter.
void NonLinLeastSq::setFunction(mu::Parser * function) {
    _func = function;
}


/** Set parameters to be fitted. Parameters "x" and "y" are not being fitted but instead
  * are being sequentially substituted for each data point */
void NonLinLeastSq::setParameters(QMap<QString, double*> parameters) {
    _vars = parameters;
}


//! Return map of used parameters
QMap<QString, double*> NonLinLeastSq::parameters() const {
    return _vars;
}

QMap<QString, double> NonLinLeastSq::uncertainties() const {

    QMap<QString, double> result;

    foreach(QString key, _vars.keys()) {

        result[key] = _uncert.value(key, 0);

    }

    return result;

}


//! Calculate chi2 value
double NonLinLeastSq::chi2() const {
    double total = 0.;
    double *x = _vars.value("x",0), *y = _vars.value("y",0);
    for(int i = 0; i < _points.size(); i++) {
        if(x) *x = _points[i].x();
        if(y) *y = _points[i].y();
        total += pow(_func->Eval() - _points[i].z(), 2); // TODO: add error handling
    }
    return total;
}

 /* Matrix inversion routine.
 Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */

template<class T>

bool InvertMatrix(const matrix<T>& input, matrix<T>& inverse)

{
    typedef permutation_matrix<std::size_t> pmatrix;

    // create a working copy of the input
    matrix<T> A(input);

    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // perform LU-factorization
    int res = lu_factorize(A, pm);

    if (res != 0)
        return false;

    // create identity matrix of "inverse"
    inverse.assign(identity_matrix<T> (A.size1()));
    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);
    return true;

}

//! Calculate uncertainities

void NonLinLeastSq::calculateUncertainities() {

    QVector<QString> fitting_pars;
    foreach(QString par, _vars.keys())
        if(par != "x" && par != "y")
            fitting_pars.append(par);
    int n_pars = fitting_pars.size();

    if(n_pars == 0 || _points.size() == 0)
        return;

    double norm_factor = chi2() / (_points.size() - fitting_pars.size());

    matrix<double> J(_points.size(), n_pars);
    for (unsigned i = 0; i < J.size1 (); ++i)
        for (unsigned j = 0; j < J.size2 (); ++j)
            J(i, j) = 0;

    double *x = _vars.value("x",0), *y = _vars.value("y",0);

    for(int j = 0; j< fitting_pars.size(); j++) {
        QString par = fitting_pars[j];

        double *p = _vars[par];

        double copy = *p;

        double dx = *p * 1e-6;

        if(dx < 1e-6)

            dx = 1e-6;

        for(int i = 0; i < _points.size(); i++) {

            if(x) *x = _points[i].x();

            if(y) *y = _points[i].y();

            double val1 = _func->Eval();

            *p += dx;

            double val2 = _func->Eval();

            *p = copy;

            J(i,j) = val1-val2;
            J(i,j) /= dx;

        }
    }

    matrix<double> cov(n_pars, n_pars), invcov(n_pars, n_pars);
    cov = prod(trans(J), J);
    InvertMatrix(cov, invcov);

    for(int i = 0; i < n_pars; i++)
        _uncert[fitting_pars[i]] = sqrt(norm_factor * invcov(i,i));

}


//! Sets parameter values; order of parameters as in iterating through _vars map.
void NonLinLeastSq::setParameters(const double *par) {
    QMap<QString, double*>::iterator it = _vars.begin();
    int idx = 0;
    while (it != _vars.end()) {
        if(it.key() != "x" && it.key() != "y")
            *(it.value()) = par[idx++];
        ++it;
    }
}


//! Function to be passed to lmmin library
void fcn_resid(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    NonLinLeastSq * obj = const_cast<NonLinLeastSq*>(static_cast<const NonLinLeastSq*>(data));
    obj->setParameters(par);
    double *x = obj->_vars.value("x",0), *y = obj->_vars.value("y",0);
    for (int i=0; i < m_dat; ++i ) {
        if(x) *x = obj->_points[i].x();
        if(y) *y = obj->_points[i].y();
        fvec[i] = obj->_func->Eval() - obj->_points[i].z();
    }

    //std::cerr << "fcn called with bg = " << par[1] << ", amp = " << par[0]  << ";  chi^2 = " << obj->chi2()  << "\n";
}


//! Perform the fitting
void NonLinLeastSq::fit() {
    // prepare parameters coherently with setParameters(const double *)
    QVector<double> par(_vars.size());
    QMap<QString, double*>::iterator it = _vars.begin();
    int idx = 0;
    while (it != _vars.end()) {
        if(it.key() != "x" && it.key() != "y")
            par[idx++] = *(it.value());
        ++it;
    }

    if(idx == 0) // number of free parameters
        return;


    /* auxiliary parameters */
    lm_status_struct status;
    lm_control_struct control = lm_control_double;
    control.maxcall = 8000;
    control.printflags = 0;

    // run fitting
    lmmin(idx, par.data(), _points.size(), this, fcn_resid, &control, &status, 0);
    calculateUncertainities();

//    std::cerr << "Fitted parameters:\n";
//    it = _vars.begin();
//    idx = 0;
//    while (it != _vars.end()) {
//        if(it.key() != "x" && it.key() != "y")
//            std::cerr << it.key().toStdString() << " = " << par[idx++] << "\n";
//        ++it;
//    }
}
