#include "mapperplugin.h"

#include <iostream>
#include <QtGui>
#include "model.h"
#include "vectorwidget.h"
#include "vorowrapper.h"
#include "colormapslider.h"

#ifndef NAN
#define NAN (qInf() - qInf())
#endif


SimpleVoronoiWidget::SimpleVoronoiWidget() {
    plotter = new VoronoiPlotItem();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}


SimpleVoronoiWidget::~SimpleVoronoiWidget() {
    delete plotter;
}


void SimpleVoronoiWidget::setPoints(QVector<double> xs, QVector<double> ys) {
    plotter->setPoints(xs, ys);
    update();
}


void SimpleVoronoiWidget::setPoints(QPolygonF poly) {
    plotter->setPoints(poly);
    update();
}



void SimpleVoronoiWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(QPen(Qt::gray, 0.));
    QRectF target =plotter->boundingRect();
    if(target.isEmpty())
        return;
    double s11 = width() / target.width();
    double s22 = -height() / target.height();
    scaling.setMatrix( s11, 0, 0, s22,  -target.left()*s11, height() - target.top()*s22);
    painter.setMatrix(scaling);

    plotter->draw(&painter);
}


QPointF SimpleVoronoiWidget::posToCoordinates(QPoint pos) const {
    return scaling.inverted().map(QPointF(pos));
}


bool SimpleVoronoiWidget::event(QEvent *event) {
    if (event->type() != QEvent::ToolTip)
        return QFrame::event(event);

    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    QPointF coor = posToCoordinates(helpEvent->pos());

    if(!coor.isNull()) {
        QToolTip::showText(helpEvent->globalPos(), QString("x=%1 , y=%2").arg(coor.x()).arg(coor.y()));
    } else {
        QToolTip::hideText();
        event->ignore();
    }

    return true;
}


void SimpleVoronoiWidget::mouseReleaseEvent(QMouseEvent * event) {
    QPointF coor = posToCoordinates(event->pos());

    int closest = -1;
    double dist = qInf();
    QPolygonF points = plotter->points();
    for(int i = 0; i < points.size(); i++) {
        QPointF diff = points[i] - coor;
        double dist2 = QPointF::dotProduct(diff, diff); //squared distance
        if(dist2 < dist) {
            dist = dist2;
            closest = i;
        }
    }

    if( (closest>=0) && !coor.isNull() && ((event->button() == Qt::MiddleButton) || (event->buttons() & Qt::MiddleButton))  )
        emit tileClicked(closest);
}

void SimpleVoronoiWidget::mouseMoveEvent(QMouseEvent *event) {
    mouseReleaseEvent(event);
}


void SimpleVoronoiWidget::showContextMenu(const QPoint &pos) {

    if(plotter->boundingRect().isEmpty())
        return;

    QPoint globalPos = mapToGlobal(pos);

    QMenu menu;
    QAction * colorAction = menu.addAction("Setup color scale");
    QAction * exportAction = menu.addAction("Export to PNG");

    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem == colorAction) {
        setupColorMap();
    } else if (selectedItem == exportAction) {
        exportPng();
    }
}


void SimpleVoronoiWidget::setupColorMap() {
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle("Configure color scale");
    QFormLayout * layout = new QFormLayout();

    ColorMapConfig * fullConfig = new ColorMapConfig(this, plotter->colorMap());
    layout->addRow(fullConfig);

    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addRow(buttons);

    dialog->setLayout(layout);

    if(dialog->exec()) {
        ColorMap cm = fullConfig->currentColorMap();
        setColorMap(cm);
    }
    delete dialog;
}


void SimpleVoronoiWidget::exportPng() {
    QRectF rect = plotter->boundingRect();
    QString filename = QFileDialog::getSaveFileName(this, "Export as PNG", "", "PNG image (*.png)");
    if(filename.isEmpty())
        return;
    if(!filename.endsWith(".png", Qt::CaseInsensitive))
        filename.append(".png");

    // let's suggest some image resolution to get map
    const int totalPixels = 900 * 900;
    double dpi = sqrt(totalPixels / rect.height() / rect.width());
    QSize res(qCeil(dpi * rect.width()), qCeil(dpi * rect.height()));

    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle("Confirm export options");
    QFormLayout * layout = new QFormLayout();

    QSpinBox *xRes = new QSpinBox(), *yRes = new QSpinBox();
    xRes->setRange(1, 100000); yRes->setRange(1, 100000);
    xRes->setValue(res.width()); yRes->setValue(res.height());
    layout->addRow("width (pixels)", xRes);
    layout->addRow("height (pixels)", yRes);

    QCheckBox * cbScale = new QCheckBox();
    cbScale->setChecked(true);
    layout->addRow("Also export color scale", cbScale);

    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));

    dialog->setLayout(layout);

    if(!dialog->exec())
        return;
    res.setWidth(xRes->value());
    res.setHeight(yRes->value());
    bool exportScale = cbScale->isChecked();
    delete dialog;

    QPixmap pixmap(res);
    QPainter * painter = new QPainter(&pixmap);
    painter->scale((double) pixmap.width()/width(), (double) pixmap.height() / height());
    render(painter);
    painter->end();
    delete painter;
    if(pixmap.save(filename, "PNG"))
        Pleview::log()->info(QString("MapperPlugin: image exported to %1").arg(filename));
    else {
        Pleview::log()->error("MapperPlugin: cannot write to file");
        return;
    }

    if(!exportScale)
        return;

    filename.replace(".png", "_scale.png");
    plotter->colorMap().exportScaleToPng(filename);
}



