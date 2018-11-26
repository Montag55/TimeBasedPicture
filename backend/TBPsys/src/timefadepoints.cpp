#include <chrono>
#include <thread>
#include <../include/interpretation.hpp>
#include <../include/timefadepoints.hpp>
#include <../include/segment.hpp>

#include <string.h>
#include <vector>
//Timefadepoints(VideoCapture& vid, int img_type, int id, int start_pnt,  std::shared_ptr<std::vector<float>> weights);

Timefadepoints::Timefadepoints(std::shared_ptr< Base > mother, int id, int type, int mode_d, int mode_f, float param, std::shared_ptr< std::vector<cv::Vec4f>> points,  int offset, int stride):
Interpretation{mother, id, type, offset, stride},
m_mode_d{mode_d},
m_mode_f{mode_f},
m_param{param},
m_points{points},
m_ptr_delta{mother->get_img_delta()},
m_pnt_min{mother->get_min_Point()},
m_pnt_max{mother->get_max_Point()}
{
  m_calc_specification = 2;
}


Timefadepoints::~Timefadepoints(){
  std::cout<<"\t > deleting interpretation-avg\n";
}

int Timefadepoints::getTypenumber(){
  return m_type;
}

int Timefadepoints::get_calculation_specification(){
  return m_calc_specification; //standard sum-game
}

void Timefadepoints::calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) {
  auto start_time = std::chrono::high_resolution_clock::now();

  int seg_start = m_base->get_seg_start(id);
  int seg_end = m_base->get_seg_end(id);

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
      compute_frame(result, fac_mat, tmp_frame, sign, start + i, seg_start, seg_end);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    #ifdef show_time
        std::cout << "\t\t + Circularfade ("<<length<<") time: \t" << duration << std::endl;
    #endif
  }
}

void Timefadepoints::compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int sign, int frame_num, int seg_start, int seg_end) {
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

      //float distance = sqrt(pow((float)col - m_mid.x, 2) + pow((float)row - m_mid.y, 2));
      /*
      todo:
      distances to all points with m_mode
      weight times
        -calc border
        cases:
          ->inside
          ->interpolate
          ->out

      */
      std::list<cv::Vec3f> pnts_sorted; //start, end, distance
      for(int i=0; i < (*m_points).size(); i++){
        //calc distance
        float pnt_dis=-1;
        if( m_mode_d == 0 ){
          //abs 1d
          pnt_dis = abs(col-(*m_points)[i][0])+abs(row-(*m_points)[i][1]);
        }else{
          //euklid 2d
          pnt_dis = sqrt(pow(col-(*m_points)[i][0],2))+sqrt(pow(row-(*m_points)[i][1],2));
        }
        //insert according to distance
        if(pnts_sorted.size()==0){
          pnts_sorted.push_back(cv::Vec3f((*m_points)[i][2],(*m_points)[i][3], pnt_dis));
        }else{
          for(std::list<cv::Vec3f>::iterator o = pnts_sorted.begin(); o != pnts_sorted.end(); ++o){
            if((*o)[2] > pnt_dis){
              pnts_sorted.insert(o, cv::Vec3f((*m_points)[i][2],(*m_points)[i][3], pnt_dis));
              break;
            }
          }
        }
      }
      //points are sorted now!

      //calc borders:
      float ref_dis = (*pnts_sorted.begin())[2] + (pnts_sorted.back())[2];
      ref_dis = pow(ref_dis,m_param);

      float start_border = 0;
      float end_border = 0;
      float fac_sum=0;
      for(std::list<cv::Vec3f>::iterator o = pnts_sorted.begin(); o != pnts_sorted.end(); ++o){
        fac_sum+=pow(ref_dis-(*o)[2],m_param);
      }

      float seg_delta= seg_end-seg_start;

      for(std::list<cv::Vec3f>::iterator o = pnts_sorted.begin(); o != pnts_sorted.end(); ++o){
        double influence = (std::pow(ref_dis-(*o)[2],m_param)) / fac_sum;//sum)*(
        start_border+=(*o)[0]*influence;
        end_border  +=(*o)[1]*influence;
      }

      start_border= seg_start + start_border*seg_delta;

      if(frame_num > (int)start_border && frame_num <= (int)end_border){
        uc_pixel_res[0] += uc_pixel_current[0];
        uc_pixel_res[1] += uc_pixel_current[1];
        uc_pixel_res[2] += uc_pixel_current[2];
        uc_pixel_fac[0] += 1;
        uc_pixel_fac[1] += 1;
        uc_pixel_fac[2] += 1;
      }
      //nun fehlt nur noch interframe interpolation...

        //max dis

        // if(m_fade_direction){
        //   if(distance < m_radius){
        //     if(frame_num >= m_start && frame_num <= m_end){
        //       uc_pixel_res[0] += uc_pixel_current[0];
        //       uc_pixel_res[1] += uc_pixel_ current[1];
        //       uc_pixel_res[2] += uc_pixel_current[2];
        //       uc_pixel_fac[0] += 1;
        //       uc_pixel_fac[1] += 1;
        //       uc_pixel_fac[2] += 1;
        //     }
        //   }

      //shift ptr:
      ptr_out     += m_ptr_delta;
      ptr_res     += m_ptr_delta;
      ptr_current += m_ptr_delta;
      ptr_fac     += m_ptr_delta;
    }
  }

}

void Timefadepoints::manipulate(int mode_d, int mode_f, float param, std::shared_ptr< std::vector<cv::Vec4f>> points,  int offset, int stride) {
  bool update_status = false;
  if(m_mode_d != mode_d){
    m_mode_d = mode_d;
    update_status = true;
  }
  if(m_mode_f != mode_f){
    m_mode_f = mode_f;
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
