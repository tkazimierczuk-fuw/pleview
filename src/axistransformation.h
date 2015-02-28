#ifndef AXISTRANSFORMATION_H
#define AXISTRANSFORMATION_H

#include <QLineEdit>
#include "parserwidget.h"
#include "datafilters.h"

/**
 * The AxisTransformation class is a core "plugin" to transform x or y axis.
 */
class AxisTransformation : public QObject, public DataFilter
{
    Q_OBJECT
public:
    AxisTransformation();
    void transform(GridData2D *data, VarDictionary * dict);
    QWidget * createControlWidget();

    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;
    void unserializeFromXml(QXmlStreamReader *reader);

private slots:
    void enableTransformX(bool on) { transformEnabled[0] = on; }
    void enableTransformY(bool on) { transformEnabled[1] = on; }

private:
    bool transformEnabled[2];
    mu::Parser parser[2];

    QCheckBox *enableTransformBox[2];
    ParserComboWidget *formulaWidget[2];

    double dummy; // dummy variable for transformation evaluation
};



class AxisTransformationFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new AxisTransformation();
    }

    //! Name of the plugin
    QString name() { return "Transform axes"; }

    //! Tagname used in saved files
    QString tagname() { return "transformaxes"; }

    //! Plugin description
    QString description() { return "Transform x or y axis according to a given formula"; }
};



#endif // AXISTRANSFORMATION_H
