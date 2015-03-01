#include "configdialog.h"

#include <QtGui>

ConfigDialog::ConfigDialog(Engine * engine) : _engine(engine)
{
     contentsWidget = new QListWidget;
     contentsWidget->setViewMode(QListView::IconMode);
     contentsWidget->setIconSize(QSize(96, 50));
     contentsWidget->setMovement(QListView::Static);
     contentsWidget->setMaximumWidth(128);
     contentsWidget->setMinimumHeight(280);
     //contentsWidget->setSpacing(12);  // bug until QT 4.7.0

     pagesWidget = new QStackedWidget;
     Page * page = new CrossSectionPage(engine);
     pagesWidget->addWidget(page);
     pages.append(page);

     page = new DataPage(engine);
     pagesWidget->addWidget(page);
     pages.append(page);

     page = new AxisScalePage(engine);
     pagesWidget->addWidget(page);
     pages.append(page);

     page = new ColorPage(engine);
     pagesWidget->addWidget(page);
     pages.append(page);


     QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
     connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


     createIcons();
     contentsWidget->setCurrentRow(0);

     QHBoxLayout *horizontalLayout = new QHBoxLayout;
     horizontalLayout->addWidget(contentsWidget);
     horizontalLayout->addWidget(pagesWidget, 1);


     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addLayout(horizontalLayout);
     mainLayout->addSpacing(12);

     QDialogButtonBox * bbox = new QDialogButtonBox();
     bbox->setStandardButtons(QDialogButtonBox::Ok |
                              QDialogButtonBox::Cancel |
                              QDialogButtonBox::Help);
     connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
     connect(bbox, SIGNAL(helpRequested()), this, SLOT(displayHelp()));
     mainLayout->addWidget(bbox);

     setLayout(mainLayout);
     setWindowTitle(tr("Config Dialog"));
}

