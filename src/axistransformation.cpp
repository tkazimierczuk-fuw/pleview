#include <QtCore>

#include "axistransformation.h"


QString axisName[2] = {QString("x"), QString("y")};


AxisTransformation::AxisTransformation(QString xformula, QString yformula): DataFilter(new AxisTransformationFactory())
{
    for(int i = 0; i < 2; i++) {
        parser[i].SetExpr(axisName[i].toStdString());
        parser[i].DefineVar(axisName[i].toStdString(), &dummy);
        parser[i].DefineVar("t", &dummy);
        transformEnabled[i] = false;
    }

    if(!xformula.isEmpty()) {
        parser[0].SetExpr(xformula.toStdString());
        transformEnabled[0] = true;
    }

    if(!yformula.isEmpty()) {
        parser[1].SetExpr(yformula.toStdString());
        transformEnabled[1] = true;
    }

    enableTransformBox[0] = enableTransformBox[1] = 0;
}


void AxisTransformation::transform(GridData2D *data, VarDictionary * dict) {

    QVector<double> coordinates[2];
    coordinates[0] = data->xValues();
    coordinates[1] = data->yValues();

    for(int i = 0; i < 2; i++) {
        if(!transformEnabled)
            continue;

        try {
            for(int j = 0 ; j < coordinates[i].size(); j++) {
                dummy = coordinates[i][j];
                coordinates[i][j] = parser[i].Eval();
            }
        } catch (mu::Parser::exception_type &e) {
        }
    }

    data->setXValues(coordinates[0]);
    data->setYValues(coordinates[1]);
}


QWidget * AxisTransformation::createControlWidget() {
    QFrame * frame = new QFrame();
    QFormLayout * layout = new QFormLayout();

    for(int axis = 0; axis< 2; axis++) {
        enableTransformBox[axis] = new QCheckBox("enabled");
        enableTransformBox[axis]->setChecked(transformEnabled[axis]);
        layout->addRow("Transform " + axisName[axis] + " axis", enableTransformBox[axis]);

        formulaWidget[axis] = new ParserComboWidget(parser+axis);
        formulaWidget[axis]->setMinimumWidth(250);
        dummy = 0;

        QStringList expressions = Pleview::settings()->value(QString("transformations")+Pleview::directionString((Pleview::Direction) axis), QStringList()).toStringList();
        if(expressions.size() > 0)
            formulaWidget[axis]->setTexts(expressions);


        //transformEdit[axis]->setToolTip("An expression where 't' is original value, e.g. '1239.8/t'");

        layout->addRow(axisName[axis] + " axis transformation", formulaWidget[axis]);

        connect(enableTransformBox[axis], SIGNAL(toggled(bool)), formulaWidget[axis], SLOT(setEnabled(bool)));

        QFrame * frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        layout->addRow(frame);
    }

    connect(enableTransformBox[0], SIGNAL(toggled(bool)), this, SLOT(enableTransformX(bool)));
    connect(enableTransformBox[1], SIGNAL(toggled(bool)), this, SLOT(enableTransformY(bool)));

    frame->setLayout(layout);

    return frame;
}



void AxisTransformation::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    for(int i = 0; i < 2; i++) {
        writer->writeStartElement("axis");
        writeXmlAttribute(writer, "name", axisName[i]);
        writeXmlAttribute(writer, "enabled", transformEnabled[i]);
        writeXmlAttribute(writer, "formula", QString::fromStdString(parser[i].GetExpr()));
        writer->writeEndElement();
    }
    writer->writeEndElement();
}


void AxisTransformation::unserializeFromXml(QXmlStreamReader *reader) {  
    while(true) {
        QString childTagname = seekChildElement(reader, "axis");
        if(childTagname.isEmpty())
            return;

        QString axisname;
        bool on;
        QString formula;
        if(!readXmlAttribute(reader, "name", &axisname)
                || !readXmlAttribute(reader, "enabled", &on)
                || !readXmlAttribute(reader, "formula", &formula))
            continue;

        int axisno = (axisname == axisName[0]) ? 0 : 1;
        transformEnabled[axisno] = on;
        if(enableTransformBox[axisno] != 0)
            enableTransformBox[axisno]->setChecked(on);

        parser[axisno].SetExpr(formula.toStdString());
    }
}

