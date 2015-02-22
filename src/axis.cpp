#include "axis.h"
#include "xml.h"

AxisConfiguration::AxisConfiguration() {
    double * arg = new double;
    *arg = 1.;
    _transform.DefineVar("t", arg);
    _transform.SetExpr("t");
    _transform.DefineVar("x", arg);
    _transform.DefineVar("y", arg);
    _type = FromFile;
    _useTransform = false;
}


AxisConfiguration::AxisConfiguration(const AxisConfiguration &other) {
    double * arg = new double;
    *arg = 1.;
    _transform.DefineVar("t", arg);
    _transform.DefineVar("x", arg);
    _transform.DefineVar("y", arg);
    _transform.SetExpr(other.transform().toStdString());
    _type = other.type();
    _useTransform = other._useTransform;
}


AxisConfiguration::~AxisConfiguration() {
        mu::varmap_type variables = _transform.GetVar();
        mu::varmap_type::const_iterator it = variables.find("t");
    delete it->second;
}


QVector<double> AxisConfiguration::values(const QVector<double> &fileValues) const {
        mu::varmap_type variables = _transform.GetVar();
        mu::varmap_type::const_iterator it = variables.find("t");
    double * t = it->second;

    QVector<double> res;
    try {
        for(int i = 0 ; i < fileValues.size(); i++) {
            *t = ((_type == FromFile) ? fileValues[i] : i);
            if(_useTransform)
                res.append(_transform.Eval());
            else
                res.append(*t);

            if(qIsNaN(res.last()) || qIsInf(res.last()))
                return fileValues;
        }
    } catch (mu::Parser::exception_type &e) {
        return fileValues;
    }

    return res;
}


QString AxisConfiguration::setTransform(const QString & transform) {
    _transform.SetExpr(transform.toStdString());

    try {
      _transform.Eval();
    }
    catch(mu::Parser::exception_type &e) {
      _transform.SetExpr("t");
      return QString::fromStdString(e.GetMsg());
    }

    return QString();
}


AxisConfiguration& AxisConfiguration::operator= (const AxisConfiguration& other)
{
    _transform.SetExpr(other._transform.GetExpr());
    _type = other._type;
    _useTransform = other._useTransform;
    return *this;
}


void AxisConfiguration::serializeAttributes(QXmlStreamWriter * writer) const {
    if(_type == FromFile)
        writeXmlAttribute(writer, "type", "fromfile");
    else
        writeXmlAttribute(writer, "type", "natural");
    writeXmlAttribute(writer, "transform", _useTransform);
    writeXmlAttribute(writer, "expression", QString::fromStdString(_transform.GetExpr()));
}


void AxisConfiguration::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "type") {
        QString typeString = attribute.value().toString();
        if(typeString == "fromfile")
            _type = FromFile;
        else if (typeString == "natural")
            _type = Natural;
    }
    else if(attribute.name() == "transform") {
        _useTransform = (attribute.value().toString().toLower() == "true");
    }
    else if(attribute.name() == "expression") {
        QString expressionString = attribute.value().toString();
        if(!expressionString.isEmpty())
            _transform.SetExpr(expressionString.toStdString());
    }
    else Model::unserializeAttribute(attribute);
}
