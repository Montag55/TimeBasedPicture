#ifndef TIMEFADEPOINTS_HPP
#define TIMEFADEPOINTS_HPP

#include <vector>

#include <../include/interpretation.hpp>
#include <../include/utils.hpp>

#include <opencv2/opencv.hpp>


class Timefadepoints : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Timefadepoints(std::shared_ptr< Base > mother, int id, int mode_distance, int num_pnts, float param, std::shared_ptr< std::vector<cv::Vec4f>> points, int offset, int stride);
  ~Timefadepoints()                   override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(int mode_d, int num_pnts, float param, std::shared_ptr< std::vector<cv::Vec4f>> points, int offset, int stride);
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int frame_num, int seg_id, int sign);
  void create_time_map(int id);
private:
  int m_mode_d;
  int m_num_pnts;
  float m_param;
  std::shared_ptr<std::vector<cv::Vec4f>> m_points;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  int m_ptr_delta;
  std::map<int,cv::Mat> m_time_map;
};

#endif