QString MapperPlugin::description() {
    return "Plugin for arranging cross-section into 2D color map";
}





MapperPluginObject::MapperPluginObject() : PlotAddon(new MapperPlugin()) {

    path = Z;
    nx = 10;
    direction = 0;
    useCustom = false;

    _frame = new QFrame();
    QVBoxLayout * layout = new QVBoxLayout();


    directionCombo = new QComboBox();
    directionCombo->addItem("Positions instead of y values");
    directionCombo->addItem("Positions instead of x values");
    layout->addWidget(directionCombo);

    QGroupBox * pathBox = new QGroupBox("Path");
    layout->addWidget(pathBox);

    QFormLayout * pathBoxLayout = new QFormLayout();

    scanTypeBox = new QComboBox();
    scanTypeBox->setIconSize(QSize(60,36));
    scanTypeBox->addItem(QIcon(":/pleview/misc/grid-z.svg"), "z-path");
    scanTypeBox->addItem(QIcon(":/pleview/misc/grid-u.svg"), "Zig-zag -path");
    scanTypeBox->addItem("Custom positions");
    scanTypeBox->setCurrentIndex((int) path);
    pathBoxLayout->addRow("Scan type", scanTypeBox);


    QHBoxLayout * hlayout = new QHBoxLayout();
    xPointsSpinBox = new QSpinBox();
    xPointsSpinBox->setMinimum(1);
    xPointsSpinBox->setValue(nx);
    xPointsSpinBox->setMaximum(300);
    yPointsSpinBox = new QSpinBox();
    yPointsSpinBox->setMinimum(1);
    yPointsSpinBox->setValue(10);
    yPointsSpinBox->setMaximum(300);
    yPointsSpinBox->setEnabled(false);
    hlayout->addWidget(xPointsSpinBox);
    QLabel * xlabel = new QLabel("x");
    hlayout->addWidget(xlabel);
    hlayout->addWidget(yPointsSpinBox);
    pathBoxLayout->addRow("Tiles", hlayout);
    pathBox->setLayout(pathBoxLayout);

    rectpathwidgets.append(xPointsSpinBox);
    rectpathwidgets.append(yPointsSpinBox);
    rectpathwidgets.append(xlabel);
    rectpathwidgets.append(pathBoxLayout->labelForField(hlayout));


    hlayout = new QHBoxLayout();
    xPointsWidget = new VectorWidget();
    yPointsWidget = new VectorWidget();
    hlayout->addWidget(xPointsWidget);
    hlayout->addWidget(yPointsWidget);
    pathBoxLayout->addRow(hlayout);
    custompathwidgets.append(xPointsWidget);
    custompathwidgets.append(yPointsWidget);


    QGroupBox * custombox = new QGroupBox("Plotted values");
    custombox->setFlat(false);

    QFormLayout * customboxlayout = new QFormLayout();

    _useCustomWidget = new QCheckBox();
    _useCustomWidget->setChecked(useCustom);
    customboxlayout->addRow("Plot custom values", _useCustomWidget);

    valuesWidget = new VectorWidget();
    valuesWidget->setValues(_customvalues);
    customboxlayout->addRow(valuesWidget);
    custombox->setLayout(customboxlayout);
    layout->addWidget(custombox);


    voronoiWidget = new SimpleVoronoiWidget();
    layout->addWidget(voronoiWidget);
    voronoiWidget->setMinimumHeight(250);

    connect(voronoiWidget, SIGNAL(tileClicked(int)), this, SLOT(showSpectrum(int)));
    connect(xPointsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(pathChanged()));
    connect(directionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMap()));
    connect(scanTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pathChanged()));
    connect(xPointsWidget, SIGNAL(valueChanged(QVector<double>)), this, SLOT(pathChanged()));
    connect(yPointsWidget, SIGNAL(valueChanged(QVector<double>)), this, SLOT(pathChanged()));
    connect(_useCustomWidget, SIGNAL(toggled(bool)), this, SLOT(setCustom(bool)));
    connect(valuesWidget, SIGNAL(valueChanged(QVector<double>)), this, SLOT(setVector(QVector<double>)));

    _frame->setLayout(layout);

    pathChanged(); // toggles visibility of some widgets
    setCustom(false); // toggles visibility of some widgets
}


