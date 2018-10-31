#ifndef SEGMENT_HPP
#define SEGMENT_HPP

/*
a SEGMENT is a piece in space and time. it is a kind of volume. it always belongs
to a base/mother. we can interpret such a segment in different ways. maybe we seperate
segment and interpretation later...
*/
#include <opencv2/opencv.hpp>
#include <string.h>
#include <mutex>

#include "../../OpenGL/include/glapplication.hpp"

using namespace cv;

class Base;
#include "base.hpp"
#include "interpretation.hpp"
#include "average.hpp"

class Segment {
public:
  Segment(std::string file, int img_type, Point min, Point max, int start_frame, int last_frame, double intensity_local, double intensity_gloabl, std::shared_ptr<Base> mother, Mat& mother_val, Mat& mother_fac, double& mother_uni, int id);
  ~Segment();

  bool work(int& work_size);
  bool interpret_sized(int& work_size);
  void update_intensity();
  /*needs to be reimplemented with a owned segment:
  void boost_diff();
  void boost_diff_smooth(int power);
  void boost_similarities(int power);//modal();
  */

  //EDIT THE SEGMENT:
  void move_in_time(int delta);
  void set_exposure(int new_exposure);
  void set_startframe(int imgs);
  void set_endframe(int imgs);
  void set_exposure_delta(int delta);
  void set_local_intensity(float i);
  void set_global_intensity(float i);
  void manipulate(int start, int end, float local_i, float global_i);
  void set_interpretation(std::shared_ptr<Interpretation> interpret);
  void update_interpretation();
  void delete_seg();
  void reset();
  int getX();
  int getY();
  float get_progress();

private:
  void ready_to_work();
  int           m_id;
  //int           m_interpretation_mode;
  //VideoCapture  m_video;
  int           m_img_type;
  bool          m_work_done=true;
  float         m_percent;

  //Definition of BBOX:
  Point         m_pnt_min;
  Point         m_pnt_max;
  int           m_frame_start_destin;
  int           m_frame_last_destin;
  int           m_frame_start_actual;
  int           m_frame_last_actual;

  float         m_intensity_local_destin;
  float         m_intensity_global_destin;
  float         m_intensity_local_actual;
  float         m_intensity_global_actual;

  //
  std::shared_ptr<Interpretation> m_new_interpretation=NULL;
  std::shared_ptr<Interpretation> m_interpretation;

  //local values
  Mat           m_values_abs;
  Mat           m_values_fac;
  float        m_uni_fac;

  //global values
  std::shared_ptr<Base>         m_mother;

  //mutex_for setter
  std::mutex    m_mutex_soll;
  std::mutex    m_mutex_state;
  bool m_needs_reset  =false;

};
#endif
