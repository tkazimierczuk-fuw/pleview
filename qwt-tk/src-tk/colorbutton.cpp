#include <QColorDialog>

#include "colorbutton.h"


ColorButton::ColorButton(QWidget * parent, const QColor & color)
                                                    : QPushButton(parent) {
    setColor(color);
    connect(this, SIGNAL(clicked()), this, SLOT(launchDialog()));
}

ColorButton::ColorButton(const QColor & color) {
    setColor(color);
    connect(this, SIGNAL(clicked()), this, SLOT(launchDialog()));
}

QColor ColorButton::color() {
    return palette().color(QPalette::Button);
}

void ColorButton::setColor(const QColor &color) {
    this->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void ColorButton::launchDialog() {
    QColor color = QColorDialog::getColor(this->color(), parentWidget());
    if(color.isValid()) {
        setColor(color);
        emit(valueChanged(color));
    }
}

