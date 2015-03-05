#include "vectorwidget.h"

VectorWidget::VectorWidget(QWidget *parent)
{
    setCheckable(true);
    setText("Data from clipboard");
    connect(this, SIGNAL(toggled(bool)), this, SLOT(paste(bool)));
}

VectorWidget::~VectorWidget()
{

}


void VectorWidget::paste(bool active) {
    if(!active) {
        emit valueChanged(QVector<double>());
        return;
    }

    QString text = QApplication::clipboard()->text();
    QTextStream stream(&text);
    stream.skipWhiteSpace();
    QVector<double> ret;
    while(!stream.atEnd() && stream.status() == QTextStream::Ok) {
        double value;
        stream >> value;
        ret.append(value);
        stream.skipWhiteSpace();
    }
    emit valueChanged(ret);
    QMessageBox::information(this, "Paste", QString("%1 values parsed").arg(ret.size()));
}