void ConfigDialog::createIcons()
{
     QListWidgetItem *crossSectionButton = new QListWidgetItem(contentsWidget);
     crossSectionButton->setIcon(QIcon(":/pleview/misc/config_upper.png"));
     crossSectionButton->setText(tr("Cross-sections"));
     crossSectionButton->setTextAlignment(Qt::AlignHCenter);
     crossSectionButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     QListWidgetItem *axesButton = new QListWidgetItem(contentsWidget);
     axesButton->setIcon(QIcon(":/pleview/misc/config_axes.png"));
     axesButton->setText(tr("XY values"));
     axesButton->setTextAlignment(Qt::AlignHCenter);
     axesButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     QListWidgetItem *axisScaleButton = new QListWidgetItem(contentsWidget);
     axisScaleButton->setIcon(QIcon(":/pleview/misc/config_axis_scales.png"));
     axisScaleButton->setText(tr("Axis scales"));
     axisScaleButton->setTextAlignment(Qt::AlignHCenter);
     axisScaleButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     QListWidgetItem *colorsButton = new QListWidgetItem(contentsWidget);
     colorsButton->setIcon(QIcon(":/pleview/misc/config_colors.png"));
     colorsButton->setText(tr("Map colors"));
     colorsButton->setTextAlignment(Qt::AlignHCenter);
     colorsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


     connect(contentsWidget,
             SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
             this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void ConfigDialog::setPage(int index) {
    contentsWidget->setCurrentRow(index);
//    pagesWidget->setCurrentIndex(index);
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

     pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void ConfigDialog::displayHelp() {
    int index = pagesWidget->currentIndex();
    if(index >= 0 && index < pages.size())
        pages[index]->displayHelp();
}


void ConfigDialog::accept() {
    for(int i = 0; i < pages.size(); i++)
        pages[i]->commit(_engine);
    QDialog::accept();
}

void ConfigDialog::reject() {
    for(int i = 0; i < pages.size(); i++)
        pages[i]->cancel(_engine);
    QDialog::reject();
}


// ------------------ Pages ---------------


DataPage::DataPage(Engine * engine, QWidget *parent)
                        : Page(parent)
{
    QString axisName[2] = {QString("x"), QString("y")};
    QFormLayout * layout = new QFormLayout();
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int axis = 0; axis< 2; axis++) {
        typeCombo[axis] = new QComboBox();
        typeCombo[axis]->addItem("From file");
        typeCombo[axis]->addItem("0, 1, 2, ...");
        connect(typeCombo[axis], SIGNAL(activated(int)), signalMapper, SLOT(map()));
        signalMapper->setMapping(typeCombo[axis], axis);
        layout->addRow(axisName[axis] + " values", typeCombo[axis]);

        enableTransformBox[axis] = new QCheckBox("enabled");
        enableTransformBox[axis]->setChecked(true);
        connect(enableTransformBox[axis], SIGNAL(toggled(bool)), signalMapper, SLOT(map()));
        signalMapper->setMapping(enableTransformBox[axis], axis);
        layout->addRow("Transform " + axisName[axis] + " axis", enableTransformBox[axis]);

        transformEdit[axis] = new ParserComboWidget();
        transformEdit[axis]->setMinimumWidth(250);
        dummy = 0.;
        transformEdit[axis]->parser()->DefineVar("t", &dummy);

        QStringList expressions = Pleview::settings()->value(QString("transformations")+Pleview::directionString((Pleview::Direction) axis), QStringList()).toStringList();
        if(expressions.size() > 0)
            transformEdit[axis]->setTexts(expressions);

        transformEdit[axis]->parser()->DefineVar(axisName[axis].toStdString(), &dummy);
        //transformEdit[axis]->setToolTip("An expression where 't' is original value, e.g. '1239.8/t'");
        connect(transformEdit[axis], SIGNAL(expressionEdited(QString)), signalMapper, SLOT(map()));
        signalMapper->setMapping(transformEdit[axis], axis);
        layout->addRow(axisName[axis] + " axis transformation", transformEdit[axis]);

        connect(enableTransformBox[axis], SIGNAL(toggled(bool)),
                transformEdit[axis], SLOT(setEnabled(bool)));

        QFrame * frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        layout->addRow(frame);
    }
    setLayout(layout);
    init(engine);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(emitSignal(int)));
    connect(this, SIGNAL(axisConfigurationChanged(int,AxisConfiguration)),
            engine, SLOT(setAxisConfiguration(int,AxisConfiguration)));
}

void DataPage::emitSignal(int axis) {
    AxisConfiguration config;
    if(typeCombo[axis]->currentIndex() == 0)
        config.setType(AxisConfiguration::FromFile);
    else
        config.setType(AxisConfiguration::Natural);
    config.setTransform(transformEdit[axis]->text());
    config.enableTransform(enableTransformBox[axis]->isChecked());

    currentConfiguration[axis] = config;
    emit(axisConfigurationChanged(axis, config));
}


void DataPage::init(Engine *engine) {
    for(int axis = 0; axis < 2; axis++) {
        axisConfiguration[axis] = engine->axisConfiguration(axis);
        currentConfiguration[axis] = axisConfiguration[axis];

        QString transformString = axisConfiguration[axis].transform().trimmed();
        QRegExp rx = QRegExp("\\s*"+QRegExp::escape(transformString)+"\\s*");
        int index = transformEdit[axis]->texts().indexOf(rx);
        if(index >= 0)
            transformEdit[axis]->setCurrentIndex(index);
        else
            transformEdit[axis]->setText(transformString);

        enableTransformBox[axis]->setChecked(axisConfiguration[axis].isTransformEnabled());
        if(axisConfiguration[axis].type() == AxisConfiguration::FromFile)
            typeCombo[axis]->setCurrentIndex(0);
        else
            typeCombo[axis]->setCurrentIndex(1);
    }
}


void DataPage::cancel(Engine * engine) {
    for(int axis = 0; axis < 2; axis++)
        emit(axisConfigurationChanged(axis, axisConfiguration[axis]));
}


void DataPage::commit(Engine * engine) {
    for(int axis = 0; axis < 2; axis++) {
        axisConfiguration[axis] = currentConfiguration[axis];
        Pleview::settings()->setValue(QString("transformations")+Pleview::directionString((Pleview::Direction) axis), transformEdit[axis]->texts());
    }
}



CrossSectionPage::CrossSectionPage(Engine * engine, QWidget *parent)
     : Page(parent)
 {
    QFormLayout *layout = new QFormLayout;

    QString axisName[2] = {QString("x"), QString("y")};

    for(int axis = 0; axis < 2; axis++) {
        widthEdit[axis] = new QLineEdit(this);
        widthEdit[axis]->setValidator(new QIntValidator(1, 501, this));
        widthEdit[axis]->setText(QString::number(engine->crossSectionWidth(axis)));

        layout->addRow("Number of pixels for " + axisName[axis] + " cross-section",
                       widthEdit[axis]);

        QFrame * frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        layout->addRow(frame);
    }
    setLayout(layout);
}


void CrossSectionPage::commit(Engine * engine) {
    for(int axis = 0; axis < 2; axis++)
        engine->setCrossSectionWidth(axis, widthEdit[axis]->text().toInt());
}



ColorPage::ColorPage(Engine * engine, QWidget *parent)
        : Page(parent) {
    configWidget = new ColorMapConfig(this, engine->colorMap());
    QLayout *layout = new QVBoxLayout();
    layout->addWidget(configWidget);
    this->setLayout(layout);

    connect(configWidget, SIGNAL(colorMapChanged(ColorMap)),
            engine, SLOT(setColorMap(ColorMap)));
}


void ColorPage::commit(Engine * engine) {
    configWidget->setColorMap(configWidget->currentColorMap());
}

void ColorPage::cancel(Engine * engine) {
    configWidget->cancel();
}


AxisScalePage::AxisScalePage(Engine * engine, QWidget *parent)
        : Page(parent) {
    QString axisName[2] = {QString("x"), QString("y")};
    PlotRangesManager::PlotAxis mapping[2] = {PlotRangesManager::X, PlotRangesManager::Y};
    QFormLayout * layout = new QFormLayout();
    for(int axis = 0; axis< 2; axis++) {
        autoOption[axis] = new QCheckBox();
        layout->addRow("Autoscale " + axisName[axis] + " axis", autoOption[axis]);

        minValueEdit[axis] = new QLineEdit();
        QDoubleValidator * validator = new QDoubleValidator(this);
        minValueEdit[axis]->setValidator(validator);
        layout->addRow("Minimum " + axisName[axis] + " value", minValueEdit[axis]);


        maxValueEdit[axis] = new QLineEdit();
        maxValueEdit[axis]->setValidator(validator);
        layout->addRow("Maximum " + axisName[axis] + " value", maxValueEdit[axis]);

        connect(autoOption[axis], SIGNAL(toggled(bool)), minValueEdit[axis], SLOT(setDisabled(bool)));
        connect(autoOption[axis], SIGNAL(toggled(bool)), maxValueEdit[axis], SLOT(setDisabled(bool)));

        if(engine->plotRangesManager->isAutoScaled(mapping[axis]))
            autoOption[axis]->setChecked(true);
        else {
            QPair<double,double> range = engine->plotRangesManager->currentRange(mapping[axis]);
            minValueEdit[axis]->setText(QString::number(range.first));
            maxValueEdit[axis]->setText(QString::number(range.second));
        }

        QFrame * frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        layout->addRow(frame);
    }

    this->setLayout(layout);

    connect(this, SIGNAL(autoscaleRequested(PlotRangesManager::PlotAxis)), engine->plotRangesManager, SLOT(unzoomAll(PlotRangesManager::PlotAxis)));
    connect(this, SIGNAL(rangeRequested(PlotRangesManager::PlotAxis,double,double)), engine->plotRangesManager, SLOT(setRange(PlotRangesManager::PlotAxis,double,double)));
}


void AxisScalePage::commit(Engine * engine) {
    PlotRangesManager::PlotAxis mapping[2] = {PlotRangesManager::X, PlotRangesManager::Y};
    for(int axis = 0; axis < 2; axis++) {
        if(autoOption[axis]->isChecked())
            emit(autoscaleRequested(mapping[axis]));
        else
            emit(rangeRequested(mapping[axis], minValueEdit[axis]->text().toDouble(), maxValueEdit[axis]->text().toDouble()));
    }    
}

void AxisScalePage::cancel(Engine * engine) {
}


ColorDock::ColorDock(Engine *engine) {
    QFrame * frame = new QFrame();
    QHBoxLayout * layout = new QHBoxLayout();

    configWidget = new ColorMapConfig(this, engine->colorMap(), false);
    connect(configWidget, SIGNAL(colorMapChanged(ColorMap)), engine, SLOT(setColorMap(ColorMap)));
    connect(engine, SIGNAL(colorMapChanged(ColorMap)), configWidget, SLOT(setColorMap(ColorMap)));
    configWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    layout->addWidget(configWidget);

    QToolButton * button = new QToolButton();
    button->setToolTip("Advanced color scale editing");
    button->setIcon(QIcon(":/icons/actions/gear.svg"));
    connect(button, SIGNAL(clicked()), this, SLOT(execDialog()));
    layout->addWidget(button);

    frame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    frame->setLayout(layout);

    setWidget(frame);
    setWindowTitle("Color scale");
    toggleViewAction()->setIcon(QIcon(":/icons/actions/colormap.png"));
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
}


void ColorDock::execDialog() {
    QDialog * dialog = new QDialog(this);
    QFormLayout * layout = new QFormLayout();

    ColorMapConfig * fullConfig = new ColorMapConfig(this, configWidget->currentColorMap());
    layout->addRow(fullConfig);

    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addRow(buttons);

    dialog->setLayout(layout);

    if(dialog->exec()) {
        emit configWidget->colorMapChanged(fullConfig->currentColorMap());
    }
    delete dialog;
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
    toggleViewAction()->setIcon(QIcon(":/icons/action/configXsection.svg"));
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
}


void XSectionDock::xXsectionChanged(int width) {
    emit xsectionChanged(0, width);
}


void XSectionDock::yXsectionChanged(int width) {
    emit xsectionChanged(1, width);
}

