// bnats2.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
#include <math.h>

#include <cstdio>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include "display.h"
#include "linked_list.h"
#include "simulator.h"

using namespace std;

const int SHOW_FOR_EACH = 30;

void init(Xsdata *xdata, Oscillator *osci, string window_name,
          LinkedList *l_list) {
  double base = 0;
  double base2 = 0;
  datainit(xdata, base, base2, l_list);
  // 最初に少し動かして地面に着かせる
  for (int i = 0; i < 7000; i++) {
    interaction(xdata, base, base2, l_list);
    partupdate(xdata);
  }

  cv::namedWindow(window_name);
}

void update(Xsdata *xdata, Oscillator *osci, LinkedList *l_list,
            string window_name) {
  for (int i = 0; i < SHOW_FOR_EACH; i++) {
    // 床のアップデート
    osci->update(dt);
    // if (t > 10 / nyu) t = 0;
    double base = osci->base_diff();
    double base2 = osci->base_diff2();
    // cout << base << " " << A << " "  << nyu << " "  << t << " " << dt <<
    // endl;

    // base2 = base*B / A;
    interaction(xdata, base, base2, l_list);
    partupdate(xdata);
  }
  double base = osci->base_diff();
  auto img = udisp(xdata, num_particles2, height, width, dcell, base);
  cv::imshow(window_name, img);
}

int main(int argc, char *argv[]) {
  Xsdata *xdata = new Xsdata[num_particles];
  LinkedList *l_list =
      new LinkedList(cellx, celly, divrate, num_particles, dcell);
  string window_name = "window";
  Oscillator *osci = new Oscillator(A, nyu);
  init(xdata, osci, window_name, l_list);

  int key;
  while (1) {
    int key;
    update(xdata, osci, l_list, window_name);
    key = cv::waitKey(1);
    // 'Esc'が押された場合に終了
    if (key == 27) break;
  }
  cv::destroyWindow(window_name);

  delete osci;
  delete l_list;
  delete[] xdata;
  return 0;
}
