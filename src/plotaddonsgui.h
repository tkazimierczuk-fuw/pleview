#ifndef PLOTADDONSGUI_H
#define PLOTADDONSGUI_H

#include <QtGui>

#include "plotaddons.h"

class PlotAddonManager::PlotAddonCatalogDialog : public QDialog {
    Q_OBJECT

public:
    PlotAddonCatalogDialog(QWidget * parent,
                           const QList<PlotAddonFactory*> &pluginList);
    PlotAddonFactory * selectedPlugin();

private slots:
    void selectionChanged();

private:
    QListWidget * listWidget;
    QTextEdit *infoWidget;
    QPushButton *acceptButton;
    QVector<PlotAddonFactory*> plugins;
};


class PlotAddonsDialog : public QDialog {
    Q_OBJECT

public:
    PlotAddonsDialog(PlotAddonManager * manager, QWidget * parent);

public slots:
    virtual void button1clicked(const QPointF &point);
    virtual void button2clicked(const QPointF &point);

private slots:
    void addPlugin(PlotAddon*);
    void removePlugin(PlotAddon*);
    void selectionChanged();
    void removeButtonClicked();

signals:
    void pluginRemovalRequested(PlotAddon*);

private:
    PlotAddon * selectedPlugin();

private:
    QListWidget * listWidget;
    QStackedWidget * controlWidget;
    QPushButton *removeButton;
    QVector<PlotAddon*> plugins;
};

#endif // PLOTADDONSGUI_H
