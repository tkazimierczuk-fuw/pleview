#include "transposefilter.h"
#include "data2d.h"


TransposeFilter::TransposeFilter()
    : DataFilter(new TransposeFilterFactory())
{
}


void TransposeFilter::transform(GridData2D *data, VarDictionary * dict) {
    QVector<double> newValues;
    newValues.reserve(data->rows() * data->cols());


    for(int ix = 0; ix < data->cols(); ix++)
        for(int iy = 0; iy < data->rows(); iy++)
            newValues.append(data->valueAtIndex(ix, iy));

    *data = GridData2D(data->xValues(), data->yValues(), newValues);
}

QWidget * TransposeFilter::createControlWidget() {
    QFrame * frame = new QFrame();
    return frame;
}


void TransposeFilter::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    writer->writeEndElement();
}


void TransposeFilter::unserializeFromXml(QXmlStreamReader *reader) {
    seekEndElement(reader);
}


