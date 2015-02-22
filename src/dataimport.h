#ifndef DATAIMPORT_H
#define DATAIMPORT_H

#include <QtGui>

#include "data1d.h"
#include "data2d.h"
#include "model.h"

class ImportFormat : public QObject{
public:
    virtual QString fileDialogFilter() const = 0;
    virtual QString actionName() const = 0;
    virtual int formatId() const = 0;
    virtual GridData2D * read(QIODevice * device, bool interactive = false, QWidget * parent = 0) = 0;
};


class AsciiImportFormat : public ImportFormat {
protected:
    bool readAsciiLine(QIODevice *device, double *x, double *y, double *value);
};

/**
 * Use this class if your individual spectra are stored in separated files.
 */
class AsciiMultiFileImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const;
    QString actionName() const;
    GridData2D * read(QIODevice * device, bool interactive, QWidget * parent);
    int formatId() const { return 1; }
protected:
    Data1D * readSingleAsciiFile(QIODevice * device);
    GridData2D * read(QMap<double, QString>files, QWidget *parent);
};


class AsciiSerialImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const;
    QString actionName() const;
    GridData2D * read(QIODevice * device, bool interactive, QWidget * parent);
    int formatId() const { return 2; }
};


class AsciiMatrixImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const;
    QString actionName() const;
    GridData2D * read(QIODevice * device, bool interactive, QWidget * parent);
    int formatId() const { return 3; }
};


class DataReader {
public:

    /**
     * Open a file
     * @param fileName a file to open
     */
    static void read(Engine * engine, const QString &filename, int formatid = -1, bool interactive = false);

    static QList<QSharedPointer<ImportFormat> > formats();
};





#endif // DATAIMPORT_H
