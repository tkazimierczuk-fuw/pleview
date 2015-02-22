#include <QtGui>
#include <cassert>

#include "../linleastsq.cpp"


void test_normal_case() {
  LinLeastSq lsq;
  
  QPolygonF poly;
  srand(1);
  for(int i = 0; i < 200; i++)
    poly.append(QPointF(i, 0.3*i*i - 0.1*i + 5 + (double) rand()/RAND_MAX));
  
  lsq.setExpData(poly);
  lsq.setOrder(2);
  
  QVector<double> res = lsq.parameters();
  double eps = 0.1;
  assert(res.size() == 3);
  assert(res[2] > 0.3-eps && res[2] < 0.3+eps);
  assert(res[1] > -0.1-eps && res[1] < -0.1+eps);
  assert(res[0] > 5.5-eps && res[0] < 5.5+eps);
}


void test_degenerate_case() {
  LinLeastSq lsq;
  
  for(int i = 0; i < 2; i++)
    lsq.appendPoint(QPointF(i, -0.1*i + 5));
  
  lsq.setOrder(2);
  
  QVector<double> res = lsq.parameters();
  double eps = 0.01;
  assert(res.size() == 3);
  assert(res[2] == 0);
  assert(res[1] > -0.1-eps && res[1] < -0.1+eps);
  assert(res[0] > 5-eps && res[0] < 5+eps);
}


void test_empty_case() {
  LinLeastSq lsq;
  
  lsq.setOrder(1);
  
  QVector<double> res = lsq.parameters();
  assert(res.size() == 2);
  assert(res[1] == 0);
  assert(res[0] == 0);
}


int main() {
  test_normal_case();
  test_degenerate_case();
  test_empty_case();
  return 0;
}
