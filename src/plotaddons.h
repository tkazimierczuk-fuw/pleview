#ifndef PLOTADDONS_H
#define PLOTADDONS_H

#include "DLLDefines.h"
#include "qwt_plot.h"
#include "model.h"

#include "plugins.h"

class Engine;

class PLEVIEW_EXPORT PlotAddon : public QObject, public FactoryObject {
    Q_OBJECT
public:
    enum PlotType {xCrossSection, yCrossSection, Map};

    PlotAddon(Factory * factory)
        : FactoryObject(factory) {}
    virtual ~PlotAddon() {}
    virtual void init(Engine * engine) = 0;
    virtual void attach(QwtPlot *plot, PlotType type) = 0;
    virtual void focusEvent(bool hasFocus) {}
    virtual QWidget * controlWidget() = 0;

public slots:
    virtual void button1clicked(const QPointF &point) {}
    virtual void button2clicked(const QPointF &point) {}
};


class PLEVIEW_EXPORT PlotAddonFactory : public Factory
{
public:
    virtual ~PlotAddonFactory() {}
    virtual PlotAddon * instantiate() = 0;
};

#define PLOTADDONFACTORY_IID "pleview.PlotAddonFactory/0.1"
Q_DECLARE_INTERFACE(PlotAddonFactory, PLOTADDONFACTORY_IID)


class PLEVIEW_EXPORT PlotAddonManager : public QObject,
                         public FactoryObjectManager<PlotAddon, PlotAddonFactory> {
    Q_OBJECT
public:
    PlotAddonManager();

     /**
      Launch a modal dialog and load selected plugin
      @param widget to parent the dialog
      @return plugin that was selected (or 0 if cancelled)
      */
    static PlotAddonFactory * getAddonByDialog(QWidget * parent = 0);

    void addAddonByDialog(QWidget * parent = 0);

    QList<PlotAddon*> plotAddons() {
        return _objects;
    }

    void add(PlotAddon * addon) {
        FactoryObjectManager<PlotAddon, PlotAddonFactory>::add(addon);
        emit addonAdded(addon);
    }

public slots:
    void remove(PlotAddon * addon);

signals:
    void addonAdded(PlotAddon * plugin);
    void addonRemoved(PlotAddon * plugin);

private:
    class PlotAddonCatalogDialog;
};


#endif // PLOTADDONS_H
