#include "dataimport_hamamatsu.h"

QString HamamatsuImgImport::actionName() const {
    return "Import ITEX image (e.g. Hamamatsu streak camera)";
}

QString HamamatsuImgImport::fileDialogFilter() const {
    return "Hamamatsu streak camera file (*.img)";
}

/**
 * Read data according to the specification from Hamamatsu website.
 * There is no need for interaction with the user.
 *
 * We assume little-endian system.
 */
std::unique_ptr<GridData2D> HamamatsuImgImport::read(QIODevice * device, bool interactive, QWidget * parent) {
    Pleview::log()->info("Trying to read data using Hamamatsu IMG format");

    if(device->read(2) != "IM") {
        Pleview::log()->warning("Error reading IMG file - should begin with \"IM\"");
        return 0;
    }

    int ccdWidth = 0, ccdHeight = 0, commentSize = 0, type = 0;

    if(device->read((char*) &commentSize, 2) != 2
            || device->read((char*) &ccdHeight, 2) != 2
            || device->read((char*) &ccdWidth, 2) != 2
            || device->read(4).length() < 4 // we assume offsets = 0
            || device->read((char*) &type, 2) != 2
            || device->read(50).length() < 50 // reserved header area
            ) {
        Pleview::log()->warning("Error reading IMG file - header too short");
        return 0;
    }

    if(ccdWidth <= 0 || ccdHeight <= 0 || commentSize < 0) {
        Pleview::log()->warning("Error reading IMG file - wrong header values");
        return 0;
    }

    int bytesPerPixel;

    switch(type) {
      case 0: bytesPerPixel = 1; break;
      case 2: bytesPerPixel = 2; break;
      case 3: bytesPerPixel = 4; break;
      default:
        Pleview::log()->warning("Error reading IMG file - unsupported bytes per pixel value");
        return 0;
    }

    int dataLength = bytesPerPixel * ccdWidth * ccdHeight;


    QString comment = device->read(commentSize);
    if(comment.size() < commentSize || device->bytesAvailable() < dataLength) {
        Pleview::log()->warning("Error reading IMG file - data too short");
        return 0;
    }

    QVector<char> buf;
    buf.resize(dataLength);

    device->read(buf.data(), dataLength);


    // At this point we have read almost all necessary data. Now we have to construct
    // a data object using variables: ccdWidth, ccdHeight, buf.

    QVector<double> xs, ys, zs;
    xs.resize(ccdHeight);
    ys.resize(ccdWidth);
    zs.resize(ccdWidth*ccdHeight);

    // We extract the data from buf.
    for(int i = 0; i < ccdWidth*ccdHeight; i++) {
        if(bytesPerPixel == 1)
            zs[i] = * ((quint8*) (buf.data()+i*bytesPerPixel));
        else if(bytesPerPixel == 2)
            zs[i] = * ((quint16*) (buf.data()+i*bytesPerPixel));
        else if(bytesPerPixel == 4)
            zs[i] = * ((quint32*) (buf.data()+i*bytesPerPixel));
    }


    // Now we extract axis calibration.
    // For simplicity, we just assume that the calibration is always just
    // after the data. In general this is not the case, but it will work
    // for Hamamatsu streak camera.

    if(sizeof(float) != 4) {
        Pleview::log()->warning("Implementation error - sizeof(float) != 4");
        return 0;
    }

    QVector<float> axisBuf;
    axisBuf.resize(ccdWidth + ccdHeight);
    Pleview::log()->warning(QString("pozycja: %1 ,  nastepny znak: %2").arg(device->pos()).arg((int) device->peek(1).at(0)));

    if(device->read((char*) axisBuf.data(), 4*(ccdWidth+ccdHeight)) < 4*(ccdWidth+ccdHeight)) {
        for(int i = 0; i < ccdWidth; i++)
            ys[i] = i;
        for(int i = 0; i < ccdHeight; i++)
            xs[i] = i;
        Pleview::log()->warning("Error reading IMG file - no calibration data");
    } else {



        for(int i = 0; i < ccdHeight; i++)
            xs[i] = axisBuf[i];
        for(int i = 0; i < ccdWidth; i++)
            ys[i] = axisBuf[i+ccdHeight];

        Pleview::log()->info("Successfully read Hamamatsu IMG file");
    }

    return std::unique_ptr<GridData2D>(new GridData2D(ys, xs, zs));
}
