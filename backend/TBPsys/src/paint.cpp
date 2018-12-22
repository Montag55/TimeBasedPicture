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
//TODO:
//  -only reset calc after manipulation at necessary pixels

Paint::Paint(std::shared_ptr< Base > mother, int id, std::shared_ptr<std::vector<ColorCoords>> colorTimes , int offset, int stride):
Interpretation{ mother, id, offset, stride},
m_time_mat{},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()},
m_ptr_delta{mother->get_img_delta()},
//m_seg_min{},
//m_seg_max{},
m_reset{},
m_reset_mask{},
m_seg_ist{}
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
  create_time_map();
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
    std::cout << "image not loaded.\n";
    std::cout<<"second try in a second:\n";
    m_mask = cv::imread("maskPaintInterpretation" + std::to_string(get_id()) + ".png");
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    if(m_mask.empty()){
      m_mask = cv::Mat(m_pnt_max.y, m_pnt_max.x, CV_32FC3, cv::Scalar(0, 0, 0));
      std::cout << "image not loaded. Loading empty Image...\n";
    }
    m_mask.convertTo(m_mask, CV_32FC3);


  }
  else {
    std::cout << "image loaded\n";

    m_mask.convertTo(m_mask, CV_32FC3);
  }
}

void Paint::create_time_map(){
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

      cv::Vec3f tmp_px_color = cv::Vec3f(uc_pixel_color[2], uc_pixel_color[1], uc_pixel_color[0]);

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
  m_time_mat = pixel_times;
}

int Paint::get_time_min(int current, int id){
  int min = m_seg_min[id];
  std::cout<<"min: "<<std::to_string(min) <<"vs. cur "<<std::to_string(current)<<"\n";
  if(min == -1){// fully computed
    std::cout<<"segment is read painted\n";
    return -1;
  }else if(min == current){
    std::cout<<"simply continue\n";
    return current;
  }else if(min < current){
    std::cout<<"setting segment calc frame from "<<std::to_string(current)<< " back to: "<< std::to_string(min)<<"\n";
    return min;
  }else if(min > current){
    std::cout<<" shortcutting from "<<std::to_string(current)<<" to: "<< std::to_string(min)<<"\n";
    return min;
  }
}

void Paint::reset_routine(cv::Mat& result, cv::Mat& fac_mat, int seg_id){

  if(m_reset[seg_id]==true){
    std::cout<<"do reset routine!\n";
    ////////////////
    int min= -1; // always update min
    for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
      //ptr:
      float* ptr_seg            =  (float*) m_seg_ist[seg_id].ptr(row); //reset partiallx
      float* ptr_dif            =  (float*) m_reset_mask[seg_id].ptr(row);  //defines reset
      float* ptr_res            =  (float*) result.ptr(row);      //reset partially
      float* ptr_fac            =  (float*) fac_mat.ptr(row);     //reset_partially
      float* ptr_source         =  (float*) m_time_mat.ptr(row);  //diese werte nehmen

      for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
        //ptr:
        float *uc_pixel_seg =ptr_seg;
        float *uc_pixel_dif =ptr_dif;
        float *uc_pixel_res =ptr_res;
        float *uc_pixel_fac =ptr_fac;
        float *uc_pixel_source =ptr_source;
        //reset:
        if(uc_pixel_dif[0] == 0){//rest neccessary
          //std::cout<<"ressetting pixel\n";
          //reset:seg.result
          uc_pixel_res[0]=0;
          uc_pixel_res[1]=0;
          uc_pixel_res[2]=0;
          //reset: seg.facs
          uc_pixel_fac[0]=0;
          uc_pixel_fac[1]=0;
          uc_pixel_fac[2]=0;
          //ist stand:
          uc_pixel_seg[0]=uc_pixel_source[0];
          uc_pixel_seg[1]=uc_pixel_source[1];
          //reset dif:
          uc_pixel_dif[0]=1;
        }

        //get min:
        if(uc_pixel_seg[0]>=0 && min == -1){
          min = uc_pixel_seg[0]; //when min is not written yet
        }else if(uc_pixel_seg[0]>=0 && min > uc_pixel_seg[0]){
          min = uc_pixel_seg[0]; //found smaller min!
        }

        //shift ptr:
        ptr_dif     += 1;
        ptr_res     += m_ptr_delta;
        ptr_fac     += m_ptr_delta;

        ptr_seg     += 2;
        ptr_source  += 2;
      }
    }
    //std::cout<<"new min: "<<std::to_string(min)<<"\n";
    m_seg_min[seg_id]=min;
    ////////////////
    m_reset[seg_id]=false;
  }else{
    std::cout<<"no reset routine!\n";
  }
}

void Paint::add_connection( int id, Segment* segment){
  std::cout<<"add_connection from derived\n";
  m_mutex_connections.lock();
  m_all_connections[id]=segment;
  m_mutex_connections.unlock();

  //get values when work starts!:
  m_reset[id]=false;
  m_reset_mask[id]= cv::Mat(m_pnt_max.y, m_pnt_max.x, CV_32FC1, cv::Scalar(1));
  m_seg_ist[id]=m_time_mat.clone();
  m_seg_min[id]=0;

}

// int Paint::get_time_max(int current){
//   std::cout<<"Paint::get_time_max(int current) not yet implemented \n";
//   return -1;
// }

