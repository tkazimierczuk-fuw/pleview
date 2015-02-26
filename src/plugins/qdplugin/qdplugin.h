#ifndef QDPlugin_H
#define QDPlugin_H

#include <QtGui>

#include "data2d.h"
#include "qwt_plot_curve.h"
#include "muParser.h"
#include "plotaddons.h"
#include "parserwidget.h"



/** @class QDPlugin
  *
  * @brief QDPlugin is a plugin to simplify analysis of magnetic field sweeps of single QDs
  *
  * QDPlugin is a plugin tailored for data featuring many sharp lines with many
  * cross-overs that make usual fitting tedious.
  *
  * User is expected to provide estimation of the line path and expected linewidth.
  * In the phase two, Pleview is automatically fitting a gaussian in a range
  * of (E-2s, E+2s), where E is indicated energy and s is the linewidth.
  */
class QDPlugin : public PlotAddon {
    Q_OBJECT
public:
    //! Constructor
    QDPlugin();

    //! Destructor
    ~QDPlugin();

    //! Initialize a plugin
    void init(Engine *engine);

    //! Attach any plot items supplied by plugin to plot widget
    void attach(QwtPlot *plot, PlotType type);

    //! Show a dialog
    QWidget * controlWidget() {
        return _frame;
    }

    //! @see Xml::serializeComponents()
    void serializeComponents(QXmlStreamWriter *writer) const;

    //! @see Xml::serializeComponents()
    void serializeAttributes(QXmlStreamWriter *writer) const;

    //! @see Xml::unserializeComponent(QXmlReader * reader)
    void unserializeComponent(QXmlStreamReader *reader);

    //! @see Xml::unserializeAttribute(QXmlStreamAttribute
    void unserializeAttribute(const QXmlStreamAttribute &attribute);

    //! Add a point to the aproximating polyline
    void button1clicked(const QPointF &point);

private slots:
    //! Reset the polyline by removing all defined points
    void clearPolyline();

    //! Run the fitting and place the result in the clipboard.
    void fit();

    //! Display usage help
    void displayHelp();


private:
    //! Toggle visibility of the polyline depending on plugin focus state
    void focusEvent(bool hasFocus);

    //! Use current polyline to map y value to x
    double approximatedX(double y) const;

    //! Pointer to the main engine. Needed in the fitting function.
    Engine * _engine;

    //! Gauss function used locally in the fitting function. Kept as an optimization.
    mu::Parser _func;

    //! The polyline nodes kept as y->x to ease sorting when adding a new node.
    QMap<double, double> polyline;

    //! The polyline curve displayed on the map
    QwtPlotCurve * polylineCurve;

    // Widgets for the interface
    QFrame *_frame;
    QPushButton * fitButton, * resetButton;
    QLineEdit *widthEdit;
    QCheckBox *outputPositionCheckBox, *outputAmplitudeCheckBox, *outputWidthCheckBox, *outputBackgroundCheckBox;
};



//! Factory for the QDPlugin plugin
class QDPluginFactory : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Follow QD lines"; }

    //! Tagname used in saved files
    QString tagname() { return "assistedfitter"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new QDPlugin();
    }
};


#endif // QDPlugin_H
