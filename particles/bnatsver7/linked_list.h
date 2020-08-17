
#pragma once
#include "ball.h"

typedef struct {
  int first, last;
} ParticleCell;

class CellIndex {
 public:
  int nx, ny;  // セル番号
  int mx, my;  // 分割セルの番号
  CellIndex(int nx_, int ny_, int mx_, int my_) {
    nx = nx_;
    ny = ny_;
    mx = mx_;
    my = my_;
  }
};

class LinkedList {
 private:
  ParticleCell *pcell;
  int *nextOf;        // nextof particle index in the cell
  int *nextOf2;       // nextof particle index in the divided cell
  int n_cell_x;       // x方向のセルの数
  int n_cell_x_real;  // x方向の実際にセルの数
  int n_cell_y;       // y方向のセルの数
  int n_cell_y_real;  // ｙy方向の実際にセルの数
  int n_all_cells;    // セルの総数
  int divrate;  //セルを分割する比率。x方向、y方向それぞれdivrateで分割するのでdivrate*divrateだけ分割される
  int divrate_real;     // divrate + 1
  int n_all_div_cells;  //分割されたセルの総数
  int cell_size;  // セルの大きさ
  int num_particles;
  int index_coeff1_;  // セル番号計算の係数
  int index_coeff2_;  // セル番号計算の係数
  int index_coeff3_;  // セル番号計算の係数

 public:
  LinkedList(int n_cell_x_, int n_cell_y_, int divrate_, int num_particles_, int cell_size_);
  ParticleCell& getPCell(int nx_, int ny_, int mx_, int my_);
  ParticleCell& getPCell(CellIndex c_index);
  int next_in_cell(int pidx) {return nextOf[pidx];}
  int next_in_divcell(int pidx){return nextOf2[pidx];}
  void linked_list_set(Xsdata* balls);
  CellIndex getcellidx(double x, double y);
  ~LinkedList();
};
