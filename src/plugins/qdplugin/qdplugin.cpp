#include "defs.h"
#include "qdplugin.h"
#include "nonlinleastsq.h"
#include "qwt_symbol.h"
#include "pleview.h"

#include <QtAlgorithms>


QDPlugin::QDPlugin()
    : PlotAddon(new QDPluginFactory()), _engine(0)
{
    _func.SetExpr("amp*exp(-(x-x0)^2/(2*(s^2+0.01^2)))+bg");
    _func.DefineVar("amp", new double());
    _func.DefineVar("x0", new double());
    _func.DefineVar("s", new double());
    _func.DefineVar("bg", new double());
    _func.DefineVar("x", new double());

    // initialize interface
    _frame = new QFrame();

    widthEdit = new QLineEdit();
    widthEdit->setValidator(new QDoubleValidator(this));
    widthEdit->setText(Pleview::settings()->value("plugins/qdplugin/width", "0.1").toString());
    fitButton = new QPushButton("Fit");
    resetButton = new QPushButton("Reset polyline");
    QPushButton * helpButton = new QPushButton("Help: how to use");

    QFormLayout * layout = new QFormLayout();
    QHBoxLayout * hlayout = new QHBoxLayout();

    outputPositionCheckBox = new QCheckBox();
    outputPositionCheckBox->setChecked( Pleview::settings()->value("plugins/qdplugin/outputPosition", true).toBool());
    outputWidthCheckBox = new QCheckBox();
    outputWidthCheckBox->setChecked( Pleview::settings()->value("plugins/qdplugin/outputWidth", false).toBool());
    outputAmplitudeCheckBox = new QCheckBox();
    outputAmplitudeCheckBox->setChecked( Pleview::settings()->value("plugins/qdplugin/outputAmplitude", false).toBool());
    outputBackgroundCheckBox = new QCheckBox();
    outputBackgroundCheckBox->setChecked( Pleview::settings()->value("plugins/qdplugin/outputBackground", false).toBool());

    layout->addRow("Expected peak width", widthEdit);
    layout->addRow("Output position", outputPositionCheckBox);
    layout->addRow("Output amplitude", outputAmplitudeCheckBox);
    layout->addRow("Output width", outputWidthCheckBox);
    layout->addRow("Output background", outputBackgroundCheckBox);
    layout->addRow(hlayout);
    hlayout->addWidget(fitButton);
    hlayout->addWidget(resetButton);
    hlayout->addWidget(helpButton);
    _frame->setLayout(layout);

    polylineCurve = new QwtPlotCurve();
    polylineCurve->setItemAttribute(QwtPlotItem::AutoScale, false);
    polylineCurve->setPen(QPen(Qt::lightGray));
    polylineCurve->setSymbol(QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::darkGray), QPen(Qt::darkGreen),QSize(5,5)));

    // connect signals to slots
    connect(fitButton, SIGNAL(clicked()), this, SLOT(fit()));
    connect(helpButton, SIGNAL(clicked()), this, SLOT(displayHelp()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(clearPolyline()));
}


QDPlugin::~QDPlugin() {
    mu::varmap_type funcMap = _func.GetVar();
    delete funcMap["amp"];
    delete funcMap["x0"];
    delete funcMap["s"];
    delete funcMap["bg"];
    delete funcMap["x"];
    // Interface objects will be deleted by Qt parent/child system
}


//! Initialize a plugin
void QDPlugin::init(Engine *engine) {
    _engine = engine;
}


//! Attach any plot items supplied by plugin to plot widget
void QDPlugin::attach(QwtPlot *plot, PlotType type) {
    if(type == PlotAddon::Map)
        polylineCurve->attach(plot);
}


//! @see Xml::serializeComponents()
void QDPlugin::serializeComponents(QXmlStreamWriter * writer) const {
    writer->writeStartElement("polyline");
    // TODO: serialize polyline
    writer->writeEndElement();
}

