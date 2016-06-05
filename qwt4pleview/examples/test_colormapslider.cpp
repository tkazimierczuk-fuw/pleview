#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <memory.h>
#include "colormapslider.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QDialog dialog;
    QVBoxLayout * layout = new QVBoxLayout();
    auto map = std::make_shared<ColorMap>();
    ColorMapSlider * slider = new ColorMapSlider(map);
    ColorMapConfig * configdialog1 = new ColorMapConfig(map, true);
    ColorMapConfig * configdialog2 = new ColorMapConfig(map, true);
    ColorMapConfig * configdialog3 = new ColorMapConfig(map, false);
    QPushButton * button1 = new QPushButton("Config dialog 1");
    QPushButton * button2 = new QPushButton("Config dialog 2");
    QPushButton * button3 = new QPushButton("Config dialog 3");
    QObject::connect(button1, SIGNAL(clicked(bool)), configdialog1, SLOT(show()));
    QObject::connect(button2, SIGNAL(clicked(bool)), configdialog2, SLOT(show()));
    QObject::connect(button3, SIGNAL(clicked(bool)), configdialog3, SLOT(show()));

    slider->setMinimumSize(150,30);
    layout->addWidget(slider);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    dialog.setLayout(layout);
    dialog.show();
    return app.exec();
}

