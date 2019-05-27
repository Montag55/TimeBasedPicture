/*
Author:     Lucas Hübner
Date:       27.05.2019
*/
/*
 * test lab to generate fast output of different settings
 * console: ffmpeg -framerate 30 -i ./video/boost_cyc_%01d.png -c:v libx264 -pix_fmt yuv420p video.avi
*/

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include <cctype>
#include <math.h>

#include "base.hpp"
#include "segment.hpp"
#include "transformation.hpp"
#include <opencv2/opencv.hpp>

int main (int argc, char **argv){

#ifndef true //Common Variables:
  std::string file_path = "/home/lucas/Videos/TBP-Footage/train/frame%01d.png";
  bool exit_status = true;
  std::shared_ptr<Base> base;
  int number_of_frames = 240;
#endif

#ifndef true //Specific Variables:
  //nothing yet
#endif

#ifndef true  //Initialize:
  base = std::make_shared<Base>(file_path);
  std::this_thread::sleep_for(std::chrono::milliseconds(500)); //safety reason
#endif

#ifndef true  //Computation:

  int   startframe    = 449;
  int   endframe      = 499;
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

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){

      base->manipulate_segment(segment_id, startframe + i, endframe + i, local_i, global_i, false);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(0));

        if(progress<100){
          // std::cout << "\t > Segment[" << segment_id << "] Progress: " << progress << "\n";
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //update rate
        }
        else{
          // std::cout << "\t > Segment[" << segment_id << "] Progress: " << progress << " -> save\n";
          base->save("./video/avg" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + Average ("<<number_of_frames<<") time: \t" << duration << std::endl;
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

    if( false  /*sigmoid points*/ ){
      for(int i = 0; i < 50; i++){
        points->push_back((float)i/50);
        points->push_back(10 / (1 + exp(-10 * ((float)i/50 - 0.5))));
      }
    }

    if( false  /*pow points*/ ){
      for(unsigned int i = 0; i < 30; i++){
        points->push_back((float)i/30);
        points->push_back(pow((float)i/30, 2) * 10);
        //std::cout << pow((float)i/30, 2) * 10 << std::endl;
      }
    }

    interpret_id = base->add_transformation(typ_i, offset, stride, start, -1, points);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){

      // base->manipulate_transformation(interpret_id, start, -1, (*points3)[i%2], offset, stride);
      // std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
      base->manipulate_segment(segment_id, startframe + i, endframe + i, local_i, global_i, false);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(0));

        if(progress<100){
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //update rate
        }
        else{
          base->save("./video/tff" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + TFF ("<<number_of_frames<<") time: \t" << duration << std::endl;
  }

  if( false /* Boost */ ){
    typ_i               = 3;
    float threshhold_b  = 10;
    int modi_b          = 0;
    int ref_id_b        = -1;
    std::string file = "/home/lucas/Documents/git/TimeBasedPicture/backend/TBPsys/build/ref.png";

    interpret_id    = base->add_transformation(typ_i, offset, stride, file, threshhold_b, modi_b);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){

      base->manipulate_transformation(interpret_id, file, threshhold_b + i%2, modi_b, offset, stride);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(segment_id));

        if(progress<100){
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //update rate
        }
        else{
          base->save("./video/avg" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + Boost ("<<number_of_frames<<") time: \t" << duration << std::endl;
  }

  if( false /*Reduce*/ ){
    typ_i               = 5;
    float threshhold_b  = 80;
    int modi_b          = 0;
    int ref_id_b        = -1;
    std::string file = "/home/lucas/Documents/git/TimeBasedPicture/backend/TBPsys/build/ref.png";

    interpret_id   = base->add_transformation(typ_i, offset, stride, file, threshhold_b, modi_b);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){

      base->manipulate_transformation(interpret_id, file, threshhold_b + i%2, modi_b, offset, stride);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      // base->manipulate_segment(segment_id, startframe + i, endframe + i, local_i, global_i_extreme, false);
      // std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(segment_id));

        if(progress<100){
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //update rate
        }
        else{
        base->save("./video/reduce" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + Reduce ("<<number_of_frames<<") time: \t" << duration << std::endl;
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

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){

      base->manipulate_transformation(interpret_id, threshhold_o + i%2, modi_o, colors, offset, stride);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      // base->manipulate_segment(segment_id, startframe + i, endframe + i, local_i, global_i_extreme, false);
      // std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(segment_id));

        if(progress<100){
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //update rate
        }
        else{
          base->save("./video/avg" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + BoostColor ("<<number_of_frames<<") time: \t" << duration << std::endl;
  }

  if( false /* Overplott */){
    typ_i               = 2;
    float threshhold_o  = 80;
    int modi_o          = 0;
    std::string file = "/home/lucas/Documents/git/TimeBasedPicture/backend/TBPsys/build/ref.png";

    interpret_id    = base->add_transformation(typ_i, offset, stride, file, threshhold_o, modi_o);
    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){

      // base->manipulate_transformation(interpret_id, file, threshhold_o + i%2, modi_o, offset, stride);
      // std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      base->manipulate_segment(segment_id, startframe + i, endframe + i, local_i, global_i_extreme, false);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(segment_id));

        if(progress<100){
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //update rate
        }
        else{
          base->save("./video/avg" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + Overplott ("<<number_of_frames<<") time: \t" << duration << std::endl;
  }

  if( true /* CircularFade */ ){
    typ_i = 6;
    int start_c             = startframe + (endframe - startframe)/2;
    int end_c               = startframe + (endframe - startframe)/2;
    int outer_circle_start  = startframe;
    int outer_circle_end    = endframe;
    int mode_c              = 1;
    float mid_x             = 0.5;
    float mid_y             = 0.5;
    float radius_c          = 10;
    bool fade_dir_c         = 1;
    float parameter_c       = 0.01;

    cv::Vec2f mid_c = cv::Vec2f(mid_x, mid_y);

    interpret_id  = base->add_transformation(typ_i, offset, stride, start_c, end_c, outer_circle_start, outer_circle_end, mode_c, mid_c, radius_c, fade_dir_c, parameter_c);
    // std::cout << "adding transformation [ID: " << interpret_id << "] \n" ;
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){
      std::cout << "cycle: " << i << "\n";

      float tmp_rad = 100 / 2 - abs(((int)radius_c + i*3) % 100 - 100 / 2);
      std::cout << "radius: " << tmp_rad << "\n";

      base->manipulate_transformation(interpret_id, start_c + i, end_c + i, outer_circle_start + i, outer_circle_end + i, mode_c, mid_c, tmp_rad, fade_dir_c, parameter_c, offset, stride);
      std::this_thread::sleep_for(std::chrono::milliseconds(200)); //safety reason

      base->manipulate_segment(segment_id, startframe + i, endframe + i, local_i, global_i, false);
      std::this_thread::sleep_for(std::chrono::milliseconds(200)); //safety reason

      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(0));

        if(progress < 100){
          std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //update rate
        }
        else{
          base->save("./video/circle" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + CircularFade ("<<number_of_frames<<") time: \t" << duration << std::endl;
  }

  if( false /* TimeFadePoints */ ){
    typ_i       = 7;
    int mode_distance = 1;
    int num_point = 0;
    float parameter  = 5;
    float start_seg = 53;
    float end_seg = 221;

    base->manipulate_segment(segment_id, start_seg, end_seg, local_i, global_i, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    std::shared_ptr<std::vector<cv::Vec4f>> points = std::make_shared<std::vector<cv::Vec4f>>();
    cv::Vec4f tmp0 {0.39993151857558634f, 0.33659260386546946f, end_seg, end_seg};
    cv::Vec4f tmp1 {0.46841294298921415f, 0.35363719373002583f, end_seg, end_seg};
    cv::Vec4f tmp2 {0.5697654511213833f, 0.3512022523208035f, end_seg, end_seg};
    cv::Vec4f tmp3 {0.39719226159904125f, 0.6726145183381524f, start_seg, start_seg};
    cv::Vec4f tmp4 {0.48347885636021226f, 0.6726145183381524f, start_seg, start_seg};
    cv::Vec4f tmp5 {0.5930491354220168f, 0.6628747527012631f, start_seg, start_seg};
    cv::Vec4f tmp6 {0.4437596302003081f, 0.7602724090701567f, start_seg, start_seg};
    cv::Vec4f tmp7 {0.5355247389145694f, 0.7724471161162684f, start_seg, start_seg};
    cv::Vec4f tmp8 {0.40267077555213143f, 0.8454953583929387f, start_seg, start_seg};
    cv::Vec4f tmp9 {0.49854476973121037f, 0.869844772485162f, start_seg, start_seg};
    cv::Vec4f tmp10 {0.5957883923985619f, 0.8649748896667173f, start_seg, start_seg};
    cv::Vec4f tmp11 {0.6382468755350111f, 0.7773169989347131f, start_seg, start_seg};
    cv::Vec4f tmp12 {0.4396507447354905f, 0.2489347131334652f, end_seg, end_seg};
    cv::Vec4f tmp13 {0.5218284540318439f, 0.2611094201795769f, end_seg, end_seg};
    cv::Vec4f tmp14 {0.6382468755350111f, 0.24162988890579817f, end_seg, end_seg};
    cv::Vec4f tmp15 {0.3862352336928608f, 0.13692740830923755f, end_seg, end_seg};
    cv::Vec4f tmp16 {0.4738914569423044f, 0.14423223253690456f, end_seg, end_seg};
    cv::Vec4f tmp17 {0.5752439650744735f, 0.15884188099223862f, end_seg, end_seg};
    points->push_back(tmp0);
    points->push_back(tmp1);
    points->push_back(tmp2);
    points->push_back(tmp3);
    points->push_back(tmp4);
    points->push_back(tmp5);
    points->push_back(tmp6);
    points->push_back(tmp7);
    points->push_back(tmp8);
    points->push_back(tmp9);
    points->push_back(tmp10);
    points->push_back(tmp11);
    points->push_back(tmp12);
    points->push_back(tmp13);
    points->push_back(tmp14);
    points->push_back(tmp15);
    points->push_back(tmp16);
    points->push_back(tmp17);

    interpret_id = base->add_transformation(typ_i, offset, stride, mode_distance, num_point, parameter, points);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    base->connect(segment_id, interpret_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_frames; i++){
      std::cout << "cycle: " << i << std::endl;

      for(unsigned int j = 0; j < points->size(); j++){
        std::cout << "segment " << start_seg + i << " " << end_seg + i <<std::endl;
        (*points)[j][2] += 1;
        (*points)[j][3] += 1;
        std::cout << "points " << (*points)[j][0] << " " << (*points)[j][1] << " " << (*points)[j][2] << " " << (*points)[j][3] << std::endl;
      }

      base->manipulate_segment(segment_id, start_seg + i, end_seg + i, local_i, global_i, false);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason

      base->manipulate_transformation(interpret_id,  mode_distance, num_point, parameter, points, offset, stride);
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); //safety reason


      bool wait = true;
      float progress = 0;

      while(wait) {

        progress = (base->get_segment_progress(0));

        if(progress<100){
          std::cout << "progress: " << progress << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //update rate
        }
        else{
          std::cout << "progress: save" << std::endl;
          base->save("./video/tfp" + std::to_string(i) + ".png");
          std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
          wait = false;
        }
      }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
    std::cout << "\t + TFP ("<<number_of_frames<<") time: \t" << duration << std::endl;
  }
#endif

  std::cout << "testlab completed!\n";
  return 0;
}
