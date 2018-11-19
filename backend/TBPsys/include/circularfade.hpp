#ifndef CIRCULARFADE_HPP
#define CIRCULARFADE_HPP
#include <math.h>
#include <../include/interpretation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
//we could detect black pixels and print a warning, at th end of calc..?

class Circularfade : public Interpretation{
public:
  Circularfade(std::shared_ptr<Base> mother, int id, int type, int start, int length, int mode, float mid, float radius, bool fade_direction, int offset, int stride);
  ~Circularfade()                     override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign);
  void manipulate(int start, int length, int mode, float mid, float radius, bool fade_direction, int offset, int stride);

private:
  int m_start;
  int m_length;
  int m_mode;
  float m_mid;
  float m_radius;
  bool m_fade_direction;
  int m_ptr_delta;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
};

#endif
