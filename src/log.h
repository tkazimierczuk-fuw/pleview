#ifndef LOG_H
#define LOG_H

#include <QtGui>
#include <QtWidgets>
#include "DLLDefines.h"

/**
 * This class suplies the log object that can be used to inform a user about errors or any
 * other events in the program flow.
 * You can show or hide the message history by calling show() or hide().
 */
class PLEVIEW_EXPORT Log : public QObject
{
    Q_OBJECT

public:
    Log();

    void info(const QString &message);
    void warning(const QString &message);
    QWidget * statusBarWidget();

public slots:
    void show(QWidget * parent = 0);
    void hide();

signals:
    void newMessage(QString msg, QPixmap msgtype);



private:
    static QString timeString();

    QDialog * dialog;
    QTextEdit * textWidget;
};


//! Helper class for a widget in the status bar
class LogStatusWidget : public QFrame {
    Q_OBJECT

public:
    LogStatusWidget();

public slots:
    void setMessage(QString text, QPixmap pixmap, int timeout = 10000);
    void clearMessage();

signals:
    void clicked();

protected:
    virtual void mousePressEvent ( QMouseEvent * event );

private:
    QTimer * timer;
    QLabel * pixmapLabel, * textLabel;
    int messageCount;
};

#endif // LOG_H
