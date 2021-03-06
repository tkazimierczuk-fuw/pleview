#include <iostream>
#include <cmath>

#include "xml.h"
#include "model.h"
#include "defs.h"
#include "dataimport.h"
#include "qtiocompressor.h"

#include "axistransformation.h"


CrossSection::CrossSection() {
    reset();
}


CrossSection::CrossSection(const CrossSection &other) {
    for(int i = 0; i < 2; i++) {
        n[i] = other.n[i];
        pxwidth[i] = other.pxwidth[i];
        pos[i] = other.pos[i];
        lower[i] = other.lower[i];
        upper[i] = other.upper[i];
        curve[i] = other.curve[i];
    }
}


void CrossSection::reset() {
    for(int i = 0; i < 2; i++) {
        n[i] = 0;
        pxwidth[i] = 1;
        pos[i] = lower[i] = upper[i] = 0.;
        curve[i].clear();
    }
}


/* Read settings stored as compressed bytearray */
void readXmlSettings(QSettings * settings, Model * obj, const QString & key) {
    QByteArray header("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
    QByteArray compressedArray = settings->value(key, "").toByteArray();
    QByteArray uncompressedArray = header + qUncompress(compressedArray);

    if(!uncompressedArray.isEmpty()) {
        QXmlStreamReader reader(uncompressedArray);
        reader.readNext();
        reader.readNext();
        obj->unserializeFromXml(&reader);
    }
}


/* Store settings as compressed bytearray */
void storeXmlSettings(QSettings * settings, Model * obj, const QString & key) {
    QByteArray uncompressedArray;
    QXmlStreamWriter writer(&uncompressedArray);
    obj->serializeToXml(&writer, "dummy");
    QByteArray compressedArray = qCompress(uncompressedArray);
    settings->setValue(key, compressedArray);
}


Engine::Engine()
{   
    svgRenderer = 0;
    plotRangesManager = new PlotRangesManager();
    pluginManager = new PlotAddonManager();
    connect(pluginManager, SIGNAL(addonAdded(PlotAddon*)), this, SLOT(addonAdded(PlotAddon*)));
    transformManager = new DataFilterManager();
    connect(transformManager, SIGNAL(dataFiltersChanged()), this, SLOT(prepareData()));
    data2d = nullptr;

    xsec.reset();

    // start with empty data
    original = std::make_shared<GridData2D>();
    prepareData();

    restoreSettings();
}


Engine::~Engine() {
    delete pluginManager;
    delete transformManager;
    delete plotRangesManager;
}


void Engine::restoreSettings() {
    readXmlSettings(Pleview::settings(), &_colorMap, "colormap");
}


void Engine::setCrossSection(int direction, int n, bool forceSignal) {
    if (n != xsec.n[direction] || forceSignal) {
        xsec.n[direction] = n;
        emitCrossSectionChanged((Direction) direction);
    }
}


void Engine::setCrossSection(int direction, double pos, bool forceSignal) {
    direction ^= 1;

    if (data2d == 0) {
        return;
    }
    QVector<double> args = (direction == X) ? data2d->xValues() : data2d->yValues();
    BinarySearchResult bisect = BinarySearchResult::search(pos, args);
    setCrossSection(direction, bisect.closest(), forceSignal);
}


void  Engine::emitCrossSectionChanged(Direction direction) {
    xsec.curve[direction] = this->crossSection(direction);


    int n_pixels = crossSectionWidth(direction);
    QVector<double> horiz = (direction == X) ? data2d->xValues()
                            : data2d->yValues();  
    xsec.pos[direction] = horiz.value(xsec.n[direction]);

    if(!horiz.isEmpty()) {
        double tmp1, tmp2;
        tmp1 = horiz.value(xsec.n[direction]-n_pixels/2 - 1, horiz.first());
        tmp2 = horiz.value(xsec.n[direction]-n_pixels/2, horiz.first());
        xsec.lower[direction] = 0.5 * (tmp1+tmp2);
        tmp1 = horiz.value(xsec.n[direction]-n_pixels/2 - 1 + n_pixels, horiz.last());
        tmp2 = horiz.value(xsec.n[direction]-n_pixels/2 + n_pixels, horiz.last());
        xsec.upper[direction] = 0.5 * (tmp1+tmp2);
    }

    emit(crossSectionChanged(xsec));
    emit(crossSectionChanged(direction, xsec.n[direction]));
    emit(crossSectionChanged(direction, xsec.pos[direction], xsec.lower[direction], xsec.upper[direction]));

    QVector<double> x, y;
    x.reserve(xsec.curve[direction].size()); y.reserve(xsec.curve[direction].size());
    for(int i = 0; i < xsec.curve[direction].size(); i++) {
        x.append(xsec.curve[direction][i].x());
        y.append(xsec.curve[direction][i].y());
    }
    emit(crossSectionChanged(direction, x, y));
}


void Engine::addonAdded(PlotAddon * addon) {
    addon->init(this);
}


bool Engine::loadData(QIODevice *device, Engine::Contents contents) {
    unsigned char buf[3];
    int ret = device->peek((char*) buf, 2);
    if(ret == 2 && buf[0] == 0x1f && buf[1] == 0x8b) {
        // gzip format detected
        QtIOCompressor compressor(device);
        compressor.setStreamFormat(QtIOCompressor::GzipFormat);
        compressor.open(QIODevice::ReadOnly);
        readXml(&compressor, contents);
        return true;
    }
    else
        return false;
}


void Engine::setData(std::shared_ptr<GridData2D> data) {
    data2d = data;

    _colorMap.setRange(data2d->minZ(), data2d->maxZ());
    original = data2d->clone();

    xsec.reset();
    emit(colorMapChanged(_colorMap));

    emitDataChanged();
}


void Engine::setColorMap(const ColorMap &map) {
    if(_colorMap != map) {
        _colorMap = map;
        storeXmlSettings(Pleview::settings(), &_colorMap, "colormap");
        emit(colorMapChanged(map));
    }
}


void Engine::setAxisConfiguration(int direction, const AxisConfiguration & axisConfig) {
    _axisConfig[direction] = axisConfig;
    if(data2d == 0)
        return;
    if(direction == X) {
        QVector<double> args = _axisConfig[direction].values(original->xValues());
        data2d->setXValues(args);
    } else {
        QVector<double> args = _axisConfig[direction].values(original->yValues());
        data2d->setYValues(args);
    }

    emitDataChanged();
}


void Engine::emitDataChanged() {
    emit(dataChanged(data2d));
    emitCrossSectionChanged(X);
    emitCrossSectionChanged(Y);
}


void Engine::readXml(QIODevice *device, Contents contents) {
    if(!device->isReadable())
        return; // error

    if(contents == All) {
        QDomDocument dom;
        QString errMsg;
        bool ok = dom.setContent(device, &errMsg);
        if(!ok) {
            Pleview::log()->error(QString("Error reading XML: ") + errMsg);
            return;
        }

        QDomNode pleviewnode = dom.documentElement().namedItem("pleview");
        if(pleviewnode.isNull()) {
            Pleview::log()->warning("No <pleview> tag found in the XML stream");
            return;
        }

        QDomNode node;

        node = pleviewnode.namedItem("xAxis");
        if(!node.isNull())
            _axisConfig[X].fromXml(node);

        node = pleviewnode.namedItem("yAxis");
        if(!node.isNull())
            _axisConfig[Y].fromXml(node);

        node = pleviewnode.namedItem("marker");
        if(!node.isNull()) {
            readXmlAttribute(node, "x", &(xsec.n[X]));
            readXmlAttribute(node, "y", &(xsec.n[Y]));
        }

        node = pleviewnode.namedItem("data");
        if(!node.isNull())
            original->fromXml(node);

        node = pleviewnode.namedItem("colormap");
        if(!node.isNull()) {
            _colorMap.fromXml(node);
            emit colorMapChanged(_colorMap);
        }

        node = pleviewnode.namedItem("plugins");
        if(!node.isNull())
            pluginManager->fromXml(node);

        node = pleviewnode.namedItem("datafilters");
        if(!node.isNull())
            transformManager->fromXml(node);

        prepareData();
        return;
    }


    QXmlStreamReader reader(device);

    while(!reader.atEnd() && (!reader.isStartElement() || reader.name() != "pleview"))
    {
        if(reader.hasError())
            return; // error

        reader.readNext();
    }

    if(reader.atEnd())
        return;

    reader.readNext(); // read past <pleview>

    while(!reader.atEnd() && !reader.isEndElement()) {
        if(reader.isStartElement()) {
            if(reader.name() == "xAxis" && (contents & ExpData))
                _axisConfig[X].unserializeFromXml(&reader);
            else if (reader.name() == "yAxis" && (contents & ExpData))
                _axisConfig[Y].unserializeFromXml(&reader);
            else if (reader.name() == "marker" && (contents & ExpData)) {
                readXmlAttribute(&reader, "x", &(xsec.n[X]));
                readXmlAttribute(&reader, "y", &(xsec.n[Y]));
                seekEndElement(&reader);
            }
            else if (reader.name() == "data" && (contents & ExpData))
                original->unserializeFromXml(&reader);
            else if (reader.name() == "colormap" && (contents & ExpData)) {
                _colorMap.unserializeFromXml(&reader);
                emit(colorMapChanged(_colorMap));
            }
            else if (reader.name() == "plugins" && (contents & Addons))
                pluginManager->unserializeFromXml(&reader);
                // TODO: emit
            else if (reader.name() == "datafilters" && (contents & Transformations)) {
                transformManager->unserializeFromXml(&reader);
            }
            else
                seekEndElement(&reader);
        }
        reader.readNext();
    }

    // prior to v0.19 axis transformation was handled by AxisConfig widget;
    // now this functionality is provided by a plugin
    if(_axisConfig[X].isTransformEnabled() || _axisConfig[Y].isTransformEnabled()) {
        QString xformula = _axisConfig[X].isTransformEnabled() ? _axisConfig[X].transform() : QString();
        QString yformula = _axisConfig[Y].isTransformEnabled() ? _axisConfig[Y].transform() : QString();
        transformManager->add(new AxisTransformation(xformula, yformula));
    }


    if(reader.hasError())
        Pleview::log()->warning("XML parsing error encountered");

    prepareData();
}


void Engine::prepareData() {
    data2d = original->clone();
    QVector<double> args = _axisConfig[X].values(original->xValues());
    data2d->setXValues(args);
    args = _axisConfig[Y].values(original->yValues());
    data2d->setYValues(args);
    // transformManager->transform(data2d); // TODO!!!
    emitDataChanged();
}


void translateExceptLastToken(QXmlStreamWriter * writer, QIODevice * device) {
    QXmlStreamReader reader(device);
    while(!reader.atEnd() && !(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "svg")) {
        writer->writeCurrentToken(reader);
        reader.readNext();
    }
}


void Engine::save(QIODevice * device) {
    if(!device->isWritable())
        return; // error

    QtIOCompressor compressor(device, 6, 512000);
    compressor.setStreamFormat(QtIOCompressor::GzipFormat);
    compressor.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&compressor);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    QDomDocument root;
    QDomNode pleviewnode;

    if(svgRenderer != 0) {
        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        svgRenderer->generateSvg(&buffer);
        buffer.seek(0);
        root.setContent(&buffer);
    }
    else {
        QFile file(":/icons/actions/editdelete.svg");
        file.open(QIODevice::ReadOnly);
        root.setContent(&file);
        file.close();
    }

    pleviewnode = root.createElement("pleview");
    writeXmlAttribute(pleviewnode, "format", PLEVIEW_VERSION);
    root.documentElement().appendChild(pleviewnode);

    //_axisConfig[X].serializeToXml(&writer, "xAxis");
    //_axisConfig[Y].serializeToXml(&writer, "yAxis");

    QDomNode node = root.createElement("data");
    original->toXml(node);
    pleviewnode.appendChild(node);

    node = root.createElement("colormap");
    _colorMap.toXml(node);
    pleviewnode.appendChild(node);

    node = root.createElement("marker");
    writeXmlAttribute(node, "x", xsec.n[X]);
    writeXmlAttribute(node, "y", xsec.n[Y]);
    pleviewnode.appendChild(node);

    node = root.createElement("plugins");
    pluginManager->toXml(node);
    pleviewnode.appendChild(node);

    node = root.createElement("datafilters");
    transformManager->toXml(node);
    pleviewnode.appendChild(node);

    QTextStream outstream(&compressor);
    root.save(outstream, 0);
    outstream.flush();

    compressor.close();
}


