#include <chrono>
#include <thread>
#include <../include/interpretation.hpp>
#include <../include/timefadepoints.hpp>
#include <../include/segment.hpp>

#include <string.h>
#include <vector>
//Timefadepoints(VideoCapture& vid, int img_type, int id, int start_pnt,  std::shared_ptr<std::vector<float>> weights);

Timefadepoints::Timefadepoints(std::shared_ptr< Base > mother, int id, int type, int mode, std::shared_ptr< std::vector<cv::Point>> points, std::shared_ptr< std::vector<int>> starts, std::shared_ptr< std::vector<int>> ends,  int offset, int stride):
Interpretation{mother, id, type, offset, stride},
m_mode{mode},
m_points{points},
m_start_frames{starts},
m_end_frames{ends}
{
  m_calc_specification = 2;
}


Timefadepoints::~Timefadepoints(){
  std::cout<<"\t > deleting interpretation-avg\n";
}

int Timefadepoints::getTypenumber(){
  return m_type;
}

int Timefadepoints::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Timefadepoints::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  //receive transfer weights, per image and calculate wieghted
  auto start_time = std::chrono::high_resolution_clock::now();
  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;
  m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
/*
  for(int i=0; i<length; i++){
    if(start + i < m_offset || (start - m_offset + i) % (m_stride + 1) != 0 ){
      m_video->grab();
    }
    else{
      m_video->read(tmp_frame);
      if( tmp_frame.empty() ){
        std::cout<<"empty\n";
      }
      tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?
      float weight = 0;

      int weight_index = start+i-m_start_pnt;
      if( weight_index >= 0 && weight_index < m_weights->size() ){
        weight= ( *m_weights )[weight_index];
      }

      if( sign>0 ){
        factor += weight;
        result += weight * tmp_frame_d;
      }else{
        factor -= weight;
        result -= weight * tmp_frame_d;
      }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + TransfF. ("<<m_weights->size()<<"): time: \t" << duration << std::endl;
    #endif
  }
  */
}

void Timefadepoints::manipulate(int mode, std::shared_ptr< std::vector<cv::Point>> points, std::shared_ptr< std::vector<int>> starts, std::shared_ptr< std::vector<int>> ends,  int offset, int stride) {
  bool update_status = false;
  if(m_mode != mode){
    m_mode = mode;
    update_status = true;
  }
  if((*m_points) != (*points)){
    m_points = points;
    update_status = true;
  }
  if((*m_start_frames) != (*starts)){
    m_start_frames = starts;
    update_status = true;
  }
  if((*m_end_frames) != (*ends)){
    m_end_frames = ends;
    update_status = true;
  }
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
