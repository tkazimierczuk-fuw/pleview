#ifndef XSectionFitter_H
#define XSectionFitter_H

#include <QtGui>

#include "data2d.h"
#include "qwt_plot_curve.h"
#include "muParser.h"
#include "plotaddons.h"
#include "variablewidget.h"
#include "parserwidget.h"
#include "signalmerger.h"
#include "sectionedcurve.h"


/** @class XSectionFitter
  *
  * @brief XSectionFitter is a class to fit the analytical model to a single spectrum.
  *
  * XSectionFitter can automaticaly fit several spectra. The initial parameter values
  * can be either defined manually or taken from the fitting result for the previous
  * spectrum.
  *
  * Note: direction==Pleview::X means that we are fitting column after column (in terms of 2D map).
  */
class XSectionFitter : public PlotAddon {
    Q_OBJECT
public:
    //! Constructor
    XSectionFitter();

    //! Destructor
    ~XSectionFitter();

    //! Initialize a plugin
    void init(Engine *engine);

    //! Attach any plot items supplied by plugin to plot widget
    void attach(QwtPlot *plot, PlotType type);

    //! Show a dialog
    QWidget * controlWidget() {
        return _frame;
    }

    //! Some plot elements are visible only if the plugin is active
    void focusEvent(bool hasFocus);

    //! @see Xml::serializeToXml()
    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;

    //! @see Xml::unserializeFromXml()
    void unserializeFromXml(QXmlStreamReader *reader);

    //! @see Xml::unserializeComponent(QXmlReader * reader)
    void unserializeComponent(QXmlStreamReader *reader);

    //! @see Xml::unserializeAttribute(QXmlStreamAttribute
    void unserializeAttribute(const QXmlStreamAttribute &attribute);

    //! Add a new variable. Returns address of the variable that can be used in the parser.
    double * addVariable(const QString &);

    //! Assign x value of a clicked point to current parameter
    void button1clicked(const QwtDoublePoint &point);

    //! Assign y value of a clicked point to current parameter
    void button2clicked(const QwtDoublePoint &point);

private slots:
    //! Change the experimental data
//    void updateData(const Data2D * data);

    //! Recalculate data for the model cross-section curve
    void recalculateCurve();

    double fit();
    void fitMulti(int forward = 1);


    void setDirection(Pleview::Direction direction);

    //! Sets direction according to the state of plugin's UI
    void setDirection();

private:
    double fit(const QVector<Point3D> &experimentalData);
    QVector<Point3D> filteredData();

    Engine * _engine;
    mu::Parser _func, _range;
    double _range_xy[2], _range_z;

    QMap<QString, double *> _varAddr;
    VariablePool _varPool;

    QFrame *_frame;
    SignalMerger * merger; // to merge signals invoking recalculateCurve() slot
    ParserWidget *_funcEdit, *_rangeEdit;
    VariableWidget * _varWidget;
    QRadioButton * _xDirectionRadio, * _yDirectionRadio;
    QPushButton *_fitButton, *_fitForwardButton, *_fitBackwardButton;
    QComboBox * _typeCombo;

    Pleview::Direction _direction;
    SectionedCurve *_curve[2], *_curve_rest[2];
    QwtPlotCurve *_fitPoints[2];
};



//! Factory for the XSectionFitter plugin
class XSectionFitterFactory : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Fit cross-section"; }

    //! Tagname used in saved files
    QString tagname() { return "spectrumfit"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new XSectionFitter();
    }
};


#endif // XSectionFitter_H
