/*
test lab to generate fast output of different settings
*/

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include <cctype>

#include "base.hpp"
#include "segment.hpp"
#include "transformation.hpp"

#include <opencv2/opencv.hpp>

int main (int argc, char **argv){

#ifndef true //Common Variables:
  std::string file_path = "/home/hagan/Videos/TBPFootage/bus/frame%01d.png";
  bool exit_status = true;
  std::shared_ptr<Base> base ;
#endif

#ifndef true //Specific Variables:
  //nothing yet
#endif

#ifndef true  //Initialize:
  base= std::make_shared<Base>(file_path);
  std::this_thread::sleep_for(std::chrono::milliseconds(500)); //safety reason
#endif

  int   startframe    = 100;
  int   endframe      = 129;
  int   local_i       = 1;
  int   global_i      = 1;
  int global_i_extreme = 1000;
  int   typ_i         = -1;
  int   offset        = 0;
  int   stride        = 0;
  int   interpret_id  = -1;
  int   segment_id    = base->add_segment(startframe, endframe, local_i, global_i);
  std::cout << "\nadding segment [ID: " << segment_id << "]\n" ;

  if( false /* Average */ ){
    typ_i             = 0;
    int   ref_id      = -1;
    float threshhold  = 0;

    interpret_id  = base->add_transformation(typ_i, offset, stride);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
    std::cout<<"connect:\n";
    base->connect(segment_id, interpret_id);
  }

  if( false /* TransferFunction */ ){
    typ_i       = 1;
    int start   = startframe;
    std::shared_ptr<std::vector<float>> points = std::make_shared<std::vector<float>>();
    if( true /*linear points*/ ){
      points->push_back(0);
      points->push_back(0);
      points->push_back(1);
      points->push_back(10);
    }

    interpret_id = base->add_transformation(typ_i, offset, stride, start, -1, points);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  }
  if( false /* Boost */ ){
    typ_i               = 3;
    float threshhold_b  = 10;
    int modi_b          = 0;
    int ref_id_b        = -1;
    std::string file = "./fin_state.png";

    interpret_id    = base->add_transformation(typ_i, offset, stride, file, threshhold_b, modi_b);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  }
  if(false /*Reduce*/ ){
    typ_i               = 5;
    float threshhold_b  = 80;
    int modi_b          = 0;
    int ref_id_b        = -1;
    std::string file = "./fin_state.png";

    interpret_id   = base->add_transformation(typ_i, offset, stride, file, threshhold_b, modi_b);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  }
  if( false /* BoostColor */ ){
    typ_i               = 4;
    int ref_id_o        = -1;
    float threshhold_o  = 80;
    int modi_o          = 0;
    std::shared_ptr<std::vector<float>> colors = std::make_shared<std::vector<float>>();
    colors->push_back(1);
    colors->push_back(1);
    colors->push_back(1);

    interpret_id    = base->add_transformation(typ_i, offset, stride, threshhold_o, modi_o, colors);
    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  }
  if( false/* Overplott */){
    typ_i               = 2;
    float threshhold_o  = 80;
    int modi_o          = 0;
    std::string file = "./fin_state.png";
    interpret_id    = base->add_transformation(typ_i, offset, stride, file, threshhold_o, modi_o);
    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  }

  if( false/* CircularFade */ ){
    typ_i = 6;
    int start_c             = startframe;// + (endframe - startframe)/2;
    int end_c               = startframe; + (endframe - startframe)/2;
    int outer_circle_start  = startframe;
    int outer_circle_end    = startframe;
    int mode_c              = 1;
    float mid_x             = 0.5;
    float mid_y             = 0.5;
    float radius_c          = 10;
    bool fade_dir_c         = 1;
    float parameter_c       = 1.0;

    cv::Vec2f mid_c = cv::Vec2f(mid_x, mid_y);

    interpret_id  = base->add_transformation(typ_i, offset, stride, start_c, end_c, outer_circle_start, outer_circle_end, mode_c, mid_c, radius_c, fade_dir_c, parameter_c);
    // std::cout << "adding transformation [ID: " << interpret_id << "] \n" ;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
  }

  if( true /* TimeFadePoints */ ){
    typ_i       = 7;
    int mode_distance = 1;
    int num_point = 5;
    float parameter  = 5;
    float start_seg = startframe;
    float end_seg = startframe;

    //base->manipulate_segment(segment_id, start_seg, end_seg, local_i, global_i, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    std::shared_ptr<std::vector<cv::Vec4f>> points = std::make_shared<std::vector<cv::Vec4f>>();
    cv::Vec4f tmp0 {0.39993151857558634f, 0.33659260386546946f, start_seg, start_seg};
    cv::Vec4f tmp1 {0.46841294298921415f, 0.35363719373002583f, start_seg, start_seg};
    cv::Vec4f tmp2 {0.5697654511213833f, 0.3512022523208035f, start_seg, start_seg};
    cv::Vec4f tmp3 {0.39719226159904125f, 0.6726145183381524f, start_seg, start_seg};
    cv::Vec4f tmp4 {0.48347885636021226f, 0.6726145183381524f, start_seg, start_seg};
    points->push_back(tmp0);
    points->push_back(tmp1);
    points->push_back(tmp2);
    points->push_back(tmp3);
    points->push_back(tmp4);

    interpret_id = base->add_transformation(typ_i, offset, stride, mode_distance, num_point, parameter, points);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  }
  while(1){

  }

  std::cout << "testlab completed!\n";
  return 0;
}
