#ifndef BOOST_HPP
#define BOOST_HPP
#include <math.h>
#include <../include/interpretation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
//we could detect black pixels and print a warning, at th end of calc..?

class Boost : public Interpretation{
public:
  Boost(std::shared_ptr< Base > mother, int id,  cv::Mat ref, float threshhold, int modi, int offset, int stride);
  ~Boost()                        override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign);
  void manipulate(cv::Mat ref_frame, float threshhold, int modi, int offset, int stride);

private:
  float m_threshhold;
  int m_modi;
  int m_ptr_delta;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  cv::Mat m_reference;
};

#endif
