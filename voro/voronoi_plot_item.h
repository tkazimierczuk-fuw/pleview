#ifndef VORONOI_PLOT_ITEM_H
#define VORONOI_PLOT_ITEM_H

#include "qwt_plot_item.h"
#include "colormap.h"

class VoronoiPlotItem : public QwtPlotItem {
public:
    VoronoiPlotItem();
    QRectF boundingRect() const override;


    //! Draw the diagram using given painter
    void draw(QPainter *painter) const;
    void draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const override;


    void setPoints(const QPolygonF &points);
    void setPoints(const QVector<double> &xs, const QVector<double> &ys);
    QPolygonF points() const;


    void setValues(QVector<double> values);

    void setColorMap(ColorMap colorMap);
    ColorMap colorMap() const;

    void setColorAutoscale(bool on);
    bool colorAutoscale() const;

    void setBoundingRect(QRectF rect);


private:
    //! If autorange is set then find limits of the color scale
    void findColorMapLimits();

    //! Calculate a new Voronoi diagram
    void findVoronoi();


    QPolygonF _points;
    QVector<double> _values;

    QVector<QPolygonF> _diagram;
    QRectF _boundingRect;
    ColorMap _colorMap;
    bool _autorange;
    bool _autoBoundingRect;
};

#endif // VORONOI_PLOT_ITEM_H
