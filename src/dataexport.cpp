#include "dataexport.h"
#include "qwt_scale_widget.h"
#include "qwt_plot.h"
#include "doublemarker.h"
#include "MapItem.h"

bool DataExport::initialized = false;
int DataExport::xpts = 600;
int DataExport::ypts = 600;
double DataExport::xmin = 0.;
double DataExport::xmax = 0.;
double DataExport::ymin = 0.;
double DataExport::ymax = 0.;

void DataExport::exportAscii(QWidget * parent, Engine * engine) {
    if(!initialized) {
        initialized = true;
        xmin = engine->data()->minX();
        xmax = engine->data()->maxX();
        ymin = engine->data()->minY();
        ymax = engine->data()->maxY();
        xpts = ypts = 600;
    }

    QDialog dialog;
    QFormLayout * layout = new QFormLayout();

    QLineEdit *xminedit = new QLineEdit(QString::number(xmin));
    xminedit->setValidator(new QDoubleValidator(xminedit));
    layout->addRow("Left border (min x)", xminedit);

    QLineEdit *xmaxedit = new QLineEdit(QString::number(xmax));
    xmaxedit->setValidator(new QDoubleValidator(xmaxedit));
    layout->addRow("Right border (max x)", xmaxedit);

    QLineEdit *xpointsEdit = new QLineEdit(QString::number(xpts));
    xpointsEdit->setValidator(new QIntValidator(2, 2000, xpointsEdit));
    layout->addRow("Output width (pixels)", xpointsEdit);

    QFrame * hline = new QFrame();
    hline->setFrameStyle(QFrame::HLine);
    layout->addRow(hline);

    QLineEdit *yminedit = new QLineEdit(QString::number(ymin));
    yminedit->setValidator(new QDoubleValidator(yminedit));
    layout->addRow("Bottom border (min y)", yminedit);

    QLineEdit *ymaxedit = new QLineEdit(QString::number(ymax));
    ymaxedit->setValidator(new QDoubleValidator(ymaxedit));
    layout->addRow("Top border (max y)", ymaxedit);

    QLineEdit *ypointsEdit = new QLineEdit(QString::number(ypts));
    ypointsEdit->setValidator(new QIntValidator(2, 2000, ypointsEdit));
    layout->addRow("Output height (pixels)", ypointsEdit);


    QDialogButtonBox * box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(box, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(box, SIGNAL(rejected()), &dialog, SLOT(reject()));
    layout->addRow(box);

    dialog.setLayout(layout);
    if(dialog.exec() != QDialog::Accepted)
        return;


    QString fname = QFileDialog::getSaveFileName(parent, "Export as ASCII", "", "*.txt");
    if(fname.isEmpty())
        return;

    QFileInfo fileinfo(fname);
    QDir::setCurrent(fileinfo.absolutePath());

    QFile file(fname);
    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);

    xpts = xpointsEdit->text().toInt();
    ypts = ypointsEdit->text().toInt();
    xmin = xminedit->text().toDouble();
    xmax = xmaxedit->text().toDouble();
    ymin = yminedit->text().toDouble();
    ymax = ymaxedit->text().toDouble();


    for(int i = 0; i < xpts; i++) {
        for(int j = 0; j < ypts; j++) {
            double x = xmin + (xmax-xmin)*i/(xpts-1);
            double y = ymin + (ymax-ymin)*j/(ypts-1);
            out << engine->data()->interpolatedValueAt(x, y) << "\t";
        }
        out << "\n";
    }

    file.close();
}



void DataExport::exportAsciiTwoColumn(QWidget * parent, Engine * engine) {

    QDialog dialog;
    QFormLayout * layout = new QFormLayout();

    QComboBox * directionCombo = new QComboBox(parent);
    directionCombo->addItem("row");
    directionCombo->addItem("column");
    directionCombo->setCurrentIndex(0);

    QComboBox * separatorCombo = new QComboBox(parent);
    separatorCombo->addItem("None", "");
    separatorCombo->addItem("\"//nc\"", "//nc\n");
    separatorCombo->setCurrentIndex(0);

    layout->addRow("Spectrum orientation", directionCombo);
    layout->addRow("Separator between spectra", separatorCombo);


    QDialogButtonBox * box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(box, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(box, SIGNAL(rejected()), &dialog, SLOT(reject()));
    layout->addRow(box);

    dialog.setLayout(layout);
    if(dialog.exec() != QDialog::Accepted)
        return;


    QString fname = QFileDialog::getSaveFileName(parent, "Export as ASCII", "", "*.txt");
    if(fname.isEmpty())
        return;

    QFileInfo fileinfo(fname);
    QDir::setCurrent(fileinfo.absolutePath());

    QFile file(fname);
    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);

    GridData2D * data = engine->data();
    QString separatorString = separatorCombo->itemData(separatorCombo->currentIndex()).toString();

    if(directionCombo->currentIndex() == 0) {
        QVector<double> xvalues = data->xValues();
        for(int iy = 0; iy < data->rows(); iy++) {
          for(int ix = 0; ix < data->cols(); ix++)
              out << xvalues.value(ix) << "\t" << data->valueAtIndexBounded(ix, iy) << "\n";
          out << separatorString;
        }
    }
    else {
        QVector<double> yvalues = data->yValues();
        for(int ix = 0; ix< data->cols(); ix++) {
            for(int iy = 0; iy < data->rows(); iy++)
                out << yvalues.value(iy) << "\t" << data->valueAtIndexBounded(ix, iy) << "\n";
            out << separatorString;
        }
    }

    file.close();
}




