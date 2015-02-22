#ifndef DATAEXPORT_H
#define DATAEXPORT_H

#include <QtGui>
#include "qwt_plot.h"
#include "model.h"

/**
  * Class responsible for "Export as ASCII" feature.
  */
class DataExport
{
public:
    /** Export a data to the ASCII file */
    static void exportAscii(QWidget * parent, Engine * engine);

    //! Export data to bitmap file
    static void exportBitmap(QwtPlot * plot);

    static void exportAsciiTwoColumn(QWidget * parent, Engine * engine);

protected:
    static bool initialized;
    static int xpts, ypts;
    static double xmin, xmax, ymin, ymax;
};


/**
 * A dialog to adjust parameters of exported image.
 */
class BitmapExportDialog : public QDialog {
    Q_OBJECT
public:
    BitmapExportDialog(const QString &fileName, QwtPlot * plot);
    ~BitmapExportDialog() {}
    void applyChanges(QwtPlot * plot);
    void undoChanges(QwtPlot * plot);

private slots:
    void selectFont();
    void hideXHair(bool);

protected:
    QFont baseFont, currentFont;
    bool xHairHiding;
};


#endif // DATAEXPORT_H
