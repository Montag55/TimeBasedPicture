#ifndef AVG_HPP
#define AVG_HPP

#include <../include/transformation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>

class Average : public Transformation{
public:
  Average(std::shared_ptr< Base > mother, int id, int offset, int stride);
  ~Average()                          override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void manipulate(int offset, int stride);



};

#endif
