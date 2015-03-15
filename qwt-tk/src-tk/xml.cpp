#include "xml.h"
#include <QTextStream>


/**
  Function reads XML stream until either child element from given list or EndElement is found.
  The initial token is rejected (even EndElement-type).

  @return tagname of found child element or null string if none found
  */
QString seekChildElement(QXmlStreamReader * reader, const QSet<QString> &children) {
    int level = 1;
    while (!reader->atEnd() && level > 0) {
        reader->readNext();
        switch(reader->tokenType()) {
            case QXmlStreamReader::StartElement: 
                {
                    QString name = reader->name().toString();
                    if(level == 1 && children.find(name) != children.end())
                      return name;
                    level++;
                    break;
                }
            case QXmlStreamReader::EndElement:
                level--;
                break;
            default:
                break;
        }
    }
    
    return QString();
}

QString seekChildElement(QXmlStreamReader * reader, const QString &childName1, const QString &childName2, const QString &childName3) {
  QSet<QString> set;
  set.insert(childName1);
  if(!childName2.isEmpty())
    set.insert(childName2);
  if(!childName3.isEmpty())
    set.insert(childName3);
  return seekChildElement(reader, set);
}


/**
  Function reads XML stream until EndElement is found (EndElement is not read).
  */
void seekEndElement(QXmlStreamReader * reader) {
  seekChildElement(reader, QSet<QString>());
}



void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, const QString & value)
{
    writer->writeAttribute(name, value);
}


bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, QString * destination)
{
    try {
        QXmlStreamAttributes attr = reader->attributes();
        if(!attr.hasAttribute(name))
            return false;
        *destination = attr.value(name).toString();
        return true;
    }
    catch (QXmlStreamReader::Error error) {
        return false;
    }
}



void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, int value)
{
    writeXmlAttribute(writer, name, QString::number(value));
}

bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, int * destination) {
    QString str;
    if(readXmlAttribute(reader, name, &str)) {
        bool ok;
        /* two steps in case of parse error */
        int v = str.toInt(&ok);
        if(!ok)
            return false;
        *destination = v;
        return true;
    }
    else return false;
}


void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, bool value)
{
    if(value)
        writeXmlAttribute(writer, name, QString("true"));
    else
        writeXmlAttribute(writer, name, QString("false"));
}

bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, bool * destination) {
    QString str;
    if(readXmlAttribute(reader, name, &str)) {
        if(str.toLower() == "true")
            *destination = true;
        else if(str.toLower() == "false")
            *destination = false;
        return true;
    }
    else return false;
}

bool readXmlAttribute(const QXmlStreamAttribute &attr, bool * target) {
    if(attr.value().compare("true", Qt::CaseInsensitive) == 0) { // TODO: check compare(...) method
        *target = true;
        return true;
    }
    else if(attr.value().compare("false", Qt::CaseInsensitive) == 0) {
        *target = false;
        return true;
    }
    else return false;
}


void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, double value)
{
    writeXmlAttribute(writer, name, QString::number(value));
}


bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, double * destination) {
    QString str;
    if(readXmlAttribute(reader, name, &str)) {
        bool ok;
        /* two steps in case of parse error */
        double v = str.toDouble(&ok);
        if(!ok)
            return false;
        *destination = v;
        return true;
    }
    else return false;
}




void writeXmlChild(QXmlStreamWriter * writer, const QString & name, const QVector<double> &value) {
    writer->writeStartElement(name);
    writeXmlAttribute(writer, "size", value.size());
    for(int i = 0; i < value.size(); i++)
        writer->writeCharacters(QString::number(value[i]) + '\n');
    writer->writeEndElement();
}



void readXmlChild(QXmlStreamReader * reader, QVector<double> * target) {
    QString text = reader->readElementText();
    QTextStream stream(&text);

    QString token;
    double value;
    bool ok;
    QVector<double> ret;
    stream.skipWhiteSpace();
    while(!stream.atEnd() && stream.status() == QTextStream::Ok) {
        stream >> token;
        value = token.toDouble(&ok);
        if(ok)
            ret.append(value);
        stream.skipWhiteSpace();
    }
    *target = ret;
}






Model::Model()
{
}

Model::~Model() {
}


void Model::serializeToXml(QXmlStreamWriter * writer, const QString & tagName) const {
    writer->writeStartElement(tagName);
    serializeAttributes(writer);
    serializeComponents(writer);
    writer->writeEndElement();
}


void Model::unserializeFromXml(QXmlStreamReader * reader) {
    foreach(QXmlStreamAttribute attr, reader->attributes())
        unserializeAttribute(attr);
    while(reader->readNextStartElement())
        unserializeComponent(reader);
}


void Model::serializeComponents(QXmlStreamWriter *writer) const {
}


void Model::serializeAttributes(QXmlStreamWriter *writer) const {
}


void Model::unserializeAttribute(const QXmlStreamAttribute &attribute) {
}


void Model::unserializeComponent(QXmlStreamReader *reader) {
    seekEndElement(reader);
}
