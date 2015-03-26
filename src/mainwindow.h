#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "model.h"
#include "plotwithmarker.h"
#include "mapwithmarker.h"
#include "qwt_plot_curve.h"
#include "MapItem.h"
#include "plotaddonsgui.h"
#include "datafiltersgui.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
     /** Constructor.
       * @param model a model (cannot be deleted before MainWindow)
       */
    MainWindow(Engine * model);

public slots:
    /** Open a file selected from a dialog.
      * @see open(QString)
      */
   void open(QString fileName = QString());

   /**
     * Import a file selected from a dialog. Parameter formatId identifies
     * proper ImportFormat class.
     */
   void import(int formatId);

private slots:
    /**
      * Save current state of application.
      * @see saveAs()
      * @see save(QString)
      */
    void save();

    /**
      * Copy current cross-section to clipboard.
      */
    void copy();

    /**
      * Save current state of application in a new file
      * @see save()
      * @see save(QString)
      */
    void saveAs();

     /** Display "About" dialog    */
    void about();

    /** Display dialog with available plugins */
    void addPlugin();

    /** Set isEnabled flag for actions */
    void enableActions();

    /** Export current map plot to text file */
    void exportAscii();
    void exportAsciiTwoColumn();

protected:
    /** Drag&drop a file to the window to open */
    void dropEvent (QDropEvent * event);
    void dragEnterEvent(QDragEnterEvent *event);

private:
     /** Instantiate all QAction objects */
    void createActions();



    /**
      * Save current state of application into given file.
      * @param fileName a file name to save
      * @see save()
      * @see saveAs()
      */
    void save(const QString &fileName);


private:
    QString fileName;

    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionSave, *actionSaveAs;
    QAction *actionCopy;
    QAction *actionAddPlugin, *actionActivePlugins, *actionDataFilters;
    QAction *actionExport, *actionExportImage, *actionExportAscii, *actionExportTwoColumn;
    QAction *actionPrint;
    QAction *transformAxesAction;
    QList<QAction*> actionImport;

    QAction *zoomAction, *moveAction;
    QAction *plotConfigAction, *colorMapAction;

    PlotAddonsDialog * activePluginsDialog;
    DataFiltersGui * dataFiltersDialog;

    QStatusBar *statusbar;
    QToolBar *toolBar;

    MapWithMarker *mapPlot;

    QAction *actionSelectPlot[2];
    PlotWithMarker * crossPlot[2];

    Engine * engine;

    void setupUi();
};


#endif // MAINWINDOW_H