//! @see Xml::serializeAttributes()
void QDPlugin::serializeAttributes(QXmlStreamWriter *writer) const {
    writeXmlAttribute(writer, "width", widthEdit->text().toDouble());
}


//! @see Xml::unserializeComponent()
void QDPlugin::unserializeComponent(QXmlStreamReader *reader) {
    // TODO: unserialize polyline
    PlotAddon::unserializeComponent(reader);
}

//! @see Xml::unserializeAttribute()
void QDPlugin::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "width") {
        widthEdit->setText(attribute.value().toString());
    }
}


//! Display help about usage
void QDPlugin::displayHelp() {
    QString text("1. Use Ctrl+[left mouse button] to approximate spectral "
                 "line on the map.\n"
                 "2. Enter expected width of the spectral line. It is used as "
                 "initial parameter in fitting and also to specify number of "
                 "data points taken for fitting (fitting range is 4x larger than "
                 "provided width)\n"
                 "3. Press \"Fit\" button and hope for the best\n"
                 "4. When the fitting is finished, the obtained values are stored in "
                 "the clipboard. You can paste it to your favorite plotting software.");
    QMessageBox::information(0, "Using \"Follow QD lines\" plugin", text);
}


void QDPlugin::fit() {
    Pleview::settings()->setValue("plugins/qdplugin/width", widthEdit->text());
    Pleview::settings()->setValue("plugins/qdplugin/outputPosition", outputPositionCheckBox->isChecked());
    Pleview::settings()->setValue("plugins/qdplugin/outputWidth", outputWidthCheckBox->isChecked());
    Pleview::settings()->setValue("plugins/qdplugin/outputAmplitude", outputAmplitudeCheckBox->isChecked());
    Pleview::settings()->setValue("plugins/qdplugin/outputBackground", outputBackgroundCheckBox->isChecked());


    double width = widthEdit->text().toDouble();

    // check initial conditions
    if(!_engine)
        return;
    if(polyline.isEmpty()) {
        Pleview::log()->warning("Approximating polyline is empty");
        return;
    }
    if(width <= 0) {
        Pleview::log()->warning("You need to provide positive width");
        return;
    }

    double safetyWidth = 0.05 * width; // helps with fitting
    QString funcString = QString("amp*exp(-(x-x0)^2/(2*(s^2+(%1)^2)))+bg").arg(safetyWidth);
    _func.SetExpr(funcString.toStdString());


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int notEnoughPoints = 0, acceptedFitResults = 0, rejectedFitResults = 0;
    int _direction = Pleview::X;


    NonLinLeastSq fitter;
    QMap<QString, double*> parameters;
    mu::varmap_type funcMap = _func.GetVar();
    parameters["amp"] = funcMap["amp"];
    parameters["bg"] = funcMap["bg"];
    parameters["x0"] = funcMap["x0"];
    parameters["s"] = funcMap["s"];
    parameters["x"] = funcMap["x"];
    fitter.setParameters(parameters);
    fitter.setFunction(&_func);

    QString results; // in the end this string will be put to clipboard

    QPolygonF xsec = _engine->currentCrossSection().curve[_direction];
    for(int i = 0; i < xsec.size(); i++) {
        _engine->setCrossSection(_direction^1, i);
        QPolygonF expData;

        double E = approximatedX(xsec.value(i).x());

        foreach(QPointF point, _engine->currentCrossSection().curve[_direction^1]) {
            if(point.x() > E - 2* width && point.x() < E + 2 * width )
                expData.append(point);
        }

        if(expData.size() < 4) {
            notEnoughPoints++;
            Pleview::log()->warning("Follow QD Lines Plugin: width too small");
            continue;
        }

        double avgY = 0., sgmY = 0.;
        foreach(QPointF point, expData)
            avgY += point.y();
        avgY /= expData.size();
        foreach(QPointF point, expData)
            sgmY += (point.y() - avgY) * (point.y() - avgY);
        sgmY = sqrt(sgmY / expData.size());
        if(sgmY == 0)
            sgmY = 1e-2; // whatever; the case is invalid anyhow


        fitter.setExpData(expData);
        *(parameters["amp"]) = 2 * sgmY;
        *(parameters["bg"]) = avgY - sgmY;
        *(parameters["x0"]) = E;
        *(parameters["s"]) = width;
        fitter.fit();

        double parE = *(parameters["x0"]);
        double parS = qSqrt(qPow(*(parameters["s"]),2) + safetyWidth*safetyWidth);
        double parAmp = *(parameters["amp"]);
        double parBg = *(parameters["bg"]);

        if(!(parE > E - 2* width && parE < E + 2 * width))
            Pleview::log()->warning(QString("Follow QD Lines plugin: Point rejected due to wrong energy: %1").arg(parE));
        else if (!(*(parameters["amp"]) > 0))
            Pleview::log()->warning(QString("Follow QD Lines plugin: Point rejected due to negative amplitude: %1").arg(*(parameters["amp"])));
        else if (!(*(parameters["amp"])< 20 * sgmY))
            Pleview::log()->warning(QString("Follow QD Lines plugin: Point rejected due to wrong amplitude relation: %1 : %2").arg(*(parameters["amp"])).arg(sgmY));

        if(parE > E - 2* width && parE < E + 2 * width && *(parameters["amp"]) > 0 && *(parameters["amp"])< 20 * sgmY) {
            QString output = QString::number(xsec.value(i).x()) + " ";
            if(outputPositionCheckBox->isChecked())
                output += QString::number(parE) + " ";
            if(outputAmplitudeCheckBox->isChecked())
                output += QString::number(parAmp) + " ";
            if(outputWidthCheckBox->isChecked())
                output += QString::number(parS) + " ";
            if(outputBackgroundCheckBox->isChecked())
                output += QString::number(parBg);
            results.append(output + "\n");
            acceptedFitResults++;
        }
        else
            rejectedFitResults++;


    }
    QApplication::clipboard()->setText(results);
    QApplication::restoreOverrideCursor();
    // TODO: send info about the results to the log
}


