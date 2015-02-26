#include "plugins.h"

#include "zeemanplugin.h"
#include "comment.h"
#include "xsectionfitter.h"
#include "rangevariablefilter.h"
#include "spectrumtransformer.h"
#include "spikeremove.h"
#include "simulatorplugin.h"
#include "qdplugin.h"
#include "mapperplugin.h"
#include "foldfilter.h"
#include "differentiate.h"
#include "compareother.h"


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


QList<QObject*> listOfPlugins() {
    QList<QObject*> plugins;
    plugins.append(new ZeemanPlugin());
    plugins.append(new CommentFactory());
    plugins.append(new XSectionFitterFactory());
    plugins.append(new RangeVariableFilterFactory());
    plugins.append(new SpectrumTransformerFactory());
    plugins.append(new SpikeRemoveFactory());
    plugins.append(new SimulatorPlugin());
    plugins.append(new MapperPlugin());
    plugins.append(new FoldFilterFactory());
    plugins.append(new DifferentiateFactory());
    plugins.append(new CompareOtherFactory());
    plugins.append(new QDPluginFactory());
    return plugins;
}
