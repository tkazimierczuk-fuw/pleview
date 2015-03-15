#ifndef VOROWRAPPER_H
#define VOROWRAPPER_H

#include <QObject>
#include <QVector>
#include <QPolygonF>


/**
 * @brief The VoroWrapper class is a wrapper using the Voro2D code
 *
 * It calculates the Voronoi diagram in a form of QVector<QPolygonF>.
 * The order of calculated polygons is the same as the given points.
 *
 * Boundary can be calculated automatically or given manually.
 * Any point outside the boundary is ignored.
 *
 */
class VoroWrapper : public QObject
{
    Q_OBJECT
public:
    explicit VoroWrapper(QObject *parent = 0);
    ~VoroWrapper();

    //! Get last calculated Voronoi diagram.
    const QVector<QPolygonF> & diagram() const;

    //! Get a boundary box (useful in automatic boundary mode)
    QRectF boundary() const;

signals:
    //! Emitted after every recalculation
    void newDiagram(const QVector<QPolygonF> &diagram, QRectF box);

public slots:
    //! Defines a set of points for which the Voronoi diagram is calculated (instantly). The vectors should have the same size.
    void setPoints(const QVector<double> &xs, const QVector<double> &ys);
    void setPoints(const QPolygonF &points);

    //! Defines a manual boundary box. The new Voronoi diagram is calculated instantly.
    void setBoundary(QRectF box);

    //! Sets boundary box to auto. The new Voronoi diagram is calculated instantly.
    void setAutoBoundary();


protected:
    void calculate();

private:
    QRectF _box; // automatic if empty
    QPolygonF _points;

    // results:
    QRectF _usedbox;
    QVector<QPolygonF> _diagram;
};


void testvoro();


#endif // VOROWRAPPER_H
