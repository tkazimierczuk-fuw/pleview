#include <iostream>
#include "log.h"


class LogWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit LogWidget(QWidget *parent = 0);
    ~LogWidget();

    QSize sizeHint() const override { return minimumSizeHint(); }


public slots:
    void warning(QString msg);
    void information(QString msg);
    void error(QString msg);
};


LogWidget::LogWidget(QWidget *parent) : QListWidget(parent)
{
    setAlternatingRowColors(true);
}

LogWidget::~LogWidget()
{
}


void LogWidget::warning(QString msg)
{
    QListWidgetItem * item = new QListWidgetItem(msg);
    item->setForeground(QBrush(QColor(127, 127, 0)));
    this->addItem(item);
}

void LogWidget::error(QString msg)
{
    QListWidgetItem * item = new QListWidgetItem(msg);
    item->setForeground(QBrush(QColor(127, 0, 0)));
    this->addItem(item);
}


void LogWidget::information(QString msg)
{
    this->addItem(msg);
}






Log::Log()
{

    _dockWidget = new QDockWidget("Log");
    _dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    _dockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);

    LogWidget * logwidget = new LogWidget();
    _dockWidget->setWidget(logwidget);
    //logwidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumQt::MinimumSize);

    connect(this, SIGNAL(newInfo(QString)), logwidget, SLOT(information(QString)));
    connect(this, SIGNAL(newWarning(QString)), logwidget, SLOT(warning(QString)));
    connect(this, SIGNAL(newError(QString)), logwidget, SLOT(error(QString)));
}


Log::~Log() {

}


QWidget * Log::statusBarWidget() {
    LogStatusWidget * widget = new LogStatusWidget();
    connect(this, SIGNAL(newMessage(QString, QPixmap)), widget, SLOT(setMessage(QString, QPixmap)));
    connect(widget, SIGNAL(clicked()), this, SLOT(show()));
    return widget;
}


QDockWidget * Log::dockWidget() {
    return _dockWidget;
}


void Log::info(const QString &message) {
    std::cout << message.trimmed().toStdString() << "\n";
    emit newMessage(message, QPixmap(":/pleview/misc/dialog-info.svg"));
    emit newInfo(message);
}

QString Log::timeString() {
    return QTime::currentTime().toString("hh:mm:ss");
}


void Log::warning(const QString &message) {
    std::cout << "[warning] " <<  message.trimmed().toStdString() << "\n";
    emit newMessage(message, QPixmap(":/pleview/misc/dialog-warning.svg"));
    emit newWarning(message);
}


void Log::error(const QString &message) {
    std::cout << "[error] " <<  message.trimmed().toStdString() << "\n";
    emit newMessage(message, QPixmap(":/pleview/misc/dialog-warning.svg"));
    emit newError(message);
}


void Log::show(QWidget *parent) {
    _dockWidget->setVisible(!_dockWidget->isVisible());
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

#include "log.moc"
