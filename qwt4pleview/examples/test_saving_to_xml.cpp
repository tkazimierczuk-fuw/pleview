#include <QtCore>
#include <QtGui>
#include "xml.h"
#include "colormap.h"

int main() {
    ColorMap m;
    QFile f("save.test");

    f.open(QIODevice::WriteOnly);
    /*
     PlvzFormat().save(&f, m);
    f.close();


    QTextStream out(stdout);
    out << "Testing exact representation of double values:\n";
    out << "3    = " << PlvzFormat().exactRepresentation(3) << " = " <<  PlvzFormat::interpretExactRepresentation(PlvzFormat::exactRepresentation(3)) << "\n";
    out << "12.3 = " << PlvzFormat().exactRepresentation(12.3) << " = " <<  PlvzFormat::interpretExactRepresentation(PlvzFormat::exactRepresentation(12.3)) << "\n";
    //out << "0/0  = " << PlvzFormat().exactRepresentation(0./0) << " = " <<  PlvzFormat::interpretExactRepresentation(PlvzFormat::exactRepresentation(0./0)) << "\n";
    //out << "1/0  = " << PlvzFormat().exactRepresentation(1./0) << " = " <<  PlvzFormat::interpretExactRepresentation(PlvzFormat::exactRepresentation(1./0)) << "\n";
    out << "\n\n\n";
    */
    return 0;
}
