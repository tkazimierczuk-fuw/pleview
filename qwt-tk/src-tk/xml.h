#ifndef MODEL_H
#define MODEL_H


#include <QObject>
#include <QSet>
#include <QXmlStreamWriter>

void seekEndElement(QXmlStreamReader * reader);
QString seekChildElement(QXmlStreamReader * reader, const QSet<QString> &children);
QString seekChildElement(QXmlStreamReader * reader, const QString &childName1, 
				const QString &childName2 = QString(), const QString &childName3 = QString());


void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, const QString & value);
void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, int value);
void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, bool value);
void writeXmlAttribute(QXmlStreamWriter * writer, const QString & name, double value);

bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, QString * target);
bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, int * target);
bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, bool * target);
bool readXmlAttribute(QXmlStreamReader * reader, const QString &name, double * target);
bool readXmlAttribute(const QXmlStreamAttribute &attr, bool * target);





class Model
{

public:
    Model();
    virtual ~Model();


    
    /**
      Pickle all properties to XML stream. Do not write EndElement.
      Do not subclass this method unless really necessary. Instead,
      use serializeAttributes and serializeComponents, which are more
      compatible with objective programming (i.e., can be reused in
      subclasses).
      */
    virtual void serializeToXml(QXmlStreamWriter * writer, const QString & tagName) const;


    /**
      Write all attributes of the start element. When subclassing, you should
      generally remember to call this function from the super-class.
      */
    virtual void serializeAttributes(QXmlStreamWriter * writer) const;


    /**
      Write all child tags. When subclassing, you should
      generally remember to call this function from the super-class.
      */
    virtual void serializeComponents(QXmlStreamWriter * writer) const;


    /**
     Update properties retrieved from XML stream. The previous
     token had type StartElement. The function should read
     all data till its EndElement is read (i.e. on exit
     reader->tokenType() == QXmlStreamReader::EndElement)
     */
    virtual void unserializeFromXml(QXmlStreamReader * reader);


    /**
      Counterpart of serializeAttributes(). When subclassing, you should
      call this function from super-class unless you recognize the attribute.
      Default implementation does nothing.
      */
    virtual void unserializeAttribute(const QXmlStreamAttribute & attribute);


    /**
      Counterpart of serializeComponents(). When subclassing, you should
      call this function from super-class unless you recognize the attribute.
      Default implementation seeks corresponding EndElement token.
      */
    virtual void unserializeComponent(QXmlStreamReader * reader);

};

#endif
