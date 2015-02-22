#ifndef SPIKEREMOVE_H
#define SPIKEREMOVE_H

#include <QtGui>
#include "datafilters.h"

class SpikeRemove : public DataFilter
{
public:
    SpikeRemove();
    void transform(GridData2D *data, VarDictionary * dict);
    QWidget * createControlWidget();

    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;
    void unserializeFromXml(QXmlStreamReader *reader);

private:
    QLineEdit *thresholdEdit;
    mutable double thresholdValue;
};



class SpikeRemoveFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new SpikeRemove();
    }

    //! Name of the plugin
    QString name() { return "Spike remover"; }

    //! Tagname used in saved files
    QString tagname() { return "spikeremover"; }

    //! Plugin description
    QString description() { return "TODO"; }
};


#endif // SPIKEREMOVE_H
