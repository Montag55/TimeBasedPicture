#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/singleimage.hpp>
#include <../include/base.hpp>

Singleimage::Singleimage(std::shared_ptr<Base> mother, int id, cv::Mat image):
Interpretation{mother, id, 0, 0},
m_image{image}
{
  m_type                  = 8;  //singel image
  m_calc_specification    = 0;  //free
  m_upload_specification  = 0;  //uni fac
}

Singleimage::~Singleimage(){}

int Singleimage::getTypenumber(){
  return m_type;
}

int Singleimage::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Singleimage::calc( int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  int seg_start = m_base->get_seg_start(id);
  if( start == seg_start ){
    result = m_image;
    factor = 1;
  }
}

void Singleimage::manipulate(cv::Mat image){
  bool update_status = false;
  cv::Mat tmp;
  double min = 0, max = 0;
  cv::absdiff(m_image, image, tmp);
  cv::minMaxLoc(tmp, &min, &max);

  if(max > 0){
    m_image = image;
    update_status = true;
  }
  if(update_status){
    update_connections();
  }
}
