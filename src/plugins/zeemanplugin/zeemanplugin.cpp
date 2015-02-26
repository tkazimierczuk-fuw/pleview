#include "zeemanplugin.h"

#include "../plugins.h"

#include <iostream>
#include <QtGui>
#include "qwt_symbol.h"

const double gB = 5.7883e-2; /* bohr magneton in meV/T */

QString ZeemanPlugin::description() {
    return "Plugin for quick determination of g-factor and diamagnetic shift "
            "from magnetic field sweep. \n\n"
            "Plugin assumes that vertical axis represents magnetic field in tesla"
            " and horizontal axis is transition energy in meV.";
}


ZeemanPluginObject::ZeemanPluginObject()
    : PlotAddon(new ZeemanPlugin()) {
    linLeastSq.setOrder(2);
    // init parameters
    _doTranspose = true;

    prepareUi();

    // prepare curves
    for(int i = 0; i < 2; i++) {
        _modelCurves.addCurve(QColor(), QPolygonF());
        _pointCurves.addCurve(QColor(), QPolygonF());
    }

    _pointCurves.setCurveSymbol(Lower, QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::red), QPen(), QSize(5,5)));
    _pointCurves.setCurveSymbol(Upper, QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::blue), QPen(), QSize(5,5)));
    _modelCurves.updateCurve(Lower, QColor(Qt::red));
    _modelCurves.updateCurve(Upper, QColor(Qt::blue));


    _func.SetExpr("sign(guB2*x) * sqrt((0.5*delta0)^2 + (guB2*x)^2) + gamma*x^2 + E0");
    _func.DefineVar("delta0", new double());
    _func.DefineVar("gamma", new double());
    _func.DefineVar("E0", new double());
    _func.DefineVar("guB2", new double());
    _func.DefineVar("x", new double());
    nonLinLeastSq.setFunction(&_func);
}


ZeemanPluginObject::~ZeemanPluginObject() {
    mu::varmap_type funcVars =  _func.GetVar();
    for(mu::varmap_type::const_iterator item = funcVars.begin(); item!=funcVars.end(); ++item) {
        delete item->second;
    }

    if(_frame)
        delete _frame;
}


void ZeemanPluginObject::prepareUi() {
    _frame = new QFrame();
    QFormLayout *layout = new QFormLayout();


    QLabel *info1 = new QLabel("Plugin fits a formula:");
    QLabel *info2 = new QLabel();
    info2->setPixmap(QPixmap(":/plugins/plugins/zeemanplugin-formula.png"));
    info2->setMaximumHeight(22);
    QLabel *info3 = new QLabel("Trace the first Zeeman branch by clicking <font color=\"#aa0000\">left button while holding SHIFT</font> "
                               "and the second branch by clicking <font color=\"#0000aa\">left button while holding CTRL</font> on the map.");
    info3->setWordWrap(true);

    layout->addRow(info1);
    layout->addRow(info2);
    layout->addRow(info3);

    QLabel * desc_e0 = new QLabel();
    desc_e0->setPixmap(QPixmap(":/plugins/plugins/zeemanplugin-e0.png"));
    desc_e0->setMaximumHeight(18);
    label_e0 = new QLabel();
    layout->addRow(desc_e0, label_e0);
    QLabel * desc_g = new QLabel();
    desc_g->setPixmap(QPixmap(":/plugins/plugins/zeemanplugin-g.png"));
    desc_g->setMaximumHeight(18);
    label_g = new QLabel();
    layout->addRow(desc_g, label_g);
    label_D = new QLabel();
    QLabel * desc_gamma = new QLabel();
    desc_gamma->setPixmap(QPixmap(":/plugins/plugins/zeemanplugin-gamma.png"));
    desc_gamma->setMaximumHeight(18);
    layout->addRow(desc_gamma, label_D);
    label_delta0 = new QLabel();
    QLabel * desc_delta0 = new QLabel();
    desc_delta0->setPixmap(QPixmap(":/plugins/plugins/zeemanplugin-delta0.png"));
    desc_delta0->setMaximumHeight(18);
    layout->addRow(desc_delta0, label_delta0);

    allow_zero_field_splitting = new QCheckBox("Allow zero field splitting");
    layout->addRow(allow_zero_field_splitting);
    connect(allow_zero_field_splitting, SIGNAL(toggled(bool)), this, SLOT(recalculate()));

    QDialogButtonBox *box = new QDialogButtonBox();
    connect(box->addButton(QDialogButtonBox::Reset),
            SIGNAL(clicked()), this, SLOT(reset()));
    layout->addRow(box);

    _frame->setLayout(layout);
}


