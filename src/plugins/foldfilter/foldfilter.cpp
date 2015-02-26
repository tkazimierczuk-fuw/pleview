#include "foldfilter.h"
#include <iostream>


FoldFilter::FoldFilter()
    : DataFilter(new FoldFilterFactory())
{
    foldAxis = Pleview::Y;
    foldSteps = 5;
    stepBox = 0;
}


void FoldFilter::transform(GridData2D *data, VarDictionary * dict) {

    QVector<double> xValues = data->xValues();
    QVector<double> yValues = data->yValues();

    if(stepBox)
        foldSteps = stepBox->value();
    int ny = foldSteps;
    if(ny > data->yValues().size() || ny < 1)
        return;
    int nx = xValues.size();

    xValues.resize(nx);
    yValues.resize(ny);
    QVector<double> values(nx * ny); // by default filled by 0

    GridData2D tmpData(yValues, xValues, values);

    for(int ix = 0; ix < tmpData.cols(); ix++)
        for(int iy = 0; iy < tmpData.rows(); iy++) {

            double v = 0;
            int jx = ix, jy = iy;
            while(jx < data->cols() && jy < data->rows()) {
                v += data->valueAtIndex(jx, jy);
                jy += ny;
            }
            tmpData.setValueAtIndex(ix, iy, v);
        }


    *data = tmpData;
}

QWidget * FoldFilter::createControlWidget() {
    QFrame * frame = new QFrame();
    QFormLayout * layout = new QFormLayout();

    stepBox = new QSpinBox();
    stepBox->setMinimum(1);
    stepBox->setMaximum(5000);
    stepBox->setValue(foldSteps);

    layout->addRow("Fold width", stepBox);


    frame->setLayout(layout);
    return frame;
}


void FoldFilter::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "foldwidth", foldSteps);
    writer->writeEndElement();
}


void FoldFilter::unserializeFromXml(QXmlStreamReader *reader) {
    readXmlAttribute(reader, "foldwidth", &foldSteps);
    seekEndElement(reader);
}

