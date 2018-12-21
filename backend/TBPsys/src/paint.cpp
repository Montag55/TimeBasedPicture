#include <chrono>
#include <thread>
#include <memory>
#include <string.h>
#include <vector>

#include <../include/interpretation.hpp>
#include <../include/paint.hpp>
#include <../include/base.hpp>
#include <../include/utils.hpp>

// addinterpretation 9 0 0 0 0 255 0 10 0 255 0 40 50 255 0 0 80 90 255 255 0 0 100 0 255 255 50 50


Paint::Paint(std::shared_ptr< Base > mother, int id, std::shared_ptr<std::vector<ColorCoords>> colorTimes , int offset, int stride):
Interpretation{ mother, id, offset, stride},
m_time_map{},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()},
m_ptr_delta{mother->get_img_delta()}
{
  update_Mask();

  for(unsigned int i = 0; i < colorTimes->size(); i++){
    m_colors.push_back((*colorTimes)[i].color);
    m_start.push_back((*colorTimes)[i].start);
    m_end.push_back((*colorTimes)[i].end);
  }
  m_type                  = 9; //paint
  m_calc_specification    = 2; //once
  m_upload_specification  = 1; //with fac mat
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

void Paint::update_Mask(){
  m_mask = cv::imread("maskPaintInterpretation" + std::to_string(get_id()) + ".png");

  if(m_mask.empty()){
    m_mask = cv::Mat(m_pnt_max.y, m_pnt_max.x, CV_32FC3, cv::Scalar(0, 0, 0));
    std::cout << "image not loaded. Loading empty Image\n";
  }
  else {
    m_mask.convertTo(m_mask, CV_32FC3);
  }
}

void Paint::create_time_map(int id){
  bool error_occured  = false;
  cv::Mat pixel_times = cv::Mat( m_pnt_max.y,  m_pnt_max.x, CV_32FC2, cv::Scalar(0.0f,0.0f));

  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_map            =  (float*) pixel_times.ptr(row);
    float* ptr_color          =  (float*) m_mask.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_map             = ptr_map;
      float *uc_pixel_color           = ptr_color;
      float start_border = -1;
      float end_border = -1;

      cv::Vec3f tmp_px_color = cv::Vec3f(uc_pixel_color[0], uc_pixel_color[1], uc_pixel_color[2]);

      if(tmp_px_color == cv::Vec3f(0, 0, 0) /* black */){
          start_border = -1;
          end_border = -1;
      }
      else /* color */{
        bool color_check = false;
        for(unsigned int idx = 0; idx < m_colors.size(); idx++){
          if(m_colors[idx] == tmp_px_color){
            start_border = m_start[idx];
            end_border = m_end[idx];
            color_check = true;
            break;
          }
        }
        if(color_check == false){
          // std::cout << tmp_px_color << std::endl;
          error_occured = true;
          start_border = -1;
          end_border = -1;
        }
      }

      uc_pixel_map[0] = start_border;  //store startframe inclusive
      uc_pixel_map[1] = end_border;    //store endframe inclusive
      //shift ptr:
      ptr_map     += 2;
      ptr_color   += 3;
    }
  }

  if(error_occured){
    std::cout<<"was sth. wrong with colors?, a color in image was not in the color vec!\n";
  }

  std::cout<<"created time map\n";
  m_time_map[id] = pixel_times;
}

int Paint::get_time_min(int current){
  int tmp_s = -1;

  for (int i = 0; i < m_start.size(); i++) {
    if(m_start[i] <= current && m_end[i] >= current){
      // std::cout <<"we cannot shortcut\n";
      return current;
    }
  }

  for (int i = 0; i < m_start.size(); i++) {
    if(m_start[i] > current){
      if(m_start[i] < tmp_s || tmp_s == -1) {
        tmp_s = m_start[i];
      }
    }
  }

  return tmp_s;
}

int Paint::get_time_max(int current){
  std::cout<<"Paint::get_time_max(int current) not yet implemented \n";
  return -1;
}

void Paint::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();

  int seg_start = m_base->get_seg_start(id);
  int seg_end = m_base->get_seg_end(id);

  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
  if( start == seg_start )
    create_time_map(id);

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

    // cv::Mat mask_start;
    // cv::Mat mask_end;
    //
    // int from_to_s[] = {0,0 , 0,1, 0,2};
    // cv::mixChannels(&m_time_map[seg_id], 1, &tmp_time_map_C3, 1, from_to_s, 3);
    // mask_start = (tmp_time_map_C3 <= frame_num);
    // mask_start.convertTo(mask_start, CV_32FC3);
    //
    // int from_to_e[] = {1,0 , 1,1, 1,2};
    // cv::mixChannels(&m_time_map[seg_id], 1, &tmp_time_map_C3, 1, from_to_e, 3);
    // mask_end = (tmp_time_map_C3 >= frame_num);
    // mask_end.convertTo(mask_end, CV_32FC3);mask_end
    //
    // result += mask_end.mul(mask_start).mul(current_frame) / 255;
    // fac_mat += mask_end.mul(mask_start) / 255;

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

        if(frame_num >= (int)start_border && frame_num <= (int)end_border){
          uc_pixel_res[0] += uc_pixel_current[0];
          uc_pixel_res[1] += uc_pixel_current[1];
          uc_pixel_res[2] += uc_pixel_current[2];
          uc_pixel_fac[0] += 1;
          uc_pixel_fac[1] += 1;
          uc_pixel_fac[2] += 1;
        }

        //shift ptr:
        ptr_res     += m_ptr_delta;
        ptr_current += m_ptr_delta;
        ptr_fac     += m_ptr_delta;
        ptr_map += 2;
      }
    }
  }

void Paint::manipulate(std::shared_ptr<std::vector<ColorCoords>> colorTimes, int offset, int stride){
  bool update_status = false;

  std::vector<cv::Vec3f> color_tmp;
  std::vector<int> start_tmp;
  std::vector<int> end_tmp;

  for(unsigned int i = 0; i < colorTimes->size(); i++){
    color_tmp.push_back((*colorTimes)[i].color);
    start_tmp.push_back((*colorTimes)[i].start);
    end_tmp.push_back((*colorTimes)[i].end);
  }

  if(m_colors != color_tmp){
    m_colors = color_tmp;
    update_status = true;
  }
  if(m_start != start_tmp){
    m_start = start_tmp;
    update_status = true;
  }
  if(m_end != end_tmp){
    m_end = end_tmp;
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
    update_Mask();
  }
}
