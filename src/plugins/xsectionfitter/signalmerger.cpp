#include "signalmerger.h"

SignalMerger::SignalMerger(QObject *parent) :
    QObject(parent)
{
    counter = 0;
    connect(this, SIGNAL(internalWakeup()), this, SLOT(wakeUp()), Qt::QueuedConnection);
}


void SignalMerger::merge() {
    counter++;
    emit internalWakeup();
}


void SignalMerger::wakeUp() {
    if(counter > 0) {
        counter = 0;
        emit merged();
    }
}
