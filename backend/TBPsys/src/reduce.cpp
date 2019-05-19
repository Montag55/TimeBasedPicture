#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/transformation.hpp>
#include <../include/reduce.hpp>
#include <../include/base.hpp>


Reduce::Reduce( std::shared_ptr<Base> mother, int id, cv::Mat ref, float threshhold, int modi, int offset, int stride):
Transformation{ mother, id,  offset, stride},
m_reference{ref},
m_threshhold{threshhold},
m_modi{modi},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()}
{
  m_type                  = 5;  //reduce
  m_calc_specification    = 0;  //free
  m_upload_specification  = 1;  //with fac mat
}

Reduce::~Reduce(){
  //std::cout<<"deleting transformation-boost\n";
}

int Reduce::getTypenumber(){
  return m_type;
}

int Reduce::get_calculation_specification(){
  return m_calc_specification;  //standard sum-game
}

void Reduce::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
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
        std::cout << "\t\t + Reduce ("<<length<<") time: \t" << duration << std::endl;
    #endif
  }
}

void Reduce::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign) {

  // if(m_modi == 0){
  //   cv::Mat mask;
  //   cv::absdiff(current_frame, m_reference, mask);
  //   cv::Mat mask_fac = mask.clone();
  //   mask = cv::Scalar(m_threshhold, m_threshhold, m_threshhold) - mask;
  //   mask_fac = (mask_fac < m_threshhold);
  //   int from_to[] = { 0,0, 0,1, 0,2};
  //   cv::mixChannels(&mask_fac, 1, &mask_fac, 1, from_to, 3);
  //   mask_fac.convertTo(mask_fac, CV_32FC3);
  //   mask = mask.mul(mask_fac);
  //   result += current_frame.mul(mask) * (float) ((float)sign / ((float)255));
  //   fac_mat += mask * (float) ((float)sign / ((float)255));
  // }

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


      float distance_RGB = -1;

      if(m_modi == 0){
        distance_RGB = sqrt(pow(uc_pixel_ref[0] - uc_pixel_current[0], 2) +
                            pow(uc_pixel_ref[1] - uc_pixel_current[1], 2) +
                            pow(uc_pixel_ref[2] - uc_pixel_current[2], 2));
      }
      else if(m_modi == 1){
        std::cout << "no monochrome yet" << std::endl;
      }
      else if(m_modi == 2){
        cv::Scalar ref = utils::rgb2lab(uc_pixel_ref[0], uc_pixel_ref[1], uc_pixel_ref[2]);
        cv::Scalar current = utils::rgb2lab(uc_pixel_current[0], uc_pixel_current[1], uc_pixel_current[2]);
        distance_RGB = utils::dE2000(ref, current, 0.1f, 100.0f, 100.0f);
      }
      else if(m_modi == 3){
        cv::Scalar ref = utils::rgb2lab(uc_pixel_ref[0], uc_pixel_ref[1], uc_pixel_ref[2]);
        cv::Scalar current = utils::rgb2lab(uc_pixel_current[0], uc_pixel_current[1], uc_pixel_current[2]);
        distance_RGB = utils::CIE76(ref, current, 1.0f, 0.0f, 0.0f);
      }
      else if(m_modi == 4){
        cv::Scalar ref = utils::rgb2lab(uc_pixel_ref[0], uc_pixel_ref[1], uc_pixel_ref[2]);
        cv::Scalar current = utils::rgb2lab(uc_pixel_current[0], uc_pixel_current[1], uc_pixel_current[2]);
        distance_RGB = utils::CIE94(ref, current, 5.0f, 100.0f, 100.0f);
      }


      if(distance_RGB < m_threshhold){
        if(sign > 0) {
          uc_pixel_res[0] += (m_threshhold-distance_RGB)*uc_pixel_current[0];
          uc_pixel_res[1] += (m_threshhold-distance_RGB)*uc_pixel_current[1];
          uc_pixel_res[2] += (m_threshhold-distance_RGB)*uc_pixel_current[2];
          uc_pixel_fac[0] += (m_threshhold-distance_RGB);
          uc_pixel_fac[1] += (m_threshhold-distance_RGB);
          uc_pixel_fac[2] += (m_threshhold-distance_RGB);
        }
        else {
          uc_pixel_res[0] -= (m_threshhold-distance_RGB)*uc_pixel_current[0];
          uc_pixel_res[1] -= (m_threshhold-distance_RGB)*uc_pixel_current[1];
          uc_pixel_res[2] -= (m_threshhold-distance_RGB)*uc_pixel_current[2];
          uc_pixel_fac[0] -= (m_threshhold-distance_RGB);
          uc_pixel_fac[1] -= (m_threshhold-distance_RGB);
          uc_pixel_fac[2] -= (m_threshhold-distance_RGB);
        }
      }


      //shift ptr:
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_ref     += m_ptr_delta;
      ptr_fac     += m_ptr_delta;
    }
  }

}

void Reduce::manipulate(cv::Mat ref_frame, float threshhold, int modi, int offset, int stride){
  bool update_status = false;
  cv::Mat tmp;
  double min = 0, max = 0;
  cv::absdiff(m_reference, ref_frame, tmp);
  cv::minMaxLoc(tmp, &min, &max);

  if(max > 0){
    m_reference = ref_frame;
    update_status = true;
  }
  if(m_threshhold != threshhold){
    m_threshhold = threshhold;
    update_status = true;
  }
  if(m_modi != modi){
    m_modi = modi;
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
