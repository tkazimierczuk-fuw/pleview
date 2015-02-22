#ifndef DIFFERENTIALMODEL_H
#define DIFFERENTIALMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QPair>
#include <QTableView>
#include <QContextMenuEvent>


class DifferentialModel : public QAbstractTableModel
{
public:
    DifferentialModel(QObject * parent = 0);

    //! Number of rows
    int rowCount(const QModelIndex & parent = QModelIndex()) const {
        return parent.isValid() ? 0 : _data.size();
    }

    //! There are two columns
    int columnCount(const QModelIndex & parent = QModelIndex()) const {
        return parent.isValid() ? 0 : 2;
    }

    //! Model data
    QVariant data(const QModelIndex &index, int role) const;

    //! Header description
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    //! Flags for model data
    Qt::ItemFlags flags(const QModelIndex & index) const;

    //! Callback for data editing
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    //! Reimplementation of inserted rows
    bool insertRows (int row, int count, const QModelIndex & parent = QModelIndex());

    //! Appends row with a given data
    void appendRow(int offset, double factor);

    //! Reimplementation of row deleting
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    //! Get data. Adds a duplicate fields.
    QMap<int, double> modelData() const;

private:
    QVector<QPair<int, double> > _data;
};



class VResizableTableView: public QTableView {
protected:
    void contextMenuEvent(QContextMenuEvent *event);
};


#endif // DIFFERENTIALMODEL_H
