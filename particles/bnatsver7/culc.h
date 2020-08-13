#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

const int divrate = 4;  // deviding rate

const int width = 220;   // width of window
const int height = 420;  // height of window
const int d = 0;         // size of holl
const double nyu = 2.0;  // frequency of base swing
const double A = 5.0;    // amplitude of base swing
const double B = 0.0;
const double k = 21000;  // spring constant
const double e = 0.75;   // repulsion coefficient
const int gx = 0;        // gravity for x axis
const int gy = 198;      // gravity for y axis
const double myu = 0.4;  // active friction coefficient
const int dcell =
    10;  // dcell must be bigger than the diameter of any particles :dcell > 2r

const double mstd2 = 8.0;
const double rstd2 = 5.0;
const int num_particles2 = 500;  // number of particles
const int wallball = 0;          //(int)(height *1.0/5 /(rstd2*2)+1)
const int num_room = 1;

const int num_particles = num_particles2;
const int cellx = (width / dcell + 1);   // cell number for x axis
const int celly = (height / dcell + 1);  // cell number for y axis
const int num_cells = (cellx * celly);   // number of cells
const double mstd = 2.0;                 // standard mass of particles
const double rstd1 = 3.0;                // standard radius of particles
const double Omeg = 40;                  // coefficient of dt
const double dt = (2 * M_PI * sqrt(mstd / k) / Omeg);  // infinitesimal time

int num_particles3 = num_particles;  // num_particles2 + (int)(height *1.0 / 6 /
                                     // (rstd2 * 2) + 1);
double base = 0, base2 = 0;

typedef struct {
  double x, y, vx, vy, omega, wx, wy, womega, th, m, r, I, etha;
} Xsdata;
typedef struct {
  int first, last;
} Particle_cell;
typedef struct {
  int nx, ny, mx, my;
} Cellvec;

Cellvec getcellidx(double x, double y) {
  Cellvec cvec;
  int nx, ny;
  nx = (int)(x / dcell);  // width * (cellx - 1));
  ny = (int)(y / dcell);  // height * (celly - 1));
  if (nx >= cellx || ny >= celly || nx < 0 || ny < 0) {  // outside of wall
    cvec.nx = cellx;
    cvec.ny = celly;
    cvec.mx = divrate;
    cvec.my = divrate;
  } else {  // inside of wall
    cvec.nx = nx;
    cvec.ny = ny;
    cvec.mx = (int)(x / dcell * divrate) -
              nx * divrate;  // width * (cellx - 1)*divrate) - nx*divrate;
    cvec.my = (int)(y / dcell * divrate) -
              ny * divrate;  // height * (celly-1)*divrate) -ny* divrate;
  }
  return cvec;
}

void linklistset(Particle_cell pcell[][celly + 1][divrate + 1][divrate + 1],
                 Xsdata *part, int *nextof, int *nextof2) {
  int cellidxx, cellidxy, divx, divy, pidx, lastPrev;
  Cellvec cidx;
  for (cellidxx = 0; cellidxx <= cellx;
       cellidxx++) {  // format of cells ,-1 means non particles
    for (cellidxy = 0; cellidxy <= celly; cellidxy++) {
      for (divx = 0; divx <= divrate; divx++) {
        for (divy = 0; divy <= divrate; divy++) {
          pcell[cellidxx][cellidxy][divx][divy].first = -1;
          pcell[cellidxx][cellidxy][divx][divy].last = -1;
        }
      }
    }
  }
  for (pidx = 0; pidx < num_particles3; pidx++) {
    nextof[pidx] = -1;
    nextof2[pidx] = -1;
  }

  for (pidx = 0; pidx < num_particles3; pidx++) {  // set of pcell and nextof
    cidx = getcellidx(part[pidx].x, part[pidx].y);

    /*for the large cell*/
    lastPrev = pcell[cidx.nx][cidx.ny][divrate][divrate]
                   .last;  // save the last particle of the cell
    pcell[cidx.nx][cidx.ny][divrate][divrate].last = pidx;  // update cell's
                                                            // last
    if (lastPrev == -1)
      pcell[cidx.nx][cidx.ny][divrate][divrate].first =
          pidx;  // if there is not the last particle of the cell before update
                 // ,set the particle as the first of the cell.
    else
      nextof[lastPrev] = pidx;  // if there is,save the particle as the next.

    /*for the small cell*/
    lastPrev = pcell[cidx.nx][cidx.ny][cidx.mx][cidx.my]
                   .last;  // save the last particle of the cell
    pcell[cidx.nx][cidx.ny][cidx.mx][cidx.my].last = pidx;  // update cell's
                                                            // last
    if (lastPrev == -1)
      pcell[cidx.nx][cidx.ny][cidx.mx][cidx.my].first =
          pidx;  // if there is not the last particle of the cell before update
                 // ,set the particle as the first of the cell.
    else
      nextof2[lastPrev] = pidx;  // if there is,save the particle as the next.
  }
}

