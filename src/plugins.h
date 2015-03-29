#ifndef PLUGINS_H
#define PLUGINS_H

#include <QtCore>
#include <QtPlugin>
#include <iostream>

#include "DLLDefines.h"
#include "xml.h"


class Factory;


/**
 * @class FactoryObject
 *
 * @brief Any kind of worker object
 *
 * An abstract class representing object supplied by a plugin,
 * e.g. a filter of data or addon to the plot.
 * The object is a worker deployed by a Factory class and by
 * default shares a description with its factory.
 */
class PLEVIEW_EXPORT FactoryObject : public Model {
protected:

    /**
     * A constructor that takes a copy of a Factory object.
     * This copy of a Factory object will be deleted in destructor.
     */
    FactoryObject(Factory * factory) : _factory(factory) {}

    /** Destructor */
    virtual ~FactoryObject();

public:
    /** Short user-friendly name of the plugin. Default behavior is that plugin proposes a name, but the user can change it */
    virtual QString name() const;
    virtual QString suggestedName() const;
    void setName(QString newname);

    /**
     * Tagname that will be used to save the object. Must be unique and
     * the same as in the factory function.
     */
    virtual QString tagname();


    /**
     * A description of the object.
     */
    virtual QString description();

private:
    Factory * _factory;
    QString _name;
};



/**
 * @class Factory
 *
 * @brief A factory for FactoryObject
 *
 * An abstract class representing factory of actual worker object.
 */
class Factory {
public:
    /** Destructor */
    virtual ~Factory() {}

    /** Short user-friend name of the factory */
    virtual QString name() = 0;

    /**
     * Tagname of the object supplied by this factory. This function
     * is used to recognize the proper factory while loading the xml file.
     */
    virtual QString tagname() = 0;

    /** A description of the factory */
    virtual QString description() = 0;
};




/**
 * @class FactoryObjectManager
 *
 * @brief A collection of FactoryObjects
 *
 * A template holding a given type of FactoryObject instances in a QList.
 * Class implements save/load function.
 */
template <class T, class TFactory>
        class FactoryObjectManager : public Model {
public:
    virtual void add(T * object) {
        _objects.append(object);
    }

    virtual void add(TFactory * factory) {
        this->add(factory->instantiate());
    }

    virtual void remove(T * object) {
        _objects.removeAll(object);
        delete object;
    }

    void clear() {
        while(!_objects.isEmpty())
            remove(_objects.first());
    }

    /** @see Model::serializeToXml(QXmlStreamWriter *, const QString &) */
    virtual void serializeToXml(QXmlStreamWriter * writer, const QString & tagName) const;

    /** @see Model::unserializeFromXml(QXmlStreamReader *) */
    virtual void unserializeFromXml(QXmlStreamReader * reader);

    static QList<TFactory*> availableFactories();

protected:
    TFactory* findFactory(const QString &tagname);
    QList<T*> _objects;
};


template <class T, class TFactory>
void FactoryObjectManager<T, TFactory>::serializeToXml(QXmlStreamWriter * writer, const QString & tagName) const {
    writer->writeStartElement(tagName);
    foreach(T * plugin, _objects)
        plugin->serializeToXml(writer, plugin->tagname());
    writer->writeEndElement();
}


template <class T, class TFactory>
void FactoryObjectManager<T, TFactory>::unserializeFromXml(QXmlStreamReader * reader) {
    clear();
    while(reader->readNextStartElement()) {

      QString tagname = reader->name().toString();
      TFactory * factory = findFactory(tagname);
      if(factory) {
          T * object = factory->instantiate();
          object->unserializeFromXml(reader);
          this->add(object);
          delete factory;
      }
      else
          seekEndElement(reader);
    }
}


template <class T, class TFactory>
TFactory * FactoryObjectManager<T, TFactory>::findFactory(const QString &tagname) {
    QList<TFactory*> factories = this->availableFactories();
    TFactory * found = 0;
    foreach(TFactory * factory , factories) {
        if(!found && factory->tagname() == tagname)
            found = factory;
        else
            delete factory;
    }
    return found;
}



QList<QObject*> listOfPlugins();


template <class T, class TFactory>
QList<TFactory*> FactoryObjectManager<T, TFactory>::availableFactories() {
    QList<TFactory*> list;

    /*
    QDir dir(QCoreApplication::applicationDirPath() + "/plugins");
    QFileInfoList fileList =  dir.entryInfoList(QDir::Files | QDir::Readable);
    foreach (QFileInfo fileInfo, fileList)  {
        std::cerr << fileInfo.fileName().toStdString();
        QPluginLoader loader(fileInfo.absoluteFilePath());
        QObject *plugin = loader.instance();
        std::cerr << "   " <<  plugin << " |  " << loader.errorString().toStdString() <<   "\n";
        if (plugin) {
            TFactory * mplugin = qobject_cast<TFactory*>(plugin);
            if(mplugin) {
                list.append(mplugin);
                std::cerr << " OK\n";
            }
            else {
                std::cerr << " FAILED\n";
                delete plugin;
            }
        }
    }*/

    QList<QObject*> plugins = listOfPlugins();


    foreach(QObject *plugin, plugins) {
        TFactory * mplugin = qobject_cast<TFactory*>(plugin);
        if(mplugin) {
            list.append(mplugin);
        }
        else {
            delete plugin;
        }
    }

    return list;
}


#endif // PLUGINS_H
