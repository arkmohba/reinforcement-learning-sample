#pragma once
#include <math.h>

#include <cstdlib>
// #include <GL/glut.h>
#include <opencv2/opencv.hpp>

#include "culc.h"

const cv::Scalar COLOR_BLACK = {0, 0, 0};
const cv::Scalar COLOR_BLUE = {255, 0, 0};
const cv::Scalar COLOR_GREEN = {0, 255, 0};
const cv::Scalar COLOR_RED = {0, 0, 255};

void draw_ball(cv::Mat img, int x, int y, int r, cv::Scalar color,
               int thickness);

void draw_ball_normal(cv::Mat img, double x, double y, double r);

void draw_ball_another(cv::Mat img, double x, double y, double r);

void rect(cv::Mat img, int x, int y, int w, int h, cv::Scalar color);

cv::Mat udisp(Xsdata *xdata, int particles_num, int height, int width,
              int dcell, double base);
