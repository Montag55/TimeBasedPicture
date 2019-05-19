#ifndef SINGLEIMAGE_HPP
#define SINGLEIMAGE_HPP

#include <../include/transformation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>

class Singleimage : public Transformation{
public:
  Singleimage(std::shared_ptr< Base > mother, int id, cv::Mat image);
  ~Singleimage()                          override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(cv::Mat image);


private:
  cv::Mat m_image;
};

#endif
