#include "defs.h"
#include "xsectionfitter.h"
#include "nonlinleastsq.h"
#include "qwt_symbol.h"
#include "pleview.h"

#include <QtAlgorithms>


double gauss(double x, double amplitude, double sigma, double x0) {
    return amplitude * exp(-(x-x0)*(x-x0)/(2*sigma*sigma));
}


double * XSectionFitterVarFactory(const char * name, void * ptr) {
    XSectionFitter * obj = static_cast<XSectionFitter*>(ptr);
    return obj->addVariable(name);
}


XSectionFitter::XSectionFitter()
    : PlotAddon(new XSectionFitterFactory()), _engine(0), _direction(Pleview::Y)
{
//    _func.DefineFun("Gauss", gauss, false);
    _func.SetVarFactory(XSectionFitterVarFactory, this);
    _func.SetExpr("(x+0.1)*(y+0.3)");
    _func.Eval(); // to acknowledge x and y variables

    _frame = new QFrame();
    QFormLayout * layout = new QFormLayout();
    _frame->setLayout(layout);

    QGroupBox * directionBox =  new QGroupBox("Direction");
    QLayout * directionBoxLayout = new QVBoxLayout();
    _xDirectionRadio = new QRadioButton("Fit f(x) function (i.e. in the lower cross-section)");
    _yDirectionRadio = new QRadioButton("Fit f(y) function (i.e. in the upper cross-section)");
    _xDirectionRadio->setChecked(true);
    directionBoxLayout->addWidget(_xDirectionRadio);
    directionBoxLayout->addWidget(_yDirectionRadio);
    directionBox->setLayout(directionBoxLayout);
    layout->addRow(directionBox);

    connect(_xDirectionRadio, SIGNAL(toggled(bool)), this, SLOT(setDirection()));
    connect(_yDirectionRadio, SIGNAL(toggled(bool)), this, SLOT(setDirection()));

    _funcEdit = new ParserWidget(&_func);
    _funcEdit->setToolTip("You can use 'x' and 'y' as point coordinates on the map");
    layout->addRow("Function", _funcEdit);

    _varWidget = new VariableWidget(&_varPool);
    layout->addRow(_varWidget);

    _range_xy[0] = _range_xy[1] = _range_z = 0.;
    _range.DefineVar("x", _range_xy + Pleview::X);
    _range.DefineVar("y", _range_xy + Pleview::Y);
    _range.DefineVar("z", &_range_z);
    _range.SetExpr("x > 1800 and x < 1900");
    _rangeEdit = new ParserWidget(&_range);
    layout->addRow("Range filter", _rangeEdit);

    _fitButton = new QPushButton("Fit");
    _fitButton->setAutoDefault(false);
    _fitForwardButton = new QPushButton("Forward fit");
    _fitForwardButton->setAutoDefault(false);
    _fitBackwardButton = new QPushButton("Backward fit");
    _fitBackwardButton->setAutoDefault(false);

    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(_fitButton);
    hlayout->addWidget(_fitForwardButton);
    hlayout->addWidget(_fitBackwardButton);
    layout->addRow(hlayout);

    for(int i = 0; i < 2; i++) {
        _curve[i] = new SectionedCurve();
        _curve[i]->setItemAttribute(QwtPlotItem::AutoScale, false);
        _curve[i]->setPen(QPen(Qt::red));
        _curve_rest[i] = new SectionedCurve();
        _curve_rest[i]->setItemAttribute(QwtPlotItem::AutoScale, false);
        _curve_rest[i]->setPen(QPen(Qt::darkGray));
        _fitPoints[i] = new QwtPlotCurve();
        _fitPoints[i]->setItemAttribute(QwtPlotItem::AutoScale, false);
        _fitPoints[i]->setPen(Qt::NoPen);
        _fitPoints[i]->setSymbol(QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::darkGreen), QPen(Qt::darkGreen),QSize(3,3)));
    }

    // connect signals to slots
    connect(_fitButton, SIGNAL(clicked()), this, SLOT(fit()));

    QSignalMapper * mapper = new QSignalMapper(this);
    mapper->setMapping(_fitForwardButton, 1);
    connect(_fitForwardButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(_fitBackwardButton, 0);
    connect(_fitBackwardButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(fitMulti(int)));

    merger = new SignalMerger();
    connect(&_varPool, SIGNAL(variablesChanged()), merger, SLOT(merge()));
    connect(&_varPool, SIGNAL(currentValuesChanged()), merger, SLOT(merge()));
    connect(merger, SIGNAL(merged()), this, SLOT(recalculateCurve()));
    connect(_funcEdit, SIGNAL(expressionEdited(QString)), merger, SLOT(merge()));
    connect(_rangeEdit, SIGNAL(expressionEdited(QString)), merger, SLOT(merge()));
}


