#include "variable.h"


Variable::Variable() : type(None) {
    fixed = false;
}


Variable::Variable(Type _type, QVector<double> _values) : type(_type), values(_values) {
    fixed = false;
}


Variable::Variable(const Variable &other) : type(other.type), values(other.values) {
    name = other.name;
    fixed = other.fixed;
}


Variable & Variable::operator=(const Variable &other) {
    type = other.type;
    values = other.values;
    name = other.name;
    fixed = other.fixed;
    return *this;
}



void Variable::serializeAttributes(QXmlStreamWriter *writer) const {
    QString typeString;
    switch(type) {
    case Row:
        typeString = "row";
        break;
    case Col:
        typeString = "column";
        break;
    case Value:
        typeString = "value";
        break;
    default:
        typeString = "none";
    }
    writeXmlAttribute(writer, "type", typeString);
    writeXmlAttribute(writer, "fixed", fixed);
    writeXmlAttribute(writer, "name", name);
}


void Variable::serializeComponents(QXmlStreamWriter *writer) const {
    for(int i = 0; i < values.size(); i++) {
        writer->writeStartElement("v");
        writer->writeCharacters(QString::number(values[i]));
        writer->writeEndElement();
    }
}


void Variable::unserializeFromXml(QXmlStreamReader * reader) {
    values.clear();
    Model::unserializeFromXml(reader);
}


void Variable::unserializeAttribute(const QXmlStreamAttribute &attribute) {
    if(attribute.name() == "type") {  // TODO: check this section
        QString typeString = attribute.value().toString();
        if(typeString == "row")
            type = Row;
        else if (typeString == "col")
            type = Col;
        else if (typeString == "value")
            typeString = Value;
        else if (!typeString.isEmpty())
            typeString = None;
    }
    else if(attribute.name() == "fixed") {
        if(attribute.value().compare("true", Qt::CaseInsensitive) == 0)
            fixed = true;
    }
    else if (attribute.name() == "name") {
        name = attribute.value().toString().trimmed();
        if(name.isEmpty())
            name = "noname";
    }
    else Model::unserializeAttribute(attribute);
}


void Variable::unserializeComponent(QXmlStreamReader *reader) {
    if(reader->name() == "v") {
        QString text = reader->readElementText();
        values.append(text.toDouble());
    }
    else Model::unserializeComponent(reader);
}


double Variable::value(int x, int y) const {
    switch(type) {
    case Value:
        return values.value(0);
    case Row:
        return values.value(x);
    case Col:
        return values.value(y);
    default:
        return 0;
    }
}


void Variable::setValue(int x, int y, double value) {
    switch(type) {
    case Value:
        if(values.size() > 0)
            values[0] = value;
        break;
    case Row:
        if(values.size() > x)
            values[x] = value;
        break;
    case Col:
        if(values.size() > y)
            values[y] = value;
        break;
    default:
        break;
    }
}


VariablePool::VariablePool(Engine * engine) {
    setEngine(engine);
}


void VariablePool::setEngine(Engine *engine) {
    this->engine = engine;
    if(engine)
        connect(engine, SIGNAL(crossSectionChanged(CrossSection)), this, SLOT(changeCurrentCrossSection()));
}


Variable & VariablePool::variable(const QString &name) {
    for(int i = 0; i < size(); i++)
        if( this->at(i).name == name)
            return (*this)[i];;
    return first(); // error
}


QMap<QString, double> VariablePool::currentValues() const {
    QMap<QString, double> result;
    int nx = 0, ny = 0;
    if(engine != 0) {
        nx = engine->currentCrossSection().n[Pleview::X];
        ny = engine->currentCrossSection().n[Pleview::Y];
    }
    for(int i = 0; i < this->size(); i++)
        result.insert(this->at(i).name, this->at(i).value(nx, ny));

    return result;
}


void VariablePool::setCurrentValue(const QString & name, double value) {
    for(int i = 0; i < this->size(); i++)
        if(this->at(i).name == name) {
            int nx = 0, ny = 0;
            if(engine != 0) {
                nx = engine->currentCrossSection().n[Pleview::X];
                ny = engine->currentCrossSection().n[Pleview::Y];
            }
            (*this)[i].setValue(nx, ny, value);
            emit currentValuesChanged();
            return;
        }
}


void VariablePool::setCurrentValues(const QMap<QString, double> &variables) {
    foreach(QString key, variables.keys())
        setCurrentValue(key, variables[key]);
}



void VariablePool::serializeComponents(QXmlStreamWriter *writer) const {
    for(int i = 0; i < this->size(); i++)
        this->at(i).serializeToXml(writer, "variable");
}


void VariablePool::unserializeFromXml(QXmlStreamReader *reader) {
    Model::unserializeFromXml(reader);
    emit variablesChanged();
}


void VariablePool::unserializeComponent(QXmlStreamReader *reader) {
    if(reader->name() == "variable") {
        Variable var;
        var.unserializeFromXml(reader);

        for(int i = 0; i < size(); i++)
            if((*this)[i].name == var.name) {
               (*this)[i] = var;
               return;
            }

        this->append(var);
    }
}


void VariablePool::changeCurrentCrossSection() {
    if(size() > 0)
        emit currentValuesChanged();
}


bool Variable::operator==(const Variable &other) const {
    return type == other.type && values == other.values && name == other.name && fixed == other.fixed;
}


uint qHash(const Variable &var) {
    return qHash(var.type) + qHash(var.fixed) + qHash(var.name);
}