void ZeemanPluginObject::recalculate() {
    QPolygonF points = _pointCurves.curveData(Upper);
    QPolygonF points2 = _pointCurves.curveData(Lower);
    transposeIfNeeded(&points);
    transposeIfNeeded(&points2);
    for(int i = 0; i < points2.size(); i++)
        points.append(QPointF(-points2[i].x(), points2[i].y()));

    linLeastSq.setExpData(points);

    label_e0->setText(QString::number(linLeastSq.parameters().at(0)));
    label_g->setText(QString::number(linLeastSq.parameters().at(1) / gB * 2)); // spin = 1/2
    label_D->setText(QString::number(linLeastSq.parameters().at(2)));
    label_delta0->setText("0");

    if(_frame && allow_zero_field_splitting->isChecked()) {

        QMap<QString, double*> parameters;
        mu::varmap_type funcVars =  _func.GetVar();

        parameters["E0"] = funcVars["E0"];
        parameters["guB2"] = funcVars["guB2"];
        parameters["gamma"] = funcVars["gamma"];
        parameters["delta0"] = funcVars["delta0"];
        parameters["x"] = funcVars["x"];
        *(parameters["E0"]) = linLeastSq.parameters().at(0);
        *(parameters["guB2"]) = linLeastSq.parameters().at(1);
        *(parameters["gamma"]) = linLeastSq.parameters().at(2);
        *(parameters["delta0"]) = linLeastSq.parameters().at(1) * 0.5;
        nonLinLeastSq.setParameters(parameters);

        // once again we take data but now we deal differently with B<=0
        QPolygonF points = _pointCurves.curveData(Upper);
        QPolygonF points2 = _pointCurves.curveData(Lower);
        transposeIfNeeded(&points);
        transposeIfNeeded(&points2);
        const double epsilon = 1e-100;
        QPolygonF expPoints;
        for(int i = 0; i < points.size(); i++)
            expPoints.append(QPointF(qAbs(points[i].x())+epsilon, points[i].y()));
        for(int i = 0; i < points2.size(); i++)
            expPoints.append(QPointF(-(qAbs(points2[i].x())+epsilon), points2[i].y()));
        nonLinLeastSq.setExpData(expPoints);

        if(expPoints.size() >= 4)
            nonLinLeastSq.fit();
        else
            *(parameters["delta0"]) = 0;

        label_e0->setText(QString::number(*(parameters["E0"])));
        label_g->setText(QString::number(*(parameters["guB2"]) / gB * 2)); // spin = 1/2
        label_D->setText(QString::number(*(parameters["gamma"])));
        label_delta0->setText(QString::number(*(parameters["delta0"])));
    }

    QPolygonF v0, v1;
    for(double b = 0.; b < 28; b+=0.1) {
        v0.append(QPointF(b, calculate(b, true)));
        v1.append(QPointF(b, calculate(b, false)));
    }

    transposeIfNeeded(&v0);
    transposeIfNeeded(&v1);
    _modelCurves.updateCurve(Lower, v1);
    _modelCurves.updateCurve(Upper, v0);
}


double ZeemanPluginObject::calculate(double B, bool upperBranch) {
    if(_frame && !allow_zero_field_splitting->isChecked()) {
      double arg = upperBranch ? B : -B;
      return linLeastSq(arg);
    }
    else {
        const double epsilon = 1e-15;
        mu::varmap_type vars = _func.GetVar();
        *(vars["x"]) = upperBranch ? (qAbs(B) +1e-15) : -(qAbs(B) +1e-15);
        return _func.Eval();
    }
}


void ZeemanPluginObject::reset() {
    _pointCurves.updateCurve(Lower, QPolygonF());
    _pointCurves.updateCurve(Upper, QPolygonF());

    emit(changed());
}


void ZeemanPluginObject::addPoint(int branch, const QPointF &point) {
    QPolygonF points = _pointCurves.curveData(branch);
    points.append(point);
    _pointCurves.updateCurve(branch, points);

    recalculate();
    emit(changed());
}


void ZeemanPluginObject::button1clicked(const QPointF &point) {
    addPoint(Upper, point);
}


void ZeemanPluginObject::button2clicked(const QPointF &point) {
    addPoint(Lower, point);
}


void ZeemanPluginObject::init(Engine *engine) {

}


void ZeemanPluginObject::attach(QwtPlot *plot, PlotType type) {
    if(type != Map)
        return;

    _modelCurves.attachCurves(plot);
    _pointCurves.attachCurves(plot);
    connect(this, SIGNAL(changed()), plot, SLOT(replot()));
}


void ZeemanPluginObject::transposeIfNeeded(QPolygonF * poly) {
    if(_doTranspose) {
        for(int i = 0; i < poly->size(); i++) {
            double tmp = (*poly)[i].x();
            (*poly)[i].setX( (*poly)[i].y() );
            (*poly)[i].setY( tmp );
        }
    }
}


void ZeemanPluginObject::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    _pointCurves.serializeToXml(writer, "curves");
    writer->writeEndElement();
}


void ZeemanPluginObject::unserializeFromXml(QXmlStreamReader *reader) {
    while(!seekChildElement(reader, "curves").isEmpty()) {
        _pointCurves.unserializeFromXml(reader);
        recalculate();
    }
}

