#include <QtAlgorithms>

#include "differentiate.h"
#include "parserxml.h"

Differentiate::Differentiate()
    : DataFilter(new DifferentiateFactory())
{
    _model = new DifferentialModel();
    _directioncombo = 0;
    _direction = 1;
}


Differentiate::~Differentiate() {
    delete _model;
}


void Differentiate::changeDirection() {
    if(_directioncombo)
        _direction = _directioncombo->currentIndex();
}


void Differentiate::transform(GridData2D *data, VarDictionary * dict) {
    QMap<int, double> modelData = _model->modelData();
    QMap<int, double>::const_iterator it;

    if(_direction == 0) { // horizontal differentiation
        QVector<double> buffer(data->cols());

        for(int iy = 0; iy < data->rows(); iy++) {
            buffer.fill(0);
            for(it = modelData.constBegin(); it != modelData.constEnd(); it++)
                for(int ix = 0; ix < data->cols(); ix++)
                    buffer[ix] += data->valueAtIndexBounded(ix+it.key(), iy) * it.value();
            for(int ix = 0; ix < data->cols(); ix++) {
                data->setValueAtIndex(ix, iy, buffer[ix]);
            }
        }
    } else { // vertical differentiation
        QVector<double> buffer(data->rows());

        for(int ix = 0; ix < data->cols(); ix++) {
            buffer.fill(0);
            for(it = modelData.constBegin(); it != modelData.constEnd(); it++)
                for(int iy = 0; iy < data->rows(); iy++)
                    buffer[iy] += data->valueAtIndexBounded(ix, iy+it.key()) * it.value();
            for(int iy = 0; iy < data->rows(); iy++) {
                data->setValueAtIndex(ix, iy, buffer[iy]);
            }
        }
    }

}

QWidget * Differentiate::createControlWidget() {
    QFrame * frame = new QFrame();
    QFormLayout * layout = new QFormLayout();

    _directioncombo  = new QComboBox();
    _directioncombo->addItem("Horizontal");
    _directioncombo->addItem("Vertical");
    _directioncombo->setCurrentIndex(_direction);
    connect(_directioncombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDirection()));
    layout->addRow("Differential direction", _directioncombo);

    QTableView * view = new VResizableTableView();
    view->setModel(_model);
    layout->addRow(view);

    frame->setLayout(layout);
    return frame;
}


void Differentiate::serializeAttributes(QXmlStreamWriter *writer) const {
    DataFilter::serializeAttributes(writer);
    if(_direction == 1) // see createControlWidget()
        writer->writeAttribute("direction", "vertical");
    else
        writer->writeAttribute("direction", "horizontal");
}


void Differentiate::serializeComponents(QXmlStreamWriter *writer) const {
    QMap<int, double> modelData = _model->modelData();
    QMap<int, double>::const_iterator it;
    for(it = modelData.constBegin(); it != modelData.constEnd(); it++) {
        writer->writeStartElement("ingredient");
        writeXmlAttribute(writer, "offset", it.key());
        writeXmlAttribute(writer, "factor", it.value());
        writer->writeEndElement();
    }
}


void Differentiate::unserializeFromXml(QXmlStreamReader *reader) {
    if(_model)
        _model->removeRows(0, _model->rowCount());
    DataFilter::unserializeFromXml(reader);
}


void Differentiate::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "direction") {
        if(attribute.value().compare("vertical",Qt::CaseInsensitive) == 0)
            _direction = 1; // see createControlWidget()
        else if(attribute.value().compare("horizontal",Qt::CaseInsensitive) == 0)
            _direction = 0;

        if(_directioncombo)
            _directioncombo->setCurrentIndex(_direction);
    }
    else DataFilter::unserializeAttribute(attribute);
}


void Differentiate::unserializeComponent(QXmlStreamReader *reader) {
    if(reader->name() == "ingredient") {
        int offset = 0;
        readXmlAttribute(reader, "offset", &offset);
        double factor = 0;
        readXmlAttribute(reader, "factor", &factor);
        _model->appendRow(offset, factor);
        seekEndElement(reader);
    }
    else DataFilter::unserializeComponent(reader);
}


Q_EXPORT_PLUGIN2(differential, DifferentiateFactory)
