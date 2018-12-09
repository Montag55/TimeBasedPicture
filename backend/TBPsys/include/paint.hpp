#ifndef PAINT_HPP
#define PAINT_HPP
#include <math.h>
#include <../include/interpretation.hpp>
#include <../include/base.hpp>
#include <../include/struct.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

class Paint : public Interpretation{
public:
  Paint(std::shared_ptr< Base > mother, int id, int type, std::shared_ptr<std::vector<ColorCoords>> colorTimes , int offset, int stride);
  ~Paint()                            override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void update_Mask();
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int frame_num, int seg_id);
  void manipulate(std::shared_ptr<std::vector<ColorCoords>> colorTimes, int offset, int stride);
  void create_time_map(int id);
private:
  std::vector<cv::Vec3f>  m_colors;
  std::vector<int>        m_start;
  std::vector<int>        m_end;
  cv::Mat                 m_mask;
  std::map<int,cv::Mat>   m_time_map;
  cv::Point               m_pnt_min;
  cv::Point               m_pnt_max;
  int                     m_ptr_delta;

};

#endif
