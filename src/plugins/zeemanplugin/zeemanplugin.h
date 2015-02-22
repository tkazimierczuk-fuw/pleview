#ifndef ZEEMANPLUGIN_H
#define ZEEMANPLUGIN_H

#include <QtGui>

#include "qwt_plot_picker.h"
#include "qwt_plot_curve.h"
#include "curvegroup.h"
#include "plotaddons.h"
#include "linleastsq.h"
#include "../xsectionfitter/nonlinleastsq.h"


class ZeemanPluginObject : public PlotAddon {
    Q_OBJECT
public:

    //! Constructor
    ZeemanPluginObject();

    //! Destructor
    ~ZeemanPluginObject();

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

    void button1clicked(const QwtDoublePoint &point);
    void button2clicked(const QwtDoublePoint &point);

private:
    void prepareUi();
    void transposeIfNeeded(QPolygonF * poly);
    double calculate(double B, bool upperBranch);
    void addPoint(int branch, const QwtDoublePoint &point);

private slots:
    void reset();
    void recalculate();

signals:
    void changed();

private:
    QFrame *_frame;

    enum{ Upper = 0, Lower = 1}; // Zeeman branches
    CurveGroup _pointCurves, _modelCurves;

    QLabel *label_e0, *label_g, *label_D, *label_delta0;
    QCheckBox *allow_zero_field_splitting;

    NonLinLeastSq nonLinLeastSq;
    mu::Parser _func;
    LinLeastSq linLeastSq;

    bool _doTranspose;
};



/*!
  \brief ZeemanPlugin is used for quick determination of g-factor and
  diamagnetic shift.

  The plugin assumes that experimental data represents sweep with magnetic
  field, i.e. the axes are related to energy and magnetic field. User
  selects some points by clicking on the map. Supplied dialog displays
  current least-square fit.
*/

class ZeemanPlugin : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Zeeman Plugin"; }

    //! Tagname used in saved files
    QString tagname() { return "zeemanplugin"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new ZeemanPluginObject();
    }
};


#endif // ZEEMANPLUGIN_H
