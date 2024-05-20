#ifndef VARIABLE_H
#define VARIABLE_H

#include <QVector>

#include "xml.h"
#include "DLLDefines.h"
#include "model.h"

/**
 * @class Variable
 *
 * This class represents a variable that can be used e.g. to pass data
 * between various data filters.
 * Variable containes either single number common for all points on the
 * 2D map or a vector (vertical or horizontal).
 */
class PLEVIEW_EXPORT Variable : public Model {
public:
    QString name;

    enum Type { Row, Col, Value, None };
    Type type;

    QVector<double> values;
    QVector<double> errors;

    Variable();
    Variable(Type _type, QVector<double> values);
    Variable(const Variable &);

    ~Variable() {
    }

    //! Construct a row-type variable
    static Variable row(QVector<double> v) {
        return Variable(Row, v);
    }

    //! Construct a column-type variable
    static Variable col(QVector<double> v) {
        return Variable(Col, v);
    }

    //! Construct a value-type variable
    static Variable simpleValue(double v) {
        QVector<double> vec;
        vec.append(v);
        return Variable(Value, vec);
    }

    //! Return a value for given index
    double value(int x, int y) const;

    //! Return a value of error for given index
    double error(int x, int y) const;

    //! Set a value at given index
    void setValue(int x, int y, double value, double error = -qInf());
    
    //! @see Xml::serializeComponents(QXmlStreamWriter*)
    virtual void serializeComponents(QXmlStreamWriter * writer) const;

    //! @see Xml::serializeAttributes(QXmlStreamWriter*)
    virtual void serializeAttributes(QXmlStreamWriter * writer) const;

    //! @see Xml::unserializeAttribute(QXmlStreamAttribute)
    virtual void unserializeAttribute(const QXmlStreamAttribute &attribute);

    //! @see Xml::unserializeComponent(QXmlStreamReader*);
    virtual void unserializeComponent(QXmlStreamReader *reader);

    //! @see Xml::unserializeFromXml(QXmlStreamReader*)
    virtual void unserializeFromXml(QXmlStreamReader*);


    Variable &operator=(const Variable &);

    bool operator==(const Variable &) const;

    bool fixed;
};


typedef QMap<QString, Variable> VarDictionary;




/**
 * @class VariablePool
 *
 * @brief Container for the Variable objects
 */
class PLEVIEW_EXPORT VariablePool : public QObject, public QList<Variable>, public Model {
    Q_OBJECT
public:
    VariablePool(Engine * engine = 0);

    //! Access a variable of given name. Undefined behavior if the name does not exist.
    Variable & variable(const QString &name);


    void append(const Variable &v) {
        QList<Variable>::append(v);
        emit variablesChanged();
    }

    //! Sets an engine. Proper pointer to engine object is required to use the VariablePool
    void setEngine(Engine * engine);


    QMap<QString, double> currentValues() const;
    void setCurrentValue(const QString & name, double value, double error = -qInf());
    void setCurrentValues(const QMap<QString, double> &variables);

    //! @see Xml::serializeComponents(QXmlStreamWriter*)
    virtual void serializeComponents(QXmlStreamWriter * writer) const;

    //! @see Xml::unserializeComponent(QXmlStreamReader * reader);
    virtual void unserializeComponent(QXmlStreamReader * reader);

    //! @see Xml::unserializeFromXml(QXmlStreamReader * reader);
    virtual void unserializeFromXml(QXmlStreamReader * reader);

signals:
    void currentValuesChanged();
    void variablesChanged();

public slots:
    void changeCurrentCrossSection();

private:
    Engine * engine;
};


uint qHash(const Variable &var);

#endif // VARIABLE_H
