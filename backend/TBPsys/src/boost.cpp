#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/boost.hpp>
#include <../include/base.hpp>



Boost::Boost( std::shared_ptr<Base> mother, int id, int type, cv::Mat ref, float threshhold):
Interpretation{ mother, id, type},
m_reference{ref},
m_threshhold{threshhold},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()}
{}

Boost::~Boost(){
  //std::cout<<"deleting interpretation-boost\n";
}

int Boost::getTypenumber(){
  return m_type;
}

int Boost::get_calculation_specification(){
  return 0; //standard sum-game
}

void Boost::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  std::cout<<"FATAL:: BOOST NOT YET IMPLEMENTED!\n";

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
}

void Boost::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign) {

  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_res            =  (float*) result.ptr(row);
    float* ptr_fac            =  (float*) fac_mat.ptr(row);
    const float* ptr_ref      =  (float*) m_reference.ptr(row);
    const float* ptr_current  =  (float*) current_frame.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_res           = ptr_res;
      float *uc_pixel_fac           = ptr_fac;
      const float *uc_pixel_ref     = ptr_ref;
      const float *uc_pixel_current = ptr_current;

      //current frame != ref frame -> result frame/ result fac
      float distance_RGB = sqrt(pow(uc_pixel_ref[0] - uc_pixel_current[0], 2) +
                                pow(uc_pixel_ref[1] - uc_pixel_current[1], 2) +
                                pow(uc_pixel_ref[2] - uc_pixel_current[2], 2));


      //std::cout<<" distance: "<<distance_RGB <<"\n";
      if(distance_RGB > m_threshhold){
        if(sign > 0) {
          uc_pixel_res[0] += uc_pixel_current[0];
          uc_pixel_res[1] += uc_pixel_current[1];
          uc_pixel_res[2] += uc_pixel_current[2];
          uc_pixel_fac[0] += 1;
        }
        else {
          uc_pixel_res[0] -= uc_pixel_current[0];
          uc_pixel_res[1] -= uc_pixel_current[1];
          uc_pixel_res[2] -= uc_pixel_current[2];
          uc_pixel_fac[0] -= 1;
        }
      }

      //shift ptr:
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_ref     += m_ptr_delta;
      ptr_fac     += 1;
    }
  }
}
