#ifndef DATAIMPORT_H
#define DATAIMPORT_H

#include <QtGui>

#include "data1d.h"
#include "data2d.h"
#include "model.h"

/**
 * @brief The ImportFormat class is an interface for importing grid 2D data
 */
class ImportFormat {
public:
    //! fileDialogFilter() returns standard extensions for given format
    virtual QString fileDialogFilter() const = 0;

    //! actionName() returns a string like "Import ASCII (two-column)", which will appear in subbmenu "Import data | ..."
    virtual QString actionName() const = 0;

    //! ???
    virtual int formatId() const = 0;

    //! Actual reading function. Returns nullptr if failed.
    virtual std::unique_ptr<GridData2D> read(QIODevice * device, bool interactive = false, QWidget * parent = 0) = 0;
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
    QString fileDialogFilter() const override;
    QString actionName() const override;
    std::unique_ptr<GridData2D> read(QIODevice * device, bool interactive, QWidget * parent) override;
    int formatId() const override { return 1; }
protected:
    Data1D * readSingleAsciiFile(QIODevice * device);
    std::unique_ptr<GridData2D> read(QMap<double, QString>files, QWidget *parent);
};


class AsciiSerialImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const override;
    QString actionName() const override;
    std::unique_ptr<GridData2D> read(QIODevice * device, bool interactive, QWidget * parent) override;
    int formatId() const override { return 2; }
};


class AsciiMatrixImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const override;
    QString actionName() const override;
    std::unique_ptr<GridData2D> read(QIODevice * device, bool interactive, QWidget * parent) override;
    int formatId() const override { return 3; }
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
