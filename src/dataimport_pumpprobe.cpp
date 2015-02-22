#include "pleview.h"
#include "dataimport_pumpprobe.h"


// AsciiMultiFileImport class


// AsciiMatrixImport class

QString AsciiPumpProbeImport::actionName() const {
    return "Import ASCII (pump-probe)";
}


QString AsciiPumpProbeImport::fileDialogFilter() const {
    return "Pump-probe ASCII files (*.txt)";
}


GridData2D * AsciiPumpProbeImport::read(QIODevice *device, bool interactive, QWidget *parent) {
    Pleview::log()->info("Trying to read data using Ascii Pump-probe format");
    // define flags
    int readXs = 1;
    int readYs = 2;
    int skipFirst = 4;  // if there are both x and y in the data then we may want to skip the first token

    int flags = readXs;

    Pleview::log()->info(QString("Flagi: %1").arg(flags));

    QProgressDialog progress("Opening file...", "Cancel",
                                                   0, device->size(), parent);
    progress.show();
    QVector<double> xs, ys, values;

    int lineNumber = 0;
    while(!device->atEnd()) {
        progress.setValue(device->pos());
        if(progress.wasCanceled()) {
            progress.close();
            return 0;
        }

        QString line = device->readLine();
        QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if(tokens.size() < 1)
            continue;

        bool ok;
        if(flags & readXs) {
            double _x = tokens[0].toDouble(&ok);
            if(!ok) {
                Pleview::log()->warning("Error converting '" + tokens[0] +"' to number. Skipping line");
                continue;
            }
            xs.append(_x);
            tokens.removeFirst();
        }
        else
            xs.append(lineNumber++);

        for(int i = 0; i < tokens.size(); i+=2) {
            double v = tokens[i].toDouble(&ok);
            if(!ok) {
                Pleview::log()->warning("Error converting '" + tokens[i] +"' to number");
                return 0;
            }
            values.append(v);
        }        
    }

    if(xs.size() == 0)
        return 0;
    int ysize = values.size() / xs.size();
    if((!ys.isEmpty() && ysize != ys.size()) || (ysize * xs.size() != values.size())) {
        Pleview::log()->warning("Error - wrong shape of the matrix");
        return 0;
    }

    QVector<double> transposed;
    transposed.resize(values.size());
    for(int row = 0; row < ysize; row++) {
        for(int col = 0; col < xs.size(); col++)
            transposed[row * xs.size() + col] = values[row + col * ysize];        
    }

    if(ys.size() > 0 && ys.size() != ysize) {
        Pleview::log()->warning("Mismatch between data size and number of x and y values");
        ys.clear();
    }

    if(ys.isEmpty()) {
        for(int row = 0; row < ysize; row++)
            ys.append(row);
    }

    progress.close();
    Pleview::log()->info("Data successfully read using Ascii pump-probe format");
    return new GridData2D(ys, xs, transposed);
}
