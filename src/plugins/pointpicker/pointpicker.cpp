#include "defs.h"
#include "pointpicker.h"
#include "qwt_symbol.h"
#include "pleview.h"

#include <QtAlgorithms>


PointPicker::PointPicker()
    : PlotAddon(new PointPickerFactory())
{
    // initialize interface
    pickedPoints = new QTextEdit();
}


PointPicker::~PointPicker() {
}


//! @see Xml::serializeComponents()
void PointPicker::serializeComponents(QXmlStreamWriter * writer) const {
    writer->writeTextElement("text", pickedPoints->toPlainText());
}



//! @see Xml::unserializeComponent()
void PointPicker::unserializeComponent(QXmlStreamReader *reader) {
    if(reader->name() == "text")
        pickedPoints->setPlainText(reader->readElementText(QXmlStreamReader::SkipChildElements));
    else
        PlotAddon::unserializeComponent(reader);
}


void PointPicker::button1clicked(const QwtDoublePoint &point) {
    pickedPoints->append(QString("%1\t%2").arg(point.x()).arg(point.y()));
}



//! Plugin description
QString PointPickerFactory::description() {
    return "Use this plugin when you need to write down coordinates of some characteristic points on the map."
            "Just [Ctrl]+click at the interesting point.";
}

Q_EXPORT_PLUGIN2(pointpicker, PointPickerFactory)