void DataExport::exportBitmap(QwtPlot *plot) {
    QMap<QString, QString> formats;
    formats.insert("bmp", "Windows Bitmap");
    formats.insert("jpeg", "Joint Photographic Experts Group");
    formats.insert("png", "Portable Network Graphics");
    formats.insert("ppm", "Portable Pixmap");
    formats.insert("tiff", "Tagged Image File Format");
    QMap<QString, QString> reverse_map;

    QString formatString;
    foreach(QString fmt, formats.keys()) {
        if(QImageWriter::supportedImageFormats().contains(fmt.toLatin1())) {
            if(!formatString.isEmpty())
                formatString.append(";;");
            QString s = formats[fmt] + " (*." + fmt+")";
            reverse_map.insert(s, fmt);
            formatString.append(s);
        }
    }
    QString selectedFilter = "Portable Network Graphics (*.png)";

    QString fileName = QFileDialog::getSaveFileName(plot, "Export map as image",
                               "", formatString, &selectedFilter);
    if(fileName.isEmpty())
        return;

    QString format = reverse_map.value(selectedFilter, "png");
    if(!fileName.endsWith("." + format))
        fileName.append("." + format);


    BitmapExportDialog dialog(fileName, plot);
    if(dialog.exec() != QDialog::Accepted)
        return;

    if(QFile::exists(fileName))
        if(QMessageBox::warning(plot, "File exists", "Are you sure to overwrite existing file?",
                                QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            return;

    dialog.applyChanges(plot);
    QImage image(plot->size(), QImage::Format_ARGB32);
    image.fill(0x00ffffff);
    QPainter painter;
    painter.begin(&image);
    plot->drawCanvas(&painter); //print(&painter, QRect(QPoint(0,0), image.size()));
    painter.end();
    image.save(fileName, format.toLatin1());
    dialog.undoChanges(plot);
}


BitmapExportDialog::BitmapExportDialog(const QString &fileName, QwtPlot *plot) : QDialog(plot) {
    currentFont = baseFont = plot->axisWidget(0)->font();
    xHairHiding = true;

    QFormLayout * layout = new QFormLayout();
    QLineEdit * fileNameEdit = new QLineEdit(fileName);
    fileNameEdit->setEnabled(false);
    layout->addRow("File name", fileNameEdit);

    QPushButton * fontButton = new QPushButton("Change");
    connect(fontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
    layout->addRow("Font for tick labels", fontButton);

    QCheckBox * hideXHairBox = new QCheckBox();
    hideXHairBox->setChecked(xHairHiding);
    connect(hideXHairBox, SIGNAL(toggled(bool)), this, SLOT(hideXHair(bool)));
    layout->addRow("Hide X-hair", hideXHairBox);

    QDialogButtonBox * box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addRow(box);
    setLayout(layout);
}


void BitmapExportDialog::hideXHair(bool hide) {
    xHairHiding = hide;
}

void BitmapExportDialog::selectFont() {
    bool ok;
    QFont next = QFontDialog::getFont(&ok, currentFont, this);
    if(ok)
        currentFont = next;
}


void BitmapExportDialog::applyChanges(QwtPlot *plot) {
    for(int i = 0; i < 4; i++)
        plot->axisWidget(i)->setFont(currentFont);
    QwtPlotItemList itemList = plot->itemList();
    foreach(QwtPlotItem * item, itemList) {
        if (item->rtti() == DoubleMarker::Rtti_XHair)
            item->setVisible(!xHairHiding);
        if (item->rtti() == MapItem::Rtti_MapItem)
            dynamic_cast<OfflineRenderItem*>(item)->setOfflineRendering(false);
    }
}


void BitmapExportDialog::undoChanges(QwtPlot *plot) {
    for(int i = 0; i < 4; i++)
        plot->axisWidget(i)->setFont(baseFont);
    QwtPlotItemList itemList = plot->itemList();
    foreach(QwtPlotItem * item, itemList) {
        if (item->rtti() == DoubleMarker::Rtti_XHair)
            item->setVisible(true);
        if (item->rtti() == MapItem::Rtti_MapItem)
            dynamic_cast<OfflineRenderItem*>(item)->setOfflineRendering(true);
    }
}
