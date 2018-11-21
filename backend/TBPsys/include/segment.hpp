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

class Base;
#include "base.hpp"
#include "interpretation.hpp"
#include "average.hpp"
#include "boost.hpp"
#include "reduce.hpp"
#include "boostColor.hpp"
#include "transferfunction.hpp"
#include "overplott.hpp"
#include "circularfade.hpp"

class Segment {
public:
  Segment(int start_frame, int last_frame, double intensity_local, double intensity_gloabl, std::shared_ptr<Base> mother, int id);
  ~Segment();

  bool work(int& work_size);
  bool interpret_free(int& work_size);
  bool interpret_extending(int& work_size);
  bool interpret_one_way( int & work_size);

  void update_intensity();
  /*needs to be reimplemented with a owned segment:
  void boost_diff();
  void boost_diff_smooth(int power);
  void boost_similarities(int power);//modal();
  */
  void revert_influence();
  void upload_influence();

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

  float get_progress();
  int get_start();
  int get_end();

private:
  void ready_to_work();
  void save_segment_out();
  void normalize_factor(cv::Mat& influence, cv::Mat& factors);

  int           m_id;
  float         m_percent;
  bool          m_work_done;

  //Definition of BBOX:
  int           m_frame_start_destin;
  int           m_frame_last_destin;
  int           m_frame_start_actual;
  int           m_frame_last_actual;

  float         m_intensity_local_destin;
  float         m_intensity_global_destin;
  float         m_intensity_local_actual;
  float         m_intensity_global_actual;

  std::shared_ptr<Interpretation> m_interpretation;
  std::shared_ptr<Interpretation> m_new_interpretation;

  //local values
  cv::Mat       m_values_abs;
  cv::Mat       m_values_fac;
  float         m_uni_fac;

  //global values
  std::shared_ptr<Base>         m_mother;

  //mutex_for setter
  std::mutex    m_mutex_soll;
  std::mutex    m_mutex_state;
  bool m_needs_reset;

};
#endif
