#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget * parent = 0, const QColor & color = Qt::black);
    ColorButton(const QColor & color);
    QColor color();
    void setColor(const QColor &color);

signals:
    void valueChanged(const QColor &color);

private slots:
    void launchDialog();
};

#endif // COLORBUTTON_H