/*void MapperPluginObject::spawnRequested() {
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
} */


void MapperPluginObject::showSpectrum(int n) {
    direction = (directionCombo->currentIndex() == 0) ? (Engine::Y) : (Engine::X);
    emit setCrossSection(direction, n);
}


void MapperPluginObject::setVector(QVector<double> v) {
    _customvalues = v;
    updateMap();
}


void MapperPluginObject::pathChanged() {
    nx = xPointsSpinBox->value();
    path = (Path) scanTypeBox->currentIndex();

    bool isCustom = (path == Custom);

    foreach(QWidget * widget, rectpathwidgets)
        widget->setVisible(!isCustom);
    foreach(QWidget * widget, custompathwidgets)
        widget->setVisible(isCustom);

    if(path == Custom) {
        voronoiWidget->setPoints(xPointsWidget->values(), yPointsWidget->values());
    } else {
        yPointsSpinBox->setValue(qCeil( (double) _values.size() / nx ));
        QPolygonF poly;
        poly.reserve(_values.size());
        for(int ix = 0, iy = 0; iy * nx < _values.size(); ix++) {
            poly.append(QPointF( path==Z ? ix : nx - ix, iy));
            if(ix >= nx-1) {
                iy++;
                ix = -1;
            }
        }
        voronoiWidget->setPoints(poly);
    }

    updateMap();
}



MapperPluginObject::~MapperPluginObject() {
    if(_frame)
        delete _frame;
}

void MapperPluginObject::init(Engine *engine) {
    voronoiWidget->setColorMap(engine->colorMap());
    connect(engine, SIGNAL(crossSectionChanged(CrossSection)), this, SLOT(crossSectionChanged(CrossSection)));
    crossSectionChanged(engine->currentCrossSection());
    connect(this, SIGNAL(setCrossSection(int,int)), engine, SLOT(setCrossSection(int,int)));
}


void MapperPluginObject::crossSectionChanged(const CrossSection &cs) {
    _crossSection = cs;
    updateMap();
}

void MapperPluginObject::updateMap() {
    direction = (directionCombo->currentIndex() == 0) ? (Engine::X) : (Engine::Y);

    QVector<double> data(_crossSection.curve[direction].size());
    for(int i = 0; i < data.size(); i++)
        data[i] = _crossSection.curve[direction].value(i).y();
    if(useCustom)
        data = _customvalues;

    int prevsize = _values.size();
    _values = data;

    if(data.size() != prevsize && path != Custom)
        pathChanged();

    voronoiWidget->setData(data);
}


void MapperPluginObject::setCustom(bool on) {
    useCustom = on;
    valuesWidget->setVisible(on);
    updateMap();
}



void MapperPluginObject::attach(QwtPlot *plot, PlotType type) {
}


void MapperPluginObject::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);

    writeXmlAttribute(writer, "xtiles", nx);

    if(scanTypeBox->currentIndex() == (int) Z)
        writeXmlAttribute(writer, "scanType", "z");
    else if(scanTypeBox->currentIndex() == (int) ZigZag)
        writeXmlAttribute(writer, "scanType", "zigzag");
    else {
        writeXmlAttribute(writer, "scanType", "custom");
        writeXmlChild(writer, "xpoints", xPointsWidget->values());
        writeXmlChild(writer, "ypoints", yPointsWidget->values());
    }

    writer->writeStartElement("data");
    if(directionCombo->currentIndex() == 0)
        writeXmlAttribute(writer, "type", "vertical");
    else
        writeXmlAttribute(writer, "type", "horizontal");
    writeXmlAttribute(writer, "usecustom", useCustom);
    if(useCustom)
        writeXmlChild(writer, "values", _customvalues);
    writer->writeEndElement();

    writer->writeEndElement();
}


void MapperPluginObject::unserializeFromXml(QXmlStreamReader *reader) {
    QString scanType;
    readXmlAttribute(reader, "scanType", &scanType);
    if(scanType == "zigzag")
        path = ZigZag;
    else if(scanType == "z")
        path = Z;
    else
        path = Custom;

    int tiles = 0;
    readXmlAttribute(reader, "xtiles", &tiles);
    if(tiles > 0)
        nx = tiles;

    scanTypeBox->setCurrentIndex((int) path);
    xPointsSpinBox->setValue(nx);

    QString tagname;
    while(! (tagname = seekChildElement(reader, "data", "xpoints", "ypoints")).isEmpty()) {
        if(tagname == "data") {
            QString direction;
            readXmlAttribute(reader, "type", &direction);
            directionCombo->setCurrentIndex((direction == "horizontal") ? 1 : 0);
            seekEndElement(reader);
        } else if(tagname == "xpoints") {
            QVector<double> v;
            readXmlChild(reader, &v);
            xPointsWidget->setValues(v);
        } else if(tagname == "ypoints") {
            QVector<double> v;
            readXmlChild(reader, &v);
            yPointsWidget->setValues(v);
        }
    }
}

