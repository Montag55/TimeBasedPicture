#ifndef STRUCT_HPP
#define STRUCT_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>
#include <math.h>

#include <opencv2/opencv.hpp>


struct ColorCoords {
  cv::Vec3f color;
  int start;
  int end;
};

#endif
