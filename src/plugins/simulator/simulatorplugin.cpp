#include "simulatorplugin.h"

const int ncurves = 4;

SimulatorPluginObject::SimulatorPluginObject()
        : PlotAddon(new SimulatorPlugin())
{
    prepareUi();

    // for ncurves == 4
    QVector<double> dummy;
    dummy.append(0); // must not be empty
    curves.addCurve(Qt::red, QPolygonF(), dummy );
    curves.addCurve(Qt::blue, QPolygonF(), dummy );
    curves.addCurve(Qt::green, QPolygonF(), dummy );
    curves.addCurve(Qt::yellow, QPolygonF(), dummy );
}

SimulatorPluginObject::~SimulatorPluginObject()
{
    if(_frame)
        delete _frame;
}

void SimulatorPluginObject::attach(QwtPlot * plot, PlotType type) {
    if(type != Map)
        return;

    curves.addCurve(Qt::green, QPolygonF());
    curves.attachCurves(plot);
    connect(this, SIGNAL(changed()), plot, SLOT(replot()));
}

void SimulatorPluginObject::init(Engine * engine) {
}

void SimulatorPluginObject::prepareUi() {
    _frame = new QFrame();
    layout = new QFormLayout();

    QHBoxLayout * hlayout = new QHBoxLayout();
    fileNameEdit = new QLineEdit();
    hlayout->addWidget(fileNameEdit);
    QToolButton *fileNameButton = new QToolButton();
    fileNameButton->setText("...");
    connect(fileNameButton, SIGNAL(clicked()),
            this, SLOT(getFileName()));
    hlayout->addWidget(fileNameButton);
    layout->addRow("Simulator executable", hlayout);

    QPushButton *getParametersButton = new QPushButton("Query parameters");
    layout->addRow(getParametersButton);
    connect(getParametersButton, SIGNAL(clicked()),
            this, SLOT(queryParameters()));

    QFrame * frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    layout->addRow(frame);

    QPushButton *runButton = new QPushButton("Run simulation");
    runButton->setDefault(true);
    layout->addRow(runButton);
    connect(runButton, SIGNAL(clicked()),
            this, SLOT(runSimulation()));

    _frame->setLayout(layout);
}

QByteArray SimulatorPluginObject::startProcess(const QByteArray & inputData) {
    QString fname = this->fileNameEdit->text();
    QProcess process(this);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    process.start(fname);
    if(process.isWritable())
        process.write(inputData);
    process.waitForFinished(10000); // wait up to 10s
    QApplication::restoreOverrideCursor();

    if(process.exitStatus() == QProcess::NormalExit
       && process.error() == QProcess::UnknownError) {
        return process.readAllStandardOutput();
    }
    else {
        QMessageBox::critical(0, "Error", "Execution of external application "
                              "failed");
        return QByteArray();
    }
}


void SimulatorPluginObject::getFileName() {
    QString fname = QFileDialog::getOpenFileName(0, "Select simulator executable");
    if(!fname.isEmpty())
        this->fileNameEdit->setText(fname);
}


void SimulatorPluginObject::runSimulation() {
   QByteArray inputData;
   QTextStream inputStream(&inputData);
   inputStream << "parameters:\n";
   foreach (QLineEdit * edit, _valueEdits) {
       inputStream << edit->text() << "\n";
   }
   // inputStream <<  "box" // TODO
   inputStream.flush(); // is it necessary?
   QByteArray outData = startProcess(inputData);

   QPolygonF poly[ncurves];
   QVector<double> intens[ncurves];
   QTextStream outputStream(&outData);
   while(!outputStream.atEnd()) {
       int type;
       double x, y, z;
       outputStream >> type >> x >> y >> z;
       if(type < 0 || type >= ncurves)
           type = 0;
       poly[type].append(QPointF(x, y));
       intens[type].append(z);
   }

   for(int i = 0; i < ncurves; i++)
    curves.updateCurve(i, poly[i], intens[i]);

   emit(changed());
}


void SimulatorPluginObject::queryParameters() {
    foreach(QWidget * widget, _valueLabels)
        delete widget;
    foreach(QWidget * widget, _valueEdits)
        delete widget;
    _valueLabels.clear();
    _valueEdits.clear();

    QString data(startProcess("?\n"));

    QDoubleValidator * validator =  new QDoubleValidator(this);
    int layoutOffset = layout->rowCount() - 2;

    QStringList pars = data.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for(int i = 0; i+1 < pars.size(); i += 2) {
        QLabel * label = new QLabel(pars[i]);
        _valueLabels.append(label);
        QLineEdit * edit = new QLineEdit(pars[i+1]);
        _valueEdits.append(edit);
        edit->setValidator(validator);
        layout->insertRow(layoutOffset + i/2, label, edit);
    }
}


QString SimulatorPlugin::description() {
    return "This plugin is designed to directly compare theoretical results with experimental data.\n\n"
            "Description below assumes that the plotted data represents magnetospectroscopy measurements "
            "(i.e. x = transition energy, y = magnetic field, color = PL intensity).\n\n"
            "User should provide external software that calculates expected transition energies "
            "in various magnetic fields. The software should be an executable file which can read "
            "from standard input and write to standard output (e.g. using read/write in Pascal, "
            "scanf/printf in C or cin/cout in C++).\n"
            "The external software should begin with reading the single text line."
            "If a single character '?' is read then the software should write out its calculation parameters "
            "and exit. Each parameter should be presented as a single line containing 2 columns: "
            "unique parameter name (single word) and its default value.\n"
            "In the second scenario the external software will read a word 'parameters:' followed "
            "by a serie of parameter values in the order specified in the first scenario. "
            "After reading all the parameters, the software should perform the calculations and "
            "write out calculated values in four columns:\n"
            "<transition type> <transition energy> <magnetic field> <transition intensity>\n"
            "Transition type yields value 1,2,3 or 4 and affects the color of the presented points "
            "used, e.g., for marking the polarization of the transition. "
            "Transition intensity should is a number between 0 and 1 and is related to the size "
            "of the plotted point."
            ;
}

