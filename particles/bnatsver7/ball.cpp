#define _USE_MATH_DEFINES

#include <math.h>

#include "ball.h"
#include "physics_constants.h"

void Xsdata::init(double m_, double r_) {
  m = m_;
  r = r_;
  I = (r * r * m / 2.);
  omega = 0;
  wx = 0;
  wy = 0;
  womega = 0;
  th = 0;
  etha = (-2 * log(e) * sqrt(m * k / (pow(M_PI, 2) + pow(e, 2))));
}

void Xsdata::interact_with(Xsdata *ball) {
  double dx, dy, dvx, dvy, vnx, vny, fpx, fpy, fnx, fny, dst;
  dx = ball->x - x;
  dy = ball->y - y;
  dst = hypot(dx, dy);

  if (dst <= (r + ball->r) && dst != 0) {
    dvx = ball->vx - vx;
    dvy = ball->vy - vy;

    /*1st particle */
    fpx = (-k * (r + ball->r - dst) * dx / dst);
    fpy = (-k * (r + ball->r - dst) * dy / dst);
    fpx += etha * (dvx * dx + dvy * dy) * dx / dst / dst;
    fpy += etha * (dvx * dx + dvy * dy) * dy / dst / dst;
    vnx = (dvx - (dvx * dx + dvy * dy) * dx / dst / dst - r * omega * dy / dst -
           ball->r * ball->omega * (-dy) / dst);
    vny = (dvy - (dvx * dx + dvy * dy) * dy / dst / dst + r * omega * dx / dst +
           ball->r * ball->omega * (-dx) / dst);
    if (hypot(vnx, vny) == 0)
      fnx = fny = 0;
    else {
      fnx = (hypot(fpx, fpy) * myu * vnx / hypot(vnx, vny));
      fny = (hypot(fpx, fpy) * myu * vny / hypot(vnx, vny));
    }
    wx += fpx + fnx;
    wy += fpy + fny;
    womega += -(r * (dx * fny - dy * fnx) / dst);

    /*2nd particle*/
    fpx = (-k * (r + ball->r - dst) * dx / dst);
    fpy = (-k * (r + ball->r - dst) * dy / dst);
    fpx += ball->etha * (dvx * dx + dvy * dy) * dx / dst / dst;
    fpy += ball->etha * (dvx * dx + dvy * dy) * dy / dst / dst;
    if (hypot(vnx, vny) != 0) {
      fnx = (hypot(fpx, fpy) * myu * vnx / hypot(vnx, vny));
      fny = (hypot(fpx, fpy) * myu * vny / hypot(vnx, vny));
    }
    ball->wx -= fpx + fnx;
    ball->wy -= fpy + fny;
    ball->womega += (ball->r * (dx * fny - dy * fnx) / dst);
  }
}
void Xsdata::update(double dt) {
  x += vx * dt;
  y += vy * dt;
  vx += wx * dt / m;
  vy += wy * dt / m;
  th += omega * dt;
  omega += womega * dt / I;
}