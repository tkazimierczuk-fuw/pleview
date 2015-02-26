#include "mapperplugin.h"

#include <iostream>
#include <QtGui>
#include "model.h"

#ifndef NAN
#define NAN (qInf() - qInf())
#endif


SimpleColorImage::SimpleColorImage() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    _max = _min = NAN;
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

void SimpleColorImage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(QColor("#d4d4d4"));

    // the same as in event
    double width = (double) size().width() / _nx;
    double height = (double) size().height() / _ny;

    double min = qInf(), max = -qInf();

    foreach(double d, _data) {
        if(d < min) min = d;
        if(d > max) max = d;
    }
    if(qIsFinite(_min))
        min = _min;
    if(qIsFinite(_max))
        max = _max;
    _colormap.setRange(min, max);

    if(scaling.m11() < 0) {
        painter.translate(size().width(),0);
        painter.scale(-1, 1);
    }
    if(scaling.m22() < 0) {
        painter.translate(0, size().height());
        painter.scale(1, -1);
    }


    for(int x = 0; x < _nx; x++)
      for(int y = 0; y < _ny; y++) {
        QRectF rect(width * x, height * (_ny-y-1), width, height);
        if(x + y * _nx < _data.size() && !qIsNaN(_data[x + y * _nx]))
            painter.fillRect(rect, _colormap.color(_data[x + y * _nx]));
        else
            painter.drawRect(rect);
      }

    QFrame::paintEvent(event);
}


// Warning: this function is related to the painting done in paintEvent
bool SimpleColorImage::event(QEvent *event) {
    if (event->type() != QEvent::ToolTip)
        return QWidget::event(event);

    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

    double width = (double) size().width() / _nx;
    double height = (double) size().height() / _ny;

    double ix = helpEvent->pos().x() / width;
    double iy = _ny - helpEvent->pos().y() / height;
    if(scaling.m11() < 0)
        ix = _nx - 1 - ix;
    if(scaling.m22() < 0)
        iy = _ny - 1 - iy;

    if (ix >= 0 && iy >= 0 && ix < _nx && iy < _ny) {
        scaling.map(ix-0.5, iy-0.5, &ix, &iy);
        QToolTip::showText(helpEvent->globalPos(), QString("x=%1 , y=%2").arg(ix).arg(iy));
    } else {
        QToolTip::hideText();
        event->ignore();
    }

    return true;
}


//! Present option to change the step size (for tool tip) or the color scale
void SimpleColorImage::showContextMenu(const QPoint& pos) // this is a slot
{
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu myMenu;
    QAction * setupStepAction  = myMenu.addAction("Setup step size");
    QAction * setupColorAction = myMenu.addAction("Setup color map");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem == setupStepAction)
        setRaster();
    else if (selectedItem == setupColorAction) {
        setColorScale();
    }
}



//! Display a dialog with step size options
void SimpleColorImage::setRaster() {
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle("Set step size");
    QFormLayout * layout = new QFormLayout();

    QLineEdit * initialXEdit = new QLineEdit();
    initialXEdit->setValidator(new QDoubleValidator(initialXEdit));
    initialXEdit->setText(QString::number(scaling.m31()));
    QLineEdit * initialYEdit = new QLineEdit();
    initialYEdit->setValidator(new QDoubleValidator(initialYEdit));
    QLineEdit * stepXEdit = new QLineEdit();
    initialYEdit->setText(QString::number(scaling.m32()));
    stepXEdit->setValidator(new QDoubleValidator(stepXEdit));
    stepXEdit->setText(QString::number(scaling.m11()));
    QLineEdit * stepYEdit = new QLineEdit();
    stepYEdit->setValidator(new QDoubleValidator(stepYEdit));
    stepYEdit->setText(QString::number(scaling.m22()));

    layout->addRow("X coordinate of the first point", initialXEdit);
    layout->addRow("Y coordinate of the first point", initialYEdit);
    layout->addRow("Spacing between two points (along x)", stepXEdit);
    layout->addRow("Spacing between two rows (along y)", stepYEdit);

    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));

    dialog->setLayout(layout);
    if(dialog->exec() == QDialog::Accepted) {
        setRaster(initialXEdit->text().toDouble(), initialYEdit->text().toDouble(),
                  stepXEdit->text().toDouble(), stepYEdit->text().toDouble() );
    }
}


