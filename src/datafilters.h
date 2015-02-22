#ifndef DATAFILTERS_H
#define DATAFILTERS_H

#include "DLLDefines.h"

#include <QtGui>

class Variable;
typedef QMap<QString, Variable> VarDictionary;

#include "data2d.h"
#include "plugins.h"
#include "variable.h"


class DataFilterFactory;
class DataFiltersGui;


class PLEVIEW_EXPORT DataFilter : public FactoryObject {
public:
    DataFilter(Factory * factory)
        : FactoryObject(factory) {}
    virtual ~DataFilter() {}
    virtual void transform(GridData2D * data, VarDictionary * dict) = 0;
    QWidget * controlWidget();

protected:
    virtual QWidget * createControlWidget();
    friend class DataFiltersGui;
    QPointer<QWidget> _widget;
};



class PLEVIEW_EXPORT DataFilterFactory : public Factory
{
public:
    virtual ~DataFilterFactory() {}
    virtual DataFilter * instantiate() = 0;
};

Q_DECLARE_INTERFACE(DataFilterFactory, "pleview.DataFilterFactory/0.1");





class PLEVIEW_EXPORT DataFilterManager : public QAbstractListModel,
                 public FactoryObjectManager<DataFilter, DataFilterFactory> {
    Q_OBJECT
public:
    DataFilterManager();

    void add(DataFilter *filter);
    void remove(DataFilter *object);
    void transform(GridData2D * data);
    QList<DataFilter*> filters();

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    int rowCount (const QModelIndex & parent = QModelIndex()) const;
    void unserializeFromXml(QXmlStreamReader *reader);

public slots:
    void notifyChange() { emit dataFiltersChanged(); }
    void swapItems(int index1, int index2);

signals:
    void dataFiltersChanged();
};


#endif // DATAFILTERS_H
