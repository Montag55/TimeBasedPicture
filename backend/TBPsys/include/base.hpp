/*
Author:     Phil Jungschlaeger, Lucas Hübner
Date:       27.05.2019
*/
#ifndef BASE_HPP
#define BASE_HPP

/*
The BASE is our main C++ Interface to load videos and interpret them as images.
We will use a QT-GUI and a Terminal Interface...

the base basically contains the reference video, organizes all segments and
provides the resulting image. you cann add segments, but you have to store
and edit them yourself...
*/

class Segment;
#include <mutex>
#include <vector>
#include <thread>
#include <string.h>

#include "segment.hpp"
#include "transformation.hpp"

#include <opencv2/opencv.hpp>

class Base : public std::enable_shared_from_this<Base> {
public:
  Base(std::string const& video_name);
  ~Base();
  void compressVideoFiles();


  //ORGA::::::::::::::::::::::::::::::::::::::::::::::::
  bool                save(std::string file);
  bool                connect(int id_segment, int id_transformation);
  int                 add_segment(int start, int end, float local_i, float global_i);
  int                 add_transformation(int typ_i, std::string file_path);
  int                 add_transformation(int typ_i, int offset, int stride);
  int                 add_transformation(int typ_i, int offset, int stride, int ref_id, float threshhold, int modi);
  int                 add_transformation(int typ_i, int offset, int stride, std::string ref_file_path, float threshhold, int modi);
  int                 add_transformation(int typ_i, int offset, int stride, std::shared_ptr<std::vector<ColorCoords>> colorTimes);
  int                 add_transformation(int typ_i, int offset, int stride, float threshhold, int modi, std::shared_ptr<std::vector<float>> values);
  int                 add_transformation(int typ_i, int offset, int stride, int start, int end, int outer_circle_start, int outer_circle_end, int mode, cv::Vec2f mid, float radius, bool fade_direction, float parameter);
  int                 add_transformation(int typ_i, int offset, int stride, int mode_distance, int mode_function, float parameter, std::shared_ptr<std::vector<cv::Vec4f>> points);
  bool                delete_segment(int id);
  bool                manipulate_transformation(int id, std::string file_path);
  bool                manipulate_segment(int id, int start, int end, float local_i, float global_i, bool hasMask);
  bool                manipulate_transformation(int id, int offset, int stride);
  bool                manipulate_transformation(int id, std::shared_ptr<std::vector<ColorCoords>> colorTimes, int offset, int stride);
  bool                manipulate_transformation(int id, int ref_id, float threshhold, int modi, int offset, int stride);
  bool                manipulate_transformation(int id, std::string ref_file_path, float threshhold, int modi, int offset, int stride);
  bool                manipulate_transformation(int id, float threshhold, int modi, std::shared_ptr<std::vector<float>> values, int offset, int stride);
  bool                manipulate_transformation(int id, int start, int end, int outer_circle_start, int outer_circle_end, int mode, cv::Vec2f mid, float radius, bool fade_direction, float parameter, int offset, int stride);
  bool                manipulate_transformation(int id, int mode_distance, int mode_function, float parameter, std::shared_ptr<std::vector<cv::Vec4f>> points, int offset, int stride);

  //WORK::::::::::::::::::::::::::::::::::::::::::::::::
  float               get_segment_progress(int id);
  bool                get_result(cv::Mat& out);
  cv::Mat             get_result();
  cv::Mat const&      get_frame(int i);
  int                 get_img_type();
  int                 get_width();
  int                 get_height();
  int                 get_typ_i(int id);
  std::string         get_videopath();
  std::shared_ptr<cv::VideoCapture> get_videocap();
  cv::Point           get_max_Point();
  cv::Point           get_min_Point();
  int                 get_start_frame();
  int                 get_last_frame();
  int                 get_img_delta();
  float               get_intensity();
  int                 get_seg_start(int id);
  int                 get_seg_end(int id);
  void                set_work_size(int i);

  void                add_work(Segment* new_seg);
  void                add_to_values_abs(cv::Mat new_values);
  void                add_to_values_fac(cv::Mat new_values);
  void                add_to_uni_fac(float new_value);

  void                thread_calc_loop();
  bool                work_to_do();
  void                continue_work();


private:
  void                update_result();

  std::vector<std::shared_ptr<Segment>>         m_segments;
  std::vector<std::shared_ptr<Transformation>>  m_transformations;
  std::shared_ptr<cv::VideoCapture>             m_video;

  bool                    m_new_output = true;
  bool                    m_in_calculation;
  std::thread             m_worker;
  std::mutex              m_mutex_update;        //m_in_calculation
  std::mutex              m_mutex_result;        //read & write update img

  std::string             m_file;
  int                     m_img_type;
  int                     m_work_size;

  cv::Point               m_pnt_min;
  cv::Point               m_pnt_max;
  int                     m_frame_start;
  int                     m_frame_last;
  int                     m_img_delta;

  std::vector<Segment*>   m_seg_in_calc;        //stores currently edited segments
  cv::Mat                 m_values_abs;
  cv::Mat                 m_values_fac;
  double                  m_uni_fac;
  float                   m_intensity;           //overall intensity: default: 1.0f
  cv::Mat                 m_result;
};

#endif
