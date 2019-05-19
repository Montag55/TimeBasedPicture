#include <chrono>
#include <thread>
#include <../include/transformation.hpp>
#include <../include/transferfunction.hpp>
#include <../include/segment.hpp>

#include <string.h>
#include <vector>
//Transferfunction(VideoCapture& vid, int img_type, int id, int start_pnt,  std::shared_ptr<std::vector<float>> weights);

Transferfunction::Transferfunction(std::shared_ptr<Base> mother, int id,  int start_pnt,  std::shared_ptr< std::vector< float > > points, int offset, int stride):
Transformation{mother, id, offset, stride},
m_points{points},
m_start_pnt{start_pnt},
m_weight_map{}
{
  m_type                  = 1;  // transfer
  m_calc_specification    = 2;  // once
  m_upload_specification  = 0;  //with uni fac
}


Transferfunction::~Transferfunction(){
  std::cout<<"\t > deleting transformation-avg\n";
}

int Transferfunction::getTypenumber(){
  return m_type;
}

int Transferfunction::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Transferfunction::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  //receive transfer weights, per image and calculate wieghted

  auto start_time = std::chrono::high_resolution_clock::now();
  int seg_start = m_base->get_seg_start(id);
  int seg_end = m_base->get_seg_end(id);

  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;
  m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  if(start == seg_start){
    // m_weight_map[id] = utils::pointsToWeights(m_points, seg_end - seg_start);
    // std::cout << "create weight map \n";
    m_weight_map[id] = utils::pointsToWeightsIntegral(m_points, seg_end - seg_start);
  }

  for(int i = 0; i<length; i++){
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
      int weight_index = start - seg_start + i - m_start_pnt;
      if( weight_index >= 0 && weight_index < m_weight_map[id]->size() ){
        weight = (*m_weight_map[id])[weight_index];
      }

      if( sign > 0 ){
        factor += weight;
        result += weight * tmp_frame_d;
      }
      else{
        factor -= weight;
        result -= weight * tmp_frame_d;
      }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + TransfF. ("<<m_weight_map[id]->size()<<"): time: \t" << duration << std::endl;
    #endif
  }
}

void Transferfunction::manipulate(int start_pnt, std::shared_ptr<std::vector<float>> points, int offset, int stride) {
  bool update_status = false;
  if(m_start_pnt != start_pnt){
    m_start_pnt = start_pnt;
    update_status = true;
  }
  if((*m_points) != (*points)){
    m_points = points;
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
