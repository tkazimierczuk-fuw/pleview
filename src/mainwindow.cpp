#include "mainwindow.h"
#include "configdialog.h"
#include "MapItem.h"
#include "colormapslider.h"
#include "plotaddonsgui.h"
#include "datafiltersgui.h"
#include "dataimport.h"
#include "dataexport.h"
#include "pleview.h"

#ifndef PLEVIEW_VERSION
#define PLEVIEW_VERSION
#endif
const char app_name[] = "PLEview " PLEVIEW_VERSION;

MainWindow::MainWindow(Engine * model) : engine(model) {
    //createWidgets();
    createActions();
    setupUi();

    QDockWidget * dock = Pleview::log()->dockWidget();
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    dock->hide();

    QDockWidget * colordock = new ColorDock(model);
    addDockWidget(Qt::BottomDockWidgetArea, colordock);
    colordock->hide();
    toolBar->addAction(colordock->toggleViewAction());


    QDockWidget * xsectiondock = new XSectionDock(model);
    addDockWidget(Qt::BottomDockWidgetArea, xsectiondock);
    xsectiondock->hide();
    toolBar->addAction(xsectiondock->toggleViewAction());


    for(int direction = 0; direction < 2; direction++) {
        connect(engine, SIGNAL(crossSectionChanged(int,QVector<double>,QVector<double>)),
                crossPlot[direction], SLOT(updateCrossSection(int,QVector<double>,QVector<double>)));
        connect(engine, SIGNAL(crossSectionChanged(int,double,double,double)),
                crossPlot[direction], SLOT(updateMarker(int,double,double,double)));
        connect(moveAction, SIGNAL(triggered()), crossPlot[direction], SLOT(setMarkerMode()));
        connect(zoomAction, SIGNAL(triggered()), crossPlot[direction], SLOT(setZoomMode()));
    }

    for(int direction = 0; direction < 2; direction++) {
        engine->setCrossSection(direction, 0, true);
        connect(crossPlot[direction], SIGNAL(markerMoved(int, double)),
                engine, SLOT(setCrossSection(int, double)));

        connect(engine->pluginManager, SIGNAL(addonAdded(PlotAddon*)),
                crossPlot[direction], SLOT(attachPlugin(PlotAddon*)));
    }

    this->setAcceptDrops(true);

    enableActions();

    engine->setSvgRenderer(mapPlot);
    connect(engine, SIGNAL(crossSectionChanged(int,double,double,double)), mapPlot, SLOT(setMarker(int,double,double,double)));
    connect(mapPlot, SIGNAL(markerMoved(int,double)), engine, SLOT(setCrossSection(int,double)));
    connect(engine, SIGNAL(dataChanged(const GridData2D *)), mapPlot, SLOT(setData(const GridData2D*)));
    connect(engine, SIGNAL(colorMapChanged(ColorMap)), mapPlot, SLOT(setColor(ColorMap)));
    connect(engine->pluginManager, SIGNAL(addonAdded(PlotAddon*)), mapPlot, SLOT(attachPlugin(PlotAddon*)));

    connect(mapPlot->picker1, SIGNAL(selected(QPointF)), activePluginsDialog, SLOT(button1clicked(QPointF)));
    connect(mapPlot->picker2, SIGNAL(selected(QPointF)), activePluginsDialog, SLOT(button2clicked(QPointF)));

    connect(actionPrint, SIGNAL(triggered()), mapPlot, SLOT(print()));
    connect(actionExportImage, SIGNAL(triggered()), mapPlot, SLOT(exportImage()));

    connect(dataFiltersDialog, SIGNAL(importTransform(QIODevice*,Engine::Contents)), model, SLOT(loadData(QIODevice*,Engine::Contents)));
}


