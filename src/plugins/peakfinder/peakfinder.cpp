#include "defs.h"
#include "peakfinder.h"
#include "qwt_symbol.h"
#include "pleview.h"

#include <QtAlgorithms>



PeakFinder::PeakFinder()
    : PlotAddon(new PeakFinderFactory()), _engine(0), _direction(Pleview::Y)
{
    _frame = new QFrame();
    QFormLayout * layout = new QFormLayout();
    _frame->setLayout(layout);


    _windowWidth = 5;
    _widthSpinBox = new QSpinBox();
    _widthSpinBox->setMinimum(3);
    _widthSpinBox->setValue(_windowWidth);
    layout->addRow("Window width (in pixels)", _widthSpinBox);


    _minArea = 2.0;
    _minAreaEdit = new QLineEdit();
    _minAreaEdit->setValidator(new QDoubleValidator(_minAreaEdit));
    _minAreaEdit->setText(QString::number(_minArea));
    layout->addRow("Threashold area", _minAreaEdit);

    _markButton = new QPushButton("Test run");
    _markButton->setAutoDefault(false);
    _scanAllButton = new QPushButton("Scan all to clipboard");
    _scanAllButton->setAutoDefault(false);

    _label = new QLabel();
    layout->addRow(_label);

    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(_markButton);
    hlayout->addWidget(_scanAllButton);
    layout->addRow(hlayout);

    for(int i = 0; i < 2; i++) {      
        _peaks[i] = new QwtPlotCurve();
        _peaks[i]->setItemAttribute(QwtPlotItem::AutoScale, false);
        _peaks[i]->setPen(QPen(Qt::NoPen));
        _peaks[i]->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::darkGreen), QPen(Qt::darkGreen),QSize(6,6)));
    }

    // connect signals to slots
    connect(_markButton, SIGNAL(clicked()), this, SLOT(markCurrent()));
    connect(_scanAllButton, SIGNAL(clicked()), this, SLOT(scanAll()));
    connect(_widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setWindowWidth(int)));
    connect(_minAreaEdit, SIGNAL(editingFinished()), this, SLOT(setMinArea()));
}





PeakFinder::~PeakFinder() {
    for(int i = 0; i < 2; i++) {
        _peaks[i]->detach();
        delete _peaks[i];
    }
}


//! Initialize a plugin
void PeakFinder::init(Engine *engine) {
    _engine = engine;
    connect(engine, SIGNAL(crossSectionChanged(CrossSection)), this, SLOT(clearTestInfo()));
}


//! Attach any plot items supplied by plugin to plot widget
void PeakFinder::attach(QwtPlot *plot, PlotType type) {
    switch(type) {
    case Pleview::X:
    case Pleview::Y:
        _peaks[type^1]->attach(plot);
        break;
    case PlotAddon::Map:
        // TODO
        break;
    }
}


//! @see Model::serializeToXml()
void PeakFinder::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "window", _windowWidth);
    writeXmlAttribute(writer, "minarea", _minArea);
    writer->writeEndElement();
}


//! @see Xml::unserializeAttribute()
void PeakFinder::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "window") {
        bool ok;
        int newWindow = attribute.value().toString().toInt(&ok);
        if(ok)
            setWindowWidth(newWindow);
    }
    else if (attribute.name() == "minarea") {
        bool ok;
        double newMinArea = attribute.value().toString().toDouble(&ok);
        if(ok)
            setMinArea(newMinArea);
    }
    else if (attribute.name() == "direction") {
        for(int i = 0; i < 2; i++) {
            Pleview::Direction dir = (Pleview::Direction) i;
            if(attribute.value().compare(Pleview::directionString(dir), Qt::CaseInsensitive))
                _direction = dir;
        }
    }
}



void PeakFinder::setWindowWidth(int newWindowWidth) {
    if(newWindowWidth < 3)
        return;
    _windowWidth = newWindowWidth;
    if(!_frame || _widthSpinBox->value() == newWindowWidth)
        return;
    bool prev = _widthSpinBox->blockSignals(true);
    _widthSpinBox->setValue(newWindowWidth);
    _widthSpinBox->blockSignals(prev);
}


