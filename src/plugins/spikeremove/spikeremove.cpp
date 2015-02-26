#include "spikeremove.h"


SpikeRemove::SpikeRemove() : DataFilter(new SpikeRemoveFactory()) {
    thresholdEdit = 0;
    thresholdValue = 1000.;
}


void SpikeRemove::transform(GridData2D *data, VarDictionary *dict) {
    QVector<QPoint> points;
    QVector<double> newValues;

    if(thresholdEdit)
        thresholdValue = thresholdEdit->text().toDouble();

    for(int ix = 0; ix < data->cols(); ix++) {
        for(int iy = 0; iy < data->rows(); iy++) {

            double sum = 0, value = data->valueAtIndex(ix, iy);
            for(int dx = -1; dx < 2; dx++)
                for(int dy = -1; dy < 2; dy++)
                    sum += data->valueAtIndexBounded(ix+dx, iy+dy);

            double avr = (sum-value) / 8;
            if(value > avr + thresholdValue) {
                points.append(QPoint(ix, iy));
                newValues.append(avr);
            }
        }
    }

    for(int i = 0; i < points.size(); i++)
        data->setValueAtIndex(points[i].x(), points[i].y(), newValues[i]);
}


QWidget * SpikeRemove::createControlWidget() {
    QFrame * frame = new QFrame();
    QFormLayout * layout = new QFormLayout();
    QString toolTip = "Maximum possible difference between a point and averaged neighbors";
    QLabel * label = new QLabel("Threshold value");
    label->setToolTip(toolTip);
    thresholdEdit = new QLineEdit(QString::number(thresholdValue));
    thresholdEdit->setValidator(new QDoubleValidator(0,1e100, 100, frame));
    thresholdEdit->setToolTip(toolTip);
    layout->addRow(label, thresholdEdit);
    frame->setLayout(layout);
    return frame;
}


void SpikeRemove::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
   writer->writeStartElement(tagName);
   if(thresholdEdit)
     thresholdValue = thresholdEdit->text().toDouble();
   writeXmlAttribute(writer, "threshold", thresholdValue);
   writer->writeEndElement();
}


void SpikeRemove::unserializeFromXml(QXmlStreamReader *reader) {
    readXmlAttribute(reader, "threshold", &thresholdValue);
    if(thresholdEdit)
        thresholdEdit->setText(QString::number(thresholdValue));
    seekEndElement(reader);
}


