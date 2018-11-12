#include <chrono>
#include <thread>
#include <../include/interpretation.hpp>
#include <../include/transferfunction.hpp>
#include <../include/segment.hpp>

#include <string.h>
#include <vector>
//Transferfunction(VideoCapture& vid, int img_type, int id, int start_pnt,  std::shared_ptr<std::vector<float>> weights);

Transferfunction::Transferfunction(std::shared_ptr<Base> mother, int id, int type, int start_pnt,  std::shared_ptr< std::vector< float > > weights):
Interpretation{mother, id, type},
m_weights{weights},
m_start_pnt{start_pnt}
{}


Transferfunction::~Transferfunction(){
  std::cout<<"\t > deleting interpretation-avg\n";
}

int Transferfunction::getTypenumber(){
  return 1;
}

int Transferfunction::get_calculation_specification(){
  return 0;//standard sum-game
}

void Transferfunction::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  //receive transfer weights, per image and calculate wieghted

  auto start_time = std::chrono::high_resolution_clock::now();
  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;
  m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  for(int i=0; i<length; i++){
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

void Transferfunction::manipulate(int start_pnt, std::shared_ptr< std::vector<float>> weights) {
  bool update_status = false;
  if(m_start_pnt != start_pnt){
    m_start_pnt = start_pnt;
    update_status = true;
  }
  if((*m_weights) != (*weights)){
    m_weights = weights;
    update_status = true;
  }
  if(update_status){
    update_connections();
  }
}
