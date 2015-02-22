#ifndef PARSERXML_H
#define PARSERXML_H

#include <QtCore>

#include "xml.h"
#include "muParser.h"

void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, mu::Parser value);

bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, mu::Parser * target);


#endif // PARSERXML_H