void MainWindow::setupUi() {
    this->resize(422, 400);
    setWindowIcon(QIcon(":/pleview/misc/logo.png"));
    setWindowTitle(app_name);

    QTabWidget *tabWidget = new QTabWidget(this);

    mapPlot = new MapWithMarker(engine->plotRangesManager);
    tabWidget->addTab(mapPlot, "Map");


    QWidget *tab = new QWidget();
    QVBoxLayout * verticalLayout = new QVBoxLayout(tab);

    for(int direction = 0; direction < 2; direction++) {
        crossPlot[direction] = new PlotWithMarker(direction, engine->plotRangesManager);
        verticalLayout->addWidget(crossPlot[direction]);
        connect(actionSelectPlot[direction], SIGNAL(toggled(bool)),
                crossPlot[direction], SLOT(select(bool)));
        connect(crossPlot[direction], SIGNAL(selected(bool)),
                actionSelectPlot[direction], SLOT(setChecked(bool)));
    }
    crossPlot[0]->select();

    tabWidget->addTab(tab, "Cross-sections");

    this->setCentralWidget(tabWidget);

    QMenuBar * menubar = new QMenuBar(this);
    QMenu * menuFile = new QMenu("&File", menubar);
    QMenu * menuEdit = new QMenu("&Edit", menubar);
    QMenu * menuHelp = new QMenu("&Help", menubar);
    this->setMenuBar(menubar);

    statusbar = new QStatusBar(this);
    statusbar->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
    statusbar->insertWidget(0, Pleview::log()->statusBarWidget(), 1);
    this->setStatusBar(statusbar);

    toolBar = new QToolBar(this);
    toolBar->setWindowTitle("Main toolbar");
    toolBar->addAction(actionOpen);
    toolBar->addAction(actionSave);
    toolBar->addSeparator();
    toolBar->addAction(actionCopy);
    toolBar->addSeparator();
    toolBar->addAction(zoomAction);
    toolBar->addAction(moveAction);
    toolBar->addSeparator();
    toolBar->addAction(plotConfigAction);
    toolBar->addAction(colorMapAction);
    this->addToolBar(Qt::TopToolBarArea, toolBar);

    menubar->addAction(menuFile->menuAction());
    menubar->addAction(menuEdit->menuAction());
    menubar->addAction(menuHelp->menuAction());

    menuFile->addAction(actionOpen);
    menuFile->addAction(actionSave);
    menuFile->addAction(actionSaveAs);
    menuFile->addSeparator();

    QMenu * menuImport = new QMenu("Import", this);
    menuImport->setIcon(QIcon(":/icons/actions/file_import.svg"));
    menuFile->addMenu(menuImport);
    foreach(QAction * action, actionImport) {
        menuImport->addAction(action);
    }

    QMenu * menuExport = new QMenu("Export", this);
    menuExport->setIcon(QIcon(":/icons/actions/file_export.svg"));
    menuExport->addAction(actionExportAscii);
    menuExport->addAction(actionExportTwoColumn);
    menuExport->addAction(actionExportImage);
    menuFile->addMenu(menuExport);

    menuFile->addSeparator();
    menuFile->addAction(actionPrint);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);

    menuEdit->addAction(actionCopy);
    menuEdit->addSeparator();
    menuEdit->addAction(plotConfigAction);
    menuEdit->addAction(actionAddPlugin);
    menuEdit->addAction(actionActivePlugins);
    menuEdit->addAction(actionDataFilters);

    menuHelp->addAction(actionAbout);

    tabWidget->setCurrentIndex(1);

    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(enableActions()));
}


