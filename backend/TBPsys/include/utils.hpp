#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>
#include <math.h>

#include <opencv2/opencv.hpp>


namespace utils {

  float dE2000(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh);
  float not_mine(cv::Scalar color1, cv::Scalar color2, float k_L, float k_C, float k_H);
  cv::Scalar rgb2lab(float r, float g, float b);

};