void XSectionFitter::recalculateCurve() {
    if(!_engine)
        return;

    try {
        const CrossSection xsec = _engine->currentCrossSection();

        // one variable (x or y) will be an argument for the function and the
        // second is constant for a given spectrum ("parameter")
        const char * argstring = Pleview::directionString((Pleview::Direction) (_direction^1)); // "x" or "y"
        const char * parstring = Pleview::directionString(_direction); // "y" or "x"

        // change parameters
        if(_varAddr.contains(parstring))
            *(_varAddr.value(parstring)) = xsec.pos[_direction ^ 1];

        QMap<QString, double> vars = _varPool.currentValues();
        QMap<QString, double>::const_iterator it = vars.constBegin();
        for( ; it != vars.constEnd() ; ++it) {
            if(_varAddr.contains(it.key()))
                *(_varAddr[it.key()]) = it.value();
        }

        // substitute x values and evaluate the parser for them
        if(_varAddr.contains(argstring)) {
            double * xaddr =_varAddr.value(argstring);

            QPolygonF poly;
            foreach(QPointF p, xsec.curve[_direction]) {
                *xaddr = p.x();
                poly.append(QPointF(p.x(), _func.Eval()));
            }

            //for(int i = 0; i < poly.size(); i+=3)
            //  poly[i] = QPointF(qSNaN(), qSNaN());

            _curve[_direction^1]->setData(QPolygonF());
            _curve[_direction]->setData(poly);

            _curve_rest[_direction^1]->setData(QPolygonF());
            _curve_rest[_direction]->setData(poly);
        }

        // Now verify experimental data (to highlight points selected to fit).
        // As a bonus, we will calculate the ranges, in which the curve above
        // should be red and where it should be gray.
        QPolygonF curveData;
        _range_xy[_direction] = _engine->currentCrossSection().pos[_direction];
        QVector<int> p_selected, p_nselected;
        int i = 0;

        foreach(QPointF p, _engine->currentCrossSection().curve[_direction]) {
            _range_xy[_direction^1] = p.x();
            _range_z = p.y();
            if(_range.Eval()) {
                curveData.append(p);
                if(p_selected.size() % 2 == 0) {
                    p_selected.append(i);
                    if(!p_nselected.isEmpty())
                        p_nselected.append(i);
                }
            } else {
                if(p_nselected.size() % 2 == 0) {
                    p_nselected.append(i);
                    if(!p_selected.isEmpty())
                        p_selected.append(i);
                }
            }
            i++;
        }

        p_selected.append(i-1);
        p_nselected.append(i-1);

        _curve[_direction]->setRange(p_selected);
        _curve_rest[_direction]->setRange(p_nselected);
        _fitPoints[_direction]->setData(curveData);
        _fitPoints[_direction^1]->setData(QPolygonF());

        for(int i = 0 ; i < 2; i++)
            if(_curve[i]->plot())
                _curve[i]->plot()->replot();
    }
    catch (mu::Parser::exception_type &e) {
        Pleview::log()->warning("Error evaluating curve in Cross-section fitting plugin");
    }

}


XSectionFitter::~XSectionFitter() {
    QwtPlot *plot = _curve[0]->plot();
    for(int i = 0; i < 2; i++) {
        _curve[i]->detach();
        delete _curve[i];
        _curve_rest[i]->detach();
        delete _curve_rest[i];
        _fitPoints[i]->detach();
        delete _fitPoints[i];
    }
    if(plot)
        plot->replot();
    delete merger;
}


//! Initialize a plugin
void XSectionFitter::init(Engine *engine) {
    _engine = engine;
    _varPool.setEngine(_engine);
    connect(engine, SIGNAL(crossSectionChanged(CrossSection)), merger, SLOT(merge()));
}


