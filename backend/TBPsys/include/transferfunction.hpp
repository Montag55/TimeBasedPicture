#ifndef TRANSFER_HPP
#define TRANSFER_HPP

#include <vector>

#include <../include/interpretation.hpp>

#include <opencv2/opencv.hpp>


class Transferfunction : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Transferfunction(std::shared_ptr< Base > mother, int id, int type, int start_pnt,  std::shared_ptr< std::vector< float >> weights);
  ~Transferfunction()                 override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  //own fcts:
  void set_weights(int start_pnt, std::shared_ptr< std::vector< float >> weights);

private:
  int m_start_pnt;
  std::shared_ptr<std::vector<float>> m_weights;
};

#endif