//! Set color scale (like manual minimum or maximum)
void SimpleColorImage::setColorScale() {
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle("Setup color mapping");
    QFormLayout * layout = new QFormLayout();

    QLineEdit * minimumEdit = new QLineEdit();
    minimumEdit->setValidator(new QDoubleValidator(minimumEdit));

    QLineEdit * maximumEdit = new QLineEdit();
    maximumEdit->setValidator(new QDoubleValidator(maximumEdit));

    QCheckBox * autoMinimumCheckbox = new QCheckBox();
    autoMinimumCheckbox->setChecked(false);
    connect(autoMinimumCheckbox, SIGNAL(toggled(bool)), minimumEdit, SLOT(setDisabled(bool)));
    QCheckBox * autoMaximumCheckbox = new QCheckBox();
    autoMaximumCheckbox->setChecked(false);
    connect(autoMaximumCheckbox, SIGNAL(toggled(bool)), maximumEdit, SLOT(setDisabled(bool)));

    layout->addRow("Auto-calculate minimum value", autoMinimumCheckbox);
    layout->addRow("Minimum value", minimumEdit);
    layout->addRow("Auto-calculate maximum value", autoMaximumCheckbox);
    layout->addRow("Maximum value", maximumEdit);

    double min = qInf(), max = -qInf();
    foreach(double d, _data) {
        if(d < min) min = d;
        if(d > max) max = d;
    }

    if(qIsFinite(_min))
        min = _min;
    else autoMinimumCheckbox->setChecked(true);
    minimumEdit->setText(QString::number(min));


    if(qIsFinite(_max))
        max = _max;
    else autoMaximumCheckbox->setChecked(true);
    maximumEdit->setText(QString::number(max));


    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));

    dialog->setLayout(layout);
    if(dialog->exec() == QDialog::Accepted) {
        if(autoMinimumCheckbox->isChecked())
            _min = NAN;
        else
            _min = minimumEdit->text().toDouble();
        if(autoMaximumCheckbox->isChecked())
            _max = NAN;
        else
            _max = maximumEdit->text().toDouble();
        update();
    }
}




QString MapperPlugin::description() {
    return "Plugin for arranging cross-section into 2D color map";
}





MapperPluginObject::MapperPluginObject() : PlotAddon(new MapperPlugin()) {
    _frame = new QFrame();
    QFormLayout * layout = new QFormLayout();

    QHBoxLayout * hlayout = new QHBoxLayout();
    xPointsSpinBox = new QSpinBox();
    xPointsSpinBox->setMinimum(1);
    xPointsSpinBox->setValue(10);
    xPointsSpinBox->setMaximum(300);
    yPointsSpinBox = new QSpinBox();
    yPointsSpinBox->setMinimum(1);
    yPointsSpinBox->setValue(10);
    yPointsSpinBox->setMaximum(300);
    hlayout->addWidget(xPointsSpinBox);
    hlayout->addWidget(new QLabel("x"));
    hlayout->addWidget(yPointsSpinBox);

    layout->addRow("Map size", hlayout);

    directionCombo = new QComboBox();
    directionCombo->addItem("Vertical");
    directionCombo->addItem("Horizontal");

    layout->addRow("Cross-section", directionCombo);

    scanTypeBox = new QComboBox();
    scanTypeBox->setIconSize(QSize(60,36));
    scanTypeBox->addItem(QIcon(":/pleview/misc/grid-z.svg"), "z-type");
    scanTypeBox->addItem(QIcon(":/pleview/misc/grid-u.svg"), "Zig-zag -type");
    layout->addRow("Scan type", scanTypeBox);


    image = new SimpleColorImage();
    image->setMinimumHeight(300);
    image->setTiles(10, 10);
    layout->addRow(image);

    QPushButton * spawnButton = new QPushButton("Open in new Pleview window");
    layout->addRow(spawnButton);

    _frame->setLayout(layout);

    connect(spawnButton, SIGNAL(clicked()), this, SLOT(spawnRequested()));
    connect(xPointsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(crossSectionChanged()));
    connect(yPointsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(crossSectionChanged()));
    connect(directionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(crossSectionChanged()));
    connect(scanTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(crossSectionChanged()));
}


