#pragma once
#include "simulator.h"

#include <iostream>

#include "linked_list.h"

using namespace std;

void wallinteract(Xsdata *xdata, double base,
                  double base2) {  // interaction with walls
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

void interaction(Xsdata *xdata, double base, double base2, LinkedList *l_list) {
  int pidx, pidx2, cellidxx, cellidxy, cellidxx2, cellidxy2, cellidxx3,
      cellidxy3;

  l_list->linked_list_set(xdata);

  // initialize acceralations with gravity
  for (pidx = 0; pidx < num_particles2; pidx++) {
    xdata[pidx].wx = gx * xdata[pidx].m;
    xdata[pidx].wy = gy * xdata[pidx].m;
    xdata[pidx].womega = 0;
  }

  /*interaction with the walls*/
  vector<ParticleCell *> &near_wall_cells = l_list->get_near_wall_cell();
  for (ParticleCell *pcell : near_wall_cells) {
    pidx = pcell->first;
    // cout << pidx << endl;
    if (pidx != -1) {
      for (;;) {
        wallinteract(&(xdata[pidx]), base, base2);
        pidx = l_list->next_in_cell(pidx);
        if (pidx == -1) break;
      }
    }
  }

  /*particles' interaction*/
  for (cellidxx = 1; cellidxx < cellx; cellidxx++) {
    for (cellidxy = 1; cellidxy < celly; cellidxy++) {
      pidx = l_list->getPCell(cellidxx, cellidxy, divrate, divrate).first;
      if (pidx != -1) {
        for (;;) {
          // paricles' interaction with own cell
          pidx2 = l_list->next_in_cell(pidx);
          if (pidx2 != -1) {
            for (;;) {
              xdata[pidx].interact_with(&(xdata[pidx2]));
              pidx2 = l_list->next_in_cell(pidx2);
              if (pidx2 == -1) break;
            }
          }
          pidx = l_list->next_in_cell(pidx);
          if (pidx == -1) break;
        }

        /*particles' interaction with next cell*/
        for (cellidxx2 = 0; cellidxx2 < divrate; cellidxx2++) {
          for (cellidxy2 = 0; cellidxy2 < divrate; cellidxy2++) {
            pidx = l_list->getPCell(cellidxx, cellidxy, cellidxx2, cellidxy2)
                       .first;
            if (pidx != -1) {
              for (;;) {
                // paricles' interaction with right cell
                pidx2 =
                    l_list->getPCell(cellidxx + 1, cellidxy, divrate, divrate)
                        .first;
                if (pidx2 != -1) {
                  for (cellidxx3 = 0; cellidxx3 <= cellidxx2; cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 < divrate; cellidxy3++) {
                      pidx2 = l_list
                                  ->getPCell(cellidxx + 1, cellidxy, cellidxx3,
                                             cellidxy3)
                                  .first;
                      if (pidx2 != -1) {
                        for (;;) {
                          // 粒子間相互作用の計算
                          xdata[pidx].interact_with(&(xdata[pidx2]));
                          pidx2 = l_list->next_in_divcell(pidx2);
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }
                // paricles' interaction with the left below cell
                pidx2 =
                    l_list
                        ->getPCell(cellidxx - 1, cellidxy + 1, divrate, divrate)
                        .first;
                if (pidx2 != -1) {
                  for (cellidxx3 = cellidxx2; cellidxx3 < divrate;
                       cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 <= cellidxy2; cellidxy3++) {
                      pidx2 = l_list
                                  ->getPCell(cellidxx - 1, cellidxy + 1,
                                             cellidxx3, cellidxy3)
                                  .first;
                      if (pidx2 != -1) {
                        for (;;) {
                          // 粒子間相互作用の計算
                          xdata[pidx].interact_with(&(xdata[pidx2]));
                          pidx2 = l_list->next_in_divcell(pidx2);
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }

                pidx2 =
                    l_list->getPCell(cellidxx, cellidxy + 1, divrate, divrate)
                        .first;  // paricles' interaction with the below cell
                if (pidx2 != -1) {
                  for (cellidxx3 = 0; cellidxx3 < divrate; cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 <= cellidxy2; cellidxy3++) {
                      pidx2 = l_list
                                  ->getPCell(cellidxx, cellidxy + 1, cellidxx3,
                                             cellidxy3)
                                  .first;
                      if (pidx2 != -1) {
                        for (;;) {
                          // 粒子間相互作用の計算
                          xdata[pidx].interact_with(&(xdata[pidx2]));
                          pidx2 = l_list->next_in_divcell(pidx2);
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }
                // paricles' interaction with the right below cell
                pidx2 =
                    l_list
                        ->getPCell(cellidxx + 1, cellidxy + 1, divrate, divrate)
                        .first;
                if (pidx2 != -1) {
                  for (cellidxx3 = 0; cellidxx3 <= cellidxx2; cellidxx3++) {
                    for (cellidxy3 = 0; cellidxy3 <= cellidxy2; cellidxy3++) {
                      pidx2 = l_list
                                  ->getPCell(cellidxx + 1, cellidxy + 1,
                                             cellidxx3, cellidxy3)
                                  .first;
                      if (pidx2 != -1) {
                        for (;;) {
                          // 粒子間相互作用の計算
                          xdata[pidx].interact_with(&(xdata[pidx2]));
                          pidx2 = l_list->next_in_divcell(pidx2);
                          if (pidx2 == -1) break;
                        }
                      }
                    }
                  }
                }

                pidx = l_list->next_in_divcell(pidx);
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

void partupdate(Xsdata *xdata) {
  int pidx;
  for (pidx = 0; pidx < num_particles2; pidx++) {
    xdata[pidx].update(dt);
  }
}

void datainit(Xsdata *xdata, double base, double base2, LinkedList *l_list) {
  int pidx, i, j, maxx, maxy;
  double sx, sy;

  /*format of particle datas*/
  srand(static_cast<unsigned int>(time(NULL)));
  for (pidx = 0; pidx < num_particles2; pidx++) {
    xdata[pidx].init(mstd, rstd1);
    xdata[pidx].vx = (drandom(-0.5, 0.5)) * 10;
    xdata[pidx].vy = (drandom(-0.5, 0.5)) * 10;
  }

  // 下から上に向かって左右均等に配置
  double lx = width - 2 * dcell - 2 * rstd1;                // 実際の横幅
  double ly = height - 2 * dcell - 2 * rstd1 - A;           // 実際の縦幅
  maxx = static_cast<int>(sqrt(lx * num_particles2 / ly));  // 横に並べる粒子の数
  maxy = static_cast<int>(sqrt(ly * num_particles2 / lx));  // 縦に並べる粒子の数
  sx = lx / maxx;  // 粒子が占めるｘ幅
  sy = ly / maxy;  // 粒子が占めるｙ幅
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
  // ブラジルナッツ本体は再設定
  pidx = 0;
  xdata[pidx].init(mstd2, rstd2);
  xdata[pidx].x = width / 2.0;
  xdata[pidx].y = height - (dcell + xdata[pidx].r + base);
  xdata[pidx].vx = 0;
  xdata[pidx].vy = 0;
}
