#ifndef TIMECOUNTER_H
#define TIMECOUNTER_H

#include <QtCore>

class TimeCounter
{
public:
    TimeCounter();
    void start(const QString &name);
    void stop();
    QString report() const;

private:
    QVector<QPair<QString, QTime> > started;
    QMap<QString, QTime> totalTimes;
    QVector<int> minus;
};

extern TimeCounter profiler;

#endif // TIMECOUNTER_H
