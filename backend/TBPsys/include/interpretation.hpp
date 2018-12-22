#ifndef INTERPRETATION_HPP
#define INTERPRETATION_HPP

#include <opencv2/opencv.hpp>
#include <mutex>
#include <memory>


class Base;
class Segment;

class Interpretation {
public:

  //KONSTRUTOREN----------------------------------------------------------------------
  Interpretation(std::shared_ptr<Base> mother, int id, int offset, int stride);
  int get_id();
  int get_offset();
  int get_stride();
  virtual ~Interpretation();
  //FUNKTIONEN------------------------------------------------------------------------
  virtual int getTypenumber() = 0 ;
  int get_upload_specification();
  virtual int get_calculation_specification() =0 ; //0: standard sum play, 1:forward sum play, 2:full recompute
  virtual void calc( int id, int start, int length, int sign, cv::Mat& result, float& factor, cv::Mat& fac_mat) = 0 ;

  virtual void add_connection(int id, Segment* segment );
  void delete_connection(int id);
  void update_connections();
  void trigger_connections();

protected:
  int                                           m_offset;
  int                                           m_stride;
  std::shared_ptr< cv::VideoCapture >           m_video;
  int                                           m_img_type;
  int                                           m_id;
  int                                           m_type;
  int                                           m_upload_specification;
  int                                           m_calc_specification;
  bool                                          m_recompute = false;
  std::mutex                                    m_mutex_connections;
  std::shared_ptr< Base >                       m_base;
  std::map< int, Segment* > m_all_connections= std::map< int, Segment* >();
};

#endif
