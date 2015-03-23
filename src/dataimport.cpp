#include "pleview.h"
#include "dataimport.h"
#include "dataimport_pumpprobe.h"
#include "dataimport_hamamatsu.h"


// AsciiMultiFileImport class

QString AsciiMultiFileImport::actionName() const {
    return "Import ASCII (multi-file)";
}


QString AsciiMultiFileImport::fileDialogFilter() const {
    return "Index of ASCII files (*.txt)";
}


GridData2D * AsciiMultiFileImport::read(QIODevice * device, bool interactive, QWidget *parent) {
    Pleview::log()->info("Trying to read data using MultiFile format");

    QMap<double, QString> files;
    while(!device->atEnd()) {
        QStringList line = QString(device->readLine()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(line.size() == 0)
            continue;
        else if (line.size() == 2) {
            bool ok;
            files.insert(line[0].toDouble(&ok), line[1]);
            if(!ok) {
                Pleview::log()->warning("Invalid conversion to double from '" + line[0] +"'.");
                return 0;
            }
        }
        else {
            Pleview::log()->warning("A single line should contain exacly two strings: a real value and a filename (not fullfilled for '" + line.join(" ") + "').");
            return 0;
        }
    }
    return read(files, parent);
}


GridData2D * AsciiMultiFileImport::read(QMap<double, QString>files, QWidget *parent) {
    QVector<double> xs, ys, zs;
    foreach(double key, files.keys()) {
        ys.append(key);
        QFile file(files[key]);
        if(!file.open(QIODevice::ReadOnly)) {
            Pleview::log()->warning("Cannot open file " + files[key]);
            return 0;
        }

        Data1D * spectrum = readSingleAsciiFile(&file);
        file.close();
        if(!spectrum)
            return 0; //error

        if(xs.isEmpty()) { // only for the first spectrum
            for(int i = 0; i < spectrum->size(); i++)
                xs.append(spectrum->point(i).x());
        }

        for(int i = 0; i < spectrum->size(); i++) {
            if(spectrum->point(i).x() != xs.value(i)) {
                Pleview::log()->info("X values of the files do not match");
                delete spectrum;
                return 0; // error
            }
            zs.append(spectrum->point(i).y());
        }
    }
    return new GridData2D(ys, xs, zs);
}


Data1D * AsciiMultiFileImport::readSingleAsciiFile(QIODevice * device) {
    QVector<double> xs, ys;
    double x, dummy, value;
    while(readAsciiLine(device, &x, &dummy, &value)) {
         xs.append(x);
         ys.append(value);
    }

    return new Data1D(xs, ys);
}



// AsciiImportFormat class

bool AsciiImportFormat::readAsciiLine(QIODevice *device, double *x, double *y, double *z) {
    if(device->atEnd())
        return false;
    QString line = device->readLine();
    QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    /* TODO: check for y */
    if(tokens.size() < 2)
        return false;
    bool ok1, ok2;
    double _x = tokens[0].toDouble(&ok1);
    double _z = tokens[1].toDouble(&ok2);
    if(ok1 && ok2) {
        *x = _x;
        *z = _z;
        return true;
    }
    else return false;
}



// AsciiSerialImport class

QString AsciiSerialImport::actionName() const {
    return "Import ASCII (two-column)";
}


QString AsciiSerialImport::fileDialogFilter() const {
    return "XY ASCII files (*.txt *.ple *.dat)";
}


GridData2D * AsciiSerialImport::read(QIODevice * device, bool interactive, QWidget *parent) {
    Pleview::log()->info("Trying to read data using PLE format");

    int pixels = 0; // 0 = autodetect repeated x value

    interactive = false; // the dialog below is probably a bad idea

    if(interactive) {

        QDialog dialog(parent);
        dialog.setWindowTitle("Import ASCII XY file");
        QVBoxLayout * layout = new QVBoxLayout();
        dialog.setLayout(layout);
        QLabel * infolabel = new QLabel("You requested import of ASCII XY file. Sequential "
                                        "data rows (spectra) could be recognized automatically "
                                        "by detecting repeated x coordinates. You may also specify "
                                        "number of pixels manually.");
        infolabel->setWordWrap(true);
        layout->addWidget(infolabel);
        QCheckBox * autoBox = new QCheckBox("Detect automatically");
        autoBox->setChecked(true);
        layout->addWidget(autoBox);
        QSpinBox * pixelBox = new QSpinBox();
        pixelBox->setRange(1, 99999);
        pixelBox->setValue(1024);
        pixelBox->setDisabled(true);
        QFormLayout * hlayout = new QFormLayout();
        hlayout->addRow("Pixels per spectrum", pixelBox);
        layout->addLayout(hlayout);
        layout->addSpacing(10);
        QDialogButtonBox * box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(box, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(box, SIGNAL(rejected()), &dialog, SLOT(reject()));
        QObject::connect(autoBox, SIGNAL(toggled(bool)), pixelBox, SLOT(setDisabled(bool)));
        layout->addWidget(box);
        if(dialog.exec() != QDialog::Accepted)
            return 0;

        if(autoBox->isChecked())
            pixels = 0;
        else
            pixels = pixelBox->value();
    }

    QProgressDialog progress("Opening file...", "Cancel", 0, device->size(), QApplication::activeWindow());
    /* TODO: who deletes progress dialog? */
    progress.show();
    int pos0 = device->pos();
    QVector<double> xs, ys, values;
    double x, y, value;

    y = 0;
    // discard initial comments
    while(!readAsciiLine(device, &x, &y, &value) && !device->atEnd())
        ;
    progress.setValue(device->pos());

    ys.append(y);
    y += 1;
    xs.append(x);
    values.append(value);

    double x0 = x;

    // read the first spectrum
    bool next = readAsciiLine(device, &x, &y, &value);
    while(next && ((pixels == 0) && (x!=x0) || values.size() < pixels)) {
        xs.append(x);
        values.append(value);
        next = readAsciiLine(device, &x, &y, &value);
    }

    if(next)
        values.append(value);

    // estimate total size
    int pos1 = device->pos();
    int estimate = (device->size() / (pos1 - pos0)) * values.size();
    estimate += 2 * values.size(); // from performance point of view it's better to over-estimate than under-estimate
    values.reserve(estimate);

    bool prev = false;
    int counter = 0;
    // read all other spectra
    while(!device->atEnd()) {
        bool next = readAsciiLine(device, &x, &y, &value);
        if(((pixels == 0) && ((x==x0) || (!prev && next))) || (next && (pixels > 0) && (values.size()-1) % pixels == 0)) { // beginning of a new spectrum
            ys.append(y);
            y += 1;
            counter++;

            if(counter % 20 == 0) {
                progress.setValue(device->pos());
                QApplication::processEvents();
                if(progress.wasCanceled()) {
                    progress.close();
                    return 0;
                }
            }

        }

        if(next)
            values.append(value);

        prev = next;
    }

    if(xs.size() * ys.size() > 1e8) // check for sanity
    {
        Pleview::log()->error("Total number of pixels exceeds 10^8");
        return 0; // error
    }

    // append missing values in case the last spectrum is not complete
    if(values.size() < xs.size() * ys.size()) {
        Pleview::log()->warning("Input data seem wrong. Incomplete last spectrum?");
        values.resize(xs.size() * ys.size());
    }

    return new GridData2D(ys, xs, values);
}



// AsciiMatrixImport class

QString AsciiMatrixImport::actionName() const {
    return "Import ASCII (matrix)";
}


QString AsciiMatrixImport::fileDialogFilter() const {
    return "Matrix ASCII files (*.txt)";
}


GridData2D * AsciiMatrixImport::read(QIODevice *device, bool interactive, QWidget *parent) {
    Pleview::log()->info("Trying to read data using Ascii Matrix format");
    // define flags
    int readXs = 1;
    int readYs = 2;
    int skipFirst = 4;  // if there are both x and y in the data then we may want to skip the first token

    int flags = 0;

    if(interactive) {

        QDialog dialog(parent);
        dialog.setWindowTitle("Import ASCII matrix");
        QVBoxLayout * layout = new QVBoxLayout();
        dialog.setLayout(layout);
        layout->addWidget(new QLabel("Select type of the matrix data in the input file"));
        QComboBox * typeCombo = new QComboBox();
        typeCombo->setIconSize(QSize(150, 70));
        typeCombo->addItem(QIcon(":/pleview/misc/matrix.svg"), "Only data", 0);
        typeCombo->addItem(QIcon(":/pleview/misc/matrix-x.svg"), "Data + X values", readXs);
        typeCombo->addItem(QIcon(":/pleview/misc/matrix-y.svg"), "Data + Y values", readYs);
        typeCombo->addItem(QIcon(":/pleview/misc/matrix-xy.svg"), "Data + XY values", readXs|readYs);
        typeCombo->addItem(QIcon(":/pleview/misc/matrix-full.svg"), "Data + XY values", readXs|readYs|skipFirst);
        layout->addWidget(typeCombo);
        layout->addSpacing(10);
        QDialogButtonBox * box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(box, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(box, SIGNAL(rejected()), &dialog, SLOT(reject()));
        layout->addWidget(box);
        if(dialog.exec() != QDialog::Accepted)
            return 0;

        flags = typeCombo->itemData(typeCombo->currentIndex()).toInt();
    }

    Pleview::log()->info(QString("Flagi: %1").arg(flags));

    QProgressDialog progress("Opening file...", "Cancel", 0, device->size(), parent);
    progress.show();
    QVector<double> xs, ys, values;

    if(flags & readYs) {

        QString line = device->readLine();
        QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(flags & skipFirst && tokens.size() > 0)
            tokens.removeFirst();

        bool ok;
        for(int i = 0; i < tokens.size(); i++) {
            double y = tokens[i].toDouble(&ok);
            if(!ok) {
                Pleview::log()->warning("Error converting '" + tokens[i] +"' to number");
                return 0;
            }
            ys.append(y);
        }
    }

    int lineNumber = 0;
    while(!device->atEnd()) {
        progress.setValue(device->pos());
        if(lineNumber % 10 == 0)
            QApplication::processEvents();
        if(progress.wasCanceled()) {
            progress.close();
            return 0;
        }

        QString line = device->readLine();
        QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if(tokens.size() < 1)
            break;

        bool ok;
        if(flags & readXs) {
            double _x = tokens[0].toDouble(&ok);
            if(!ok) {
                Pleview::log()->warning("Error converting '" + tokens[0] +"' to number");
                return 0;
            }
            xs.append(_x);
            tokens.removeFirst();
        }
        else
            xs.append(lineNumber++);

        for(int i = 0; i < tokens.size(); i++) {
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
    Pleview::log()->info("Data successfully read using Ascii Matrix format");
    return new GridData2D(ys, xs, transposed);
}


void DataReader::read(Engine *engine, const QString &filename, int formatId, bool interactive) {
        QFile file(filename);
        QDir::setCurrent(QFileInfo(filename).path());
        QString fileSuffix = QFileInfo(filename).suffix();

        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, "IO error", "Unable to read file:" + filename);
            return; /* error opening the file */
        }

        if(formatId < 0 && engine->loadData(&file)) {
            file.close();
            return;
        }

        if(formatId < 0 && !fileSuffix.isEmpty()) {
            formatId = Pleview::settings()->value("extensions/" + fileSuffix,-1).toInt();
        }

        foreach(QSharedPointer<ImportFormat> format, formats()) {
            if(formatId > 0 && formatId != format->formatId())
                continue;

            file.seek(0);
            GridData2D * data2d = format->read(&file, interactive || formatId > 0, 0);
            if(data2d) {
                if(data2d->size() > 0) {
                    engine->setData(data2d);
                    file.close();
                    if(!fileSuffix.isEmpty())
                        Pleview::settings()->setValue("extensions/"+fileSuffix, format->formatId());
                    return;
                }
                else
                    delete data2d;
            }
        }

        file.close();
        Pleview::log()->warning("Reading file " + filename + " failed.");
}


QList<QSharedPointer<ImportFormat> > DataReader::formats() {
    QList<QSharedPointer<ImportFormat> > list;
    list.append(QSharedPointer<ImportFormat>(new AsciiSerialImport()));
    list.append(QSharedPointer<ImportFormat>(new AsciiMatrixImport()));
    list.append(QSharedPointer<ImportFormat>(new AsciiPumpProbeImport()));
    list.append(QSharedPointer<ImportFormat>(new AsciiMultiFileImport()));
    list.append(QSharedPointer<ImportFormat>(new HamamatsuImgImport()));

    return list;
}
