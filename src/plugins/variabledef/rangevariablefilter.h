#ifndef RANGEVARIABLEFILTER_H
#define RANGEVARIABLEFILTER_H


#include "datafilters.h"
#include "muParser.h"
#include "parserwidget.h"

/**
 * @class RangeVariableFilter
 *
 * This data filter is used to define a variable that can be later used
 * in data transformation.
 *
 * The value of the variable can be assigned in two ways:
 * a) The variable is calculated as an average valueof given expression. The
 * average is calculated over all data pointsthat fulfill condition given in
 * form of equation taking coordinates ('x', 'y') and data intensity ('z').
 * b) The values are pasted manually in the text box.
 *
 * A variable can be either a single value or a vector (horizontal or vertical).
 */
class RangeVariableFilter : public QObject, public DataFilter {
    Q_OBJECT
public:
    enum Type {Row = 0, Col = 1, Value = 2};
    enum DefinitionType {RangedAverage = 0, PastedText = 1};
    RangeVariableFilter();
    void transform(GridData2D *data, VarDictionary * dict);

    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;
    void unserializeFromXml(QXmlStreamReader *reader);
    QString name();

protected:
    QWidget * createControlWidget();

    //! parse editContents to valuesEditContents
    void parseEditContents();

private slots:
    //! React to change in definition-type combo
    void changeDefinitionType();

    //! React to new text
    void textChanged();

private:
    void updateFromGui();
    void transformPaste(GridData2D *data, VarDictionary * dict);
    void transformAverage(GridData2D *data, VarDictionary * dict);

    QComboBox * typeCombo, * defTypeCombo;
    QLineEdit * nameEdit;
    QTextEdit * valuesEdit;
    QString editContents;
    QVector<double> valuesEditContents;
    //QCheckBox * useSecondColumnCheckbox;
    ParserWidget *conditionEdit, *exprEdit;
    QFormLayout * layout;

    Type _type;
    DefinitionType _defType;
    mu::Parser _condition, _variable;
    QString _varname;
};

class RangeVariableFilterFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new RangeVariableFilter();
    }

    //! Name of the plugin
    QString name() { return "Define a variable"; }

    //! Tagname used in saved files
    QString tagname() { return "rangevariable"; }

    //! Plugin description
    QString description() {
        return "Define a variable that can be used later, e.g. in transforming "
                "the data according do some parametrized formula.";
    }
};


#endif // RANGEVARIABLEFILTER_H
