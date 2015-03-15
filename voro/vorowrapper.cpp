#include "vorowrapper.h"
#include "voro_2d.hh"
using namespace voro;

#include <iostream>

VoroWrapper::VoroWrapper(QObject *parent) : QObject(parent) {
}


VoroWrapper::~VoroWrapper() {
}


QRectF VoroWrapper::boundary() const {
    return _usedbox;
}


const QVector<QPolygonF> & VoroWrapper::diagram() const {
    return _diagram;
}


void VoroWrapper::setPoints(const QVector<double> &xs, const QVector<double> &ys) {
    QPolygonF points(qMin(xs.size(), ys.size()));
    for(int i = 0; i < points.size(); i++)
        points[i] = QPointF(xs[i], ys[i]);
    setPoints(points);
}


void VoroWrapper::setPoints(const QPolygonF &points) {
    _points = points;
    calculate();
}


void VoroWrapper::setBoundary(QRectF box) {
    _box = box;
    calculate();
}

void VoroWrapper::setAutoBoundary() {
    _box = QRectF();
    calculate();
}




void VoroWrapper::calculate() {

    // first determine the boundary box
    if(!_box.isEmpty()) { // manual
        _usedbox = _box;
    } else { // auto
        _usedbox = _points.boundingRect();
        if(_points.size() == 1)
            _usedbox.adjust(-0.5, -0.5, 0.5, 0.5);
        else if(_points.size() > 1) {
            double avgdist = sqrt(_usedbox.width() * _usedbox.height() / (_points.size()-1));
            _usedbox.adjust(-avgdist, -avgdist, avgdist, avgdist);
        }
    }

    // Initialize the container class to be the unit square, with
    // non-periodic boundary conditions. Divide it into a 10 by 10 grid,
    // with an initial memory allocation of 16 particles per grid square.
    container_2d con(_usedbox.left(), _usedbox.right(), _usedbox.top(), _usedbox.bottom(), 10,10,false,false,16);
    particle_order porder;
    for(int i = 0; i < _points.size(); i++)
        con.put(porder, i, _points[i].x(), _points[i].y());

    _diagram.resize(0);

    c_loop_order_2d vl(con, porder);
    voronoicell_2d c;double *pp;
    if(vl.start()) do {
        QPolygonF poly;
        if(con.compute_cell(c,vl)) {
            pp=con.p[vl.ij] + con.ps*vl.q;
            vector<double> v;
            c.ordered_vertices(pp[0], pp[1], v);

            for(int i = 0; i < v.size()-1; i += 2)
                poly.append(QPointF(v[i], v[i+1]));
        }
        if(poly.size() > 0)
            poly.append(poly[0]);
        _diagram.append(poly);
    } while(vl.inc());

    emit newDiagram(_diagram, _usedbox);
}
