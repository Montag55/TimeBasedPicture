#ifndef AVG_HPP
#define AVG_HPP
#include <../include/interpretation.hpp>
#include <../include/base.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;

class Average : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Average(std::shared_ptr<Base> mother, int id);
  int getTypenumber() override;
  ~Average() override;
  void calc(int id, int start, int length, int sign, Mat& result, float& factor) override;
private:
  int m_test;
};

#endif
