#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/transformation.hpp>
#include <../include/average.hpp>
#include <../include/base.hpp>

Average::Average(std::shared_ptr<Base> mother, int id, int offset, int stride):
Transformation{mother, id,  offset, stride}
{
  m_type                  = 0;  //avg
  m_calc_specification    = 0;  //free
  m_upload_specification  = 0;  //with uni_fac
}

Average::~Average(){}

int Average::getTypenumber(){
  return m_type;
}

int Average::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Average::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();

  cv::Mat tmp_frame;
  m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  for(int i = 0; i < length; i++) {

    if(start + i < m_offset || (start - m_offset + i) % (m_stride + 1) != 0 ){
      m_video->grab();
    }
    else{
      m_video->read(tmp_frame);
      if(tmp_frame.empty()){
        std::cout<<"empty\n";
      }

      tmp_frame.convertTo(tmp_frame, m_img_type);   //do this for the whole video right at the start!?

      if( sign > 0){
        factor += 1;
        result += tmp_frame;
      }
      else{
        factor -= 1;
        result -= tmp_frame;
      }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds > ( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + Average ("<<length<<"): time: \t" << duration << std::endl;
    #endif
  }
}

void Average::manipulate(int offset, int stride){
  bool update_status = false;
  if(offset != m_offset){
    m_offset = offset;
    update_status = true;
  }
  if(stride != m_stride){
    m_stride = stride;
    update_status = true;
  }
  if(update_status){
    update_connections();
  }
}