//! Attach any plot items supplied by plugin to plot widget
void XSectionFitter::attach(QwtPlot *plot, PlotType type) {
    switch(type) {
    case Pleview::X:
    case Pleview::Y:
        _curve[type^1]->attach(plot);
        _curve_rest[type^1]->attach(plot);
        _fitPoints[type^1]->attach(plot);
        break;
    case PlotAddon::Map:
        // TODO
        break;
    }
}


//! @see Model::serializeToXml()
void XSectionFitter::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const {
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "function", QString::fromStdString(_func.GetExpr()));
    writeXmlAttribute(writer, "range", QString::fromStdString(_range.GetExpr()));
    writeXmlAttribute(writer, "direction", Pleview::directionString(_direction));
    _varPool.serializeToXml(writer, "variables");
    writer->writeEndElement();
}


//! @see unserializeFromXml()
void XSectionFitter::unserializeFromXml(QXmlStreamReader *reader) {
    bool sigblocked = signalsBlocked();
    blockSignals(true);
    PlotAddon::unserializeFromXml(reader);

    for(int i = 0; i < _varPool.size(); i++) {
        QString name = _varPool[i].name;
        _varPool[i].type = Variable::Col;
        if(!name.isEmpty() && !_varAddr.contains(name)) {
            double * addr = new double(0);
            *addr = _varPool.currentValues().value(name);
            _varAddr.insert(name, addr);
            _func.DefineVar(name.toStdString(), addr);
            _range.DefineVar(name.toStdString(), addr);
        }
    }

    blockSignals(sigblocked);
}


//! @see Xml::unserializeComponent()
void XSectionFitter::unserializeComponent(QXmlStreamReader *reader) {
    if(reader->name() == "variables")
        _varPool.unserializeFromXml(reader);
    else
        PlotAddon::unserializeComponent(reader);
}

//! @see Xml::unserializeAttribute()
void XSectionFitter::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "function") {
        QString expr = attribute.value().toString();
        if(!expr.isEmpty())
            _funcEdit->setText(expr);
    }
    else if (attribute.name() == "range") {
        QString expr = attribute.value().toString();
        if(!expr.isEmpty())
            _rangeEdit->setText(expr);
    }
    else if (attribute.name() == "direction") {
        for(int i = 0; i < 2; i++) {
            Pleview::Direction dir = (Pleview::Direction) i;
            if(attribute.value().compare(Pleview::directionString(dir), Qt::CaseInsensitive))
                _direction = dir;
        }
    }
}


double * XSectionFitter::addVariable(const QString &name) {
    if(_varAddr.contains(name))
        return _varAddr.value(name);

    if(name != "x" && name != "y") {
            Variable var;
            if(this->_direction == Pleview::Y) {
                var.type = Variable::Col;
                if(_engine)
                      var.values.fill(0, _engine->data()->rows());
            }
            else {
                var.type = Variable::Row;
                if(_engine)
                    var.values.fill(0, _engine->data()->cols());
            }

            var.name = name;
            _varPool.append(var);
        }

    double * addr = new double(0);
    _varAddr.insert(name, addr);
    _range.DefineVar(name.toStdString(), addr);
    return addr;
}


QVector<Point3D> XSectionFitter::filteredData() {
    if(!_engine)
        return QVector<Point3D>();

    QVector<Point3D> data;
    _range_xy[_direction] = _engine->currentCrossSection().pos[_direction];

    foreach(QPointF p, _engine->currentCrossSection().curve[_direction]) {
        _range_xy[_direction^1] = p.x();
        _range_z = p.y();
        if(_range.Eval()) {
            data.append(Point3D(_range_xy[Pleview::X], _range_xy[Pleview::Y], _range_z));
        }
    }

    return data;
}


double XSectionFitter::fit(const QVector<Point3D> &data) {
    if(!_engine)
        return -1;

    if(data.size() <= _varAddr.size()) {
        Pleview::log()->warning(QString("Fitting %1-parameter function to %2 data points").arg(_varAddr.size()-2).arg(data.size()));
    }

    NonLinLeastSq fitter;
    fitter.setExpData(data);
    fitter.setFunction(&_func);

    QMap<QString, double *> _freeVarAddr;
    foreach(QString key, _varAddr.keys()) {
        if(!_varPool.variable(key).fixed)
            _freeVarAddr[key] = _varAddr[key];
    }

    fitter.setParameters(_freeVarAddr);
    fitter.fit();
    double result = fitter.chi2();

    foreach(QString key, _varAddr.keys()) {
        _varPool.setCurrentValue(key, *(_varAddr[key]));
    }

    return result;
}


