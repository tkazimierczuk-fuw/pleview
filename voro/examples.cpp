#include <QtCore>

#include "vorowrapper.h"


// This is an application testing the calculation of a Voronoi diagram.




void writePolygon(QTextStream &out, const QPolygonF &polygon) {
    foreach(QPointF p, polygon)
        out << "(" << p.x() << "," << p.y() << ") ";
    out << "\n";
}

void writeRect(QTextStream &out, const QRectF &rect) {
    out << rect.left() <<"x" << rect.top() << ", " << rect.bottom() << "x" << rect.right() << "\n";
}


void writePolygons(QTextStream &out, const QVector<QPolygonF> &polygons) {
    for(int i = 0; i < polygons.size(); i++) {
        out << i << " ";
        writePolygon(out, polygons[i]);
    }
}

int main(int argc, char **argv) {
    QTextStream out(stdout);
    QPolygonF points;
    VoroWrapper vw;

    // -----------------------------------------------

    points.clear();
    points.append(QPointF(1, 1));
    points.append(QPointF(2, 2));
    points.append(QPointF(3, 3));
    points.append(QPointF(4, 4));

    out << "------- Test case 1 --------\n";
    out << "Input points:\n";
    writePolygon(out, points);

    vw.setPoints(points);

    out << "Output cells:\n";
    writePolygons(out, vw.diagram());
    out << "Output rect:\n";
    writeRect(out, vw.boundary());
    out << "\n";


    // -----------------------------------------------

}

