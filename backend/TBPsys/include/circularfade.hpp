#ifndef CIRCULARFADE_HPP
#define CIRCULARFADE_HPP
#include <math.h>
#include <../include/transformation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
//we could detect black pixels and print a warning, at th end of calc..?

class Circularfade : public Transformation{
public:
  Circularfade(std::shared_ptr<Base> mother, int id, int start, int end, int outer_circle_start, int outer_circle_end, int mode, cv::Vec2f mid, float radius, bool fade_direction, float parameter,int offset, int stride);
  ~Circularfade()                     override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign, int frame_num, int seg_start, int seg_end, int seg_id);
  void manipulate(int start, int end, int outer_circle_start, int outer_circle_end, int mode, cv::Vec2f mid, float radius, bool fade_direction, float parameter, int offset, int stride);
  void create_time_map(int id);

private:
  int m_start;
  int m_end;
  int m_outer_circle_start;
  int m_outer_circle_end;
  int m_mode;
  cv::Vec2f m_mid;
  float m_radius;
  bool m_fade_direction;
  float m_parameter;
  int m_ptr_delta;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  std::map<int,cv::Mat> m_time_map;

};

#endif
