#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/paint.hpp>
#include <../include/base.hpp>
#include <../include/utils.hpp>


Paint::Paint(std::shared_ptr< Base > mother, int id, int type, std::shared_ptr<std::vector<ColorCoords>> colorTimes , int offset, int stride):
Interpretation{ mother, id, type, offset, stride},
m_time_map{},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()},
m_ptr_delta{mother->get_img_delta()}
{
  for(unsigned int i = 0; i < colorTimes->size(); i++){
    m_colors.push_back((*colorTimes)[i].color);
    m_start.push_back((*colorTimes)[i].start);
    m_end.push_back((*colorTimes)[i].end);
  }

  m_calc_specification = 2;
}

Paint::~Paint(){
  //std::cout<<"deleting interpretation-boost\n";
}

int Paint::getTypenumber(){
  return m_type;
}

int Paint::get_calculation_specification(){
  return m_calc_specification;  //standard sum-game
}
void Paint::create_time_map(){
  std::cout<<"creating time map\n";
  std::cout<<"impimmlment here :)\n";
}

void Paint::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();

  int seg_start = m_base->get_seg_start(id);
  int seg_end = m_base->get_seg_end(id);

  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
  if( start == seg_start )
    create_time_map();

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
      compute_frame(result, fac_mat, tmp_frame, start + i, id);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + Paint ("<<length<<") time: \t" << duration << std::endl;
    #endif
  }
}

void Paint::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int frame_num,  int seg_id) {
    cv::Mat out = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
    for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
      //ptr:
      float* ptr_map            =  (float*) m_time_map[seg_id].ptr(row);
      float* ptr_out            =  (float*) out.ptr(row);
      float* ptr_res            =  (float*) result.ptr(row);
      float* ptr_fac            =  (float*) fac_mat.ptr(row);
      const float* ptr_current  =  (float*) current_frame.ptr(row);

      for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
        //ptr:
        float *uc_pixel_map           = ptr_map;
        float *uc_pixel_out           = ptr_out;
        float *uc_pixel_res           = ptr_res;
        float *uc_pixel_fac           = ptr_fac;
        const float *uc_pixel_current = ptr_current;

        float start_border = uc_pixel_map[0];
        float end_border = uc_pixel_map[1];

        if(frame_num > (int)start_border && frame_num <= (int)end_border){
          uc_pixel_res[0] += uc_pixel_current[0];
          uc_pixel_res[1] += uc_pixel_current[1];
          uc_pixel_res[2] += uc_pixel_current[2];
          uc_pixel_fac[0] += 1;
          uc_pixel_fac[1] += 1;
          uc_pixel_fac[2] += 1;
        }
        else if(frame_num == (int)start_border) {
          float weight = 1 - fabs((float)start_border - (int)start_border);
          uc_pixel_res[0] += weight * uc_pixel_current[0];
          uc_pixel_res[1] += weight * uc_pixel_current[1];
          uc_pixel_res[2] += weight * uc_pixel_current[2];
          uc_pixel_fac[0] += weight;
          uc_pixel_fac[1] += weight;
          uc_pixel_fac[2] += weight;
        }
        else if(frame_num == (int)end_border + 1){
          float weight = fabs((float)end_border - (int)end_border);
          uc_pixel_res[0] += weight * uc_pixel_current[0];
          uc_pixel_res[1] += weight * uc_pixel_current[1];
          uc_pixel_res[2] += weight * uc_pixel_current[2];
          uc_pixel_fac[0] += weight;
          uc_pixel_fac[1] += weight;
          uc_pixel_fac[2] += weight;
        }

        //shift ptr:
        ptr_out     += m_ptr_delta;
        ptr_res     += m_ptr_delta;
        ptr_current += m_ptr_delta;
        ptr_fac     += m_ptr_delta;
        ptr_map += 2;
      }
    }

  }

void Paint::manipulate(cv::Mat ref_frame, float threshhold, int offset, int stride){
  // bool update_status = false;
  // cv::Mat tmp;
  // double min = 0, max = 0;
  // cv::absdiff(m_reference, ref_frame, tmp);
  // cv::minMaxLoc(tmp, &min, &max);
  //
  // if(max > 0){
  //   m_reference = ref_frame;
  //   update_status = true;
  // }
  // if(m_threshhold != threshhold){
  //   m_threshhold = threshhold;
  //   update_status = true;
  // }
  // if(offset != m_offset){
  //   m_offset = offset;
  //   update_status = true;
  // }
  // if(stride != m_stride){
  //   m_stride = stride;
  //   update_status = true;
  // }
  // if(update_status){
  //   update_connections();
  // }
}
