/*
Author:     Lucas Hübner
Date:       27.05.2019
*/
#ifndef BOOSTCOLOR_HPP
#define BOOSTCOLOR_HPP
#include <math.h>
#include <../include/transformation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

class BoostColor : public Transformation{
public:
  BoostColor(std::shared_ptr< Base > mother, int id, float threshhold, int modi, std::shared_ptr<std::vector<float>> color, int offset, int stride);
  ~BoostColor()                       override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign);
  void manipulate(float threshhold, int modi, std::shared_ptr<std::vector<float>> colors, int offset, int stride);

private:
  float m_threshhold;
  int m_ptr_delta;
  int m_modi;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  std::shared_ptr<std::vector<float>> m_colors;
};

#endif
