#ifndef VECTORWIDGET_H
#define VECTORWIDGET_H

#include <QtCore>
#include <QtWidgets>


/**
 * @brief The VectorWidget class represents a QVector<double>
 *
  */
class VectorWidget : public QLineEdit
{
    Q_OBJECT
public:
    explicit VectorWidget(QWidget *parent = 0);
    ~VectorWidget();

    QVector<double> values() const {
        return _values;
    }

    void setValues(QVector<double> values);

signals:
    void valueChanged(QVector<double> values);


private slots:
    //! Reads values from clipboard. It can display a message dialog in an interactive mode.
    void paste(bool interactive = true);

    //! Reads values from a text file. It displays at least one message dialog.
    void load();

    //! Displays dialog with basic information about the usage
    void displayInfo();

private:
    QVector<double> _values;
};

#endif // VECTORWIDGET_H
