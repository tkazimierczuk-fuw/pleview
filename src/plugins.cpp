#include "plugins.h"


FactoryObject::~FactoryObject() {
    delete _factory;
}

QString FactoryObject::name() {
    return _factory->name();
}

QString FactoryObject::tagname() {
    return _factory->tagname();
}

QString FactoryObject::description() {
    return _factory->description();
}
