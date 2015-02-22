#ifndef MAPPERPLUGIN_H
#define MAPPERPLUGIN_H

#include <QtGui>

#include "plotaddons.h"



class SimpleColorImage : public QFrame {
    Q_OBJECT
public:
    SimpleColorImage();

    //! Set number of tiles to be plotted
    void setTiles(int nx, int ny) {
        _nx = nx; _ny = ny; update();
    }

    //! Set colormap that will be used
    void setColorMap(const ColorMap &map) {
        _colormap = map; update();
    }

    //! Return data used
    QVector<double> data() const {
        return _data;
    }

    //! Return size of image in tiles
    QSize resolution() const {
        return QSize(_nx, _ny);
    }

    //! Set information about the scaling of the image
    void setRaster(const QTransform &transform) {
        scaling = transform;
        update();
    }

    //! Set information about the scaling of the image
    void setRaster(double x0, double y0, double dx, double dy) {
        setRaster( QTransform::fromScale(dx, dy).translate(x0, y0) );
    }

    //! Set raster manually (called by context menu)
    void setRaster();

    //! Set color scale manually (called by context menu)
    void setColorScale();

    //! Return information about the scaling of the image
    QTransform raster() const {
        return scaling;
    }

    //! Set new color scale. NaN mean automatic calculation of given limit.
    void setColorScale(double min, double max) {
        _min = min;
        _max = max;
        update();
    }

    double colorScaleMinimum() const {
        return _min;
    }

    double colorScaleMaximum() const {
        return _max;
    }

protected:
    //! @see QWidget::paintEvent(QPaintEvent*)
    void paintEvent(QPaintEvent *);

    //! Dynamic tool-tip
    bool event(QEvent *event);

public slots:
    //! Set cross-section data
    void setData(int nx, int ny, const QVector<double> &data) {
        _nx = nx; _ny = ny; _data = data; update();
    }

    //! Present an option to set step size
    void showContextMenu(const QPoint& pos);


private:
    int _nx, _ny;
    double _min, _max; // NaN if the limits are automatically calculated
    QVector<double> _data;
    ColorMap _colormap;
    QTransform scaling;
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


private:
    //! Delegated part of the constructor devoted controls for the advanced options
    //void createAdvancedOptions();

private slots:
    //! Update map after changing one of control widgets
    void crossSectionChanged();

    //! Set new cross-section
    void crossSectionChanged(const CrossSection &cs);

    //! Starts a new Pleview instance. Data is passed through temporary file
    void spawnRequested();

private:
    QFrame *_frame;
    SimpleColorImage * image;
    CrossSection _crossSection;
    QSpinBox *xPointsSpinBox, *yPointsSpinBox;
    QComboBox * directionCombo;
    QComboBox * scanTypeBox;
    // QCheckBox *flipXCheckBox, *flipYCheckBox;

    //QCheckBox *manualMinimum, *manualMaximum;
    //QLineEdit *minimumEdit, *maximumEdit;

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
