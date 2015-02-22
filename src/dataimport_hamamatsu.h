#ifndef DATAIMPORT_HAMAMATSU_H
#define DATAIMPORT_HAMAMATSU_H

#include "dataimport.h"

/**
 * Use this class for Hamamatsu streak camera images (img).
 */
class HamamatsuImgImport : public ImportFormat  {
public:
    QString fileDialogFilter() const;

    QString actionName() const;

    GridData2D * read(QIODevice * device, bool interactive, QWidget * parent);

    //! Any unique number will do.
    int formatId() const { return 123; }
};

#endif // DATAIMPORT_HAMAMATSU_H
