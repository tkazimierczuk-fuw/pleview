#ifndef DIFFERENTIATE_H
#define DIFFERENTIATE_H

#include <QtGui>
#include "datafilters.h"
#include "muParser.h"
#include "parserwidget.h"
#include "differentialmodel.h"

class Differentiate : public QObject, public DataFilter
{
    Q_OBJECT
public:
    Differentiate();
    ~Differentiate();

    void transform(GridData2D *data, VarDictionary * dict);
    QWidget * createControlWidget();

    void serializeAttributes(QXmlStreamWriter *writer) const;
    void serializeComponents(QXmlStreamWriter *writer) const;

    void unserializeFromXml(QXmlStreamReader *reader);
    void unserializeAttribute(const QXmlStreamAttribute &attribute);
    void unserializeComponent(QXmlStreamReader *reader);

private slots:
    void changeDirection();

private:
    QComboBox * _directioncombo;
    int _direction;
    DifferentialModel * _model;
};


class DifferentiateFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new Differentiate();
    }

    //! Name of the plugin
    QString name() { return "Differentiate data"; }

    //! Tagname used in saved files
    QString tagname() { return "differential"; }

    //! Plugin description
    QString description() { return "This plugin allows to compute a differential of the provided data"; }
};

#endif // DIFFERENTIATE_H