void MainWindow::createActions() {
    actionOpen = new QAction(QIcon(":/icons/actions/document-open.svg"), "&Open", this);
    actionOpen->setShortcut(QKeySequence("Ctrl+O"));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    actionSave = new QAction(QIcon(":/icons/actions/document-save.svg"), "&Save", this);
    actionSave->setShortcut(QKeySequence("Ctrl+S"));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));

    actionSaveAs = new QAction(QIcon(":/icons/actions/document-save-as.svg"), "Save as", this);
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

    actionExit = new QAction("E&xit", this);
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    actionCopy = new QAction(QIcon(":/icons/actions/edit-copy.svg"), "&Copy", this);
    actionCopy->setShortcut(QKeySequence("Ctrl+C"));
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));

    actionAbout = new QAction("&About", this);
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    colorMapAction = new QAction(QIcon(":/icons/actions/colormap.png"), "Configure color scale", this);
    connect(colorMapAction, SIGNAL(triggered()), this, SLOT(configColorMap()));

    QActionGroup * mouseGroup = new QActionGroup(this);
    mouseGroup->setExclusive(true);

    zoomAction = new QAction(QIcon(":/icons/actions/zoom.svg"), "Zoom mode", this);
    zoomAction->setCheckable(true);
    zoomAction->setChecked(true);
    mouseGroup->addAction(zoomAction);
    QToolButton * zoomButton = new QToolButton();
    zoomButton->setDefaultAction(zoomAction);

    moveAction = new QAction(QIcon(":/icons/actions/transform-move.svg"), "Marker mode", this);
    moveAction->setCheckable(true);
    mouseGroup->addAction(moveAction);
    QToolButton * moveButton = new QToolButton();
    moveButton->setDefaultAction(moveAction);

    plotConfigAction = new QAction(QIcon(":/icons/actions/configure.svg"), "Configure", this);
    connect(plotConfigAction, SIGNAL(triggered()), this, SLOT(config()));

    actionAddPlugin = new QAction(QIcon(":/icons/actions/plugin-add.png"), "Load a plugin", this);
    connect(actionAddPlugin, SIGNAL(triggered()), this, SLOT(addPlugin()));

    actionActivePlugins = new QAction(QIcon(":/icons/actions/plugin.png"), "Active plugin dialog", this);
    actionActivePlugins->setCheckable(true);

    activePluginsDialog = new PlotAddonsDialog(engine->pluginManager, this);
    connect(actionActivePlugins, SIGNAL(toggled(bool)), activePluginsDialog, SLOT(setVisible(bool)));
    connect(activePluginsDialog, SIGNAL(finished(int)), actionActivePlugins, SLOT(toggle()));

    actionDataFilters = new QAction(QIcon(":/icons/actions/filter.png"), "Transformations dialog", this);
    actionDataFilters->setCheckable(true);

    dataFiltersDialog = new DataFiltersGui(engine->transformManager, this);
    connect(actionDataFilters, SIGNAL(toggled(bool)), dataFiltersDialog, SLOT(setVisible(bool)));
    connect(dataFiltersDialog, SIGNAL(finished(int)), actionDataFilters, SLOT(toggle()));

    QActionGroup * selectGroup = new QActionGroup(this);
    selectGroup->setExclusive(true);
    for(int direction = 0; direction < 2; direction++) {
        actionSelectPlot[direction] = new QAction(this);
        actionSelectPlot[direction]->setCheckable(true);
        selectGroup->addAction(actionSelectPlot[direction]);
    }

    actionExportImage = new QAction(QIcon(":/icons/actions/file_export.svg"), "Export map as image", this);
    actionExportAscii = new QAction(QIcon(":/icons/actions/file_export.svg"), "Export map as ASCII", this);
    actionExportTwoColumn = new QAction(QIcon(":/icons/actions/file_export.svg"), "Export map as 2-column ASCII", this);
    connect(actionExportAscii, SIGNAL(triggered()), this, SLOT(exportAscii()));
    connect(actionExportTwoColumn, SIGNAL(triggered()), this, SLOT(exportAsciiTwoColumn()));

    QSignalMapper * mapper = new QSignalMapper(this);
    foreach(QSharedPointer<ImportFormat> format, DataReader::formats()) {
        QAction * action = new QAction(QIcon(":/icons/actions/file_import.svg"), format->actionName(), this);
        mapper->setMapping(action, format->formatId());
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
        actionImport.append(action);
    }
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(import(int)));

    actionPrint = new QAction(QIcon(":/icons/actions/fileprint.svg"), "Print map", this);   
}


void MainWindow::exportAscii() {
    DataExport::exportAscii(this, engine);
}


void MainWindow::exportAsciiTwoColumn() {
    DataExport::exportAsciiTwoColumn(this, engine);
}


void MainWindow::saveAs() {
    QString fname = QFileDialog::getSaveFileName(this, "Save file", "", "PLEview files (*.plvz)");
    if(!fname.isEmpty()) {
        if(!fname.endsWith(".plvz"))
            fname = fname + ".plvz";
        fileName = fname;
        save(fname);

        QFileInfo fileinfo(fname);
        QDir::setCurrent(fileinfo.absolutePath());
    }
}


void MainWindow::save() {
    if(fileName.endsWith(".plvz"))
        save(fileName);
    else
        saveAs();
}


void MainWindow::save(const QString &fileName) {
    QFile file(fileName);
    setWindowTitle(QString(app_name) + " - " + QFileInfo(file).fileName());
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "IO error", "Unable to open file:" + fileName);
        return; /* error opening the file */
    }
    engine->save(&file);
    file.close();
}


