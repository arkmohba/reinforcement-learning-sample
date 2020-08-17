#include "linked_list.h"

LinkedList::LinkedList(int n_cell_x_, int n_cell_y_, int divrate_,
                       int num_particles_, int cell_size_)
    : n_cell_x(n_cell_x_),
      n_cell_x_real(n_cell_x_ + 1),
      n_cell_y(n_cell_y_),
      n_cell_y_real(n_cell_y_ + 1),
      divrate(divrate_),
      divrate_real(divrate_ + 1),
      num_particles(num_particles_),
      cell_size(cell_size_) {
  // セルの総数（xyそれぞれに１＋して確保）
  n_all_cells = n_cell_x_real * n_cell_y_real;
  // 分割セルの総数（xyそれぞれに１＋して確保）
  n_all_div_cells = n_all_cells * divrate_real * divrate_real;

  // 配列の確保
  pcell = new ParticleCell[n_all_div_cells];
  nextOf = new int[num_particles];
  nextOf2 = new int[num_particles];

  // セルインデックス計算の係数
  index_coeff3_ = divrate_real;
  index_coeff2_ = index_coeff3_ * divrate_real;
  index_coeff1_ = index_coeff2_ * n_cell_y_real;
}

ParticleCell& LinkedList::getPCell(int nx, int ny, int mx, int my) {
  int index = index_coeff1_ * nx + index_coeff2_ * ny + index_coeff3_ * mx + my;
  return pcell[index];
}

ParticleCell& LinkedList::getPCell(CellIndex c_index) {
  return getPCell(c_index.nx, c_index.ny, c_index.mx, c_index.my);
}

CellIndex LinkedList::getcellidx(double x, double y) {
  int nx, ny, mx, my;
  nx = (int)(x / cell_size);  // xセル番号
  ny = (int)(y / cell_size);  // yセル番号
  if (nx >= n_cell_x || ny >= n_cell_y || nx < 0 ||
      ny < 0) {  // outside of wall
    nx = n_cell_x;
    ny = n_cell_y;
    mx = divrate;
    my = divrate;

  } else {  // inside of wall
    mx = (int)(x / cell_size * divrate) -
         nx * divrate;
    my = (int)(y / cell_size * divrate) -
         ny * divrate;
  }
  CellIndex cell_idx(nx, ny, mx, my);
  return cell_idx;
}

void LinkedList::linked_list_set(Xsdata* part) {
  int cellidxx, cellidxy, divx, divy, pidx, lastPrev;

  // init cells ,-1 means non particles
  for (int cell_idx = 0; cell_idx < n_all_div_cells; cell_idx++) {
    pcell[cell_idx].first = -1;
    pcell[cell_idx].last = -1;
  }
  // init nextOf
  for (pidx = 0; pidx < num_particles; pidx++) {
    nextOf[pidx] = -1;
    nextOf2[pidx] = -1;
  }

  for (pidx = 0; pidx < num_particles; pidx++) {  // set of pcell and nextof
    CellIndex cidx = getcellidx(part[pidx].x, part[pidx].y);

    /*for the large cell*/
    ParticleCell& cell = getPCell(cidx.nx, cidx.ny, divrate, divrate);
    lastPrev = cell.last;  // save the last particle of the cell
    cell.last = pidx;      // update cell's
                           // last
    if (lastPrev == -1)
      // if there is not the last particle of the cell before update
      // ,set the particle as the first of the cell.
      cell.first = pidx;
    else
      nextOf[lastPrev] = pidx;  // if there is,save the particle as the next.

    /*for the small cell*/
    ParticleCell& cell_internal = getPCell(cidx);
    // save the last particle of the cell
    lastPrev = cell_internal.last;
    // update cell's
    cell_internal.last = pidx;
    
    // last
    if (lastPrev == -1)
      // if there is not the last particle of the cell before update
      // ,set the particle as the first of the cell.
      cell_internal.first = pidx;
    else
      // if there is,save the particle as the next.
      nextOf2[lastPrev] = pidx;
  }
}

LinkedList::~LinkedList() {
  delete[] pcell;
  delete[] nextOf;
  delete[] nextOf2;
}