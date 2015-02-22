#ifndef TRANSPOSEFILTER_H
#define TRANSPOSEFILTER_H

#include <QtGui>
#include "datafilters.h"

class TransposeFilter : public DataFilter
{
public:
    TransposeFilter();
    void transform(GridData2D *data, VarDictionary * dict);
    QWidget * createControlWidget();

    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;
    void unserializeFromXml(QXmlStreamReader *reader);
};


class TransposeFilterFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new TransposeFilter();
    }

    //! Name of the plugin
    QString name() { return "Transpose data"; }

    //! Tagname used in saved files
    QString tagname() { return "transpose"; }

    //! Plugin description
    QString description() { return "Transpose data, i.e., exchange role of x and y."
                            "This can be useful for plugins that assume certain interpretation "
                            "of x and y axis, e.g., Zeeman plugin."; }
};

#endif // TRANSPOSEFILTER_H
