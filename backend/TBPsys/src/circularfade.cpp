#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>
#include <algorithm>

#include <../include/interpretation.hpp>
#include <../include/circularfade.hpp>
#include <../include/base.hpp>
#include <../include/utils.hpp>


Circularfade::Circularfade(std::shared_ptr<Base> mother, int id, int start, int end, int outer_circle_start, int outer_circle_end, int mode, cv::Vec2f mid, float radius, bool fade_direction, float parameter, int offset, int stride):
Interpretation{ mother, id, offset, stride},
m_start{start},
m_end{end},
m_outer_circle_start{outer_circle_start},
m_outer_circle_end{outer_circle_end},
m_mode{mode},
m_mid{mid},
m_radius{radius},
m_fade_direction{fade_direction},
m_parameter{parameter},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()},
m_time_map{}
{
  m_type                  = 6; // circfade
  m_calc_specification    = 0; // once
  m_upload_specification  = 1; // with fac mat

  m_mid = cv::Vec2f(m_mid[0] * m_base->get_width(), m_mid[1] * m_base->get_height());
  if(m_mode > 1)
    std::cout << "\t ! you fool. Now the mode = linealinear." << std::endl;
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

void Circularfade::create_time_map(int id){
  std::cout<<"calc time map\n";
  cv::Mat pixel_times = cv::Mat( m_pnt_max.y,  m_pnt_max.x, CV_32FC2, cv::Vec2f(0.0f,0.0f));

  // int seg_start = m_base->get_seg_start(id);
  // int seg_end = m_base->get_seg_end(id);
  // int seg_start = m_base->get_start_frame();
  // int seg_end = m_base->get_last_frame();
  int seg_start = m_outer_circle_start;
  int seg_end = m_outer_circle_end;
  // std::cout << "init state \t start: " << m_outer_circle_start << "\t end: " << m_outer_circle_end << std::endl;
  float null_null = sqrt(pow(m_pnt_min.x - m_mid[0], 2) + pow(m_pnt_min.y - m_mid[1], 2));
  float null_max = sqrt(pow(m_pnt_min.x - m_mid[0], 2) + pow(m_pnt_max.y - m_mid[1], 2));
  float max_null = sqrt(pow(m_pnt_max.x - m_mid[0], 2) + pow(m_pnt_min.y - m_mid[1], 2));
  float max_max = sqrt(pow(m_pnt_max.x - m_mid[0], 2) + pow(m_pnt_max.y - m_mid[1], 2));
  float max_distance = 0;

  if(max_distance < null_null)
    max_distance = null_null;
  else if(max_distance < null_max)
    max_distance = null_max;
  else if(max_distance < max_null)
    max_distance = max_null;
  else if(max_distance < max_max)
    max_distance = max_max;

  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {

    float* ptr_map = (float*) pixel_times.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {

      float *uc_pixel_map = ptr_map;
      float distance = sqrt(pow((float)col - m_mid[0], 2) + pow((float)row - m_mid[1], 2));
      float max_tmp=max_distance;

      float start_border = -1;
      float end_border = -1;
      if(m_fade_direction){
        if(distance < m_radius){
          start_border = m_start;
          end_border = m_end;
          // std::cout << "dis < rad \t\t start_b: " << start_border << "\t end_b: " << end_border << std::endl;
        }
        else{
            max_tmp -= m_radius;
            distance -= m_radius;
            // std::cout << "before dis > rad \t dis: " << distance << "\t max_dis: " << max_distance << std::endl;

            if(m_mode == 0 /*power*/){
              max_tmp = pow(max_tmp, m_parameter);
              distance = pow(distance, m_parameter);
            }
            else if(m_mode == 1 /*sigmoid*/){
              distance = utils::sigmoid(distance, m_parameter, max_tmp / 2);
              float min_clip = utils::sigmoid(0, m_parameter, max_tmp / 2);
              max_tmp = utils::sigmoid(max_tmp, m_parameter, max_tmp / 2);
              max_tmp -= min_clip;
              distance -= min_clip;
            }
            else /*linear*/{
              max_tmp = pow(max_tmp, 1);
              distance = pow(distance, 1);
            }

            // std::cout << "after dis > rad \t dis: " << distance << "\t max_dis: " << max_distance << std::endl;

            // std::cout << "b dis > rad \t\t start_b: " << start_border << "\t end_b: " << end_border << std::endl;
            float fade_fac = distance / max_tmp;
            start_border = fade_fac * (seg_start - m_start) + m_start;
            end_border = fade_fac * (seg_end - m_end) + m_end;
            //std::cout << "a dis > rad \t\t start_b: " << start_border << "\t end_b: " << end_border << std::endl;
          }
      }
      else{
        if(distance > m_radius){
          start_border = m_start;
          end_border = m_end;
        }
        else{
          max_tmp = m_radius;
          distance = m_radius - distance;

          if(m_mode == 0 /*power*/){
            max_tmp = pow(max_tmp, m_parameter);
            distance = pow(distance, m_parameter);
          }
          else if(m_mode == 1 /*sigmoid*/){
            distance = utils::sigmoid(distance, m_parameter, max_tmp / 2);
            float min_clip = utils::sigmoid(0, m_parameter, max_tmp / 2);
            max_tmp = utils::sigmoid(max_tmp, m_parameter, max_tmp / 2);
            max_tmp -= min_clip;
            distance -= min_clip;
          }
          else /*linear*/{
            max_tmp = pow(max_tmp, 1);
            distance = pow(distance, 1);
          }

          //wirte
          float fade_fac = distance / max_tmp;
          start_border = fade_fac * (seg_start - m_start) + m_start;
          end_border = fade_fac * (seg_end - m_end) + m_end;
        }
      }

      uc_pixel_map[0] = start_border;
      uc_pixel_map[1] = end_border;
      ptr_map += 2;
    }
  }
  m_time_map[id] = pixel_times;

}

void Circularfade::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();
  int first = m_base->get_seg_start(id);
  // int seg_end = m_base->get_seg_end(id);
  // int seg_start = m_base->get_start_frame();
  // int seg_end = m_base->get_last_frame();
  int seg_start = m_outer_circle_start;
  int seg_end = m_outer_circle_end;

  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);


    if( start == first )
    {
      create_time_map(id);
      #ifdef show_time
          auto end_time = std::chrono::high_resolution_clock::now();
          auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
          std::cout << "\t\t + Circularfade (TimeMap) time: \t" << duration << std::endl;
      #endif
    }

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
      compute_frame(result, fac_mat, tmp_frame, sign, start + i, seg_start, seg_end, id);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + Circularfade ("<<length<<") time: \t" << duration << std::endl;
    #endif
  }
}

