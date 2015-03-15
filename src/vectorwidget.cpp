#include "vectorwidget.h"
#include "pleview.h"

VectorWidget::VectorWidget(QWidget *parent)
{
    //QAction * loadAction = addAction(QIcon(":/icons/actions/document-open.svg"), QLineEdit::TrailingPosition);
    //loadAction->setText("Load values from a text file");
    //loadAction->setEnabled(false);
    QAction * infoAction = addAction(QIcon(":/icons/actions/help_about.svg"), QLineEdit::TrailingPosition);
    infoAction->setText("How to use this widget");
    QAction * pasteAction = addAction(QIcon(":/icons/actions/edit-paste.svg"), QLineEdit::TrailingPosition);
    pasteAction->setText("Paste values");
    setReadOnly(true);
    setText("<empty>");
    setAlignment(Qt::AlignHCenter);

    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    //connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(infoAction, SIGNAL(triggered()), this, SLOT(displayInfo()));
}

VectorWidget::~VectorWidget()
{
}


void VectorWidget::setValues(QVector<double> values) {
    bool different = (_values != values);

    _values = values;

    if(_values.size() == 0)
        setText("<empty>");
    else if(_values.size() == 1)
        setText("1 value");
    else setText(QString("%1 values").arg(_values.size()));

    if(different)
        emit(valueChanged(_values));
}


void VectorWidget::paste(bool interactive) {
    QString text = QApplication::clipboard()->text();
    QTextStream stream(&text);


    QString token;
    double value;
    bool ok;
    QVector<double> ret;
    stream.skipWhiteSpace();
    while(!stream.atEnd() && stream.status() == QTextStream::Ok) {
        stream >> token;
        value = token.toDouble(&ok);
        if(ok)
            ret.append(value);
        stream.skipWhiteSpace();
    }
    setValues(ret);
}


void VectorWidget::load() {
    QString filename = QFileDialog::getOpenFileName(this, "Import a vector of values from a file", "", "Text file (*.txt)");
    if(filename.isEmpty())
        return;
}


void VectorWidget::displayInfo() {
    QString text;
    if(_values.isEmpty())
        text = QString("Paste here a text containing a vector of numbers, e.g. \"0 0.1 0.2 0.3 0.4\".\n"
                        "The values can be separated by any whitespace character.\n Currently the container is empty");
    else
        if(_values.size() > 1)
        text = QString("Paste here a text containing a vector of numbers, e.g. \"0 0.1 0.2 0.3 0.4\".\n"
                        "The values can be separated by any whitespace character.\n Currently the container has %1 elements, starting with %2, %3, ...")
                .arg(_values.size()).arg(_values[0]).arg(_values[1]);
    else
        text = QString("Paste here a text containing a vector of numbers, e.g. \"0 0.1 0.2 0.3 0.4\".\n"
                       "The values can be separated by any whitespace character.\n Currently the container has 1 element: %1").arg(_values[0]);
    QMessageBox::information(this, "Usage info", text);
}
