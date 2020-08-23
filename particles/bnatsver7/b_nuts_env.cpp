#include "b_nuts_env.h"

#include <iostream>
#include <string>

#include "display.h"
#include "simulator.h"

using namespace std;

BNutsEnv::BNutsEnv() {
  xdata = new Xsdata[num_particles];
  l_list =
      new LinkedList(cellx, celly, divrate, num_particles, dcell);
  window_name = "window";
  osci = new Oscillator(A, nyu);
}

void BNutsEnv::init() {
  double base = 0;
  double base2 = 0;
  cout << "Initilizing...";
  datainit(xdata, base, base2, l_list);
  cout << " Done." << endl;

  // 最初に少し動かして地面に着かせる
  cout << "Warm up Start ...";
  for (int i = 0; i < WARMUP_START_UP; i++) {
    interaction(xdata, base, base2, l_list);
    partupdate(xdata);
  }
  cout << " End" << endl;

  cv::namedWindow(window_name);
}

void BNutsEnv::update() {
  // 床のアップデート
  osci->update(dt);
  double base = osci->base_diff();
  double base2 = osci->base_diff2();

  interaction(xdata, base, base2, l_list);
  partupdate(xdata);
}

cv::Mat BNutsEnv::get_image() {
  double base = osci->base_diff();
  auto img = udisp(xdata, num_particles2, height, width, dcell, base);
  return img;
}

void BNutsEnv::show_image() {
  auto img = get_image();
  cv::imshow(window_name, img);
}

BNutsEnv::~BNutsEnv() {
  cv::destroyWindow(window_name);

  delete osci;
  delete l_list;
  delete[] xdata;
}