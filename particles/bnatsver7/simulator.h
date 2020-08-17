#pragma once
#define _USE_MATH_DEFINES
#include <ball.h>
#include <math.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "physics_constants.h"
#include "linked_list.h"

const int divrate = 4;  // deviding rate

const int width = 220;   // width of window
const int height = 420;  // height of window
const int d = 0;         // size of holl
const double nyu = 2.0;  // frequency of base swing
const double A = 5.0;    // amplitude of base swing
const double B = 0.0;

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

const int num_particles3 = num_particles;  // num_particles2 + (int)(height *1.0
                                           // / 6 / (rstd2 * 2) + 1);

// CellIndex getcellidx(double x, double y);

// void linklistset(ParticleCell pcell[][celly + 1][divrate + 1][divrate + 1],
//                  Xsdata *part, int *nextof, int *nextof2);

void wallinteract(Xsdata *xdata);

void interaction(Xsdata *xdata, double base, double base2, LinkedList *l_list);

double drandom(double first, double end);

void partupdate(Xsdata *xdata);

void datainit(Xsdata *xdata, double base, double base2, LinkedList *l_list);
