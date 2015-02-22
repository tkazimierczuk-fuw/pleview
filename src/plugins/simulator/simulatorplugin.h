#ifndef SIMULATORPLUGIN_H
#define SIMULATORPLUGIN_H

#include <QtGui>
#include "plotaddons.h"
#include "curvegroup.h"


class SimulatorPluginObject : public PlotAddon {
    Q_OBJECT
public:
    SimulatorPluginObject();
    ~SimulatorPluginObject();

    void init(Engine *engine);
    void attach(QwtPlot *plot, PlotType type);
    QWidget * controlWidget() {
        return _frame;
    }

signals:
    void changed();


private slots:
    void getFileName();
    void runSimulation();
    void queryParameters();

private:
    void prepareUi();
    QByteArray startProcess(const QByteArray & inputData);

private:
    CurveGroup curves;

    QFrame * _frame;
    QFormLayout * layout;

    QLineEdit * fileNameEdit;

    QVector<QLabel*> _valueLabels;
    QVector<QLineEdit*> _valueEdits;

    QVector<QString> _parNames;
    QVector<double> _parValues;
};


class SimulatorPlugin : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:
    QString name() { return "Simulation Plugin"; }
    QString tagname() { return "simulationplugin"; }
    QString description();
    PlotAddon * instantiate() {
        return new SimulatorPluginObject();
    }

};

#endif // SIMULATORPLUGIN_H
