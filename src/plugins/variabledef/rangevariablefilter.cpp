#include "rangevariablefilter.h"
#include "parserxml.h"

RangeVariableFilter::RangeVariableFilter()
    : DataFilter(new RangeVariableFilterFactory())
{
    _condition.SetExpr("x > 650 and x < 680");
    _variable.SetExpr("z");
    _type = Row;
    _defType = RangedAverage;
    _varname = "parameter0";
}


void RangeVariableFilter::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "name", _varname);
    writeXmlAttribute(writer, "range", _condition);
    writeXmlAttribute(writer, "expression", _variable);
    writeXmlAttribute(writer, "type", (int) _type);
    writer->writeEndElement();
}


void RangeVariableFilter::unserializeFromXml(QXmlStreamReader *reader) {
    readXmlAttribute(reader, "name", &_varname);
    readXmlAttribute(reader, "range", &_condition);
    readXmlAttribute(reader, "expression", &_variable);
    int n = 0;
    readXmlAttribute(reader, "type", &n);
    if(n >= 0 && n <= 3) // check if casting n to Type is valid
        _type = (Type) n;
    while(seekChildElement(reader, "pasted") == "pasted") {
        editContents = reader->readElementText();
        parseEditContents();
    }
}


void RangeVariableFilter::transformPaste(GridData2D *data, VarDictionary * dict) {
    switch(_type) {
    case Col:
        dict->insert(_varname, Variable::col(valuesEditContents));
        break;
    case Row:
        dict->insert(_varname, Variable::row(valuesEditContents));
        break;
    default:
        dict->insert(_varname, Variable::simpleValue(valuesEditContents.value(0,0)));
    }
}

void RangeVariableFilter::transformAverage(GridData2D *data, VarDictionary * dict) {
    double x, y, z;
    _condition.DefineVar("x", &x);
    _variable.DefineVar("x", &x);
    _condition.DefineVar("y", &y);
    _variable.DefineVar("y", &y);
    _condition.DefineVar("z", &z);
    _variable.DefineVar("z", &z);

    QVector<double> result;
    QVector<int> count;

    if(_type == Col) {
        for(int ix = 0; ix < data->cols(); ix++)
        {
            result.append(0);
            count.append(0);
            x = data->xValues().value(ix);
            for(int iy = 0; iy < data->rows(); iy++) {
                y = data->yValues().value(iy);
                z = data->valueAtIndexBounded(ix, iy);
                if(_condition.Eval()) {
                    count.last()++;
                    result.last() += _variable.Eval();
                }
            }
            if(count.last())
                result.last() /= count.last();
        }
        dict->insert(_varname, Variable::col(result));
        return;
    }

    // type == Row || type == Value
    for(int iy = 0; iy < data->rows(); iy++)
    {
        result.append(0);
        count.append(0);
        y = data->yValues().value(iy);
        for(int ix = 0; ix < data->cols(); ix++) {
            x = data->xValues().value(ix);
            z = data->valueAtIndexBounded(ix, iy);
            if(_condition.Eval()) {
                count.last()++;
                result.last() += _variable.Eval();
            }
        }
        if(count.last())
           result.last() /= count.last();
    }

    if(_type == Value) {
        double s = 0.;
        int c = 0;
        for(int i = 0; i < result.size(); i++) {
            s += result[i] * count[i];
            c += count[i];
        }
        if(c > 0)
            s /= c;
        result.clear();
        dict->insert(_varname, Variable::simpleValue(s));
    }
    else
        dict->insert(_varname, Variable::row(result));
}

void RangeVariableFilter::transform(GridData2D *data, VarDictionary * dict) {
    if(!_widget.isNull())
        updateFromGui();

    if(_defType == RangedAverage)
        transformAverage(data, dict);
    else
        transformPaste(data, dict);
}


QWidget * RangeVariableFilter::createControlWidget() {
    QFrame * frame = new QFrame();
    layout = new QFormLayout();

    nameEdit = new QLineEdit(_varname);
    layout->addRow("Variable name", nameEdit);

    typeCombo = new QComboBox();
    typeCombo->addItem("Value averaged over Xs (i.e. f(y))");
    typeCombo->addItem("Value averaged over Ys (i.e. f(x))");
    typeCombo->addItem("Single value");
    typeCombo->setCurrentIndex((int) _type);
    layout->addRow("Variable type", typeCombo);

    defTypeCombo = new QComboBox();
    defTypeCombo->addItem("Average some data values");
    defTypeCombo->addItem("Type or paste values");
    defTypeCombo->setCurrentIndex((int) _defType);
    connect(defTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDefinitionType()));

    layout->addRow("Assigning value(s)", defTypeCombo);

    QFrame * separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine);
    layout->addRow(separator);

    conditionEdit = new ParserWidget(&_condition);
    layout->addRow("Range definition", conditionEdit);

    exprEdit = new ParserWidget(&_variable);
    layout->addRow("Value to average", exprEdit);

    valuesEdit = new QTextEdit();
    valuesEdit->setAcceptRichText(false);
    valuesEdit->setMinimumHeight(valuesEdit->minimumHeight()*1.5);
    valuesEdit->setText(editContents);
    connect(valuesEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    layout->addRow("Values", valuesEdit);

    frame->setLayout(layout);
    changeDefinitionType(); // sync the visibility of alternative widgets
    return frame;
}

/**
 * This function (slot) is called after changing the contents of the
 * text field to provide the feedback about any parsing errors
 */
void RangeVariableFilter::textChanged() {
    editContents = valuesEdit->toPlainText();
    parseEditContents();
}


void RangeVariableFilter::parseEditContents() {
    valuesEditContents.clear();
    QString text = editContents;
    QTextStream stream(&text);
    stream.skipWhiteSpace();
    double tmp;
    while(!stream.atEnd() && stream.status() == QTextStream::Ok) {
        stream >> tmp;
        stream.skipWhiteSpace();
        if(stream.status() != QTextStream::ReadCorruptData)
            valuesEditContents.append(tmp);
    }
 //   Pleview::log()->info(QString("[%1] %2 numbers recognized successfully").arg(name()).arg(valuesEditContents.size()));
}



void RangeVariableFilter::updateFromGui() {
    _varname = nameEdit->text().trimmed();
    _type = (Type) typeCombo->currentIndex();
}


/**
 * This function serves to make sure that the visibility of widgets
 * matches the currently selected definition type. It is called only when the control widget is ready.
 */
void RangeVariableFilter::changeDefinitionType() {
    if(defTypeCombo->currentIndex() == RangedAverage) {
        valuesEdit->setVisible(false);
        layout->labelForField(valuesEdit)->setVisible(false);
        conditionEdit->setVisible(true);
        layout->labelForField(conditionEdit)->setVisible(true);
        exprEdit->setVisible(true);
        layout->labelForField(exprEdit)->setVisible(true);
    } else {
        conditionEdit->setVisible(false);
        layout->labelForField(conditionEdit)->setVisible(false);
        exprEdit->setVisible(false);
        layout->labelForField(exprEdit)->setVisible(false);
        valuesEdit->setVisible(true);
        layout->labelForField(valuesEdit)->setVisible(true);
    }
}


QString RangeVariableFilter::name() {
    return QString("define ") + _varname;
}

