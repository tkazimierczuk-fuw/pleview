#ifndef VECTORWIDGET_H
#define VECTORWIDGET_H

#include <QtCore>
#include <QtWidgets>


/**
 * @brief The VectorWidget class represents a QVector<double>
 *
 * At the moment the functionality is very limited.
 */
class VectorWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit VectorWidget(QWidget *parent = 0);
    ~VectorWidget();


signals:
    void valueChanged(QVector<double> values);

private slots:
    //! If active==true then get and parse contents of the clipboard.
    void paste(bool active);
};

#endif // VECTORWIDGET_H
