#ifndef SPECTRUMTRANSFORMER_H
#define SPECTRUMTRANSFORMER_H

#include <QtGui>
#include "datafilters.h"
#include "muParser.h"
#include "parserwidget.h"

class SpectrumTransformer : public DataFilter
{
public:
    SpectrumTransformer();
    void transform(GridData2D *data, VarDictionary * dict);
    QWidget * createControlWidget();

    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;
    void unserializeFromXml(QXmlStreamReader *reader);

    QString name();

private:
    QVector<double> parspace;
    void transformSpectrum(GridData2D *data, int index);
    ParserWidget *exprEdit;
    QLabel * errorLabel;
    mu::Parser _expression;
};


class SpectrumTransformerFactory : public QObject, public DataFilterFactory {
    Q_OBJECT
    Q_INTERFACES(DataFilterFactory)

public:
    DataFilter * instantiate() {
        return new SpectrumTransformer();
    }

    //! Name of the plugin
    QString name() { return "Spectrum transformer"; }

    //! Tagname used in saved files
    QString tagname() { return "spectrumtransfromer"; }

    //! Plugin description
    QString description() { return "TODO"; }
};

#endif // SPECTRUMTRANSFORMER_H
