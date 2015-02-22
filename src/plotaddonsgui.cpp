#include "plotaddonsgui.h"

PlotAddonManager::PlotAddonCatalogDialog
        ::PlotAddonCatalogDialog(QWidget *parent,
                                 const QList<PlotAddonFactory*> &pluginList)
            : QDialog(parent)
{
    QLayout *layout = new QVBoxLayout();
    QBoxLayout *hlayout = new QHBoxLayout();

    listWidget = new QListWidget();
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    foreach(PlotAddonFactory * plugin , pluginList) {
        listWidget->addItem(plugin->name());
        plugins.append(plugin);
    }
    hlayout->addWidget(listWidget,1);

    infoWidget = new QTextEdit();
    infoWidget->setReadOnly(true);
    hlayout->addWidget(infoWidget,2);

    layout->addItem(hlayout);

    QDialogButtonBox * buttons = new QDialogButtonBox();
    acceptButton = buttons->addButton("Select plugin", QDialogButtonBox::AcceptRole);
    acceptButton->setEnabled(false);
    buttons->addButton(QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttons);

    this->setLayout(layout);

    connect(listWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(selectionChanged()));
    setWindowTitle("Select a plugin");
}

PlotAddonFactory * PlotAddonManager::PlotAddonCatalogDialog::selectedPlugin() {
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    if(selectedItems.size() == 1) {
        int row = listWidget->row(selectedItems.first());
        return plugins.value(row);
    }
    return 0;
}

void PlotAddonManager::PlotAddonCatalogDialog::selectionChanged() {
    PlotAddonFactory * plugin = selectedPlugin();
    acceptButton->setEnabled(plugin);
    if(plugin)
        infoWidget->setText(plugin->description());
    else
        infoWidget->clear();
}



PlotAddonsDialog::PlotAddonsDialog(PlotAddonManager * manager, QWidget * parent)
    : QDialog(parent) {

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout();
    listWidget = new QListWidget();
    listWidget->setMaximumWidth(150);
    listWidget->setMinimumWidth(150);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    hlayout->addWidget(listWidget);
    connect(listWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(selectionChanged()));

    controlWidget = new QStackedWidget();
    controlWidget->addWidget(new QFrame());
    hlayout->addWidget(controlWidget);

    layout->addLayout(hlayout);

    QDialogButtonBox * box = new QDialogButtonBox();
    QPushButton * closeButton = box->addButton(QDialogButtonBox::Close);
    closeButton->setAutoDefault(false);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    removeButton = box->addButton("Remove plugin",QDialogButtonBox::DestructiveRole);
    removeButton->setAutoDefault(false);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));

    layout->addWidget(box);

    this->setLayout(layout);
    setWindowTitle("Active plugins");
    selectionChanged();

    // TODO: query existing plugins
    connect(manager, SIGNAL(addonAdded(PlotAddon*)),
            this, SLOT(addPlugin(PlotAddon*)));
    connect(manager, SIGNAL(addonRemoved(PlotAddon*)),
            this, SLOT(removePlugin(PlotAddon*)));
    connect(this, SIGNAL(pluginRemovalRequested(PlotAddon*)),
            manager, SLOT(remove(PlotAddon*)));
}


void PlotAddonsDialog::addPlugin(PlotAddon *plugin) {
    controlWidget->addWidget(plugin->controlWidget());
    plugins.append(plugin);
    QListWidgetItem * item = new QListWidgetItem(plugin->name());
    listWidget->addItem(item);
    listWidget->setItemSelected(item, true);
}


void PlotAddonsDialog::removePlugin(PlotAddon *plugin) {
    controlWidget->removeWidget(plugin->controlWidget());
    for(int i = 0; i < plugins.size(); i++) {
        if(plugins[i] == plugin) {
            QListWidgetItem *item = listWidget->takeItem(i);
            if(item)
                delete item;
            plugins.remove(i);
            return;
        }
    }
}


PlotAddon * PlotAddonsDialog::selectedPlugin() {
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    if(selectedItems.size() == 1) {
        int row = listWidget->row(selectedItems.first());
        PlotAddon * addon = plugins.value(row);
        if(controlWidget->currentWidget() != addon->controlWidget()) {
            foreach(PlotAddon * oldAddon, plugins)
                if(oldAddon->controlWidget() == controlWidget->currentWidget())
                    oldAddon->focusEvent(false);
            controlWidget->setCurrentWidget(addon->controlWidget());
            addon->focusEvent(true);
        }
        return addon;
    }
    controlWidget->setCurrentIndex(0);
    return 0;
}


void PlotAddonsDialog::selectionChanged() {
    PlotAddon * plugin = selectedPlugin();
    removeButton->setEnabled(plugin);
}


void PlotAddonsDialog::removeButtonClicked() {
    PlotAddon * plugin = selectedPlugin();
    if(plugin)
        emit pluginRemovalRequested(plugin);
}

void PlotAddonsDialog::button1clicked(const QPointF &point) {
    PlotAddon * addon = selectedPlugin();
    if(addon)
        addon->button1clicked(point);
}

void PlotAddonsDialog::button2clicked(const QPointF &point) {
    PlotAddon * addon = selectedPlugin();
    if(addon)
        addon->button2clicked(point);
}
