#include "differentialmodel.h"
#include <QAction>
#include <QMenu>

DifferentialModel::DifferentialModel(QObject * parent) : QAbstractTableModel(parent)
{
    _data.append(QPair<int, double>(0,-1));
    _data.append(QPair<int, double>(1, 1));
}



QVariant DifferentialModel::data(const QModelIndex &index, int role) const {
    if(index.row() < 0 || index.row() >= _data.size() || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column()) {
    case 0:
        return _data[index.row()].second;
    case 1:
        return _data[index.row()].first;
    default:
        return QVariant();
    }
}


bool DifferentialModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(index.row() < 0 || index.row() >= _data.size() || role != Qt::EditRole)
        return false;

    switch(index.column()) {
        bool ok;
        double dval;
        int ival;
    case 0:
        dval = value.toDouble(&ok);
        if(ok)
            _data[index.row()].second = dval;
        return ok;
    case 1:
        ival = value.toDouble(&ok);
        if(ok)
            _data[index.row()].first = ival;
        return ok;
    default:
        return false;
    }
}


QVariant DifferentialModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical)
        return section;
    switch(section) {
    case 0:
        return "Weight";
    case 1:
        return "Spectrum offset";
    default:
        return QVariant();
    }
}


Qt::ItemFlags DifferentialModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}


bool DifferentialModel::insertRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid())
        return false;

    beginInsertRows(parent, row, row+count - 1);
    if(row < 0)
        row = 0;
    if(row > rowCount())
        row = rowCount();
    _data.insert(row, count, QPair<int,double>(0,0));
    endInsertRows();
    return true;
}


bool DifferentialModel::removeRows(int row, int count, const QModelIndex &parent) {
    if(parent.isValid())
        return false;

    beginRemoveRows(parent, row, row+count - 1);
    _data.remove(row, count);
    endRemoveRows();
    return true;
}


QMap<int, double> DifferentialModel::modelData() const {
    QMap<int, double> result;
    for(int i = 0; i < _data.size(); i++)
        result[_data[i].first] = _data[i].second + result.value(_data[i].first, 0);
    foreach(int key, result.keys())
        if(result[key] == 0.)
            result.remove(key);
    return result;
}


void DifferentialModel::appendRow(int offset, double factor) {
    QModelIndex parent = QModelIndex();
    beginInsertRows(parent, rowCount(), rowCount());
    _data.append(QPair<int,double>(offset,factor));
    endInsertRows();
}


void VResizableTableView::contextMenuEvent(QContextMenuEvent *event) {
    int row = this->rowAt(event->pos().y());

    QAction insertAction("Insert row", this);
    QAction removeAction("Remove row", this);
    QMenu menu(this);
    menu.addAction(&insertAction);

    if(row >= 0) {
        menu.addAction(&removeAction);
    }



    QAction * chosen = menu.exec(event->globalPos());
    if(chosen == &insertAction)
        model()->insertRow(row >= 0 ? row : model()->rowCount());
    else if (chosen == &removeAction)
        model()->removeRow(row);
}
