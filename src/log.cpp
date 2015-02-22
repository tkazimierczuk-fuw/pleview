#include <iostream>
#include "log.h"





Log::Log()
{
    dialog = new QDialog();
    textWidget = new QTextEdit(dialog);
    textWidget->setReadOnly(true);
    QVBoxLayout * layout = new QVBoxLayout();
    QPushButton * closeButton = new QPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), dialog, SLOT(hide()));
    layout->addWidget(textWidget);
    layout->addWidget(closeButton);
    dialog->setLayout(layout);
    dialog->setWindowTitle("Message log");
}


QWidget * Log::statusBarWidget() {
    LogStatusWidget * widget = new LogStatusWidget();
    connect(this, SIGNAL(newMessage(QString, QPixmap)), widget, SLOT(setMessage(QString, QPixmap)));
    connect(widget, SIGNAL(clicked()), this, SLOT(show()));
    return widget;
}


void Log::info(const QString &message) {
    std::cout << message.trimmed().toStdString() << "\n";
    QString br;
    if(!textWidget->toPlainText().isEmpty())
        br = "<br/>";
    textWidget->insertHtml(br + timeString() + " " + message);
    emit newMessage(message, QPixmap(":/pleview/misc/dialog-info.svg"));
}

QString Log::timeString() {
    return QTime::currentTime().toString("hh:mm:ss");
}


void Log::warning(const QString &message) {
    std::cout << "[warning] " <<  message.trimmed().toStdString() << "\n";
    QString br;
    if(!textWidget->toPlainText().isEmpty())
        br = "<br/>";
    textWidget->insertHtml(br+"<font color=\"#aaaa00\">" + timeString() + " " + message + "</font><br/>");
    emit newMessage(message, QPixmap(":/pleview/misc/dialog-warning.svg"));
}


void Log::show(QWidget *parent) {
    if(dialog) {
        dialog->setParent(parent);
        dialog->show();
    }
}


void Log::hide() {
    if(dialog)
        dialog->hide();
}


LogStatusWidget::LogStatusWidget() {
    messageCount = 0;
    QHBoxLayout * layout = new QHBoxLayout();
    pixmapLabel = new QLabel();
    pixmapLabel->setScaledContents(true);
    pixmapLabel->setMaximumSize(15, 15);
    layout->addWidget(pixmapLabel);
    textLabel = new QLabel("No messages");
    pixmapLabel->setPixmap(QPixmap(":/pleview/misc/dialog-noinfo.svg"));
    layout->addWidget(textLabel);
    this->setLayout(layout);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(clearMessage()));
    this->setMaximumHeight(15);

    layout->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);
}

void LogStatusWidget::setMessage(QString text, QPixmap pixmap, int timeout) {
    messageCount++;
    QString messageString;
    if(messageCount > 1)
        messageString = QString(" (%1 messages)").arg(messageCount);

    pixmapLabel->setPixmap(pixmap);
    textLabel->setText(text + messageString);
    if(timeout > 0)
        timer->start(timeout);
    else
        timer->stop();
}

void LogStatusWidget::clearMessage() {
    pixmapLabel->clear();
    textLabel->clear();
    if(messageCount == 0) {
        pixmapLabel->setPixmap(QPixmap(":/pleview/misc/dialog-noinfo.svg"));
        textLabel->setText("No messages");
    }
    else if(messageCount == 1) {
        pixmapLabel->setPixmap(QPixmap(":/pleview/misc/dialog-info.svg"));
        textLabel->setText("1 message");
    }
    else {
        pixmapLabel->setPixmap(QPixmap(":/pleview/misc/dialog-info.svg"));
        textLabel->setText(QString("%1 messages").arg(messageCount));
    }
}

void  LogStatusWidget::mousePressEvent (QMouseEvent * event) {
    messageCount = 0;
    clearMessage();
    emit clicked();
}
