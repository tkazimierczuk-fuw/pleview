#include "timecounter.h"

TimeCounter profiler;

TimeCounter::TimeCounter()
{
}



void TimeCounter::start(const QString &name) {
    QTime time = QTime::currentTime();
    started.append(QPair<QString, QTime>(name, time));
}

void TimeCounter::stop() {
    QPair<QString, QTime> last = started.last();
    started.pop_back();
    QTime time = totalTimes.value(last.first, QTime(0, 0, 0, 0));
    int przyrost = last.second.elapsed();
    time = time.addMSecs(przyrost);
    totalTimes.insert(last.first, time);
}


QString TimeCounter::report() const {
    QString buf;
    QTextStream out(&buf);

     QMapIterator<QString, QTime> i(totalTimes);
    while (i.hasNext()) {
        i.next();
        out << i.key() << "\t" << i.value().toString("mm:ss:zzz") << "\n";
    }
    return buf;
}
