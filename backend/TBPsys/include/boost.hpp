#ifndef BOOST_HPP
#define BOST_HPP
#include <../include/interpretation.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>

class Boost : public Interpretation{
public:
  Boost(std::shared_ptr< Base > mother, int id);
  ~Boost()                            override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor) override;
private:
  int m_test;
};

#endif
