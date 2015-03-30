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
        : FactoryObject(factory), _enabled(true) {}
    virtual ~DataFilter() {}
    virtual void transform(GridData2D * data, VarDictionary * dict) = 0;
    QWidget * controlWidget();

    void setEnabled(bool ok);
    bool enabled() const;



protected:
    virtual QWidget * createControlWidget();
    friend class DataFiltersGui;
    QPointer<QWidget> _widget;
    bool _enabled;
};



class PLEVIEW_EXPORT DataFilterFactory : public Factory
{
public:
    virtual ~DataFilterFactory() {}
    virtual DataFilter * instantiate() = 0;
};

Q_DECLARE_INTERFACE(DataFilterFactory, "pleview.DataFilterFactory/0.1");




class AxisTransformation;

/**
 * @brief The DataFilterManager class
 *
 */
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
    bool setData(const QModelIndex & index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;
    int rowCount (const QModelIndex & parent = QModelIndex()) const;
    void fromXml(const QDomNode &node) override;
    void toXml(QDomNode &node) const override;

public slots:
    void notifyChange() { emit dataFiltersChanged(); }
    void swapItems(int index1, int index2);

signals:
    void dataFiltersChanged();
    void dataChanged(const GridData2D*);
};


#endif // DATAFILTERS_H
