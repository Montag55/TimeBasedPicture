#ifndef REDUCE_HPP
#define REDUCE_HPP
#include <math.h>
#include <../include/interpretation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

class Reduce : public Interpretation{
public:
  Reduce(std::shared_ptr< Base > mother, int id, int type, cv::Mat ref, float threshhold, int offset, int stride);
  ~Reduce()                           override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign);
  void manipulate(cv::Mat ref_frame, float threshhold, int offset, int stride);

private:
  float m_threshhold;
  int m_ptr_delta;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  cv::Mat m_reference;
};

#endif
