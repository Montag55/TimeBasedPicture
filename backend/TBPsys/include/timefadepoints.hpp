#ifndef TIMEFADEPOINTS_HPP
#define TIMEFADEPOINTS_HPP

#include <vector>

#include <../include/interpretation.hpp>

#include <opencv2/opencv.hpp>


class Timefadepoints : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Timefadepoints(std::shared_ptr< Base > mother, int id, int type, int mode, std::shared_ptr< std::vector<cv::Point>> points, std::shared_ptr< std::vector<int>> starts, std::shared_ptr< std::vector<int>> ends,  int offset, int stride);
  ~Timefadepoints()                   override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(int mode, std::shared_ptr< std::vector<cv::Point>> points, std::shared_ptr< std::vector<int>> starts, std::shared_ptr< std::vector<int>> ends,  int offset, int stride);

private:
  int m_mode;
  std::shared_ptr<std::vector<cv::Point>> m_points;
  std::shared_ptr<std::vector<int>> m_start_frames;
  std::shared_ptr<std::vector<int>> m_end_frames;
};

#endif
