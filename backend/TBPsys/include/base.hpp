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
#include "interpretation.hpp"

#include <opencv2/opencv.hpp>
#include "../../OpenGL/include/glapplication.hpp"


using namespace cv;
using namespace std;

class Base : public std::enable_shared_from_this<Base> {
public:
  Base(std::string const& video_name);
  ~Base();

  //ORGA::::::::::::::::::::::::::::::::::::::::::::::::
  int                 add_segment(int start, int end, float local_i, float global_i);
  bool                delete_segment(int id);
  bool                manipulate_segment(int id, int start, int end, float local_i, float global_i);
  float               get_segment_progress(int id);
  int                 add_interpretation(int typ_i);
  bool                connect(int id_segment, int id_interpretation);
  bool                save(std::string file);

  //WORK::::::::::::::::::::::::::::::::::::::::::::::::
  void                thread_calc_loop();
  void                thread_GLAPP_loop();
  bool                work_to_do();
  void                continue_work();
  void                set_work_size(int i);
  bool                get_result(Mat& out);
  Mat                 get_result();
  Mat const&          get_frame(int i);
  int                 get_img_type();
  int                 get_width();
  int                 get_height();
  void                add_to_values_abs(cv::Mat new_values);
  void                add_to_values_fac(cv::Mat new_values);
  void                add_to_uni_fac(float new_value);

  //PROT?:::::::::::::::::::::::::::::::::::::::::::::::
  void                add_work(Segment* new_seg);

  GLApplication*      getGLAPP();
  bool                use_gpu();
  std::string         get_videopath();
  std::shared_ptr<VideoCapture>      get_videocap();

private:

  void                update_result();

private:
  bool                m_in_calculation;
  bool                m_new_output=true;

  //origin:
  std::string         m_file;
  std::shared_ptr<VideoCapture>        m_video;
  int                 m_img_type;
  int                 m_work_size;

  //domain:
  Point               m_pnt_min;
  Point               m_pnt_max;
  int                 m_frame_start;
  int                 m_frame_last;

  //daat:
  std::vector<Segment*>   m_seg_in_calc;        //stores currently edited segments
  Mat                     m_values_abs;
  Mat                     m_values_fac;
  double                  m_uni_fac;
  float                   m_intensity;           //overall intensity: default: 1.0f
  Mat                     m_result;
  std::thread             m_worker;
  GLApplication*          m_GLapp;
  bool                    m_use_gpu=false;
  std::thread             m_thread_GLAPP;
  bool                    m_update_work;

   //simple mutex solution for the start:
  std::mutex              m_mutex_update;        //m_in_calculation
  std::mutex              m_mutex_result;        //read & write update img
  std::vector<std::shared_ptr<Segment>> m_segments;
  std::vector<std::shared_ptr<Interpretation>> m_interpretations;
};

#endif
