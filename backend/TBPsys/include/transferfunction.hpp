#ifndef TRANSFER_HPP
#define TRANSFER_HPP
#include <../include/interpretation.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

class Transferfunction : public Interpretation{
public:
  //KONSTRUTOREN----------------------------------------------------------------
  Transferfunction(std::shared_ptr<Base> mother, int id, int start_pnt,  std::shared_ptr<std::vector<float>> weights);
  ~Transferfunction() override;
  int getTypenumber() override;

  void set_weights(int start_pnt, std::shared_ptr<std::vector<float>> weights);
  void calc(int id, int start, int length, int sign, Mat& result, float& factor) override;
private:
  int m_start_pnt;
  std::shared_ptr<std::vector<float>> m_weights;
};

#endif