QVector<QPointF> Engine::crossSection(int direction) const {
    QVector<QPointF> vec;

    if(data2d == 0 || ((direction == X) ? data2d->cols() : data2d->rows()) <= xsec.n[direction])
        return QVector<QPointF>();

    QVector<double> xs = (direction == X) ? data2d->xValues() : data2d->yValues();
    QVector<double> ys = (direction == X) ? data2d->yValues() : data2d->xValues();

    for(int i = 0; i < ys.size(); i++) {
        vec.append(QPointF(ys[i], 0));
    }

    int n_pixels = crossSectionWidth(direction);

    int offset = n_pixels / 2;
    for(int i = 0; i < ys.size(); i++) {
        for(int j = 0; j < n_pixels; j++) {
            int pos = xsec.n[direction] - offset + j;
            double z = (direction == X) ? data2d->valueAtIndexBounded(pos, i)
                                    : data2d->valueAtIndexBounded(i, pos);
            vec[i].setY(vec[i].y() + z);
        }
    }

    int pos0 = xsec.n[direction] - n_pixels/2;
    if(pos0 < 0)
        pos0 = 0;
    int pos1 = xsec.n[direction] - n_pixels/2 + n_pixels - 1;
    if(pos1 >= xs.size())
        pos1 = xs.size()-1;

    if(pos1 < pos0)
        return vec;

    double factor = (double) n_pixels / (pos1 - pos0 + 1);

    for(int i = 0; i < ys.size(); i++) {
        if(direction == X)
            vec[i].setY(data2d->sumInIndexRange(pos0, pos1, i, i) * factor);
        else
            vec[i].setY(data2d->sumInIndexRange(i, i, pos0, pos1) * factor);
    }
    
    return vec;
}


QPointF Engine::crossSectionPoint() const {
    if(data2d->size() > 0)
        return QPointF(data2d->xValues().at(xsec.n[X]), data2d->yValues().at(xsec.n[Y]));
    else
        return QPointF();
}


int Engine::crossSectionWidth(int direction) const {
    if(xsec.pxwidth[direction] < 1)
        return 1;
    else
        return xsec.pxwidth[direction];
}


void Engine::setCrossSectionWidth(int direction, int pixels) {
    xsec.pxwidth[direction] = pixels;
    xsec.pxwidth[direction] = crossSectionWidth(direction); // imposes limits on width
    this->emitCrossSectionChanged(X);
    this->emitCrossSectionChanged(Y);
}


shared_ptr<const GridData2D> Engine::data() const {
    return data2d;
}


CrossSection Engine::currentCrossSection() const {
    return xsec;
}
