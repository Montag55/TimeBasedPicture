#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/average.hpp>
#include <../include/base.hpp>



Average::Average(std::shared_ptr<Base> mother, int id, int type):
Interpretation{mother, id, type},
m_test{0}
{}

Average::~Average(){}

int Average::getTypenumber(){
  return m_type;
}

int Average::get_calculation_specification(){
  return 0;//standard sum-game
}

void Average::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time=std::chrono::high_resolution_clock::now();

  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;
  m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  for(int i=0; i<length; i++){
    m_video->read(tmp_frame);
    if(tmp_frame.empty()){
      std::cout<<"empty\n";
    }
    tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?

    if( sign > 0){
      factor += 1;
      result += tmp_frame_d;
    }
    else{
      factor -= 1;
      result -= tmp_frame_d;
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  // std::cout<<"AVERAGE: with work_size:"<< length <<" took ";
  // std::cout<< duration << " milli-seconds in computation\n";
}
