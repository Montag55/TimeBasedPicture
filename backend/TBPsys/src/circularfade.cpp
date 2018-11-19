#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/circularfade.hpp>
#include <../include/base.hpp>
#include <../include/utils.hpp>


Circularfade::Circularfade(std::shared_ptr<Base> mother, int id, int type, int start, int length, int mode, float mid, float radius, bool fade_direction, int offset, int stride):
Interpretation{ mother, id, type, offset, stride},
m_start{start},
m_length{length},
m_mode{mode},
m_mid{mid},
m_radius{radius},
m_fade_direction{fade_direction},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()}
{
  m_calc_specification = 2;
}

Circularfade::~Circularfade(){
  //std::cout<<"deleting interpretation-boost\n";
}

int Circularfade::getTypenumber(){
  return m_type;
}

int Circularfade::get_calculation_specification(){
  return m_calc_specification;
}

void Circularfade::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();
  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  for(int i = 0; i < length; i++){
    if(start + i < m_offset || (start - m_offset + i) % (m_stride + 1) != 0 ){
      m_video->grab();
    }
    else{
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
        std::cout << "\t\t + Circularfade ("<<length<<") time: \t" << duration << std::endl;
    #endif
  }
}

void Circularfade::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign) {

  cv::Mat out = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_out            =  (float*) out.ptr(row);
    float* ptr_res            =  (float*) result.ptr(row);
    float* ptr_fac            =  (float*) fac_mat.ptr(row);
    const float* ptr_current  =  (float*) current_frame.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_out           = ptr_out;
      float *uc_pixel_res           = ptr_res;
      float *uc_pixel_fac           = ptr_fac;
      const float *uc_pixel_current = ptr_current;
      //tobe implemented
          // uc_pixel_res[0] += uc_pixel_current[0];
          // uc_pixel_res[1] += uc_pixel_current[1];
          // uc_pixel_res[2] += uc_pixel_current[2];
          // uc_pixel_fac[0] += 1;
          // uc_pixel_fac[1] += 1;
          // uc_pixel_fac[2] += 1;

      //shift ptr:
      ptr_out     += m_ptr_delta;
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_fac     += m_ptr_delta;
    }
  }
  cv::imwrite("./pimmel.png", out);
}

void Circularfade::manipulate(int start, int length, int mode, float mid, float radius, bool fade_direction, int offset, int stride){
  bool update_status = false;
  if(m_start != start){
    m_start = start;
    update_status = true;
  }
  if(m_length != length){
    m_length = length;
    update_status = true;
  }
  if(m_mode != mode){
    m_mode = mode;
    update_status = true;
  }
  if(m_mid != mid){
    m_mid = mid;
    update_status = true;
  }
  if(m_radius != radius){
    m_radius = radius;
    update_status = true;
  }
  if(m_fade_direction != fade_direction){
    m_fade_direction = fade_direction;
    update_status = true;
  }
  if(m_offset != offset){
    m_offset = offset;
    update_status = true;
  }
  if(m_stride != stride){
    m_stride = stride;
    update_status = true;
  }
  if(update_status){
    update_connections();
  }
}
