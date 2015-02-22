#include "defs.h"
#include "compareother.h"
#include "qwt_symbol.h"
#include "pleview.h"
#include "log.h"

#include <QtAlgorithms>




CompareOther::CompareOther()
    : PlotAddon(new CompareOtherFactory()), _engine(0), _other(0)
{   
    if(!_other)
        _other = new Engine();

    _frame = new QFrame();
    QFormLayout * layout = new QFormLayout();
    _frame->setLayout(layout);

    QPushButton * _loadButton = new QPushButton("Load data");
    _loadButton->setAutoDefault(false);
    layout->addRow(_loadButton);
    connect(_loadButton, SIGNAL(clicked()), this, SLOT(load()));


    for(int i = 0; i < 2; i++) {
        _xsection[i] = new QwtPlotCurve();
        _xsection[i]->setPen(QPen(QColor::fromRgb(255, 128, 0)));
    }

    _mapItem = DoubleMapItem::createDoubleMapItem();
    _mapItem->setItemAttribute(QwtPlotItem::AutoScale, false);


    _mapCheckBox = new QCheckBox();
    _mapCheckBox->setChecked(false);
    layout->addRow("Override map", _mapCheckBox);
    connect(_mapCheckBox, SIGNAL(toggled(bool)), this, SLOT(setMapVisibility(bool)));

    _colorButton1 = new ColorButton(QColor::fromRgb(255, 55, 0));
    _colorButtonLabel1 = new QLabel("Color 1");
    layout->addRow(_colorButtonLabel1, _colorButton1);
    connect(_colorButton1, SIGNAL(valueChanged(QColor)), _mapItem, SLOT(setColor0(QColor)));

    _colorButton2 = new ColorButton(QColor::fromRgb(0, 200, 255));
    _colorButtonLabel2 = new QLabel("Color 2");
    layout->addRow(_colorButtonLabel2, _colorButton2);
    connect(_colorButton2, SIGNAL(valueChanged(QColor)), _mapItem, SLOT(setColor1(QColor)));

    setMapVisibility(false);

    // connect signals to slots
    connect(_other, SIGNAL(crossSectionChanged(int,QVector<double>,QVector<double>)), this, SLOT(changePlot(int, const QVector<double> &, const QVector<double> &)));
}


void CompareOther::setMapVisibility(bool visible) {
    _mapItem->setVisible(visible);
    _colorButtonLabel1->setEnabled(visible);
    _colorButtonLabel2->setEnabled(visible);
    _colorButton1->setEnabled(visible);
    _colorButton2->setEnabled(visible);
    if(_mapItem->plot())
        _mapItem->plot()->replot();
}



void CompareOther::load() {
    QString fileName = QFileDialog::getOpenFileName(_frame, "Load data", "", "PLVZ file (*.plvz)");
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        Pleview::log()->warning("Cannot access file " + fileName);
        return;
    }

    if(!_other->loadData(&file)) {
        Pleview::log()->warning("Error reading file " + fileName);
        return; // opened file will be closed automatically
    }
    _mapItem->setSecondData(_other->data());
}



CompareOther::~CompareOther() {
    for(int i = 0; i < 2; i++) {
        _xsection[i]->detach();
        delete _xsection[i];
    }
    _mapItem->detach();
    delete _mapItem;
    delete _other;
}


//! Initialize a plugin
void CompareOther::init(Engine *engine) {
    _engine = engine;
    connect(_engine, SIGNAL(crossSectionChanged(int,double,double,double)), this, SLOT(changeCrossSection(int,double)));
    _mapItem->setColorMap(engine->colorMap());
    _mapItem->setData(_engine->data());
    connect(_engine, SIGNAL(colorMapChanged(ColorMap)), _mapItem, SLOT(setColorMap(ColorMap)));
    connect(_engine, SIGNAL(dataChanged(const GridData2D*)), _mapItem, SLOT(setData(const GridData2D*)));
}


//! Attach any plot items supplied by plugin to plot widget
void CompareOther::attach(QwtPlot *plot, PlotType type) {
    switch(type) {
    case Pleview::X:
    case Pleview::Y:
        _xsection[type^1]->attach(plot);
        break;
    case PlotAddon::Map:
        _mapItem->attach(plot);
        break;
    }
}


//! @see Model::serializeToXml()
void CompareOther::serializeComponents(QXmlStreamWriter *writer) const {
    _other->data()->serializeToXml(writer, "data");
}

void CompareOther::serializeAttributes(QXmlStreamWriter *writer) const {
    writeXmlAttribute(writer, "override", _mapCheckBox->isChecked());
    writeXmlAttribute(writer, "color1", _colorButton1->color().name());
    writeXmlAttribute(writer, "color2", _colorButton2->color().name());
}


void CompareOther::unserializeComponent(QXmlStreamReader *reader) {
    if(reader->name() == "data") {
        GridData2D * data = new GridData2D();
        data->unserializeFromXml(reader);
        _other->setData(data);
        _mapItem->setSecondData(_other->data());
    }
    else PlotAddon::unserializeComponent(reader);

}


//! @see Xml::unserializeAttribute()
void CompareOther::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "override") {
        bool ok = false;
        readXmlAttribute(attribute, &ok);
        _mapCheckBox->setChecked(ok);

    }
    else if (attribute.name() == "color1") {
        QColor color(attribute.value().toString());
        _colorButton1->setColor(color);
    }
    else if (attribute.name() == "color2") {
        QColor color(attribute.value().toString());
        _colorButton2->setColor(color);
    }
    else PlotAddon::unserializeAttribute(attribute);
}


void CompareOther::changePlot(int direction, const QVector<double> &xs, const QVector<double> &ys) {
    _xsection[direction]->setData(xs, ys);
    if(_xsection[direction]->plot())
        _xsection[direction]->plot()->replot();
}


void CompareOther::changeCrossSection(int direction, double pos) {
    _other->setCrossSection(direction^1, pos);
}



//void CompareOther::focusEvent(bool hasFocus) {
//    for(int i = 0 ; i < 2; i++)
//        _peaks[i]->setVisible(hasFocus);
//}






//! Plugin description
QString CompareOtherFactory::description() {
    return "Compare current dataset with another one saved as PLVZ file. "
            "The other dataset is presented through its cross-sections. "
            "Optionally you can override the colormap to present two datasets combined, "
            "e.g., with brightness as total intensity and hue as the polarization.";
}

Q_EXPORT_PLUGIN2(compareother, CompareOtherFactory)
