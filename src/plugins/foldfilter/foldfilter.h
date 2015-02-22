#ifndef FOLDFILTER_H
#define FOLDFILTER_H

#include <QtGui>
#include "datafilters.h"

class FoldFilter : public DataFilter
{
public:
    FoldFilter();
    void transform(GridData2D *data, VarDictionary * dict);
    QWidget * createControlWidget();

    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;
    void unserializeFromXml(QXmlStreamReader *reader);

private:
    int foldSteps;
    QSpinBox *stepBox;
    QComboBox *typeCombo;
    Pleview::Direction foldAxis;
};


class FoldFilterFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new FoldFilter();
    }

    //! Name of the plugin
    QString name() { return "Fold data"; }

    //! Tagname used in saved files
    QString tagname() { return "fold"; }

    //! Plugin description
    QString description() { return "Fold and sum the data, e.g. if you re-run the sweep for several times in your experiment";}
};

#endif // FOLDFILTER_H
