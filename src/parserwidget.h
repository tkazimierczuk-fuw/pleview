#ifndef PARSERWIDGET_H
#define PARSERWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPointer>
#include <QVBoxLayout>

#include "DLLDefines.h"
#include "muParser.h"

class SilentParser : public mu::Parser {
public:
    double Eval() {
        try {
            return mu::Parser::Eval();
        } catch (mu::Parser::exception_type &e) {
            return 0;
        }
    }


    double Eval(bool * ok) {
        if(ok)
            *ok = true;
        try {
            return mu::Parser::Eval();
        } catch (mu::Parser::exception_type &e) {
            if(ok)
                *ok = false;
            return 0;
        }
    }
};


/**
  * @class ParserInterface
  *
  * @brief A controller providing evaluation of expression entered in QLineEdit
  *
  * ParserInterface uses a muParser object given through the constructor. If no parser was supplied,
  * the ParserWidget will work on its own local parser object. In the latter case, the parser will be
  * owned by the ParserInterface.
  *
  * ParserInterface works on a  QLineEdit object and a QLabel object. Once QLineEdit text is changed,
  * the expression is being evaluated. In case of error or warning, the background of QLineEdit
  * changes color and the reason is printed on QLabel.
  * Not specifying QLineEdit or QLabel does not lead to error, but does not make much sense.
  */

class PLEVIEW_EXPORT ParserInterface : public QObject {
    Q_OBJECT
public:
    //! Constructor
    ParserInterface(mu::Parser * parser = 0);

    //! Pointer to underlying parser. This pointer can be invalidated on destruction of the widget.
    mu::Parser * parser() {
        return _parser;
    }

    //! Set label to display error
    void setLabel(QLabel * label);

    //! Set line edit with expression
    void setLineEdit(QLineEdit * edit);

signals:
    void expressionChanged(QString expr);

public slots:
    //! Try to evaluate the expression and change the color depending on the result
    void evaluate();

protected:
    //! Display error message
    void displayErrorMsg(const QString &msg);

    //! Display warning message
    void displayWarningMsg(const QString &msg);

    //! Clear error or warning message
    void displayOK();


    mu::Parser * _parser; // underlying parser object
    mu::Parser _localParser; // fallback parser owned by the ParserInterface

    QPointer<QLineEdit> _exprEdit; // widget to edit the parser expression
    QPointer<QLabel> _errorLabel;
};


//! Common elements of ParserWidget and ParserComboWidget
class PLEVIEW_EXPORT ParserGenericWidget : public QWidget {
    Q_OBJECT
public:
    //! Constructor
    explicit ParserGenericWidget(mu::Parser * parser = 0);


    //! Pointer to underlying parser. This pointer may be invalidated on destruction of the widget.
    mu::Parser * parser() {
        return _parserInterface.parser();
    }

    //! Set parser expression
    virtual void setText(const QString & text) {}

    //! Get parser expression
    virtual QString text() const { return QString(); }


signals:
    //! Signal emitted after the edit field lost focus
    void expressionEdited(const QString &);


protected:
    /**
      * This function should be called only once, preferentially from the
      * constructor of the derived class.
      */
    void createInputWidget(QWidget * widget);

    ParserInterface _parserInterface;
    QLabel * _errorLabel;
    QVBoxLayout * layout;
};



/**
  * @class ParserWidget
  *
  * @brief A widget designed for editing parsed expression.
  *
  * ParserWidget works on a muParser object given through the constructor. If no parser was supplied,
  * the ParserWidget will work on its own local parser object.
  *
  * The widget paints QLineEdit with current parser expression. The lineedit does not provide any
  * validator, but insted tries to evaluate the expression on the exit and reports any errors.
  *
  * The widget does not delete the supplied parser object in destructor.
  *
  */
class PLEVIEW_EXPORT ParserWidget : public ParserGenericWidget
{
public:
    ParserWidget(mu::Parser * parser = 0);

    //! Set parser expression
    void setText(const QString & text);

    //! Get parser expression
    QString text() const;

private:
    QLineEdit * editWidget; // widget to edit the parser expression
};



class PLEVIEW_EXPORT ParserComboWidget : public ParserGenericWidget
{
    Q_OBJECT
public:
    ParserComboWidget(mu::Parser * parser = 0);

    //! Set parser expression
    void setText(const QString & text);

    //! Get parser expression
    QString text() const;

    //! Get all texts (usually for storing the settings)
    QStringList texts() const;

    //! Set all texts (usually when loading stored settings)
    void setTexts(const QStringList &texts);

    //! Changes current index
    void setCurrentIndex(int index);

private slots:
    void updateItemText();

private:
    QComboBox * _exprCombo; // widget to edit the parser expression
};




#endif // PARSERWIDGET_H