double QDPlugin::approximatedX(double y) const {
    if(polyline.isEmpty())
        return 0;

    double prevY = qSNaN();
    double prevX = qSNaN();
    QMapIterator<double, double> it(polyline);
    while (it.hasNext()) {
        it.next();
        if(it.key() >= y) {
            if(qIsNaN(prevY) || prevY == it.key())
                return it.value();
            else
                return prevX + (it.value() - prevX) * (y - prevY)/(it.key() - prevY);
        }
        prevY = it.key();
        prevX = it.value();
    }
    return prevX;
}


void QDPlugin::focusEvent(bool hasFocus) {
    polylineCurve->setVisible(hasFocus);
}


void QDPlugin::button1clicked(const QwtDoublePoint &point) {
    polyline.insert(point.y(), point.x());
    QPolygonF poly;

    QMapIterator<double, double> it(polyline);
    while (it.hasNext()) {
        it.next();
        poly.append(QPointF(it.value(), it.key()));
    }

    polylineCurve->setData(poly);
    if(polylineCurve->plot())
        polylineCurve->plot()->replot();
}


void QDPlugin::clearPolyline() {
    polyline.clear();
    polylineCurve->setData(QPolygonF());
    if(polylineCurve->plot())
        polylineCurve->plot()->replot();
}


//! Plugin description
QString QDPluginFactory::description() {
    return "This plugin is aimed to address the problem encountered in QD magnetospectroscopy: "
            "several sharp lines with many crossings. Traditional automated fitting is tedious "
            "because the fitted function (e.g. gaussian or lorenzian) tend to be mislead at "
            "the crossings.\n\n"
            "In this plugin you have to approximate the line you want to fit and the plugin "
            "only adjusts the approximation within the small range.";
}

Q_EXPORT_PLUGIN2(qdplugin, QDPluginFactory)
