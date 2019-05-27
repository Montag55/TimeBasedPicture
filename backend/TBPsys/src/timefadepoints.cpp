/*
Author:     Phil Jungschlaeger
Date:       27.05.2019
*/
#include <chrono>
#include <thread>
#include <../include/transformation.hpp>
#include <../include/timefadepoints.hpp>
#include <../include/segment.hpp>

#include <string.h>
#include <vector>
//            addtransformation 7 0 0 1 4 2 0 0 0 0 0 1079 0.5 0.5 1919 0 0.7 0.7 1919 1079 0.8 1

Timefadepoints::Timefadepoints(std::shared_ptr< Base > mother, int id, int mode_d, int num_pnts, float param, std::shared_ptr< std::vector<cv::Vec4f>> points,  int offset, int stride):
Transformation{mother, id, offset, stride},
m_mode_d{mode_d},
m_num_pnts{num_pnts},
m_param{param},
m_points{points},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()},
m_time_map{}
{
  m_type                = 7; // fadepnts
  m_calc_specification  = 0; // free
  m_upload_specification = 1; //with fac mat
  if(m_num_pnts < 2 && num_pnts != -1){
    m_num_pnts = -1;
    std::cout << "\t ! number of points smaller 2. Automaticly set to 2. \n";
  }
}


Timefadepoints::~Timefadepoints(){
  std::cout<<"\t > deleting transformation-avg\n";
}

int Timefadepoints::getTypenumber(){
  return m_type;
}

int Timefadepoints::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Timefadepoints::create_time_map(int id){
  bool error_occured  = false;
  int seg_start = m_base->get_seg_start(id);
  int seg_end = m_base->get_seg_end(id);

  std::cout<<"calc time map\n";
  cv::Mat pixel_times = cv::Mat( m_pnt_max.y,  m_pnt_max.x, CV_32FC2, cv::Vec2f(0.0f,0.0f));
  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_map            =  (float*) pixel_times.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_map           = ptr_map;

      std::list<cv::Vec3f> pnts_sorted; //start, end, distance
      for(int i = 0; i < (*m_points).size(); i++){
        //calc distance
        float pnt_dis = -1;
        //std::cout<<(*m_points)[i][0]<<", "<<(*m_points)[i][1]<<"\n";
        if( m_mode_d == 0 /*abd 1D*/) {
          pnt_dis = abs(col - (*m_points)[i][0] * m_base->get_width()) + abs(row - (*m_points)[i][1] * m_base->get_height());
        }
        else /*euklid 2D*/ {
          pnt_dis = sqrt(pow(col - (*m_points)[i][0] * m_base->get_width(), 2) + pow(row - (*m_points)[i][1] * m_base->get_height(), 2));
        }
        //insert according to distance
        if(pnts_sorted.size() == 0){
          pnts_sorted.push_back(cv::Vec3f((*m_points)[i][2], (*m_points)[i][3], pnt_dis));
        }
        else{
          bool inserted = false;
          for(std::list<cv::Vec3f>::iterator o = pnts_sorted.begin(); o != pnts_sorted.end(); ++o){
            if((*o)[2] > pnt_dis){
              pnts_sorted.insert(o, cv::Vec3f((*m_points)[i][2], (*m_points)[i][3], pnt_dis));
              inserted = true;
              break;
            }
          }
          if(!inserted){
            pnts_sorted.push_back(cv::Vec3f((*m_points)[i][2], (*m_points)[i][3], pnt_dis));
          }
        }
      }
      //calc borders:
      if(m_num_pnts!=-1){
        pnts_sorted.resize(m_num_pnts);
      }

      float ref_dis = (*pnts_sorted.begin())[2] + (pnts_sorted.back())[2];
      float start_border = 0;
      float end_border = 0;
      float fac_sum = 0;

      for(std::list<cv::Vec3f>::iterator o = pnts_sorted.begin(); o != pnts_sorted.end(); ++o){

        if( m_param < 0 /*abs 1D*/){
          fac_sum += utils::sigmoid(ref_dis - (*o)[2], -m_param, ref_dis / 2);
        }
        else /*euklid 2D*/{
          //fac_sum += sqrt(std::pow(abs(ref_dis - (*o)[2]), m_param));
          fac_sum += std::pow(abs(1 -((*o)[2]/ref_dis)), m_param);
        }
        //std::cout<<std::pow(abs(ref_dis-(*o)[2]),m_param)<<"val\n";
      }

      float seg_delta = seg_end - seg_start;

      for(std::list<cv::Vec3f>::iterator o = pnts_sorted.begin(); o != pnts_sorted.end(); ++o){

        double influence = 0;
        if( m_param < 0 /*abs 1D*/){
          influence = utils::sigmoid(ref_dis - (*o)[2], -m_param, ref_dis / 2) / fac_sum;
        }
        else if(m_param > 0){
          //influence = sqrt(std::pow(abs(ref_dis - (*o)[2]), m_param)) / fac_sum;
          influence = std::pow(abs(1 -((*o)[2]/ref_dis)), m_param) / fac_sum;
        }

        //std::cout<<"influ "<< influence<<"\n";
        start_border += (*o)[0] * influence;
        end_border += (*o)[1] * influence;
      }
      /*: to map fade [0-1] to segment
      start_border = seg_start + start_border * seg_delta;
      end_border = seg_start + end_border * seg_delta;
      */
      uc_pixel_map[0] = start_border;
      uc_pixel_map[1] = end_border;
      //shift ptr:
      ptr_map     += 2;
    }
  }

  if(error_occured){
    std::cout<<"was sth. wrong creating time map!\n";
  }
  m_time_map[id] = pixel_times;
}

void Timefadepoints::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();

  int seg_start = m_base->get_seg_start(id);
  //int seg_end = m_base->get_seg_end(id);

  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
  if( start == seg_start )
  {
    create_time_map(id);
    #ifdef show_time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
        std::cout << "\t\t + TimeFadePoints (TimeMap) time: \t" << duration << std::endl;
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
      compute_frame(result, fac_mat, tmp_frame, start + i, id, sign);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + Timefadepoints ("<<length<<") time: \t" << duration << std::endl;
    #endif
  }
}

void Timefadepoints::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int frame_num, int seg_id, int sign) {
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
      ptr_map     += 2;
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_fac     += m_ptr_delta;
    }
  }

}

void Timefadepoints::manipulate(int mode_d, int num_pnts, float param, std::shared_ptr< std::vector<cv::Vec4f>> points,  int offset, int stride) {
  bool update_status = false;
  if(m_mode_d != mode_d){
    m_mode_d = mode_d;
    update_status = true;
  }
  if(m_num_pnts != num_pnts){
    if(num_pnts < 2 && num_pnts != -1){
      num_pnts = -1;
      std::cout << "\t ! number of points smaller 2. Automaticly set to 2. \n";
    }

    m_num_pnts = num_pnts;
    update_status = true;
  }
  if(m_param != param){
    m_param = param;
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
