#ifndef AXIS_H
#define AXIS_H

#include <QtCore>
#include <muParser.h>
#include "xml.h"

/**
  * @class AxisConfiguration
  *
  * @brief This class describes x or y values of data grid.
  *
  * This class is a container of data grid properties for one axis.
  * A values could either be obtained from the input file or
  * be consecutive natural numbers (1, 2, ...).
  * Moreover, these values can be further transformed by an
  * arbitrary transformation to obtained desired effect.
  */
class AxisConfiguration : public Model {
public:
    AxisConfiguration();
    AxisConfiguration(const AxisConfiguration &other);
    ~AxisConfiguration();

    AxisConfiguration& operator= (const AxisConfiguration& other);

    enum Type {FromFile = 1, Natural = 2};
    void setType(Type type) { _type = type; }
    Type type() const { return _type; }

    QString setTransform(const QString &transform);
    QString transform() const {return QString::fromStdString(_transform.GetExpr()); }

    void enableTransform(bool enable) { _useTransform = enable; }
    bool isTransformEnabled() const { return _useTransform; }

    QVector<double> values(const QVector<double> &fileValues) const;


    //! @see Xml::serializeAttributes(QXmlStreamWriter*)
    virtual void serializeAttributes(QXmlStreamWriter * writer) const;

    //! @see Xml::unserializeAttribute(QXmlStreamAttribute)
    virtual void unserializeAttribute(const QXmlStreamAttribute &attribute);


private:
    Type _type;
    bool _useTransform;
    mu::Parser _transform;
};

#endif // AXIS_H
