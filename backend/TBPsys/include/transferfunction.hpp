#ifndef TRANSFER_HPP
#define TRANSFER_HPP

#include <vector>

#include <../include/interpretation.hpp>
#include <../include/utils.hpp>

#include <opencv2/opencv.hpp>


class Transferfunction : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Transferfunction(std::shared_ptr< Base > mother, int id, int type, int start_pnt, std::shared_ptr< std::vector< float >> points, int offset, int stride);
  ~Transferfunction()                 override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(int start_pnt, std::shared_ptr<std::vector<float>> points, int offset, int stride);

private:
  int m_start_pnt;
  std::map<int, std::shared_ptr<std::vector<float>>> m_weight_map;
  std::shared_ptr<std::vector<float>> m_points;
};

#endif
