#ifndef MAPWITHMARKER_H
#define MAPWITHMARKER_H

#include "data2d.h"
#include "plotaddons.h"
#include "MapItem.h"
#include "advancedplot.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_zoomer.h"
#include "model.h"
#include "plotrangesmanager.h"
#include "doublemarker.h"

class MapWithMarker : public AdvancedPlot, public SvgRenderer
{
    Q_OBJECT

public:
    MapWithMarker(PlotRangesManager * plotRangesManager);

    static QString button1String();
    static QString button2String();

    QwtPlotPicker *picker1, *picker2; // TODO: move to private section

public slots:
    void setMarker(int direction, double pos0, double pos1, double pos2);
    void setData(const GridData2D * data);
    void setColor(const ColorMap &colorMap);
    void attachPlugin(PlotAddon * plugin);
    void print();
    void exportImage();
    void generateSvg(QIODevice * device);

private slots:
    void midClick(QPointF pos);


signals:
    void markerMoved(int direction, double pos);

private:
    DoubleMarker * vMarker, * hMarker;
    MapItem * mapItem;

    QwtPlotPicker *picker, *zoomer, *unzoomer;
};

#endif // MAPWITHMARKER_H
