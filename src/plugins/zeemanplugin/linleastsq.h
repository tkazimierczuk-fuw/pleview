#ifndef LINLEASTSQ_H
#define LINLEASTSQ_H

#include <QPointF>
#include <QPolygonF>
#include <QVector>


/**
  * @class LinLeastSq
  *
  * @brief Class implementing linear least squares fitting
  *
  * LinLeastSq object represents a linear model:
  * y = beta_n * x^n + .. + beta_1 * x + beta_0
  * where beta_k are best values in a sense of least squares.
  *
  * The experimental data is supplied by setExpData() function
  * and fitted parameters are retrieved by calling parameters().
  */

class LinLeastSq
{
public:
    //! Constructor
    LinLeastSq();

    //! Destructor
    virtual ~LinLeastSq();

    //! Set max order of fitted polynomial (0<= order <= 10)
    void setOrder(int order);

    //! Get max order of fitted polynomial
    int order() const;

    //! Define the experimental data to be fitted to
    void setExpData(const QPolygonF & points);

    //! Add another data point to be fitted to
    void appendPoint(const QPointF &point);

    //! Returns fitted parameters in order (beta_[order], ..., beta_0)
    QVector<double> parameters() const;

    //! Use fitted polynomial to calculate theoretical value at given x
    double operator()(double x) const;

private:
    //! Calculate least squares coefficients
    void calculate() const;

    //! Invalidate cached fitting results
    void invalidate() const;

    QPolygonF _points; // experimental data
    mutable QVector<double> _beta; // fitted coefficients
    int _order; // max order of fitted polynomial
};

#endif // LINLEASTSQ_H
