#ifndef VARIABLEWIDGET_H
#define VARIABLEWIDGET_H

#include <QtGui>
#include "muParser.h"
#include "variable.h"
#include "model.h"


class VariableWidget : public QFrame
{
    Q_OBJECT
public:
    VariableWidget(VariablePool * varPool, QWidget * parent = 0);

    void setVariables(VariablePool * list);

    //! Returns a name of currently focused parameter or null if none is being edited
    QString focusedParameter() const;


    void clear();
    QString mouseActiveName; // TODO: remove this public variable

private slots:
    void updateVariables();
    void updateValues();
    void editingFinished(const QString &name);
    void optionsRequested(const QString &name);


private:
    class Item {
    public:
        Item(QLabel * _label = 0, QLineEdit *_edit = 0, QToolButton * _button = 0)
            : label(_label), edit(_edit), optionButton(_button) {}

        QLabel * label;
        QLineEdit * edit;
        QToolButton * optionButton;

        bool operator<(const Item &other) const {
            return label->text() < other.label->text();
        }

        bool isValid() const {
            return label != 0;
        }

        int fitVarIndex;
    };

    Item findItem(const QString &name);
    Item createItem(int fitvarIndex);
    void copy(const Item & item);

    int spectrum;
    QVector<Item>  _varWidgets;
    VariablePool * varPool;

    QGridLayout * _layout;
    QSignalMapper _optionMapper, _editMapper;
};



#endif // VARIABLEWIDGET_H
