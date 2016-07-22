#ifndef PLEVIEW_H
#define PLEVIEW_H

#include <QSettings>
#include <memory.h>

#include "DLLDefines.h"
#include "model.h"
#include "log.h"

class Engine;

class PLEVIEW_EXPORT Pleview {
public:

    enum Direction {X = 0, Y = 1};

    //! Symbol of given direction (e.g. to use in equations)
    static const char * directionString(Direction dir) {
        return (dir == X) ? "x" : "y";
    }


    //! Main settings in the application
    static QSettings * settings();

    /** Return main Engine object in the application
     *  Do not use this function unless you really need it */
    static Engine * engine();

    //! Return global application log
    static Log * log();

private:
    static std::shared_ptr<Engine> _engine;
    static std::shared_ptr<Log> _log;
    static std::shared_ptr<QSettings> _settings;
    static QString _applicationFile;
};

#endif // PLEVIEW_H
