#ifndef CompareOther_H
#define CompareOther_H

#include <QtGui>

#include "data2d.h"
#include "qwt_plot_curve.h"
#include "plotaddons.h"
#include "doublemapitem.h"
#include <colorbutton.h>



/** @class CompareOther
  *
  */
class CompareOther : public PlotAddon {
    Q_OBJECT
public:
    //! Constructor
    CompareOther();

    //! Destructor
    ~CompareOther();

    //! Initialize a plugin
    void init(Engine *engine);

    //! Attach any plot items supplied by plugin to plot widget
    void attach(QwtPlot *plot, PlotType type);

    //! Show a dialog
    QWidget * controlWidget() {
        return _frame;
    }

    //! Some plot elements are visible only if the plugin is active
//    void focusEvent(bool hasFocus);

    //! @see Xml::serializeToXml()
    void serializeComponents(QXmlStreamWriter *writer) const;
    void serializeAttributes(QXmlStreamWriter *writer) const;

    //! @see Xml::unserializeAttribute(QXmlStreamAttribute
    void unserializeComponent(QXmlStreamReader *reader);
    void unserializeAttribute(const QXmlStreamAttribute &attribute);

private slots:
    void load();
    void changePlot(int direction, const QVector<double> &xs, const QVector<double> &ys);
    void changeCrossSection(int direction, double pos);
    void setMapVisibility(bool);

private:
    Engine * _engine; // main engine
    Engine * _other; // the other dataset

    QFrame *_frame;
    QCheckBox *_mapCheckBox;
    QLabel *_colorButtonLabel1, *_colorButtonLabel2;
    ColorButton *_colorButton1, *_colorButton2;

    QwtPlotCurve *_xsection[2];
    DoubleMapItem *_mapItem;
};



//! Factory for the XSectionFitter plugin
class CompareOtherFactory : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Compare with other"; }

    //! Tagname used in saved files
    QString tagname() { return "compareother"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new CompareOther();
    }
};


#endif // CompareOther_H
