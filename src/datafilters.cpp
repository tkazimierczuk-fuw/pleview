#include "datafilters.h"
#include "axistransformation.h"


QWidget * DataFilter::controlWidget() {
    if(_widget.isNull()) {
        _widget = createControlWidget();
    }
    return _widget;
}

void DataFilter::setEnabled(bool ok) {
    _enabled = ok;
}

bool DataFilter::enabled() const {
    return _enabled;
}


QWidget * DataFilter::createControlWidget() {
    return new QFrame();
}


DataFilterManager::DataFilterManager() {
}


void DataFilterManager::transform(GridData2D *data) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    VarDictionary dict;

    foreach(DataFilter * filter, _objects) {
        if(filter->enabled())
            filter->transform(data, &dict);
    }

    //if(!_objects.isEmpty())

    data->dataChangingFinished();
    emit dataChanged(data);
    QApplication::restoreOverrideCursor();
}


void DataFilterManager::add(DataFilter *filter) {
    beginResetModel();
    FactoryObjectManager<DataFilter, DataFilterFactory>::add(filter);
    endResetModel();
    emit dataFiltersChanged();
}


void DataFilterManager::remove(DataFilter *filter) {
    beginResetModel();
    FactoryObjectManager<DataFilter, DataFilterFactory>::remove(filter);
    endResetModel();
    emit dataFiltersChanged();
}


void DataFilterManager::swapItems(int index1, int index2) {
    if(index2 < 0 || index2 >= _objects.size()) {
        remove(_objects.value(index1, 0));
        return;
    }

    beginResetModel();
    if(index1 >= 0 && index1 < _objects.size()) // index2 already checked
        _objects.swap(index1, index2);
    endResetModel();
}


QList<DataFilter*> DataFilterManager::filters() {
    return _objects;
}


QVariant DataFilterManager::data(const QModelIndex & index, int role) const {
    DataFilter * filter =  _objects.value(index.row());

    if(!filter)
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return filter->name();
    case Qt::UserRole:
        return qVariantFromValue(filter->controlWidget());
    case Qt::CheckStateRole:
        return filter->enabled() ? Qt::Checked : Qt::Unchecked;
    default:
        return QVariant();
    }
}


bool DataFilterManager::setData(const QModelIndex &index, const QVariant &value, int role) {
    DataFilter * filter = _objects.value(index.row());
    if(!filter)
        return false;

    if(role == Qt::CheckStateRole) {
        filter->setEnabled(value.toBool());
        emit QAbstractListModel::dataChanged(index, index);
        return true;
    }
    else if(role == Qt::EditRole) {
        filter->setName(value.toString());
        emit QAbstractListModel::dataChanged(index, index);
        return true;
    }

    return false;
}


Qt::ItemFlags DataFilterManager::flags(const QModelIndex &index) const {
    return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


int DataFilterManager::rowCount(const QModelIndex & parent) const {
    return parent.isValid() ? 0 : _objects.size();
}


void DataFilterManager::unserializeFromXml(QXmlStreamReader *reader) {
    blockSignals(true);
    FactoryObjectManager<DataFilter, DataFilterFactory>::unserializeFromXml(reader);
    blockSignals(false);
    notifyChange();
}
