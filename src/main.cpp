#include <QtCore>
#include <QtGui>

#include "qwt_plot.h"

#include "data2d.h"
#include "MapItem.h"
#include "offlinerenderitem.h"
#include "ColorMapSlider.h"
#include "timecounter.h"
#include "advancedplot.h"
#include "mainwindow.h"
#include "model.h"
#include "plugins.h"
#include "datafilters.h"
#include "datafiltersgui.h"
#include "pleview.h"

#include <iostream>


int main(int argc, char **argv) {
    QApplication app(argc, argv);



    QString dir = Pleview::settings()->value("path","").toString();
    if(!dir.isEmpty())
        QDir::setCurrent(dir);

    MainWindow window(Pleview::engine());
    window.show();

    if(argc == 2)
        window.open(argv[1]);

    //-----------
    if(argc == 4 && argv[1] == QString("--convert")) {
        window.open(argv[2]);

        QSettings sett(QString(argv[2])+".ini", QSettings::IniFormat);
        const double invalid = 1e9;
        double pos0 = sett.value("Calibration/InitialPosition", invalid).toDouble();
        double incr = sett.value("Calibration/Increment", 1).toDouble();
        bool usecal = sett.value("Calibration/UseCalibration", false).toBool();
        double cal0 = sett.value("Calibration/ZeroPoint", 0).toDouble();
        double cal1 = sett.value("Calibration/Step", 1).toDouble();

        AxisConfiguration xaxis = Pleview::engine()->axisConfiguration(Engine::X);
        AxisConfiguration yaxis = Pleview::engine()->axisConfiguration(Engine::Y);
        xaxis.setTransform("1239.8e3/t");
        xaxis.enableTransform(true);
        Pleview::engine()->setAxisConfiguration(Engine::X, xaxis);

        if(pos0 != invalid) {
            if(usecal) {
                yaxis.setTransform(QString("(%1 * t + %2) * %3 + %4").arg(incr).arg(pos0).arg(cal1).arg(cal0));
            }
            else {
                yaxis.setTransform(QString("%1 * t + %2").arg(incr).arg(pos0));
            }
            yaxis.enableTransform(true);
        }

        Pleview::engine()->setAxisConfiguration(Engine::Y, yaxis);

        QFile file(argv[3]);
        file.open(QIODevice::WriteOnly);
        Pleview::engine()->save(&file);
        file.close();
        QApplication::processEvents();
        return 0;
    }
    //-----------

    int appResult = app.exec();
    Pleview::settings()->setValue("path", QDir::currentPath());
    return appResult;
}

