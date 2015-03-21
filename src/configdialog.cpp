#include "configdialog.h"

#include <QtGui>


ColorDock::ColorDock(Engine *engine) {
    QFrame * frame = new QFrame();
    QHBoxLayout * layout = new QHBoxLayout();

    configWidget = new ColorMapConfig(this, engine->colorMap(), false);
    connect(configWidget, SIGNAL(colorMapChanged(ColorMap)), engine, SLOT(setColorMap(ColorMap)));
    connect(engine, SIGNAL(colorMapChanged(ColorMap)), configWidget, SLOT(setColorMap(ColorMap)));
    configWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    layout->addWidget(configWidget);

    frame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    frame->setLayout(layout);

    setWidget(frame);
    setWindowTitle("Color scale");
    toggleViewAction()->setIcon(QIcon(":/icons/actions/colormap.png"));
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
}


XSectionDock::XSectionDock(Engine * engine) {
    QFrame * frame = new QFrame();
    QFormLayout *layout = new QFormLayout;

    QString axisName[2] = {QString("vertical"), QString("horizontal")};

    for(int axis = 0; axis < 2; axis++) {
        widthEdit[axis] = new QSpinBox(this);
        widthEdit[axis]->setValue(engine->crossSectionWidth(axis));
        widthEdit[axis]->setMinimum(1);
        widthEdit[axis]->setMaximum(10000);
        widthEdit[axis]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        layout->addRow("Pixels for " + axisName[axis] + " section", widthEdit[axis]);

        QFrame * line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        layout->addRow(line);
    }
    frame->setLayout(layout);
    setWidget(frame);

    connect(widthEdit[0], SIGNAL(valueChanged(int)), this, SLOT(xXsectionChanged(int)));
    connect(widthEdit[1], SIGNAL(valueChanged(int)), this, SLOT(yXsectionChanged(int)));
    connect(this, SIGNAL(xsectionChanged(int,int)), engine, SLOT(setCrossSectionWidth(int,int)));

    setWindowTitle("Cross-section");
    toggleViewAction()->setIcon(QIcon(":/icons/actions/configureXsection.svg"));
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
}


void XSectionDock::xXsectionChanged(int width) {
    emit xsectionChanged(0, width);
}


void XSectionDock::yXsectionChanged(int width) {
    emit xsectionChanged(1, width);
}

