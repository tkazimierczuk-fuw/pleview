#ifndef PointPicker_H
#define PointPicker_H

#include <QtGui>

#include "data2d.h"
#include "qwt_plot_curve.h"
#include "muParser.h"
#include "plotaddons.h"
#include "parserwidget.h"



/** @class PointPicker
  *
  * @brief PointPicker provides mouse-based logging of point coordinates
  *
  *
  */
class PointPicker : public PlotAddon {
    Q_OBJECT
public:
    //! Constructor
    PointPicker();

    //! Destructor
    ~PointPicker();

    //! Show a dialog
    QWidget * controlWidget() {
        return pickedPoints;
    }

    void init(Engine * engine) {
    }

    void attach(QwtPlot *plot, PlotType type) {
    }

    //! @see Xml::serializeComponents()
    void serializeComponents(QXmlStreamWriter *writer) const;

    //! @see Xml::unserializeComponent(QXmlReader * reader)
    void unserializeComponent(QXmlStreamReader *reader);

    //! Add a point to the aproximating polyline
    void button1clicked(const QwtDoublePoint &point);

private:
    // Widgets for the interface
    QTextEdit *pickedPoints;
};



//! Factory for the PointPicker plugin
class PointPickerFactory : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Pick point coordinates"; }

    //! Tagname used in saved files
    QString tagname() { return "pointpicker"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new PointPicker();
    }
};


#endif // PointPicker_H
