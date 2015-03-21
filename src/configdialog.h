#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui>
#include "model.h"
#include "colormapslider.h"
#include "parserwidget.h"
#include "plotrangesmanager.h"


class ColorDock : public QDockWidget {
    Q_OBJECT
public:
    ColorDock(Engine * engine);

private:
    ColorMapConfig * configWidget;
};




class XSectionDock : public QDockWidget {
    Q_OBJECT
public:
    XSectionDock(Engine * engine);

private slots:
    void xXsectionChanged(int);
    void yXsectionChanged(int);

signals:
    void xsectionChanged(int, int);

private:
    QSpinBox * widthEdit[2];

};

#endif // CONFIGDIALOG_H
