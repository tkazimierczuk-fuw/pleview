#include <QtAlgorithms>

#include "spectrumtransformer.h"
#include "parserxml.h"

SpectrumTransformer::SpectrumTransformer()
    : DataFilter(new SpectrumTransformerFactory())
{
    _expression.SetExpr("-z");
}


void SpectrumTransformer::transform(GridData2D *data, VarDictionary * dict) {
    //if(!_widget.isNull())
    //    exprEdit->displayOK();

    try
    {

    _expression.ClearVar();

    parspace.resize(3+dict->size());
    //parspace.resize(3);
    double *x = parspace.data(), *y = parspace.data()+1, *z = parspace.data()+2;
    _expression.DefineVar("x", x);
    _expression.DefineVar("y", y);
    _expression.DefineVar("z", z);

    int counter = 3;

    QVector<QPair<double*, QVector<double> > > xpars, ypars;
    foreach(QString parname, dict->keys()) {
        Variable var = (*dict)[parname];
        double * addr = parspace.data() + counter;
        _expression.DefineVar(parname.toStdString(), addr);
        parspace[counter] = var.values.value(0);

        if(var.type == Variable::Col)
            xpars.append(QPair<double*, QVector<double> >(addr, var.values));
        else if (var.type == Variable::Row)
            ypars.append(QPair<double*, QVector<double> >(addr, var.values));
        counter++;
    }

    for(int ix = 0; ix < data->cols(); ix++) {
        *x = data->xValues().value(ix);
        for(int i = 0; i < xpars.size(); i++)
            *(xpars[i].first) = xpars[i].second.value(ix);

        for(int iy = 0; iy < data->rows(); iy++) {
            *y = data->yValues().value(iy);
            *z = data->valueAtIndexBounded(ix, iy);

            for(int i = 0; i < ypars.size(); i++)
                *(ypars[i].first) = ypars[i].second.value(iy);

            double value = _expression.Eval();
            data->setValueAtIndex(ix, iy, value);
        }
    }

  }
  catch(mu::Parser::exception_type &e)
  {
      //if(!_widget.isNull())
      //    exprEdit->displayErrorMsg(QString::fromStdString(e.GetMsg()));
  }
}

QWidget * SpectrumTransformer::createControlWidget() {
    QFrame * frame = new QFrame();
    QFormLayout * layout = new QFormLayout();
    exprEdit = new ParserWidget(&_expression);
    exprEdit->setToolTip("Enter transformation expression, e.g., \"3*z^2\".\n"
                         "You can use point coordinates (x, y), current data "
                         "value (z) and parameters \n"
                         "defined by previous transformations.");
    layout->addRow("Expression", exprEdit);

    frame->setLayout(layout);
    return frame;
}


void SpectrumTransformer::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "expression", _expression);
    writer->writeEndElement();
}


void SpectrumTransformer::unserializeFromXml(QXmlStreamReader *reader) {
    readXmlAttribute(reader, "expression", &_expression);
    seekEndElement(reader);
}


QString SpectrumTransformer::suggestedName() const {
    return QString("z:= ") + QString::fromStdString(_expression.GetExpr());
}

