#ifndef BOOSTCOLOR_HPP
#define BOOSTCOLOR_HPP
#include <math.h>
#include <../include/interpretation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

class BoostColor : public Interpretation{
public:
  BoostColor(std::shared_ptr< Base > mother, int id, int type, float threshhold, std::shared_ptr<std::vector<float>> color);
  ~BoostColor()                       override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign);
  void manipulate(float threshhold, std::shared_ptr<std::vector<float>> colors);

private:
  float m_threshhold;
  int m_ptr_delta;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  std::shared_ptr<std::vector<float>> m_colors;
};

#endif
