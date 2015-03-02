#include "datafilters.h"


QWidget * DataFilter::controlWidget() {
    if(_widget.isNull()) {
        _widget = createControlWidget();
    }
    return _widget;
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
        filter->transform(data, &dict);
    }
    if(!_objects.isEmpty())
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
    default:
        return QVariant();
    }
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
