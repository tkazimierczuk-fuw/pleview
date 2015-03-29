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
#include "axistransformation.h"
#include "transposefilter.h"


FactoryObject::~FactoryObject() {
    delete _factory;
}

QString FactoryObject::name() const {
    if(!_name.isEmpty())
        return _name;
    else if(!suggestedName().isEmpty())
        return suggestedName();
    else return _factory->name();
}


void FactoryObject::setName(QString newname) {
    _name = newname;
}


QString FactoryObject::suggestedName() const {
    return QString();
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
    plugins.append(new AxisTransformationFactory());
    plugins.append(new TransposeFilterFactory());
    return plugins;
}
