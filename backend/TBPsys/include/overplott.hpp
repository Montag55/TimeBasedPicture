#ifndef OVERPLOTT_HPP
#define OVERPLOTT_HPP
#include <math.h>
#include <../include/transformation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
//we could detect black pixels and print a warning, at th end of calc..?

class Overplott : public Transformation{
public:
  Overplott(std::shared_ptr< Base > mother, int id, cv::Mat ref, float threshhold, int modi, int offset, int stride);
  ~Overplott()                        override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame);
  void manipulate(cv::Mat ref_frame, float threshhold, int modi, int offset, int stride);
  void late_merge(cv::Mat& result, cv::Mat& facs, cv::Mat& tmp_result);

private:
  float m_threshhold;
  int m_ptr_delta;
  int m_modi;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  cv::Mat m_reference;
};

#endif
