#ifndef NONLINLEASTSQ_H
#define NONLINLEASTSQ_H

#include <QVector>
#include <QPolygonF>
#include <QMap>
#include <QString>

#include "data.h"
#include "muParser.h"

/**
  * @class NonLinLeastSq
  *
  * @brief Class implementing non-linear least sqares fitting
  *
  * NonLinLeastSq uses non-linear least squares method to fit best parameters of given
  * function. The function is given as a muParser object and parameters are given as
  * a map of pairs name->address. Parameters named "x" and "y" (if defined) are substituted
  * accordingly for each data point.
  * User can also specify several different initial guesses of the
  * parameters - the fitting function will choose the one with the smallest chi^2 value.
  *
  * Current implementation uses Levenberg-Marquardt from lmmin library to minimize
  * chi^2 function.
  */
class NonLinLeastSq
{
public:
    //! Constructor
    NonLinLeastSq();

    //! Destructor
    virtual ~NonLinLeastSq();

    //! Set data to be fitted
    void setExpData(const QVector<Point3D> &data);

    //! Set data to be fitted (used if y dimension is not relevant)
    void setExpData(QPolygonF &data);

    //! Set function to be fitted. Do not delete the parser object while using the fitter.
    void setFunction(mu::Parser * function);


    /** Set parameters to be fitted. Parameters "x" and "y" are not being fitted but instead
      * are being sequentially substituted for each data point */
    void setParameters(QMap<QString, double*> parameters);

    //! Return map of used parameters
    QMap<QString, double*> parameters() const;

    //TODO: implement alternative starting points

    //! Perform the fitting
    void fit();

    //! Calculate current chi2 value
    double chi2() const;

private:
    //! Sets parameter values; order of parameters as in iterating through _vars map.
    void setParameters(const double *par);

    // Function needed by lmmin library
    friend void fcn_resid(const double *par, int m_dat, const void *data, double *fvec, int *info);

    mu::Parser * _func;
    QVector<Point3D> _points;
    QMap<QString, double*> _vars;
};

#endif // NONLINLEASTSQ_H