double XSectionFitter::fit() {
    return fit(filteredData());
}


void XSectionFitter::fitMulti(int forward) {
    if(!_engine)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    CrossSection xsec = _engine->currentCrossSection();
    if(xsec.curve[_direction^1].last().x() < xsec.curve[_direction^1].first().x())
        forward = !forward;

    int totalFitCount = forward ? xsec.curve[_direction^1].size() - xsec.n[_direction]
                                : xsec.n[_direction];

    QProgressDialog * progressDialog = new QProgressDialog("Fitting multiple spectra",
                                                           "Cancel", 0, totalFitCount + 1,
                                                           this->controlWidget());
    progressDialog->setWindowModality(Qt::WindowModal);


    fit(); // fit the first spectrum
    int fitCount = 1;

    for(int i = xsec.n[_direction]; 0 <= i && i < xsec.curve[_direction^1].size(); i += forward ? 1 : -1) {
        progressDialog->setValue(fitCount++);
        if(progressDialog->wasCanceled()) {
            Pleview::log()->warning("Fitting was canceled");
            break;
        }
        QMap<QString, double> prev_pars = _varPool.currentValues(); // save the original parameters
        _engine->setCrossSection(_direction^1, xsec.curve[_direction^1][i].x()); // apply the parameters from the previous spectrum
        QVector<Point3D> expData = filteredData(); // calculate experimental data once
        double curr_chi2 = fit(expData);
        QMap<QString, double> curr_pars = _varPool.currentValues(); // return to original values (for test
        _varPool.setCurrentValues(prev_pars);
        double prev_chi2 = fit(expData);
        if(curr_chi2 < prev_chi2)
            _varPool.setCurrentValues(curr_pars); // choose the better parameters
    }

    progressDialog->setValue(progressDialog->maximum());

    QApplication::restoreOverrideCursor();
}


void XSectionFitter::focusEvent(bool hasFocus) {
    for(int i = 0 ; i < 2; i++) {
        _curve_rest[i]->setVisible(hasFocus);
        _fitPoints[i]->setVisible(hasFocus);

        if(_curve_rest[i]->plot())
            _curve_rest[i]->plot()->replot();
    }
}


//! Sets direction according to the state of plugin's UI
void XSectionFitter::setDirection() {
    Pleview::Direction newDirection = _xDirectionRadio->isChecked() ? Pleview::Y : Pleview::X;
    if(_direction != newDirection)
        setDirection(newDirection);
}


void XSectionFitter::setDirection(Pleview::Direction direction) {   
    _direction = direction;
    Variable::Type oldVarType, newVarType;
    int vecSize = 1;
    if(direction == Pleview::Y) {
        _xDirectionRadio->setChecked(true);
        oldVarType = Variable::Row;
        newVarType = Variable::Col;
        if (_engine)
            vecSize = _engine->data()->rows();
    }
    else {
        _yDirectionRadio->setChecked(true);
        oldVarType = Variable::Col;
        newVarType = Variable::Row;
        if (_engine)
            vecSize = _engine->data()->cols();
    }

    for(int i = 0; i < _varPool.size(); i++) {
        if(_varPool[i].type == oldVarType) {
            _varPool[i].type = newVarType;
            _varPool[i].values.resize(vecSize);
        }
    }

    _varPool.changeCurrentCrossSection();
}


void XSectionFitter::button1clicked(const QwtDoublePoint &point) {
    // validity of focusedParameter() will be check by the _varPool object
    _varPool.setCurrentValue(_varWidget->mouseActiveName, point.x());
}


void XSectionFitter::button2clicked(const QwtDoublePoint &point) {
    // validity of focusedParameter() will be check by the _varPool object
    _varPool.setCurrentValue(_varWidget->mouseActiveName, point.y());
}


//! Plugin description
QString XSectionFitterFactory::description() {
    return "Plugin perform non-linear least squares fitting to a single spectrum. "
           "Several spectra can be fitted one after another. In this case the initial "
           "parameters are taken from the result of fitting for previous spectrum";

}

Q_EXPORT_PLUGIN2(xsectionfitter, XSectionFitterFactory)
