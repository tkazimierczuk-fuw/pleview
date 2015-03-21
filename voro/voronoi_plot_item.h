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


    QVector<double> values() const;
    void setValues(QVector<double> values);

    void setColorMap(const ColorMap & colorMap);
    ColorMap colorMap() const;

    void setBoundingRect(QRectF rect);

    bool drawingEdges() const;
    void setDrawingEdges(bool on);

private:
    //! If colormap autorange is set then find limits of the color scale
    void findColorMapLimits();

    //! Calculate a new Voronoi diagram
    void findVoronoi();


    QPolygonF _points;
    QVector<double> _values;

    QVector<QPolygonF> _diagram;
    QRectF _boundingRect;
    ColorMap _colorMap;
    bool _autoBoundingRect;
    bool _drawEdges;
};

#endif // VORONOI_PLOT_ITEM_H