void MainWindow::open(QString fname) {
    if(fname.isEmpty()) {
        QString filters = "Any supported file (*.plvz *.ple *.txt)";

        foreach(QSharedPointer<ImportFormat> format, DataReader::formats()) {
            filters.append(";;");
            filters.append(format->fileDialogFilter());
        }

        fname = QFileDialog::getOpenFileName(this, "Open file", "", filters);
    }

    if(!fname.isEmpty()) {
        DataReader::read(engine, fname);
        this->fileName = fname;
        setWindowTitle(QString(app_name) + " - " + QFileInfo(fname).completeBaseName()+"."+QFileInfo(fname).suffix());
        enableActions();
    }
}


void MainWindow::import(int formatId) {
    QString filters;
    foreach(QSharedPointer<ImportFormat> format, DataReader::formats()) {
        if(format->formatId() == formatId)
            filters = format->fileDialogFilter();
    }

    if(filters.isEmpty())
        return;

    filters.append(";;Any file (*.*)");


    QString fname = QFileDialog::getOpenFileName(this, "Import data", "", filters);

    if(!fname.isEmpty()) {
        DataReader::read(engine, fname, formatId);
        this->fileName = fname;
        setWindowTitle(QString(app_name) + " - " + QFileInfo(fname).completeBaseName()+"."+QFileInfo(fname).suffix());
        enableActions();
    }
}


void MainWindow::about() {
    QMessageBox::about(this, "About PLEview",   
        "<p style=\"-qt-block-indent:0; text-indent:0px; font-size:16pt; font-weight:600\">PLEview - next generation<span style=\"font-size:10pt;\">"
        " v" PLEVIEW_VERSION "</span></p>\n"
        "<p>This program is intentended to help with 2D data analysis, e.g. in PLE experiments where excitation and detection energies are two independent parameters.</p>\n"
        "<p>The program is released under GPL 2 license. You can use it, you can copy it, you can modify it.</p>\n"
        "<p>Copyright (C) 2009 Tomasz Kazimierczuk &lt;<a href=\"mailto:Tomasz.Kazimierczuk@fuw.edu.pl\" style=\"text-decoration: underline; color:#0000ff;\">Tomasz.Kazimierczuk@fuw.edu.pl</a>&gt;</p>"
    );
}


void MainWindow::configColorMap() {
    ConfigDialog dialog(engine);
    dialog.setPage(3); // set color page active
    dialog.exec();
}


void MainWindow::config() {
    ConfigDialog dialog(engine);
    dialog.exec();
}


void MainWindow::enableActions() {
    bool hasData = (engine->isValid());
    bool isMapVisible = (mapPlot->isVisible());

    actionSave->setEnabled(hasData);
    actionSaveAs->setEnabled(hasData);
    actionCopy->setEnabled(hasData && !isMapVisible);
    plotConfigAction->setEnabled(hasData);
    colorMapAction->setEnabled(hasData && isMapVisible);
    actionAddPlugin->setEnabled(hasData);
    actionActivePlugins->setEnabled(hasData);
}


void MainWindow::copy() {
    int axis = (actionSelectPlot[0]->isChecked() ? 0 : 1);
    QString string;
    QTextStream out(&string);
    QVector<QPointF> points = engine->crossSection(axis);
    for(int i = 0; i < points.size(); i++) {
        out << points[i].x() << "\t" << points[i].y() << "\n";
    }
    out.flush();
    QApplication::clipboard()->setText(string);
}


void MainWindow::addPlugin() {
    PlotAddonFactory * plugin = PlotAddonManager::getAddonByDialog(this);
    if(plugin) {
        PlotAddon * obj = plugin->instantiate();
        engine->pluginManager->add(obj);
        this->actionActivePlugins->setChecked(true);
    }
}

/** Alternative opening method: drop a file */
void MainWindow::dropEvent(QDropEvent *event) {
    if(!event->mimeData()->hasUrls()) {
        event->ignore();
        return;
    }

    if(event->mimeData()->urls().isEmpty()) // should never happen if hasUrls() is true
        return;

    QUrl url = event->mimeData()->urls().first();

    Pleview::log()->info(QString("Drop event with text-uri:  %1").arg(url.toString()));
    QString fname = url.toLocalFile();
    this->open(fname);
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}
