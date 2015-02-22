#include "nonlinleastsq.h"
#include "lmmin.h"

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

//    std::cerr << "Fitted parameters:\n";
//    it = _vars.begin();
//    idx = 0;
//    while (it != _vars.end()) {
//        if(it.key() != "x" && it.key() != "y")
//            std::cerr << it.key().toStdString() << " = " << par[idx++] << "\n";
//        ++it;
//    }
}
