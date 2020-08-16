#include "disp.h"
#include <opencv2/opencv.hpp>

void draw_ball(cv::Mat img, int x, int y, int r, cv::Scalar color,
               int thickness) {
  cv::circle(img, cv::Point2i(x, y), r, color, thickness);
}

void draw_ball_normal(cv::Mat img, double x, double y, double r) {
  auto color = COLOR_BLACK;
  int thickness = 1;
  draw_ball(img, x, y, r, color, thickness);
}

void draw_ball_another(cv::Mat img, double x, double y, double r) {
  auto color = COLOR_RED;
  int thickness = -1;
  draw_ball(img, x, y, r, color, thickness);
}

void rect(cv::Mat img, int x, int y, int w, int h, cv::Scalar color) {
  cv::rectangle(img, cv::Point(x, y), cv::Point(x + w, y + h), color, 3, 4);
}

cv::Mat udisp(Xsdata *xdata, int particles_num, int height, int width,
              int dcell, double base) {
  // background
  cv::Mat img = cv::Mat::ones(height, width, CV_8UC3);
  img = cv::Scalar(255, 255, 255);  //画面を白で塗りつぶし

  /*draw walls*/
  rect(img, 0.0, 0.0, dcell, height, COLOR_BLACK);            // left wall
  rect(img, width - dcell, 0.0, dcell, height, COLOR_BLACK);  // right wall
  rect(img, 0.0, 0.0, width, dcell, COLOR_BLACK);             // top
  rect(img, 0.0, height - dcell - base, width, dcell + base,
       COLOR_BLACK);  // bottom

  /*draw the particles*/
  for (int p_idx = 1; p_idx < particles_num; p_idx++) {
    draw_ball_normal(img, xdata[p_idx].x, xdata[p_idx].y,
                     xdata[p_idx].r);  // display the particles
  }
  // draw the brazil nat.
  int p_idx = 0;
  draw_ball_another(img, xdata[p_idx].x, xdata[p_idx].y, xdata[p_idx].r);

  return img;
}
