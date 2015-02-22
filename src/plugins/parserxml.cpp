#include "parserxml.h"

void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, mu::Parser value)
{
    writeXmlAttribute(writer, name, QString::fromStdString(value.GetExpr()));
}


bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, mu::Parser * destination) {
    QString str;
    if(readXmlAttribute(reader, name, &str)) {
        destination->SetExpr(str.toStdString());
        return true;
    }
    else return false;
}
