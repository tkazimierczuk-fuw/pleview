#ifndef DATAIMPORT_PUMP_PROBE_H
#define DATAIMPORT_PUMP_PROBE_H

#include <QtGui>

#include "dataimport.h"


class AsciiPumpProbeImport : public AsciiImportFormat {
public:
    QString fileDialogFilter() const override;
    QString actionName() const override;
    std::unique_ptr<GridData2D> read(QIODevice * device, bool interactive, QWidget * parent) override;
    int formatId() const { return 9; }
};



#endif // DATAIMPORT_PUMP_PROBE_H
