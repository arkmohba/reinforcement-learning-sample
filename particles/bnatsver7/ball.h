#pragma once

class Xsdata {
public:
  double x, y, vx, vy, omega, wx, wy, womega, th, m, r, I, etha;
  void init(double m, double r);
  void interact_with(Xsdata *ball);
  void update(double dt) ;
};