void MapperPluginObject::spawnRequested() {
    int nx = image->resolution().width();
    int ny = image->resolution().height();
    QVector<double> data = image->data();

    QTemporaryFile file;
    file.setAutoRemove(false);
    file.open();
    QTextStream out(&file);
    for(int y = 0; y < ny; y++)
      for(int x = 0; x < nx; x++)
          out << x << " " << data.value(x + nx * y) << "\n";
    file.close();

    // Disclaimer: according to QT documentation, QCoreApplication::applicationFilePath()
    // assumes that application dir has not changed which is not true in our case.
    QStringList args;
    args.append(QFileInfo(file).absoluteFilePath());
    QProcess::startDetached(QApplication::applicationFilePath(), args);
}


MapperPluginObject::~MapperPluginObject() {
    if(_frame)
        delete _frame;
}

void MapperPluginObject::init(Engine *engine) {
    image->setColorMap(engine->colorMap());
    connect(engine, SIGNAL(crossSectionChanged(CrossSection)), this, SLOT(crossSectionChanged(CrossSection)));
    crossSectionChanged(engine->currentCrossSection());
}


void MapperPluginObject::crossSectionChanged(const CrossSection &cs) {
    _crossSection = cs;
    crossSectionChanged();
}

void MapperPluginObject::crossSectionChanged() {
    int nx = xPointsSpinBox->value();
    int ny = yPointsSpinBox->value();
    QVector<double> data(nx * ny);
    int direction = (directionCombo->currentIndex() == 0) ? (Engine::X) : (Engine::Y);
    bool zigzag = (scanTypeBox->currentIndex() == 1);

    for(int x = 0; x < nx; x++)
      for(int y = 0; y < ny; y++) {
        int index = (zigzag && (y & 1)) ? (nx - x - 1 + nx * y) : (x + nx * y);
        data[x + y * nx] = _crossSection.curve[direction].value(index, QPointF(0, qSNaN())).y();
    }

    image->setData(nx, ny, data);
}


void MapperPluginObject::attach(QwtPlot *plot, PlotType type) {
}


void MapperPluginObject::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);

    if(scanTypeBox->currentIndex() == 0)
        writeXmlAttribute(writer, "scanType", "z");
    else
        writeXmlAttribute(writer, "scanType", "zigzag");
    writeXmlAttribute(writer, "xtiles", xPointsSpinBox->value());
    writeXmlAttribute(writer, "ytiles", yPointsSpinBox->value());
    writeXmlAttribute(writer, "colorScaleMinimum", image->colorScaleMinimum());
    writeXmlAttribute(writer, "colorScaleMaximum", image->colorScaleMaximum());
    writeXmlAttribute(writer, "step_x0", image->raster().m31());
    writeXmlAttribute(writer, "step_y0", image->raster().m32());
    writeXmlAttribute(writer, "step_dx", image->raster().m11());
    writeXmlAttribute(writer, "step_dy", image->raster().m22());

    writer->writeStartElement("data");
    if(directionCombo->currentIndex() == 0)
        writeXmlAttribute(writer, "type", "vertical");
    else
        writeXmlAttribute(writer, "type", "horizontal");
    writer->writeEndElement();

    writer->writeEndElement();
}


void MapperPluginObject::unserializeFromXml(QXmlStreamReader *reader) {
    QString scanType;
    readXmlAttribute(reader, "scanType", &scanType);
    scanTypeBox->setCurrentIndex((scanType == "zigzag") ? 1 : 0);
    int tiles = 0;
    readXmlAttribute(reader, "xtiles", &tiles);
    if(tiles > 0)
        xPointsSpinBox->setValue(tiles);
    readXmlAttribute(reader, "ytiles", &tiles);
    if(tiles > 0)
        yPointsSpinBox->setValue(tiles);

    double x0 = 0., y0 = 0., dx = 1., dy = 1.;
    readXmlAttribute(reader, "step_x0", &x0);
    readXmlAttribute(reader, "step_y0", &y0);
    readXmlAttribute(reader, "step_dx", &dx);
    readXmlAttribute(reader, "step_dy", &dy);
    image->setRaster(x0, y0, dx, dy);

    double _min = NAN, _max = NAN;
    readXmlAttribute(reader, "colorScaleMinimum", &_min);
    readXmlAttribute(reader, "colorScaleMaximum", &_max);
    image->setColorScale(_min, _max);

    while(!seekChildElement(reader, "data").isEmpty()) {
        QString direction;
        readXmlAttribute(reader, "type", &direction);
        directionCombo->setCurrentIndex((direction == "horizontal") ? 1 : 0);
        seekEndElement(reader);
    }
}


