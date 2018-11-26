#ifndef TIMEFADEPOINTS_HPP
#define TIMEFADEPOINTS_HPP

#include <vector>

#include <../include/interpretation.hpp>

#include <opencv2/opencv.hpp>


class Timefadepoints : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Timefadepoints(std::shared_ptr< Base > mother, int id, int type, int start_pnt, std::shared_ptr< std::vector< float >> weights, int offset, int stride);
  ~Timefadepoints()                 override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(int start_pnt, std::shared_ptr<std::vector<float>> weights, int offset, int stride);

private:
  int m_start_pnt;
  std::shared_ptr<std::vector<float>> m_weights;
};

#endif