void Paint::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();

  int seg_start = m_base->get_seg_start(id);
  int seg_end = m_base->get_seg_end(id);

  cv::Mat tmp_frame;
  m_video->set( CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
  //check if time map and all exit

  //if( start == seg_start )
  //   create_time_map();

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
    //std::cout<<"framenum: "<<frame_num<<"\n";
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
    int min= -1; // always update min

    for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
      //ptr:
      float* ptr_map            =  (float*) m_seg_ist[seg_id].ptr(row);
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

        if(frame_num == (int)start_border ){
          //std::cout<<"pixel_get vals\n";
          uc_pixel_res[0] += uc_pixel_current[0];
          uc_pixel_res[1] += uc_pixel_current[1];
          uc_pixel_res[2] += uc_pixel_current[2];
          uc_pixel_fac[0] += 1;
          uc_pixel_fac[1] += 1;
          uc_pixel_fac[2] += 1;
          uc_pixel_map[0] =frame_num +1 ;

          if(start_border == end_border && start_border>=0){
            //std::cout<<std::to_string(start_border)<<" <-f: pixel done\n";
            uc_pixel_map[0]= -1; //set as done!
          }
        }

        //min:
        if(uc_pixel_map[0]>=0 && min == -1){
          min=uc_pixel_map[0]; //when min is not written yet
        }else if( uc_pixel_map[0]>=0 && min >   uc_pixel_map[0]){
          min= uc_pixel_map[0]; //found smaller min!
        }

        //shift ptr:
        ptr_res     += m_ptr_delta;
        ptr_current += m_ptr_delta;
        ptr_fac     += m_ptr_delta;
        ptr_map     += 2;
      }
    }
    //std::cout<<"new min: "<<std::to_string(min)<<"\n";
    m_seg_min[seg_id]=min;
  }

void Paint::manipulate(std::shared_ptr<std::vector<ColorCoords>> colorTimes, int offset, int stride){
  bool update_status = false;
  bool update_status_soft = false;
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
    update_status_soft = true;
  }
  if(m_start != start_tmp){
    m_start = start_tmp;
    update_status_soft = true;
  }
  if(m_end != end_tmp){
    m_end = end_tmp;
    update_status_soft = true;
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
    std::cout<<"big update\n";
    update_Mask();
    create_time_map();  //new time_map

    for (std::map<int,bool>::iterator it=m_reset.begin(); it!=m_reset.end(); ++it){
      it->second = true;
    }

    for (std::map<int,cv::Mat>::iterator it=m_reset_mask.begin(); it!=m_reset_mask.end(); ++it){
      cv::Mat new_reset_mask=cv::Mat(m_pnt_max.y, m_pnt_max.x, CV_32FC1, cv::Scalar(0));
      it->second = new_reset_mask;
    }

    update_connections();

  }else /*if(update_status_soft)*/{
    //get values when work starts!:

    ///////////
    std::cout<<"soft update\n";

    cv::Mat old_time_mat= m_time_mat.clone();
    update_Mask();
    create_time_map();  //new time_map
    bool changes=false;
    cv::Mat diff_mat = compare_mats(m_time_mat, old_time_mat, changes);

    if(changes){
      //chnages available
      for (std::map<int,cv::Mat>::iterator it=m_reset_mask.begin(); it!=m_reset_mask.end(); ++it){
        std::cout<<"multiplying diffs to Mat of Seg with id: "<<std::to_string(it->first)<<"\n";
        // cv::Mat new_time_map= m_time_mat.clone();
        cv::Mat C =it->second.mul(diff_mat);
        it->second= C;
        //multiplay_diffs(it->second, diff_mat);
        //it->second=cv::Mat(m_pnt_max.y, m_pnt_max.x, CV_8UC1, cv::Scalar(0));
      }

      for (std::map<int,bool>::iterator it=m_reset.begin(); it!=m_reset.end(); ++it){
        it->second = true;
      }

      trigger_connections();
    }
  }
  // }else{
  //   std::cout<<"alternative soft update\n";
  //
  //   cv::Mat old_time_mat= m_time_mat.clone();
  //   update_Mask();
  //   create_time_map();  //new time_map
  //   bool changes=false;
  //   cv::Mat diff_mat = compare_mats(m_time_mat, old_time_mat, changes);
  //
  //   if(changes){
  //     //chnages available
  //     for (std::map<int,cv::Mat>::iterator it=m_seg_ist.begin(); it!=m_seg_ist.end(); ++it){
  //       std::cout<<"multiplying diffs to Mat of Seg with id: "<<std::to_string(it->first)<<"\n";
  //       // cv::Mat new_time_map= m_time_mat.clone();
  //       it->second.mul(diff_mat);
  //     }
  //
  //     for (std::map<int,bool>::iterator it=m_reset.begin(); it!=m_reset.end(); ++it){
  //       it->second = true;
  //     }
  //
  //     trigger_connections();
  //   }
  // }
}

cv::Mat Paint::compare_mats(cv::Mat current, cv::Mat old, bool& changes){
  //if different: pixel = 1
  changes=false;
  cv::Mat out_mat=cv::Mat(m_pnt_max.y, m_pnt_max.x, CV_32FC1, cv::Scalar(1));
  for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row) {
    //ptr:
    float* ptr_current            =  (float*) current.ptr(row);
    float* ptr_old            =  (float*) old.ptr(row);
    float* ptr_out            =  (float*) out_mat.ptr(row);

    for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
      //ptr:
      float *uc_pixel_current       = ptr_current;
      float *uc_pixel_old           = ptr_old;
      float *uc_pixel_out           = ptr_out;

      if(uc_pixel_current[0] != uc_pixel_old[0] || uc_pixel_current[1] != uc_pixel_old[1]){
        uc_pixel_out[0]=0;  //needs to be reset!
        changes=true;
      }else{
        uc_pixel_out[0]=1;
      }

      //shift ptr:
      ptr_current += 2;
      ptr_old     += 2;
      ptr_out     += 1;
    }
  }
  return out_mat;
}
