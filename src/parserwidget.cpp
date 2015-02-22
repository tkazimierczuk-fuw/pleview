#include <iostream>
#include <QtCore>

#include "parserwidget.h"

#include "muParserError.h"


//! Constructor
ParserInterface::ParserInterface(mu::Parser * parser) : _exprEdit(0), _errorLabel(0) {
    _parser = parser ? parser : &_localParser;
}

// try to evaluate new expression
void ParserInterface::evaluate() {
    if(!_exprEdit)
        return;

    QString expr = _exprEdit->text();
    try {
        parser()->SetExpr(expr.toStdString());
        double d = parser()->Eval();

        if(qIsNaN(d) || qIsInf(d)) {
            if(parser()->GetUsedVar().size() > 0)
                displayWarningMsg("Expression may not work for some variables");
            else
                displayErrorMsg(QString("Expression returned %1").arg(d));
        }
        else
            displayOK();
        emit(expressionChanged(expr));
    } catch (mu::Parser::exception_type &e) {
        displayErrorMsg(QString::fromStdString(e.GetMsg()));
    }
}


void ParserInterface::displayOK() {
    if(_exprEdit)
        _exprEdit->setStyleSheet("background:white");
    if(_errorLabel)
        _errorLabel->setText(QString());
}


void ParserInterface::displayWarningMsg(const QString &msg) {
    if(_errorLabel) {
        _errorLabel->setStyleSheet("color:#aaaa00; font:9px");
        _errorLabel->setText(msg);
    }
    if(_exprEdit)
        _exprEdit->setStyleSheet("background:#ffffaa");
}


void ParserInterface::displayErrorMsg(const QString &msg) {
    if(_errorLabel) {
        _errorLabel->setStyleSheet("color:red; font:9px");
        _errorLabel->setText(msg);
    }
    if(_exprEdit)
        _exprEdit->setStyleSheet("background:#ffaaaa");
}


void ParserInterface::setLabel(QLabel *label) {
    _errorLabel = label;
    evaluate();
}

void ParserInterface::setLineEdit(QLineEdit *edit) {
    if(_exprEdit)
        disconnect(this);
    if(edit)
    connect(edit, SIGNAL(editingFinished()), this, SLOT(evaluate()));
    _exprEdit = edit;
    evaluate();
}


//! Constructor
ParserGenericWidget::ParserGenericWidget(mu::Parser * parser) : _parserInterface(parser)
{    
    layout = new QVBoxLayout();
    _errorLabel = new QLabel();
    _errorLabel->setWordWrap(true);
    layout->addWidget(_errorLabel);
    setLayout(layout);

    _parserInterface.setLabel(_errorLabel);
    connect(&_parserInterface, SIGNAL(expressionChanged(QString)), this, SIGNAL(expressionEdited(QString)));
}


void ParserGenericWidget::createInputWidget(QWidget * widget) {
    layout->setMargin(0);
    layout->insertWidget(0, widget);
}


ParserWidget::ParserWidget(mu::Parser * parser) : ParserGenericWidget(parser) {
    editWidget = new QLineEdit();
    if(parser)
        editWidget->setText(QString::fromStdString(parser->GetExpr()));
    createInputWidget(editWidget);
    _parserInterface.setLineEdit(editWidget);
}


void ParserWidget::setText(const QString &text) {
    editWidget->setText(text);
    _parserInterface.evaluate();
}


QString ParserWidget::text() const {
    return editWidget->text();
}



void ParserComboWidget::setText(const QString &text) {
    _exprCombo->setItemText(_exprCombo->currentIndex(), text);
    _parserInterface.evaluate();
}


QString ParserComboWidget::text() const {
    return _exprCombo->currentText().trimmed();
}


ParserComboWidget::ParserComboWidget(mu::Parser * parser) : ParserGenericWidget(parser) {
    _exprCombo = new QComboBox();
    _exprCombo->setDuplicatesEnabled(true);
    _exprCombo->setEditable(true);
    _exprCombo->setInsertPolicy(QComboBox::NoInsert);
    _exprCombo->addItem("t");
    _exprCombo->addItem("t*2");
    _exprCombo->addItem("t*3");
    createInputWidget(_exprCombo);
    _parserInterface.setLineEdit(_exprCombo->lineEdit());
    connect(_exprCombo->lineEdit(), SIGNAL(editingFinished()), this, SLOT(updateItemText()));
}


void ParserComboWidget::updateItemText() {
    _exprCombo->setItemText(_exprCombo->currentIndex(), _exprCombo->currentText().trimmed());
}


void ParserComboWidget::setTexts(const QStringList &texts) {
    // we remove from the excess items starting from the end
    _exprCombo->clear();
    _exprCombo->addItems(texts);
}


QStringList ParserComboWidget::texts() const {
    QStringList result;
    for(int i = 0; i < _exprCombo->count(); i++)
        result.append(_exprCombo->itemText(i));
    return result;
}

void ParserComboWidget::setCurrentIndex(int index) {
    _exprCombo->setCurrentIndex(index);
    _parserInterface.evaluate();
}

