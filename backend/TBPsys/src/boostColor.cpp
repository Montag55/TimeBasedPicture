#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/boostColor.hpp>
#include <../include/base.hpp>


BoostColor::BoostColor( std::shared_ptr<Base> mother, int id, int type, float threshhold, std::shared_ptr<std::vector<float>> colors):
Interpretation{ mother, id, type},
m_colors{colors},
m_threshhold{threshhold},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()}
{}

BoostColor::~BoostColor(){
  //std::cout<<"deleting interpretation-boost\n";
}

int BoostColor::getTypenumber(){
  return m_type;
}

int BoostColor::get_calculation_specification(){
  return 0; //standard sum-game
}

void BoostColor::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();
  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  for(int i = 0; i < length; i++){
    m_video->read(tmp_frame);

    if(tmp_frame.empty()){
      std::cout << "frame not loaded. \n";
    }

    tmp_frame.convertTo(tmp_frame, m_img_type);   //do this for the whole video right at the start!?
    compute_frame(result, fac_mat, tmp_frame, sign);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  #ifdef show_time
      std::cout << "\t\t + B_Color ("<<length<<") time: \t" << duration << std::endl;
  #endif
}

void BoostColor::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign) {

  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_res            =  (float*) result.ptr(row);
    float* ptr_fac            =  (float*) fac_mat.ptr(row);
    const float* ptr_current  =  (float*) current_frame.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_res           = ptr_res;
      float *uc_pixel_fac           = ptr_fac;
      const float *uc_pixel_current = ptr_current;

      float distance_RGB = -1;
      for(unsigned int i = 0; i < m_colors->size(); i+=3){
        float distance_tmp  = sqrt(pow((*m_colors)[i] - uc_pixel_current[0], 2) +
                                   pow((*m_colors)[i+1] - uc_pixel_current[1], 2) +
                                   pow((*m_colors)[i+2] - uc_pixel_current[2], 2));

        if(distance_tmp <= m_threshhold){
          distance_RGB = distance_tmp;
          break;
        }
      }


      //std::cout<<" distance: "<<distance_RGB <<"\n";
      if(distance_RGB > 0){
        if(sign > 0) {
          uc_pixel_res[0] += uc_pixel_current[0];
          uc_pixel_res[1] += uc_pixel_current[1];
          uc_pixel_res[2] += uc_pixel_current[2];
          uc_pixel_fac[0] += 1;
          uc_pixel_fac[1] += 1;
          uc_pixel_fac[2] += 1;
        }
        else {
          uc_pixel_res[0] -= uc_pixel_current[0];
          uc_pixel_res[1] -= uc_pixel_current[1];
          uc_pixel_res[2] -= uc_pixel_current[2];
          uc_pixel_fac[0] -= 1;
          uc_pixel_fac[1] -= 1;
          uc_pixel_fac[2] -= 1;
        }
      }

      //shift ptr:
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_fac     += m_ptr_delta;
    }
  }
}

void BoostColor::manipulate(float threshhold, std::shared_ptr<std::vector<float>> colors){

  bool update_status = false;
  if((*m_colors) != (*colors)){
    m_colors = colors;
    update_status = true;
  }
  if(m_threshhold != threshhold){
    m_threshhold = threshhold;
    update_status = true;
  }
  if(update_status){
    update_connections();
  }
}
