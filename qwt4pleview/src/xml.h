#ifndef MODEL_H
#define MODEL_H


#include <QObject>
#include <QSet>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QMetaProperty>
#include <QSharedPointer>
#include <QVector>

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


template <typename Type> int userType() {
    static int type = 0;
    if(!type)
        type = qRegisterMetaType<Type>();
    return type;
}



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

class SaveableModel;

/**
 * @brief The Saver class is a prototype for saving the Pleview data in various formats
 *
 * To some extent, the saving process follows a visitor pattern. In order to save an
 * object inheriting SaveableModel interface, the Saver calls object's save(Saver &) method
 * passing a referance to the saver itself. The object is supposed to call back the methods
 * setData(...) and/or addChild(...) of the particular Saver.
 *
 * The Saver declares several overloaded addChild(...) methods, which are supposed to be implemented
 * in the subclasses.
 */
class SaveFormat {
public:
    virtual void save(QIODevice * device, const SaveableModel &model) {
    }

    virtual void load(QIODevice * device, SaveableModel &model) {
    }

    //! Set the data of the child node
    virtual void addChild(const QString &name, const QVariant &model) {}

    /** Retrieve the data of the child node */
    virtual QVariant getChild(const QString &name, QVariant::Type type, int usertype = 0) {}

    /** Retrieve the data of the child node enforcing interpretation as a list */
    virtual QList<QVariant> getChildAsList(const QString &name, QVariant::Type type, int usertype = 0) {
        QVariant v = getChild(name, type, usertype);
        if(v.type() == QVariant::List)
            return v.toList();
        else if(v.isNull())
            return QList<QVariant>();
        else
            return QList<QVariant>( {v} );
    }

    /** Retrieve the data of the child node enforcing it NOT to be interpreted as a list.
     *  If the getChild would return a QVariant of List type, then we pick its last element */
    virtual QVariant getChildAsSingle(const QString &name, QVariant::Type type, int usertype = 0) {
        QVariant v = getChild(name, type, usertype);
        if(v.type() != QVariant::List)
            return v;
        else {
            QVariantList list = v.toList();
            if(list.isEmpty())
                return QVariant();
            else
                return list.last();
        }
    }
};


class SaveableModel : public Model {
public:
    virtual void save(SaveFormat &saver) const {}

    virtual void load(SaveFormat &saver) {}
};

typedef QSharedPointer<SaveableModel> pSaveableModel;
Q_DECLARE_METATYPE(pSaveableModel)

#define P_SAVEABLE_GADGET Q_GADGET \
public: \
    virtual void save(SaveFormat &saver) const override { \
        for(int propertyIndex=0; propertyIndex < staticMetaObject.propertyCount(); propertyIndex++) { \
            auto property = staticMetaObject.property(propertyIndex); \
            if(property.isStored() && property.name() != QString("objectName")) \
                saver.addChild(property.name(), property.readOnGadget(this)); \
        } \
    } \
    virtual void load(SaveFormat &saver) override { \
        for(int propertyIndex=0; propertyIndex < staticMetaObject.propertyCount(); propertyIndex++) { \
            auto property = staticMetaObject.property(propertyIndex); \
            if(property.isStored()) { \
                auto readValue = saver.getChild(property.name(), property.type(), property.userType()); \
                auto convertedValue = readValue.convert(property.type()); \
                property.writeOnGadget(this, convertedValue); \
            } \
        } \
    }
//end of P_SAVEABLE_GADGET macro

class PropertyModel : public QObject, public SaveableModel {
    Q_OBJECT

public:
    virtual void save(SaveFormat &saver) const override {
        for(int propertyIndex=0; propertyIndex < this->metaObject()->propertyCount(); propertyIndex++) {
            auto property = metaObject()->property(propertyIndex);
            if(property.isStored() && property.name() != QString("objectName"))
                saver.addChild(property.name(), property.read(this));
        }
    }

    virtual void load(SaveFormat &saver) override {
        for(int propertyIndex=0; propertyIndex < this->metaObject()->propertyCount(); propertyIndex++) {
            auto property = metaObject()->property(propertyIndex);
            if(property.isStored()) {
                auto readValue = saver.getChild(property.name(), property.type(), property.userType());
                auto convertedValue = readValue.convert(property.type());
                property.write(this, convertedValue);
            }
        }
    }
};




class PlvzFormat : public SaveFormat {
public:
    PlvzFormat() : reader(nullptr), writer(nullptr) {
    }

    virtual void save(QIODevice * device, const SaveableModel &model) override;

    //! @todo
    virtual void load(QIODevice * device, SaveableModel &model) override;

    //! Set the data of the child node
    virtual void addChild(const QString &name, const QVariant &model) override;

    static QString exactRepresentation(double d);

    static double interpretExactRepresentation(const QString &encoded);


private:
    QXmlStreamReader * reader;
    QXmlStreamWriter * writer;
    bool acceptingAttributes;   // true if we could call writer->writeAttribute, i.e. the last token was StartElement

    void writeAttributeOrTextElement(const QString &name, const QString &value) {
        if(acceptingAttributes)
            writer->writeAttribute(name, value);
        else {
            writer->writeTextElement(name, value);
            acceptingAttributes = false;
        }
    }
};

#endif
