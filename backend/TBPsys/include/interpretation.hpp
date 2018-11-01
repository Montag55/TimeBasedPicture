#ifndef INTERPRETATION_HPP
#define INTERPRETATION_HPP
#include <opencv2/opencv.hpp>
#include <mutex>
#include <memory>

using namespace cv;

class Base;
class Segment;

class Interpretation {
public:

  //KONSTRUTOREN----------------------------------------------------------------------
//  Interpretation(VideoCapture& vid, int img_type, int id);
  Interpretation(std::shared_ptr<Base> mother, int id);
  int get_id();
  virtual ~Interpretation();
  virtual int getTypenumber() =0;
  //FUNKTIONEN------------------------------------------------------------------------
  virtual void calc(int id, int start, int length, int sign, Mat& result, float& factor) =0;
  void add_connection(int id, Segment* segment );
  void delete_connection(int id);
  void update_connections();
protected:

  std::shared_ptr<VideoCapture>  m_video;
  int           m_img_type;
  int           m_id;
  bool          m_recompute=false;
  std::mutex    m_mutex_connections;
  std::shared_ptr<Base> m_base;
  std::map<int, Segment*> m_all_connections = std::map<int, Segment*>();
};

#endif
