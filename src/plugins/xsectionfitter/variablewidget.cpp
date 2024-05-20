#include <QtAlgorithms>
#include <iostream>

#include "variablewidget.h"
#include "signalmerger.h"


VariableWidget::VariableWidget(VariablePool * varPool, QWidget * parent) : QFrame(parent), varPool(varPool) {
    _layout = new QGridLayout();
    this->setLayout(_layout);
    connect(&_optionMapper, SIGNAL(mapped(QString)), this, SLOT(optionsRequested(QString)));
    connect(&_editMapper, SIGNAL(mapped(QString)), this, SLOT(editingFinished(QString)));
    updateVariables();

    SignalMerger *valueMerger = new SignalMerger(this), *variableMerger = new SignalMerger(this);

    connect(varPool, SIGNAL(currentValuesChanged()), valueMerger, SLOT(merge()));
    connect(valueMerger, SIGNAL(merged()), this, SLOT(updateValues()));
    connect(varPool, SIGNAL(variablesChanged()), variableMerger, SLOT(merge()));
    connect(variableMerger, SIGNAL(merged()), this, SLOT(updateVariables()));

}


VariableWidget::Item VariableWidget::findItem(const QString &name) {
    foreach(Item item, _varWidgets)
        if(item.label->text() == name)
            return item;

    return Item(0,0,0);
}


VariableWidget::Item VariableWidget::createItem(int fitvarIndex) {
    QString name = (*varPool)[fitvarIndex].name;
    QLabel * label = new QLabel(name);
    QLineEdit * edit = new QLineEdit(QString::number(varPool->currentValues()[name]));
    edit->setValidator(new QDoubleValidator(this));
    QToolButton * button = new QToolButton();
    button->setIcon(QIcon(":icons/actions/gear.svg"));

    _optionMapper.setMapping(button, name);
    connect(button, SIGNAL(clicked()), &_optionMapper, SLOT(map()));
    _editMapper.setMapping(edit, name);
    connect(edit, SIGNAL(editingFinished()), &_editMapper, SLOT(map()));

    Item item(label, edit, button);
    item.fitVarIndex = fitvarIndex;
    return item;
}


void VariableWidget::clear() {
    varPool->clear();
}


void VariableWidget::editingFinished(const QString &name) {
    Item item = findItem(name);
    if(item.isValid()) {
        varPool->setCurrentValue(name, item.edit->text().toDouble());
    }
}


void VariableWidget::copy(const Item & item) {
    QString string;
    QTextStream out(&string);
    out.setRealNumberPrecision(10);
    QVector<double> &vec = (*varPool)[item.fitVarIndex].values;
    for(int i = 0; i < vec.size(); i++) {
        out << vec[i] << "\n";
    }
    out.flush();
    QApplication::clipboard()->setText(string);
}

void VariableWidget::copyErrors(const Item & item) {

    QString string;

    QTextStream out(&string);

    out.setRealNumberPrecision(10);

    QVector<double> &vec = (*varPool)[item.fitVarIndex].errors;

    for(int i = 0; i < vec.size(); i++) {

        out << vec[i] << "\n";

    }

    out.flush();

    QApplication::clipboard()->setText(string);

}


void VariableWidget::optionsRequested(const QString &name) {
    Item item = findItem(name);
    if(!item.isValid())
        return;

    QAction copyAll(QIcon(":icons/actions/edit-copy.svg"), "Copy for all spectra", this);
    QAction copyErrorsOnly(QIcon(":icons/actions/edit-copy.svg"), "Copy uncertainity for all spectra", this);
    QAction pasteAll(QIcon(":icons/actions/edit-paste.svg"), "Paste for all spectra", this);
    QAction setAll("Set value for all spectra", this);
    QAction setValueByMouse("Set value by mouse click", this);
    QAction fixAll("Fix for all spectra", this);
    if((*varPool)[item.fitVarIndex].fixed)
        fixAll.setText("Release (unfix) for all spectra");
    QList<QAction*> list;
    list.append(&copyAll);
    list.append(&copyErrorsOnly);
    list.append(&pasteAll);
    list.append(&setAll);
    list.append(&setValueByMouse);
    list.append(&fixAll);
    QAction * action = QMenu::exec(list, item.optionButton->mapToGlobal(QPoint(0,0)), 0, item.optionButton);
    if(action == &copyAll) {
        copy(item);
    }else if(action == &copyErrorsOnly) {
        copyErrors(item);
    } else if (action == &pasteAll) {
        QMessageBox::critical(this, "Not implemented", "Feature not implemented yet");
    } else if(action == &fixAll) {
        (*varPool)[item.fitVarIndex].fixed = !(*varPool)[item.fitVarIndex].fixed;
    } else if(action == &setAll) {
        double value;
        bool ok;
        QString svalue = QInputDialog::getText(this, "Set value", "Set value for all spectra:",QLineEdit::Normal, "0", &ok);
        if(!value)
            return;
        value = svalue.toDouble(&ok);
        if(!ok)
            return;
        (*varPool)[item.fitVarIndex].values.fill(value);
        varPool->setCurrentValue((*varPool)[item.fitVarIndex].name, value); // notify about the value change
    } else if (action == &setValueByMouse) {
        mouseActiveName = item.label->text();
    }
}


// Not optimal implementation !!! Some flicker can be avoided
void VariableWidget::updateVariables() {
    foreach(Item item, _varWidgets) {
        delete item.label;
        delete item.edit;
        delete item.optionButton;
    }

    _varWidgets.clear();

    for(int i = 0; i < varPool->size(); i++) {
        Item item = createItem(i);
        _varWidgets.append(item);
    }

    qSort(_varWidgets.begin(), _varWidgets.end());

    for(int i = 0; i < _varWidgets.size(); i++) {
        _layout->addWidget(_varWidgets[i].label, i, 0);
        _layout->addWidget(_varWidgets[i].edit, i, 1);
        _layout->addWidget(_varWidgets[i].optionButton, i, 2);
    }
}


void VariableWidget::updateValues() {
    QMap<QString, double> currentValues = varPool->currentValues();
    foreach(Item item, _varWidgets)
        item.edit->setText(QString::number( currentValues.value(item.label->text(), qSNaN())));
}


QString VariableWidget::focusedParameter() const {
    foreach(Item item, _varWidgets) {
        if(item.edit->hasFocus())
            return item.label->text();
    }
    return QString();
}