void wallinteract(Xsdata *xdata) {  // interaction with walls
  double fp, fn;
  if (xdata->x > width - dcell - xdata->r - base2) {  // with the right wall
    fp = -k * (xdata->x + xdata->r - (width - dcell - base2)) -
         xdata->etha * xdata->vx;
    if (fabs(xdata->vy + xdata->omega * xdata->r) == 0)
      fn = 0;
    else
      fn = -myu * fabs(fp) * (xdata->vy + xdata->omega * xdata->r) /
           fabs(xdata->vy + xdata->omega * xdata->r);
    xdata->wx += fp;
    xdata->wy += fn;
    xdata->womega += xdata->r * fn;
  } else if (xdata->x < dcell + xdata->r + base2) {  // with the left wall
    fp = -k * (xdata->x - xdata->r - dcell - base2) - xdata->etha * xdata->vx;
    if (fabs(xdata->vy - xdata->omega * xdata->r) == 0)
      fn = 0;
    else
      fn = -myu * fabs(fp) * (xdata->vy - xdata->omega * xdata->r) /
           fabs(xdata->vy - xdata->omega * xdata->r);
    xdata->wx += fp;
    xdata->wy += fn;
    xdata->womega += -xdata->r * fn;
  }
  if ((xdata->y >
       (height - dcell - xdata->r - base))) {  //&& (xdata->x<w/2.-d/2. ||
    // xdata->x>w/2.+d/2.)){//with the base
    fp = -k * (xdata->y + xdata->r - (height - dcell - base)) -
         xdata->etha * xdata->vy;
    if (fabs(xdata->vx - xdata->omega * xdata->r) == 0)
      fn = 0;
    else
      fn = -myu * fabs(fp) * (xdata->vx - xdata->omega * xdata->r) /
           fabs(xdata->vx - xdata->omega * xdata->r);
    xdata->wy += fp;
    xdata->wx += fn;
    xdata->womega += xdata->r * (-fn);
  } else if (xdata->y < dcell + xdata->r) {  // with the ceiling
    fp = -k * (xdata->y - xdata->r - dcell) - xdata->etha * xdata->vy;
    if (fabs(xdata->vx + xdata->omega * xdata->r) == 0)
      fn = 0;
    else
      fn = -myu * fabs(fp) * (xdata->vx + xdata->omega * xdata->r) /
           fabs(xdata->vx + xdata->omega * xdata->r);
    xdata->wy += fp;
    xdata->wx += fn;
    xdata->womega += xdata->r * fn;
  }
}

void psinteract(Xsdata *data1,
                Xsdata *data2) {  // interaction between two particles
  double dx, dy, dvx, dvy, vnx, vny, fpx, fpy, fnx, fny, dst;
  dx = data2->x - data1->x;
  dy = data2->y - data1->y;
  dst = hypot(dx, dy);

  if (dst <= (data1->r + data2->r) && dst != 0) {
    dvx = data2->vx - data1->vx;
    dvy = data2->vy - data1->vy;

    /*1st particle */
    fpx = (-k * (data1->r + data2->r - dst) * dx / dst);
    fpy = (-k * (data1->r + data2->r - dst) * dy / dst);
    fpx += data1->etha * (dvx * dx + dvy * dy) * dx / dst / dst;
    fpy += data1->etha * (dvx * dx + dvy * dy) * dy / dst / dst;
    vnx = (dvx - (dvx * dx + dvy * dy) * dx / dst / dst -
           data1->r * data1->omega * dy / dst -
           data2->r * data2->omega * (-dy) / dst);
    vny = (dvy - (dvx * dx + dvy * dy) * dy / dst / dst +
           data1->r * data1->omega * dx / dst +
           data2->r * data2->omega * (-dx) / dst);
    if (hypot(vnx, vny) == 0)
      fnx = fny = 0;
    else {
      fnx = (hypot(fpx, fpy) * myu * vnx / hypot(vnx, vny));
      fny = (hypot(fpx, fpy) * myu * vny / hypot(vnx, vny));
    }
    data1->wx += fpx + fnx;
    data1->wy += fpy + fny;
    data1->womega += -(data1->r * (dx * fny - dy * fnx) / dst);

    /*2nd particle*/
    fpx = (-k * (data1->r + data2->r - dst) * dx / dst);
    fpy = (-k * (data1->r + data2->r - dst) * dy / dst);
    fpx += data2->etha * (dvx * dx + dvy * dy) * dx / dst / dst;
    fpy += data2->etha * (dvx * dx + dvy * dy) * dy / dst / dst;
    if (hypot(vnx, vny) != 0) {
      fnx = (hypot(fpx, fpy) * myu * vnx / hypot(vnx, vny));
      fny = (hypot(fpx, fpy) * myu * vny / hypot(vnx, vny));
    }
    data2->wx -= fpx + fnx;
    data2->wy -= fpy + fny;
    data2->womega += (data2->r * (dx * fny - dy * fnx) / dst);
  }
}
Particle_cell
    pcell[cellx + 1][celly + 1][divrate + 1]
         [divrate + 1];  // pcell[cellx][celly] is for particles out of walls