void PeakFinder::setMinArea(double newMinArea) {
    _minArea = newMinArea;
    if(!_frame || _minAreaEdit->text().toDouble() == newMinArea)
        return;
    bool prev = _minAreaEdit->blockSignals(true);
    _minAreaEdit->setText(QString::number(_minArea));
    _minAreaEdit->blockSignals(prev);
}


void PeakFinder::setMinArea() {
    _minArea = _minAreaEdit->text().toDouble();
}



QPolygonF PeakFinder::findPeaks(const QPolygonF &curve) const {
    QSet<int> setresult;
    for(int first = 0; first <= curve.size() - _windowWidth; first++) {
        int last = first + _windowWidth - 1;
        double area = 0.;
        double maxy = -qInf();
        for(int i = first; i < last; i++) {
            double h1 = curve[i].y() - ((curve[i].x()-curve[first].x()) / (curve[last].x() - curve[first].x()) * (curve[last].y() - curve[first].y()) + curve[first].y());
            double h2 = curve[i+1].y() - ((curve[i+1].x()-curve[first].x()) / (curve[last].x() - curve[first].x()) * (curve[last].y() - curve[first].y()) + curve[first].y());
            area += 0.5 * qAbs(curve[i+1].x() - curve[i].x()) * (h1+h2);
            if(curve[i+1].y() > maxy)
                maxy = curve[i+1].y();
        }
        if(area >= _minArea) {
            for(int i = first+1; i < last; i++)
                if(curve[i].y() == maxy)
                    setresult.insert(i);
        }
    }

    QPolygonF result;
    foreach(int i, setresult)
        result.append(curve[i]);
    return result;
}


void PeakFinder::markCurrent() {
    if(!_engine)
        return;
    QPolygonF poly = findPeaks(_engine->currentCrossSection().curve[_direction]);
    _peaks[_direction]->setSamples(poly);
    if(_peaks[_direction]->plot())
        _peaks[_direction]->plot()->replot();
    if(_frame) {
        if(poly.size() == 0)
            _label->setText("No peaks found. Try lowering the threashold.");
        else if(poly.size() == 1)
            _label->setText("1 peak found in the spectrum");
        else
            _label->setText(QString("%1 peaks found in the spectrum ").arg(poly.size()));
    }
}


void PeakFinder::scanAll() {
    if(!_engine)
        return;
    QString str;
    QTextStream stream(&str);

    CrossSection xsec = _engine->currentCrossSection();
    for(int i = 0; i < xsec.curve[_direction^1].size(); i ++) {
        _engine->setCrossSection(_direction, i);
        QPolygonF poly = findPeaks(_engine->currentCrossSection().curve[_direction]);
        foreach(QPointF p, poly)
            stream << xsec.curve[_direction^1].at(i).x() << " " << p.x() << "\n";
    }
    QApplication::clipboard()->setText(str);
}


void PeakFinder::clearTestInfo() {
    if(_frame)
        _label->clear();
    for(int i = 0; i < 2; i++) {
      _peaks[i]->setSamples(QPolygonF());
      if(_peaks[i]->plot())
            _peaks[i]->plot()->replot();
    }
}


void PeakFinder::focusEvent(bool hasFocus) {
    for(int i = 0 ; i < 2; i++)
        _peaks[i]->setVisible(hasFocus);
}


void PeakFinder::setDirection(Pleview::Direction direction) {
    if(_direction == direction)
        return;

    _direction = direction;
}



//! Plugin description
QString PeakFinderFactory::description() {
    return "The plugin provides primitive peak-recognition algorithm. "
            "It takes a \"window\" of several pixels and compares the area under "
            "selected pixels with user-defined threashold. If the area is bigger than "
            "the threashold then the highest-value pixel is recognized as a peak. "
            "This procedure is repeated for each cross-section and the results are stored "
            "in the clipboard.\n\n"
            "The optimal window width should correspond to full width of a peak and "
            "the threashold value can be tested for a single spectrum by pressing \"Test\" button. "
            "Peaks found in the test run are marked with green circles.";
}

