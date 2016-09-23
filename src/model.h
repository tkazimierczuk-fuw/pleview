#ifndef Engine_H
#define Engine_H

#include <QtCore>
#include <memory>
using std::shared_ptr;

class Engine;

#include "DLLDefines.h"
#include "data2d.h"
#include "colormap.h"
#include "muParser.h"
#include "plotaddons.h"
#include "datafilters.h"
#include "axis.h"
#include "defs.h"
#include "pleview.h"
#include "plotrangesmanager.h"

/**
  * @class CrossSection
  *
  * @brief Holder of information about cross-sections.
  *
  * CrossSection is a class that holds all information about a cross-section through 2D data.
  * Information kept in a CrossSection object includes:
  *   n - central pixel of the cross-section
  *   pxwidth - width of the cross-section in pixels (always >0)
  *   pos - coordinates of the cross-section point (i.e. coordinates corresponding to pixel n)
  *   lower, upper - coordinates of the range covered by the cross-section (i.e. coordinates corresponding to n-pxwidth/2 and n+(pxwidth+1)/2)
  *   curve - actual cross-section (i.e. point that can be plotted directly)
  *
  * Each of the abovementioned values is kept in two instances: one for horizontal cross-section
  * and one for vertical one. Generally, index Pleview::X corresponds to vertical cross-section
  * and Pleview::Y is related to values on y axis.
  * E.g., curve[X] is a vertical cross-section taken at y=pos[X] (which in turn corresponds to pixel number n[X]).
  */
class PLEVIEW_EXPORT CrossSection {
public:
    // Default constructor
    CrossSection();

    // Copy constructor
    CrossSection(const CrossSection &other);

    // Sets all members to 0
    void reset();

    int n[2]; // Central pixels of the cross-sections
    int pxwidth[2]; // Width of the cross-section in pixels
    double pos[2]; // Coordinates of the cross-section point
    double lower[2], upper[2]; // Coordinates of the range covered by the cross-section
    QPolygonF curve[2]; // Curves obtained by the cross-sections
};


class SvgRenderer {
public:
    virtual void generateSvg(QIODevice * device) = 0;
};


class PlotAddon;
class PlotAddonManager;
class DataFilterManager;


/**
  * @class Engine
  *
  * @brief A heart of application logic
  *
  * This is a main class of the application logic. It should be self-sufficient
  * (in a sense that it contains all saveable information) and gui-agnostic.
  *
  */
class PLEVIEW_EXPORT Engine : public PropertyModel
{
    Q_OBJECT

    Q_PROPERTY(ColorMap colormap READ colorMap WRITE setColorMap STORED true)
    Q_PROPERTY(GridData2D data READ originalData WRITE setOriginalData STORED true)

public:
    enum Direction {X = 0, Y = 1}; // compatible with Pleview::Direction

    //! Constructor
    Engine();

    //! Destructor
    virtual ~Engine();

    //! Return true if non-empty data is loaded
    bool isValid() { return data2d != nullptr && data2d->size() > 0; }

    /** Current underlying data. If you need access to it you should probably do a data()->clone()
      * and update the copy via dataChanged() signal */
    shared_ptr<const GridData2D> data() const;

    /** Set underlying data. */
    void setData(shared_ptr<GridData2D> data);

    //! Color map used to plot the map
    ColorMap colorMap() { return *_colorMap; }

    //! Direct pointer to the colormap
    std::shared_ptr<ColorMap> ptr_colorMap() { return _colorMap; }

    //! Configuration of given axis
    AxisConfiguration axisConfiguration(int direction) const { return _axisConfig[direction]; }

    /** (Obsolete - use currentCrossSection() )
     * Width of cross-section along the given axis (in pixels)
     */
    int crossSectionWidth(int direction) const;

    /** (Obsolete - use currentCrossSection() )
     * Return cross-section along the given axis.
     */
    QVector<QPointF> crossSection(int axis) const;

    /** (Obsolete - use currentCrossSection() )
     * Return cross-section positions as QPointF coordinates
     */
    QPointF crossSectionPoint() const;

    //! Return info about cross-section (positions, widths, etc.)
    CrossSection currentCrossSection() const;

    //! Export data to XML. Device should be open to write.
    void save(QIODevice * device);

    //! Parse XML from device. Parameter contents specifies sections to be imported.
    void readXml(QIODevice * device);


    PlotAddonManager * pluginManager;
    DataFilterManager * transformManager;
    PlotRangesManager * plotRangesManager;

    void setSvgRenderer(SvgRenderer * renderer) { svgRenderer = renderer; }

public slots:
    void setColorMap(const ColorMap & map);
    void setCrossSection(int direction, int n, bool forceSignal = false);
    void setCrossSection(int direction, double pos, bool forceSignal = false);
    void setCrossSectionWidth(int direction, int pixels);
    void setAxisConfiguration(int direction, const AxisConfiguration & axisConfig);
    bool loadData(QIODevice * device);

private:
    void emitCrossSectionChanged(Direction direction);
    void emitDataChanged();
    void restoreSettings();

private slots:
    void prepareData();
    void addonAdded(PlotAddon *);

signals:
    void dataChanged(std::shared_ptr<const GridData2D> data);
    void crossSectionChanged(const CrossSection &);
    void crossSectionChanged(int direction, int n);
    void crossSectionChanged(int direction, double pos0, double pos1, double pos2);
    void crossSectionChanged(int direction, const QVector<double> &x, const QVector<double> &y);
    void colorMapChanged(const ColorMap &colorMap);


private:
    GridData2D & originalData() const { return *original; } // for saving framework
    void setOriginalData(const GridData2D &d) { original = d.clone(); } // for saving framework


private:
    AxisConfiguration _axisConfig[2];
    CrossSection xsec;
    std::shared_ptr<GridData2D> data2d, original;
    std::shared_ptr<ColorMap> _colorMap;
    SvgRenderer * svgRenderer;
};




#endif // Engine_H
