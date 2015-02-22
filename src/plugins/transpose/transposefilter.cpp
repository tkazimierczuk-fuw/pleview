#include "transposefilter.h"


TransposeFilter::TransposeFilter()
    : DataFilter(new TransposeFilterFactory())
{
}


void TransposeFilter::transform(GridData2D *data, VarDictionary * dict) {
    GridData2D * backup = data->clone();
    data->setXValues(backup->yValues());
    data->setYValues(backup->xValues());

    for(int ix = 0; ix < data->cols(); ix++)
        for(int iy = 0; iy < data->rows(); iy++)
            data->setValueAtIndex(ix, iy, backup->valueAtIndex(iy,ix));

    delete backup;
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


Q_EXPORT_PLUGIN2(tranpose, TransposeFilterFactory)
