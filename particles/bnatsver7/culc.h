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

const int num_particles3 = num_particles;  // num_particles2 + (int)(height *1.0 / 6 /
                                     // (rstd2 * 2) + 1);

typedef struct {
  double x, y, vx, vy, omega, wx, wy, womega, th, m, r, I, etha;
} Xsdata;
typedef struct {
  int first, last;
} Particle_cell;
typedef struct {
  int nx, ny, mx, my;
} Cellvec;

Cellvec getcellidx(double x, double y) ;

void linklistset(Particle_cell pcell[][celly + 1][divrate + 1][divrate + 1],
                 Xsdata *part, int *nextof, int *nextof2) ;

void wallinteract(Xsdata *xdata);

void psinteract(Xsdata *data1,
                Xsdata *data2) ;

void interaction(Xsdata *xdata, double base, double base2) ;

double drandom(double first, double end) ;


void partupdate(Xsdata *xdata) ;

void datainit(Xsdata *xdata, double base, double base2);
