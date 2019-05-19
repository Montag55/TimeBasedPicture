#ifndef PAINT_HPP
#define PAINT_HPP
#include <math.h>
#include <../include/transformation.hpp>
#include <../include/base.hpp>
#include <../include/struct.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

class Paint : public Transformation{
public:
  Paint(std::shared_ptr< Base > mother, int id, std::shared_ptr<std::vector<ColorCoords>> colorTimes , int offset, int stride);
  ~Paint()                            override;
  int getTypenumber()                 override;
  int get_calculation_specification() override;
  void update_Mask();
  void calc(int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) override;
  void compute_frame(cv::Mat& result, cv::Mat& fac_mat, cv::Mat& current_frame, int frame_num, int seg_id);
  void manipulate(std::shared_ptr<std::vector<ColorCoords>> colorTimes, int offset, int stride);
  void create_time_map();
  int get_time_min(int current, int id);
  int compute_seg_min(int id);
  void reset_routine(cv::Mat& result, cv::Mat& fac_mat, int seg_id);

  //int get_time_max(int current);
  cv::Mat compare_mats(cv::Mat current, cv::Mat old, bool& changes);
  void add_connection( int id, Segment* segment) override;

private:
  //CLR <-> Time Date:
  std::vector<cv::Vec3f>  m_colors;
  std::vector<int>        m_start;
  std::vector<int>        m_end;


  cv::Mat                 m_mask;
  cv::Mat                 m_time_mat;

  cv::Point               m_pnt_min;
  cv::Point               m_pnt_max;
  int                     m_ptr_delta;

  std::map<int,bool>      m_reset;
  std::map<int,cv::Mat>   m_reset_mask;

  //Segment specific
  std::map<int,int>       m_seg_min;  //stores the current start frame
  //std::map<int,int>       m_seg_max;  //stores the current end frame
  std::map<int,cv::Mat>   m_seg_ist;  //two channel start and end: pondo to time_map, saves what is already calced

  //reset:
  /* state 0:
  if diff in start or end pix:
    -abs mat[x,y] = 0
    -fac mat[x,y] = 0
    -seg_ist[x,y] = new_time_map

    -set current frame if neccessary earlier
  */

  /* state 1:
  // vec of to visit parts
    kick according the actual frame
    when manipulate: check for new time zones and insert
      -some mat ops and comparison when manipulation
      -precise/perfect skipping

    or
    each frame check m_seg_ist[0].min and jump to that frame
      -mat op once per work cycle:
        -not precised(work_size lose) skipping, not perfect

  */

};

#endif
