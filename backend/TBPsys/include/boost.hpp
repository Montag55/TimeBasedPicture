#ifndef BOOST_HPP
#define BOST_HPP
#include <../include/interpretation.hpp>
#include <../include/base.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;

class Boost : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Boost(std::shared_ptr<Base> mother, int id);
  int getTypenumber() override;
  ~Boost() override;
  void calc(int id, int start, int length, int sign, Mat& result, float& factor) override;
private:
  int m_test;
};

#endif
