#ifndef DATAFILTERSGUI_H
#define DATAFILTERSGUI_H

#include <QtGui>

#include "model.h"
#include "datafilters.h"


/**
 * @class WidgetView
 *
 * WidgetView is a scroll area widget which content changes according
 * to current item in the given selectionModel. The model is assumed
 * to supply valid QWidget* pointer in the UserRole role.
 */
class WidgetView : public QScrollArea {
    Q_OBJECT
public:
    WidgetView(QItemSelectionModel * selectionModel, QWidget * parent = 0);

private slots:
    void currentChanged(QModelIndex current = QModelIndex());
};



/**
 * @class DataFiltersGui
 *
 * @brief A dialog to manage data filters.
 */
class DataFiltersGui : public QDialog
{
    Q_OBJECT
public:
    DataFiltersGui(DataFilterManager * manager, QWidget *parent = 0);

public slots:
    void displayTrailingAxisTransform();

private slots:
    void createFilter();
    void apply();
    void requestImport();

signals:
    void importTransform(QIODevice * device);

private:
    class ModelChangeAction;

    DataFilterManager * _manager;
    QListView * listWidget;
};


/**
 * Three types of QAction: move up/down or remove currently selected data filter.
 */
class DataFiltersGui::ModelChangeAction : public QAction {
    Q_OBJECT
public:
    enum Type {Up, Down, Delete};
    ModelChangeAction(Type type, QItemSelectionModel * selection, QObject * parent);

private slots:
    void currentChanged(QModelIndex current);
    void sendSignal();

signals:
    void swapItems(int indexA, int indexB);
    void setCurrent(QModelIndex index, QItemSelectionModel::SelectionFlags command);

private:
    Type _type;
    int row;
};


#endif // DATAFILTERSGUI_H
