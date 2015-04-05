#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "ColorMapSlider.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QDialog dialog;
    QVBoxLayout layout;
    ColorMapSlider slider;
    slider.setMinimumSize(150,30);
    layout.addWidget(&slider);
    dialog.setLayout(&layout);
    dialog.show();
    return app.exec();
}

