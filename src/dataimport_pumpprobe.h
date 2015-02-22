#ifndef DATAIMPORT_PUMP_PROBE_H
#define DATAIMPORT_PUMP_PROBE_H

#include <QtGui>

#include "dataimport.h"


class AsciiPumpProbeImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const;
    QString actionName() const;
    GridData2D * read(QIODevice * device, bool interactive, QWidget * parent);
    int formatId() const { return 9; }
};



#endif // DATAIMPORT_PUMP_PROBE_H
