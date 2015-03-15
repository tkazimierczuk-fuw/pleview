#ifndef MAPPERPLUGIN_H
#define MAPPERPLUGIN_H

#include <QtGui>
#include <QGroupBox>

#include "plotaddons.h"
#include "vectorwidget.h"

#include "advancedplot.h"
#include "voronoi_plot_item.h"


class SimpleVoronoiWidget : public QFrame {
    Q_OBJECT
public:
    SimpleVoronoiWidget();
    ~SimpleVoronoiWidget();

    //! Set colormap that will be used
    void setColorMap(const ColorMap &map) {
      plotter->setColorMap(map); update();
    }

    //! Set the grid of points
    void setPoints(QVector<double> xs, QVector<double> ys);
    void setPoints(QPolygonF);


    //! Set color scale in a dialog window (called by context menu)
    void setColorScale();


public slots:
    //! Set cross-section data
    void setData(const QVector<double> &data) {
        plotter->setValues(data); update();
    }

    //! Present a menu to change the color scale or export the map as graphics
    void showContextMenu(const QPoint& pos);

signals:
    //! Emitted when the mouse is middle-clicked over some voronoi polygon
    void tileClicked(int n);


protected:
    //! @see QWidget::paintEvent(QPaintEvent*)
    void paintEvent(QPaintEvent *) override;

    //! Dynamic tool-tip
    bool event(QEvent *event) override;

    //! Emits a signal if a tile is clicked with middle button
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;

    //! Translates mouse coordinate into tile coordinates
    QPointF posToCoordinates(QPoint pos) const;


private:
    QMatrix scaling;
    VoronoiPlotItem * plotter;

};




class MapperPluginObject : public PlotAddon {
    Q_OBJECT
public:

    //! Constructor
    MapperPluginObject();

    //! Destructor
    ~MapperPluginObject();

    //! Initialize a plugin
    void init(Engine *engine);

    //! Attach any plot items supplied by plugin to plot widget
    void attach(QwtPlot *plot, PlotType type);

    //! Show a dialog
    QWidget * controlWidget() {
        return _frame;
    }

    //! @see Model::serializeToXml()
    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;

    //! @see Model::unserializeFromXml()
    void unserializeFromXml(QXmlStreamReader *reader);

    enum Path {Z, ZigZag, Custom};


private:
    //! Delegated part of the constructor devoted controls for the advanced options
    //void createAdvancedOptions();

private slots:
    //! Update the displayed map.
    void updateMap();

    //! Set new cross-section
    void crossSectionChanged(const CrossSection &cs);


    //! Requests showing a cross-section corresponding to a given tile
    void showSpectrum(int n);

    //! Sets a custom vector
    void setVector(QVector<double> vec);

    //! Switch between custom vector and cross-section values
    void setCustom(bool);

    //! Updates current path according to the widget state
    void pathChanged();

signals:
    void setCrossSection(int direction, int pixel);


private:
    QVector<double> _values, _customvalues;
    Path path;
    int direction;
    int nx; // number of tiles in one row
    bool useCustom;


    // Widgets
    QFrame *_frame;
    SimpleVoronoiWidget * voronoiWidget;
    CrossSection _crossSection;
    QSpinBox *xPointsSpinBox, *yPointsSpinBox;
    VectorWidget *xPointsWidget, *yPointsWidget, *valuesWidget;
    QComboBox * directionCombo;
    QComboBox * scanTypeBox;
    QCheckBox * _useCustomWidget;

    QList<QWidget*> custompathwidgets, rectpathwidgets;

};



/*!
  \brief MapperPlugin used for creating 2D map from a cross-section.

  The plugin assumes that experimental data represents a kind of spatial
  mapping, e.g., the spectra were collected for spots aligned in a square
  grid.
*/

class MapperPlugin : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Mapper Plugin"; }

    //! Tagname used in saved files
    QString tagname() { return "mapperplugin"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new MapperPluginObject();
    }
};



#endif // MAPPERPLUGIN_H
