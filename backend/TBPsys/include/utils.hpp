/*
Author:     Lucas Hübner
Date:       27.05.2019
*/
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

  float CIE76(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh);

  float CIE94(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh);

  cv::Scalar rgb2lab(float r, float g, float b);

  float sigmoid(float x, float exponent, float mid);

  std::shared_ptr<std::vector<float>> pointsToWeights(std::shared_ptr<std::vector<float>> points, int length);

  std::shared_ptr<std::vector<float>> pointsToWeightsIntegral(std::shared_ptr<std::vector<float>> points, int length);
};
