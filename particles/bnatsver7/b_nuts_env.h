#pragma once
#include <opencv2/opencv.hpp>
#include <string>

#include "linked_list.h"
#include "physics_constants.h"
#include "simulator.h"

class BNutsEnv {
 private:
  Xsdata *xdata = nullptr;
  LinkedList *l_list = nullptr;
  std::string window_name = "";
  Oscillator *osci = nullptr;

 public:
  BNutsEnv();
  void update();
  void init();
  cv::Mat get_image();
  void show_image();
  ~BNutsEnv();
};