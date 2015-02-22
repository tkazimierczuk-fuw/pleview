#ifndef SIGNALMERGER_H
#define SIGNALMERGER_H

#include <QObject>

class SignalMerger : public QObject
{
    Q_OBJECT

public:
    SignalMerger(QObject *parent = 0);
    virtual ~SignalMerger() {}

signals:
    void merged();
    void internalWakeup();

public slots:
    void merge();

private slots:
    void wakeUp();

private:
    int counter;

};

#endif // SIGNALMERGER_H
