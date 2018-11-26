#ifndef TIMEFADEPOINTS_HPP
#define TIMEFADEPOINTS_HPP

#include <vector>

#include <../include/interpretation.hpp>

#include <opencv2/opencv.hpp>


class Timefadepoints : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Timefadepoints(std::shared_ptr< Base > mother, int id, int type, int mode_d, int mode_f, float param, std::shared_ptr< std::vector<cv::Point>> points, std::shared_ptr< std::vector<int>> starts, std::shared_ptr< std::vector<int>> ends,  int offset, int stride);
  ~Timefadepoints()                   override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(int mode_d, int mode_f, float param, std::shared_ptr< std::vector<cv::Point>> points, std::shared_ptr< std::vector<int>> starts, std::shared_ptr< std::vector<int>> ends,  int offset, int stride);
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign, int frame_num, int seg_start, int seg_end);
private:
  int m_mode_d;
  int m_mode_f;
  float m_param;
  std::shared_ptr<std::vector<cv::Point>> m_points;
  std::shared_ptr<std::vector<int>> m_start_frames;
  std::shared_ptr<std::vector<int>> m_end_frames;
  cv::Point m_pnt_min;
  cv::Point m_pnt_max;
  int m_ptr_delta;
};

#endif
