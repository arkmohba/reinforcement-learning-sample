// bnats2.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
#include <math.h>

#include <cstdio>
#include <iostream>
#include <string>

#include "simulator.h"
#include "display.h"
#include "linked_list.h"

using namespace std;

const int SHOW_FOR_EACH = 30;

void init(Xsdata *xdata, double base, double base2, string window_name, LinkedList *l_list) {
  datainit(xdata, base, base2, l_list);
  cv::namedWindow(window_name);
}

void update(Xsdata *xdata, double &t, double &base, double &base2, LinkedList* l_list,
            string window_name) {
  // static double time = 0;

  for (int i = 0; i < SHOW_FOR_EACH; i++) {
    // 床のアップデート
    // t;
    t = t + dt;
    // cout << t << endl;
    if (t > 10 / nyu) t = 0;
    base = A * pow(sin(2 * M_PI * nyu * t), 1);

    // base2 = base*B / A;
    interaction(xdata, base, base2, l_list);
    partupdate(xdata);
  }
  auto img = udisp(xdata, num_particles2, height, width, dcell, base);
  cv::imshow(window_name, img);
}

int main(int argc, char *argv[]) {
  Xsdata *xdata = new Xsdata[num_particles];
  LinkedList * l_list = new LinkedList(cellx, celly, divrate, num_particles, dcell);
  string window_name = "";
  double base, base2, t;
  base = 0;
  base2 = 0;
  init(xdata, base, base2, window_name, l_list);

  int key;
  t = 0;
  while (1) {
    int key;
    update(xdata, t, base, base2, l_list, window_name);
    key = cv::waitKey(1);
    // 'Esc'が押された場合に終了
    if (key == 27) break;
  }
  cv::destroyWindow(window_name);

  delete l_list;
  delete[] xdata;
  return 0;
}
