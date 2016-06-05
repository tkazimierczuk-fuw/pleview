#ifndef MODEL_H
#define MODEL_H


#include <QObject>
#include <QSet>
#include <QXmlStreamWriter>
#include <QDomNode>

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


void writeXmlChild(QXmlStreamWriter * writer, const QString & name, const QVector<double> &value);
void readXmlChild(QXmlStreamReader * reader, QVector<double> * target);



void writeXmlAttribute(QDomNode &node, const QString & name, const QString & value);
void writeXmlAttribute(QDomNode &node, const QString & name, int value);
void writeXmlAttribute(QDomNode &node, const QString & name, bool value);
void writeXmlAttribute(QDomNode &node, const QString & name, double value);

bool readXmlAttribute(const QDomNode &node, const QString &name, QString * target);
bool readXmlAttribute(const QDomNode &node, const QString &name, int * target);
bool readXmlAttribute(const QDomNode &node, const QString &name, bool * target);
bool readXmlAttribute(const QDomNode &node, const QString &name, double * target);



class Model
{

public:
    Model();
    virtual ~Model();



    /**
      Return an XML node representing this object.

      Due to legacy reasons, default implementation converts SAX-style functions
      */
    virtual void toXml(QDomNode &node) const;

    /**
      Restore properties of the object according to given XML node.
      Due to legacy reasons, default implementation refers to the SAX-style function.
      */
    virtual void fromXml(const QDomNode &node);


//    QByteArray uncompressedArray;
//    QXmlStreamWriter writer(&uncompressedArray);
//    obj->serializeToXml(&writer, "dummy");

    
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
