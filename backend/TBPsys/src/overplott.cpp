/*
Author:     Lucas Hübner
Date:       27.05.2019
*/
#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/transformation.hpp>
#include <../include/overplott.hpp>
#include <../include/base.hpp>
#include <../include/utils.hpp>


Overplott::Overplott( std::shared_ptr<Base> mother, int id,  cv::Mat ref, float threshhold, int modi, int offset, int stride):
Transformation{ mother, id,  offset, stride},
m_reference{ref},
m_threshhold{threshhold},
m_modi{modi},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()}
{
  m_type                  = 2;  //overplott
  m_calc_specification    = 1;  //extending
  m_upload_specification  = 1;  //with fac mat
}

Overplott::~Overplott(){
  //std::cout<<"deleting transformation-boost\n";
}

int Overplott::getTypenumber(){
  return m_type;
}

int Overplott::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Overplott::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();
  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
  if(sign > 0)
  {
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
        compute_frame(result, fac_mat, tmp_frame);
      }

      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
      #ifdef show_time
          std::cout << "\t\t + Overplot ("<<length<<") time: \t" << duration << std::endl;
      #endif
    }
  }
  else{
    //late merge
    cv::Mat tmp_result = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
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
        compute_frame(tmp_result, fac_mat, tmp_frame);
      }
    }
    late_merge(result, fac_mat, tmp_result);
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  #ifdef show_time
      std::cout << "\t\t + Overplot ("<<length<<") time: \t" << duration << std::endl;
  #endif
}

void Overplott::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame) {
  // if(m_modi == 0){
  //   cv::Mat mask;
  //   cv::absdiff(current_frame, m_reference, mask);
  //   mask = (mask > m_threshhold);
  //   mask.convertTo(mask, CV_32FC3);
  //   int from_to[] = { 0,0, 0,1, 0,2};
  //   cv::mixChannels(&mask, 1, &mask, 1, from_to, 3);
  //   result = current_frame.mul(mask) * (float) ((float)1 / ((float)255));
  //   fac_mat = mask * (float) ((float)1 / ((float)255));
  // }
  // else if(m_modi == 1){
  //   cv::Mat mask;
  //   cv::Mat grey_ref = m_reference.clone();
  //   cv::Mat grey_current = current_frame.clone();
  //   cv::Mat tmp = cv::Mat(m_base->get_height(), m_base->get_width(), m_base->get_img_type(), cv::Scalar(0,0,0));
  //
  //   cv::cvtColor(current_frame, grey_current, CV_BGR2GRAY);
  //   cv::cvtColor(m_reference, grey_ref, CV_BGR2GRAY);
  //   grey_current.convertTo(grey_current, CV_32FC3);
  //   grey_ref.convertTo(grey_ref, CV_32FC3);
  //
  //   cv::absdiff(grey_current, grey_ref, mask);
  //   mask = (mask > m_threshhold);
  //   mask.convertTo(mask, CV_32FC3);
  //
  //   int from_to[] = { 0,0, 0,1, 0,2};
  //   cv::mixChannels(&mask, 1, &tmp, 1, from_to, 3);
  //   result = current_frame.mul(tmp) * (float) ((float)1 / ((float)255));
  //   fac_mat = tmp * (float) ((float)1 / ((float)255));
  // }
  //else if(m_modi == 0 || m_modi == 1 || m_modi == 2 || m_modi == 3 || m_modi == 4){
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
        }else if(m_modi == 1){
          distance_RGB = abs(((uc_pixel_ref[0]+uc_pixel_ref[1] + uc_pixel_ref[2])/3)-((uc_pixel_current[0]+uc_pixel_current[1] + uc_pixel_current[2])/3));

        }else if(m_modi == 2){
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

        //std::cout<<" distance: "<<distance_RGB <<"\n";
        if(distance_RGB > m_threshhold){
            uc_pixel_res[0] = uc_pixel_current[0];
            uc_pixel_res[1] = uc_pixel_current[1];
            uc_pixel_res[2] = uc_pixel_current[2];
            uc_pixel_fac[0] = 1;//necessary if sign > 0
            uc_pixel_fac[1] = 1;
            uc_pixel_fac[2] = 1;
        }


        //shift ptr:
        ptr_res     += m_ptr_delta;
        ptr_current += m_ptr_delta;
        ptr_ref     += m_ptr_delta;
        ptr_fac     += m_ptr_delta;
      }
    }
}

void Overplott::late_merge(cv::Mat& result, cv::Mat& facs, cv::Mat& tmp_result){

  cv::Mat mask;
  mask = (result == 0); // only update new parts
  mask.convertTo(mask, CV_32FC3);
  result      += tmp_result.mul(mask)/(float)255;

  tmp_result   = (tmp_result > 0);
  tmp_result.convertTo(tmp_result, CV_32FC3);
  int from_to[] = { 0,0, 0,1, 0,2};
  cv::mixChannels(&tmp_result, 1, &tmp_result, 1, from_to, 3);
  facs        += mask.mul(tmp_result)/(float)255;
}

void Overplott::manipulate(cv::Mat ref_frame, float threshhold, int modi, int offset, int stride){
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