int nextOf[num_particles];
int nextOf2[num_particles];
void interaction(Xsdata *xdata) {
  int pidx, pidx2, cellidxx, cellidxy, cellidxx2, cellidxy2, cellidxx3,
      cellidxy3;

  linklistset(pcell, xdata, nextOf, nextOf2);

  for (pidx = 0; pidx < num_particles2; pidx++) {  // format with gravity
    xdata[pidx].wx = gx * xdata[pidx].m;
    xdata[pidx].wy = gy * xdata[pidx].m;
    xdata[pidx].womega = 0;
  }
  /*interaction with the walls*/
  cellidxy = 0;
  for (cellidxx = 0; cellidxx <= cellx; cellidxx++) {  // with upper wall
    pidx = pcell[cellidxx][cellidxy][divrate][divrate].first;
    if (pidx != -1) {
      for (;;) {
        wallinteract(&(xdata[pidx]));
        pidx = nextOf[pidx];
        if (pidx == -1) break;
      }
    }
    pidx = pcell[cellidxx][cellidxy + 1][divrate][divrate].first;
    if (pidx != -1) {
      for (;;) {
        wallinteract(&(xdata[pidx]));
        pidx = nextOf[pidx];
        if (pidx == -1) break;
      }
    }
  }
  for (cellidxy = 1; cellidxy <= celly * 3 / 4;
       cellidxy++) {  // with lefft the wall
    pidx = pcell[0][cellidxy][divrate][divrate].first;
    if (pidx != -1) {
      for (;;) {
        wallinteract(&(xdata[pidx]));
        pidx = nextOf[pidx];
        if (pidx == -1) break;
      }
    }
    pidx = pcell[1][cellidxy][divrate][divrate].first;
    if (pidx != -1) {
      for (;;) {
        wallinteract(&(xdata[pidx]));
        pidx = nextOf[pidx];
        if (pidx == -1) break;
      }
    }
  }
  for (cellidxy = 1; cellidxy <= celly * 3 / 4;
       cellidxy++) {  // with the right wall
    for (int i = 0; i <= 3; i++) {
      pidx = pcell[cellx - i][cellidxy][divrate][divrate].first;
      if (pidx != -1) {
        for (;;) {
          wallinteract(&(xdata[pidx]));
          pidx = nextOf[pidx];
          if (pidx == -1) break;
        }
      }
    }
  }

  for (cellidxx = 0; cellidxx <= cellx; cellidxx++) {  // with the base
    for (cellidxy = celly * 3 / 4; cellidxy <= celly; cellidxy++) {
      pidx = pcell[cellidxx][cellidxy][divrate][divrate].first;
      if (pidx != -1) {
        for (;;) {
          wallinteract(&(xdata[pidx]));
          pidx = nextOf[pidx];
          if (pidx == -1) break;
        }
      }
    }
  }

  /*particles' interaction*/
  for (cellidxx = 1; cellidxx < cellx; cellidxx++) {
    for (cellidxy = 1; cellidxy < celly; cellidxy++) {
      pidx = pcell[cellidxx][cellidxy][divrate][divrate].first;
      if (pidx != -1) {
        for (;;) {
          pidx2 = nextOf[pidx];  // paricles' interaction with own cell
          if (pidx2 !=
              -1) {  //|| !(pidx>=num_particles2 && pidx2>=num_particles2)){
            for (;;) {
              psinteract(&(xdata[pidx]), &(xdata[pidx2]));
              pidx2 = nextOf[pidx2];
              if (pidx2 == -1) break;
            }
          }
          pidx = nextOf[pidx];
          if (pidx == -1) break;
        }

        /*particles' interaction with next cell*/
        for (cellidxx2 = 0; cellidxx2 < divrate; cellidxx2++) {
          for (cellidxy2 = 0; cellidxy2 < divrate; cellidxy2++) {
            pidx = pcell[cellidxx][cellidxy][cellidxx2][cellidxy2].first;
            if (pidx != -1) {
              for (;;) {
                pidx2 = pcell[cellidxx + 1][cellidxy][divrate][divrate]
                            .first;  // paricles' interaction with right cell
                if (pidx2 != -1) {
                  for (cellidxx3 = 0; cellidxx3 <= cellidxx2; cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 < divrate; cellidxy3++) {
                      pidx2 =
                          pcell[cellidxx + 1][cellidxy][cellidxx3][cellidxy3]
                              .first;
                      if (pidx2 != -1) {  //|| !(pidx >= num_particles2 && pidx2
                                          //>= num_particles2)){
                        for (;;) {
                          psinteract(&(xdata[pidx]),
                                     &(xdata[pidx2]));  //粒子間相互作用の計算
                          pidx2 = nextOf2[pidx2];
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }

                pidx2 = pcell[cellidxx - 1][cellidxy + 1][divrate][divrate]
                            .first;  // paricles' interaction with the left
                                     // below cell
                if (pidx2 != -1) {
                  for (cellidxx3 = cellidxx2; cellidxx3 < divrate;
                       cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 <= cellidxy2; cellidxy3++) {
                      pidx2 = pcell[cellidxx - 1][cellidxy + 1][cellidxx3]
                                   [cellidxy3]
                                       .first;
                      if (pidx2 != -1) {  // || !(pidx >= num_particles2 &&
                                          // pidx2 >= num_particles2)){
                        for (;;) {
                          psinteract(&(xdata[pidx]), &(xdata[pidx2]));
                          pidx2 = nextOf2[pidx2];
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }

                pidx2 =
                    pcell[cellidxx][cellidxy + 1][divrate][divrate]
                        .first;  // paricles' interaction with the below cell
                if (pidx2 != -1) {
                  for (cellidxx3 = 0; cellidxx3 < divrate; cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 <= cellidxy2; cellidxy3++) {
                      pidx2 =
                          pcell[cellidxx][cellidxy + 1][cellidxx3][cellidxy3]
                              .first;
                      if (pidx2 != -1) {  //|| !(pidx >= num_particles2 && pidx2
                                          //>= num_particles2)){
                        for (;;) {
                          psinteract(&(xdata[pidx]), &(xdata[pidx2]));
                          pidx2 = nextOf2[pidx2];
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }

                pidx2 = pcell[cellidxx + 1][cellidxy + 1][divrate][divrate]
                            .first;  // paricles' interaction with the right
                                     // below cell
                if (pidx2 != -1) {
                  for (cellidxx3 = 0; cellidxx3 <= cellidxx2; cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 <= cellidxy2; cellidxy3++) {
                      pidx2 = pcell[cellidxx + 1][cellidxy + 1][cellidxx3]
                                   [cellidxy3]
                                       .first;
                      if (pidx2 != -1) {  // ||!(pidx >= num_particles2 && pidx2
                                          // >= num_particles2)){
                        for (;;) {
                          psinteract(&xdata[pidx], &(xdata[pidx2]));
                          pidx2 = nextOf2[pidx2];
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }

                pidx = nextOf2[pidx];
                if (pidx == -1) break;
              }
            }
          }
        }
      }
    }
  }
}

double drandom(double first, double end) {
  double ra, distance;

  ra = (double)rand() / RAND_MAX;
  distance = end - first;
  ra = ra * distance;
  ra = ra + first;
  return ra;
}

double sq(double x) { return (x * x); }

void partupdate(Xsdata *xdata) {
  int pidx;
  for (pidx = 0; pidx < num_particles2; pidx++) {
    xdata[pidx].x += xdata[pidx].vx * dt;
    xdata[pidx].y += xdata[pidx].vy * dt;
    xdata[pidx].vx += xdata[pidx].wx * dt / xdata[pidx].m;
    xdata[pidx].vy += xdata[pidx].wy * dt / xdata[pidx].m;
    xdata[pidx].th += xdata[pidx].omega * dt;
    xdata[pidx].omega += xdata[pidx].womega * dt / xdata[pidx].I;
  }
}

void datainit(Xsdata *xdata) {
  int pidx, i, j, m, l, maxx, maxy, room;
  double sx, sy;

  /*format of particle datas*/
  srand(time(NULL));
  for (pidx = 0; pidx < num_particles2; pidx++) {
    xdata[pidx].m = mstd;
    xdata[pidx].r = rstd1;
    xdata[pidx].I = (xdata[pidx].r * xdata[pidx].r * xdata[pidx].m / 2.);
    // xdata[pidx].x = dcell + xdata[pidx].r + drandom(0.0, 1.0)*(width - 2 *
    // dcell - 2 * xdata[pidx].r); xdata[pidx].y = dcell + xdata[pidx].r +
    // drandom(0.0, 1.0)*(height - 2 * dcell - 2 * xdata[pidx].r - 2 * rstd2);
    xdata[pidx].vx = (drandom(-0.5, 0.5)) * 10;
    xdata[pidx].vy = (drandom(-0.5, 0.5)) * 10;
    xdata[pidx].omega = 0;
    xdata[pidx].wx = 0;
    xdata[pidx].wy = 0;
    xdata[pidx].womega = 0;
    xdata[pidx].th = 0;
    xdata[pidx].etha =
        (-2 * log(e) * sqrt(xdata[pidx].m * k / (sq(M_PI) + sq(e))));
  }
  double lx = width - 2 * dcell - 2 * rstd1;       // 実際の横幅
  double ly = height - 2 * dcell - 2 * rstd1 - A;  // 実際の縦幅
  maxx = sqrt(lx * num_particles2 / ly);           //横に並べる粒子の数
  maxy = sqrt(ly * num_particles2 / lx);           //縦に並べる粒子の数
  sx = lx / maxx;                                  //粒子が占めるｘ幅
  sy = ly / maxy;                                  //粒子が占めるｙ幅
  i = 0;
  j = 0;
  for (pidx = 0; pidx < num_particles2; pidx += 2) {
    xdata[pidx].x = dcell + xdata[pidx].r + sx * i;
    xdata[pidx].y = dcell + xdata[pidx].r + sy * j;
    xdata[pidx + 1].x = width - (dcell + xdata[pidx].r + sx * i);
    xdata[pidx + 1].y = dcell + xdata[pidx].r + sy * j;
    i++;
    if (i >= maxx / 2) {
      i = 0;
      j++;
    }
  }
  pidx = 0;
  xdata[pidx].m = mstd2;
  xdata[pidx].r = rstd2;
  xdata[pidx].I = (xdata[pidx].r * xdata[pidx].r * xdata[pidx].m / 2.);
  xdata[pidx].x = width / 2.0;
  xdata[pidx].y = height - (dcell + xdata[pidx].r + base);
  xdata[pidx].vx = 0;  //(drandom(-0.5, 0.5)) * 10;
  xdata[pidx].vy = 0;  //(drandom(-0.5, 0.5)) * 10;
  xdata[pidx].omega = 0;
  xdata[pidx].wx = 0;
  xdata[pidx].wy = 0;
  xdata[pidx].womega = 0;
  xdata[pidx].th = 0;
  xdata[pidx].etha =
      (-2 * log(e) * sqrt(xdata[pidx].m * k / (sq(M_PI) + sq(e))));

  /*for (room = 1; room < num_room; room++){
          for (int pidx2 = 0; pidx2 < wallball; pidx2++){
                  pidx = pidx2 + num_particles2 + (room - 1)*wallball;
                  xdata[pidx].m = mstd;
                  xdata[pidx].r = rstd2;
                  xdata[pidx].I = (xdata[pidx].r*xdata[pidx].r*xdata[pidx].m
  / 2.); xdata[pidx].x = dcell + (width - 2 * dcell) *room / num_room;
                  xdata[pidx].y = height - pidx2 * rstd2 * 2 - rstd2;
                  xdata[pidx].vx = 0;
                  xdata[pidx].vy = 0;
                  xdata[pidx].omega = 0;
                  xdata[pidx].wx = 0;
                  xdata[pidx].wy = 0;
                  xdata[pidx].womega = 0;
                  xdata[pidx].th = 0;
                  xdata[pidx].etha = (-2 * log(e)*sqrt(xdata[pidx].m * k /
  (sq(M_PI) + sq(e))));

          }
  }*/
  for (i = 0; i < 7000; i++) {
    interaction(xdata);
    partupdate(xdata);
  }
}
