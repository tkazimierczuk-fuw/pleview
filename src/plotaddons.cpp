#include "plotaddons.h"
#include "plotaddonsgui.h"

PlotAddonManager::PlotAddonManager() {
}


void PlotAddonManager::remove(PlotAddon * addon) {
    _objects.removeAll(addon);
    emit addonRemoved(addon);
    delete addon;
}


PlotAddonFactory * PlotAddonManager::getAddonByDialog(QWidget * parent) {
    PlotAddonCatalogDialog * dialog =
            new PlotAddonCatalogDialog(parent, availableFactories());
    if(dialog->exec() == QDialog::Accepted)
        return dialog->selectedPlugin();
    else
        return 0;
}


void PlotAddonManager::addAddonByDialog(QWidget * parent) {
    PlotAddonFactory * factory = getAddonByDialog(parent);
    if(factory)
        FactoryObjectManager<PlotAddon, PlotAddonFactory>::add(factory);
}