void Circularfade::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign, int frame_num, int seg_start, int seg_end,  int seg_id) {
  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_map            =  (float*) m_time_map[seg_id].ptr(row);
    float* ptr_res            =  (float*) result.ptr(row);
    float* ptr_fac            =  (float*) fac_mat.ptr(row);
    const float* ptr_current  =  (float*) current_frame.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_map           = ptr_map;
      float *uc_pixel_res           = ptr_res;
      float *uc_pixel_fac           = ptr_fac;
      const float *uc_pixel_current = ptr_current;

      float start_border = uc_pixel_map[0];
      float end_border = uc_pixel_map[1];

      if(frame_num > (int)start_border && frame_num <= (int)end_border){
        uc_pixel_res[0] += sign*uc_pixel_current[0];
        uc_pixel_res[1] += sign*uc_pixel_current[1];
        uc_pixel_res[2] += sign*uc_pixel_current[2];
        uc_pixel_fac[0] += sign;
        uc_pixel_fac[1] += sign;
        uc_pixel_fac[2] += sign;
      }
      else if(frame_num == (int)start_border) {
        float weight = 1 - fabs((float)start_border - (int)start_border);
        weight*=sign;
        uc_pixel_res[0] += weight * uc_pixel_current[0];
        uc_pixel_res[1] += weight * uc_pixel_current[1];
        uc_pixel_res[2] += weight * uc_pixel_current[2];
        uc_pixel_fac[0] += weight;
        uc_pixel_fac[1] += weight;
        uc_pixel_fac[2] += weight;
      }
      else if(frame_num == (int)end_border + 1){
        float weight = fabs((float)end_border - (int)end_border);
        weight*=sign;
        uc_pixel_res[0] += weight * uc_pixel_current[0];
        uc_pixel_res[1] += weight * uc_pixel_current[1];
        uc_pixel_res[2] += weight * uc_pixel_current[2];
        uc_pixel_fac[0] += weight;
        uc_pixel_fac[1] += weight;
        uc_pixel_fac[2] += weight;
      }

      //shift ptr:
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_fac     += m_ptr_delta;
      ptr_map += 2;
    }
  }
}

void Circularfade::manipulate(int start, int end, int outer_circle_start, int outer_circle_end, int mode, cv::Vec2f mid, float radius, bool fade_direction, float parameter, int offset, int stride){
  bool update_status = false;

  mid = cv::Vec2f(mid[0] * m_base->get_width(), mid[1] * m_base->get_height());

  if(m_start != start){
    m_start = start;
    update_status = true;
  }
  if(m_end != end){
    m_end = end;
    update_status = true;
  }
  if(m_outer_circle_start != outer_circle_start){
    m_outer_circle_start = outer_circle_start;
    update_status = true;
  }
  if(m_outer_circle_end != outer_circle_end){
    m_outer_circle_end = outer_circle_end;
    update_status = true;
  }
  if(m_mode != mode){
    if(mode > 1){
      std::cout << "\t ! you fool. Now the mode = linealinear." << std::endl;
    }
    m_mode = mode;
    update_status = true;
  }
  if(m_mid != mid){
    m_mid = mid;
    update_status = true;
  }
  if(m_radius != radius){
    // std::cout << radius << "; ->"<< m_radius << std::endl;
    m_radius = radius;
    update_status = true;
  }
  if(m_fade_direction != fade_direction){
    m_fade_direction = fade_direction;
    update_status = true;
  }
  if(m_parameter != parameter){
    m_parameter = parameter;
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
