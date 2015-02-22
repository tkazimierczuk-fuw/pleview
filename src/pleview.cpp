#include "pleview.h"

std::auto_ptr<Engine> Pleview::_engine;
std::auto_ptr<Log> Pleview::_log;
std::auto_ptr<QSettings> Pleview::_settings;


Engine * Pleview::engine() {
    if(!_engine.get())
        _engine.reset(new Engine());
    return _engine.get();
}


Log * Pleview::log() {
    if( !_log.get())
        _log.reset(new Log());
    return _log.get();
}

QSettings * Pleview::settings() {
    if( !_settings.get())
        _settings.reset(new QSettings("TK", "pleview"));
    return _settings.get();
}
