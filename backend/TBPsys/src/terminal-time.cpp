/*
Author:     Phil Jungschlaeger, Lucas Hübner
Date:       27.05.2019
*/
/*
a simple terminal interface:
-input video via parameter
##
addsegment       [int]startframe [int] exposure [float] local_i [float] global_i //makes a new segment

% delete segment [int]id
startframe      [int]id   [int]frame
%endframe       [int]id   [int]frame
move            [int]id   [int]delta
exposure        [int]id   [int]imgs
exposure_delta  [int]id   [int]delta
##
-always copmpute full segment
-erstmal nur avg
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

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch){
  size_t pos = txt.find( ch );
  size_t initialPos = 0;
  strs.clear();

  // Decompose statement
  while( pos != std::string::npos ) {
      strs.push_back( txt.substr( initialPos, pos - initialPos ) );
      initialPos = pos + 1;
      pos = txt.find( ch, initialPos );
  }

  strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos +1 ) );
  return strs.size();
}

int main (int argc, char **argv){

  std::shared_ptr<Base> base;

  if((argc == 2)||((argc == 1))) {

    std::string file_path;

    if(argc == 1) {
      std::cout<<"no path specified: default=./../../../videos/street/frame%01d.png\n";
      file_path = "./../../../videos/street/frame%01d.png";
    }
    else {
      file_path = argv[1];
    }

    base = std::make_shared<Base>(file_path);
    bool exit_status = false;
    std::cout<<"\nExpecting a segment definition: \n";

    while(!exit_status) {

        std::cout<<" > ";
        std::string command;
        std::vector<std::string> v;
        getline (std::cin, command);
        split( command, v, ' ' );

        if(v[0]      == "addsegment") {
          int startframe  = std::stoi (v[1]);
          int endframe    = std::stoi (v[2]);
          int local_i     = std::stof (v[3]);
          int global_i    = std::stof (v[4]);

          int segment_id = base->add_segment(startframe, endframe, local_i, global_i);
          if(segment_id >= 0 ){
            std::cout << "\t > segment id: "<< segment_id << "\n";
            std::cout << "\t > start frame: " << startframe << "\n";
            std::cout << "\t > end frame: " << endframe << "\n";
            std::cout << "\t > local intensity: " << local_i << "\n";
            std::cout << "\t > global intensity: " << global_i << "\n\n";
          }
          else {
            std::cout << "add_segment() failed. \n";
          }
        }
        else if(v[0] == "addtransformation") {
          int interpret_id  = -1;
          int typ_i         = std::stoi (v[1]);
          int offset        = std::stoi (v[2]);
          int stride        = std::stoi (v[3]);

          if(typ_i == 0 /*Average*/){
            interpret_id  = base->add_transformation(typ_i, offset, stride);
          }
          else if(typ_i == 1 /*Transferfunction*/){
            std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>();
            int modi = -1;
            float start = std::stoi (v[4]);


            for(int idx = 5; idx < v.size(); idx++){
              weights->push_back(std::stof(v[idx]));
            }
            std::cout << "\t > start_frame: " << start << "\n";
            std::cout << "\t > weights: [";
            for(unsigned int i = 0; i < weights->size(); i++){
              std::cout << (*weights)[i] << ", ";
            }
            std::cout << "]\n";
            interpret_id  = base->add_transformation(typ_i, offset, stride, start, modi, weights);
          }
          else if(typ_i == 2 /*Overplott*/){
            if(std::any_of(v[4].begin(), v[4].begin() + 2, ::isdigit)){
              int ref_id        = std::stoi (v[4]);
              float threshhold  = std::stof (v[5]);
              int modi          = std::stoi (v[6]);
              interpret_id  = base->add_transformation(typ_i, offset, stride, ref_id, threshhold, modi);
            }
            else{
              std::string ref_file_path = v[4];
              float threshhold = std::stof(v[5]);
              int modi          = std::stoi (v[6]);
              interpret_id = base->add_transformation(typ_i, offset, stride, ref_file_path, threshhold, modi);
            }
          }
          else if(typ_i == 3 /*Boost*/){
            if(std::any_of(v[4].begin(), v[4].begin() + 2, ::isdigit)){
              int ref_id        = std::stoi (v[4]);
              float threshhold  = std::stof (v[5]);
              int modi          = std::stoi (v[6]);
              interpret_id  = base->add_transformation(typ_i, offset, stride, ref_id, threshhold, modi);
            }
            else{
              std::string ref_file_path = v[4];
              float threshhold = std::stof(v[5]);
              int modi         = std::stoi (v[6]);
              interpret_id = base->add_transformation(typ_i, offset, stride, ref_file_path, threshhold, modi);
            }
          }
          else if(typ_i == 4 /*BoostColor*/){
            std::shared_ptr<std::vector<float>> colors = std::make_shared<std::vector<float>>();
            float threshhold  = std::stoi (v[4]);
            int modi          = std::stoi (v[5]);

            for(int idx = 6; idx < v.size(); idx++){
              colors->push_back(std::stof(v[idx]));
            }

            if(colors->size()%3 == 0){
              interpret_id = base->add_transformation(typ_i, offset, stride, threshhold, modi, colors);
              std::cout << "\t > threshhold: " << threshhold << "\n";
              std::cout << "\t > colors: ";
              for(unsigned int i = 0; i < colors->size(); i+=3){
                std::cout << "[" << (*colors)[i] << ", " << (*colors)[i+1] << ", " << (*colors)[i+2] << "], ";
              }
              std::cout << "\n";
            }
            else
              std::cout << "\t too few arguments (tpye, float, [r, g, b]*n) \n";
          }
          else if(typ_i == 5 /*Reduce*/){

            if(std::any_of(v[4].begin(), v[4].begin() + 2, ::isdigit)){
              int ref_id        = std::stoi (v[4]);
              float threshhold  = std::stof (v[5]);
              int modi          = std::stoi (v[6]);
              interpret_id  = base->add_transformation(typ_i, offset, stride, ref_id, threshhold, modi);
            }
            else{
              std::string ref_file_path = v[4];
              float threshhold = std::stof(v[5]);
              int modi          = std::stoi (v[6]);
              interpret_id = base->add_transformation(typ_i, offset, stride, ref_file_path, threshhold, modi);
            }
          }
          else if(typ_i == 6 /*CircularFade*/){
            int start       = std::stoi (v[4]);
            int end         = std::stoi (v[5]);
            int outer_circle_start = std::stoi (v[6]);
            int outer_circle_end = std::stoi (v[7]);
            int mode        = std::stoi (v[8]);
            float mid_x     = std::stof (v[9]);
            float mid_y     = std::stof (v[10]);
            float radius    = std::stof (v[11]);
            bool fade_dir   = std::stoi (v[12]);
            float parameter   = std::stof (v[13]);

            cv::Vec2f mid = cv::Vec2f(mid_x, mid_y);

            std::cout << "\t > start: " << start << "\n";
            std::cout << "\t > end: " << end << "\n";
            std::cout << "\t > outer_circle_start: " << outer_circle_start << "\n";
            std::cout << "\t > outer_circle_end: " << outer_circle_end << "\n";
            std::cout << "\t > func_mode: " << mode << "\n";
            std::cout << "\t > middle: " << mid_x << ", " << mid_y << "\n";
            std::cout << "\t > radius: " << radius << "\n";
            std::cout << "\t > fade direction: " << fade_dir << "\n";
            std::cout << "\t > parameter: " << parameter << "\n";

            interpret_id  = base->add_transformation(typ_i, offset, stride, start, end, outer_circle_start, outer_circle_end, mode, mid, radius, fade_dir, parameter);
          }
          else if(typ_i == 7 /*Timefadepoints*/){
            std::shared_ptr<std::vector<cv::Vec4f>> points = std::make_shared<std::vector<cv::Vec4f>>();
            int mode_distance = std::stoi (v[4]);
            int mode_function = std::stoi (v[5]);
            float parameter  = std::stof (v[6]);

            for(int idx = 7; idx < v.size(); idx += 4){
              cv::Vec4f tmp {std::stof(v[idx]), std::stof(v[idx+1]),
                             std::stof(v[idx+2]), std::stof(v[idx+3])};
              points->push_back(tmp);
            }

            std::cout << "\t > mode_distance: " << mode_distance << "\n";
            std::cout << "\t > mode_function: " << mode_function << "\n";
            std::cout << "\t > parameter: " << parameter << "\n";
            std::cout << "\t > points: [";
            for(unsigned int i = 0; i < points->size(); i++){
              std::cout << (*points)[i] << ", ";
            }
            std::cout << "]\n";
            interpret_id  = base->add_transformation(typ_i, offset, stride, mode_distance, mode_function, parameter, points);
          }
          else if(typ_i == 8 /*Singleimage*/){
            std::string file_path = v[4];
            interpret_id = base->add_transformation(typ_i, file_path);
          }
          else if(typ_i == 9 /*Paint*/){
            std::shared_ptr<std::vector<ColorCoords>> colorTimes = std::make_shared<std::vector<ColorCoords>>();

            for(unsigned int idx = 4; idx < v.size(); idx += 5){
              ColorCoords tmp;
              tmp.color = cv::Vec3f(std::stof(v[idx]), std::stof(v[idx+1]), std::stof(v[idx+2]));
              tmp.start = std::stoi(v[idx+3]);
              tmp.end = std::stoi(v[idx+4]);
              colorTimes->push_back(tmp);
            }

            interpret_id = base->add_transformation(typ_i, offset, stride, colorTimes);
            std::cout << "\t > ColorTimes: [";
            for(unsigned int i = 0; i < colorTimes->size(); i++){
              std::cout << (*colorTimes)[i].color << ", " << (*colorTimes)[i].start << ", " << (*colorTimes)[i].end << "; ";
            }
            std::cout << "]\n";
          }
          if(interpret_id >= 0 ) {
            std::cout << "\t > offset: " << offset << "\n";
            std::cout << "\t > stride: " << stride << "\n";
            std::cout << "\t > intpretation id: "<< interpret_id << "\n";
            std::cout << "\t > typ: " << typ_i << "\n\n";
          }
          else {
            std::cout << "\t add_transformation() failed. \n\n";
          }

        }
        else if(v[0] == "manipulate") {
          if(v[1] == "segment"){
            int id          = std::stoi(v[2]);
            int start       = std::stoi(v[3]);
            int end         = std::stoi(v[4]);
            float local_i   = std::stof(v[5]);
            float global_i  = std::stof(v[6]);
            bool hasMask    = bool(std::stoi(v[7]));

            if(base->manipulate_segment(id, start, end, local_i, global_i, hasMask)){
              std::cout<<"\t Segment id: " << id << "\n";
            }
            else{
              std::cout << "\t manipulate_segment() failed. \n";
            }
          }
          else if(v[1] == "transformation") {
            //all segments need to recalculate!
            int id = std::stoi(v[2]);
            int offset = std::stoi(v[3]);
            int stride = std::stoi(v[4]);

            int typ_i = base->get_typ_i(id);
            if(typ_i == 0 /*average*/ ) {
              if(base->manipulate_transformation(id, offset, stride)){
                std::cout << "\t > you fool. The Average-transformation needs no manipulation. \n";
                std::cout << "\t > ... maybe it actually does ... \n";
                std::cout << "\t > transformation id: " << id << "\n";
                std::cout << "\t > typ: " << typ_i << "\n";
                std::cout << "\t > offset: " << offset << "\n";
                std::cout << "\t > stride: " << stride << "\n";
              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }
            }
            else if(typ_i == 1 /*Transferfrunction*/ ) {
              float start  = std::stoi (v[5]);
              std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>();
              int modi = -1;

              for(int idx = 6; idx < v.size(); idx++){
                weights->push_back(std::stof(v[idx]));
              }

              if(base->manipulate_transformation(id, start, modi, weights, offset, stride)){
                std::cout << "\t > transformation id: " << id << "\n";
                std::cout << "\t > typ: " << typ_i << "\n";
                std::cout << "\t > start_frame: " << start << "\n";
                std::cout << "\t > offset: " << offset << "\n";
                std::cout << "\t > stride: " << stride << "\n";
                std::cout << "\t > weights: [";
                for(unsigned int i = 0; i < weights->size(); i++){
                  std::cout << (*weights)[i]  << "," ;
                }
                std::cout << "]\n\n";
              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }
            }
            else if(typ_i == 2 || typ_i == 3 || typ_i == 5 /*Overplott, Boost, Reduce*/){

              if(std::any_of(v[5].begin(), v[5].begin() + 2, ::isdigit)){
                int ref_id        = std::stoi (v[5]);
                float threshhold  = std::stof (v[6]);
                int modi          = std::stoi (v[7]);

                if(base->manipulate_transformation(id, ref_id, threshhold, modi, offset, stride)){
                  std::cout << "\t > transformation id: " << id << "\n";
                  std::cout << "\t > typ: " << typ_i << "\n";
                  std::cout << "\t > reference id: " << ref_id << "\n";
                  std::cout << "\t > threshhold: " << threshhold << "\n";
                  std::cout << "\t > modi: " << modi << "\n";
                  std::cout << "\t > offset: " << offset << "\n";
                  std::cout << "\t > stride: " << stride << "\n";
                }
                else{
                  std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
                }
              }
              else{
                std::string ref_file_path = v[5];
                float threshhold = std::stof(v[6]);
                int modi         = std::stoi (v[7]);

                if(base->manipulate_transformation(id, ref_file_path, threshhold, modi, offset, stride)){
                  std::cout << "\t > transformation id: " << id << "\n";
                  std::cout << "\t > typ: " << typ_i << "\n";
                  std::cout << "\t > reference path: " << ref_file_path << "\n";
                  std::cout << "\t > threshhold: " << threshhold << "\n";
                  std::cout << "\t > modi: " << modi << "\n";
                  std::cout << "\t > offset: " << offset << "\n";
                  std::cout << "\t > stride: " << stride << "\n";
                }
                else{
                  std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
                }
              }
            }
            else if(typ_i == 4 /*BoostColor*/){
              std::shared_ptr<std::vector<float>> colors = std::make_shared<std::vector<float>>();
              float threshhold = std::stof(v[5]);
              int modi         = std::stoi(v[6]);

              for(int idx = 7; idx < v.size(); idx++){
                colors->push_back(std::stof(v[idx]));
              }

              if(colors->size()%3 != 0)
                std::cout << "\t too few arguments (tpye, float, [r, g, b]*n) \n";

              if(base->manipulate_transformation(id, threshhold, modi, colors, offset, stride)){
                std::cout << "\t > transformation id: " << id << "\n";
                std::cout << "\t > typ: " << typ_i << "\n";
                std::cout << "\t > threshhold: " << threshhold << "\n";
                std::cout << "\t > modi: " << modi << "\n";
                std::cout << "\t > offset: " << offset << "\n";
                std::cout << "\t > stride: " << stride << "\n";
                std::cout << "\t > colors: ";
                for(unsigned int i = 0; i < colors->size(); i+=3){
                  std::cout << "[" << (*colors)[i] << ", " << (*colors)[i+1] << ", " << (*colors)[i+2] << "], ";
                }
                std::cout << "\n\n";
              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }

            }
            else if(typ_i == 6 /*CircularFade*/){
              int start       = std::stoi (v[5]);
              int end         = std::stoi (v[6]);
              int outer_circle_start = std::stoi (v[7]);
              int outer_circle_end = std::stoi (v[8]);
              int mode        = std::stoi (v[9]);
              float mid_x     = std::stof (v[10]);
              float mid_y     = std::stof (v[11]);
              float radius    = std::stof (v[12]);
              bool fade_dir   = std::stoi (v[13]);
              float parameter = std::stof (v[14]);

              cv::Vec2f mid = cv::Vec2f(mid_x, mid_y);

              if(base->manipulate_transformation(id, start, end, outer_circle_start, outer_circle_end, mode, mid, radius, fade_dir, parameter ,offset, stride)){
                std::cout << "\t > transformation id: " << id << "\n";
                std::cout << "\t > typ: " << typ_i << "\n";
                std::cout << "\t > start: " << start << "\n";
                std::cout << "\t > end: " << end << "\n";
                std::cout << "\t > outer_circle_start: " << outer_circle_start << "\n";
                std::cout << "\t > outer_circle_end: " << outer_circle_end << "\n";
                std::cout << "\t > func_mode: " << mode << "\n";
                std::cout << "\t > middle: " << mid_x << ", " << mid_y << "\n";
                std::cout << "\t > radius: " << radius << "\n";
                std::cout << "\t > fade direction: " << fade_dir << "\n";
                std::cout << "\t > parameter: " << parameter << "\n";
                std::cout << "\t > offset: " << offset << "\n";
                std::cout << "\t > stride: " << stride << "\n\n";
              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }

            }
            else if(typ_i == 7 /*TimeFadePoints*/){
              std::shared_ptr<std::vector<cv::Vec4f>> points = std::make_shared<std::vector<cv::Vec4f>>();
              int mode_distance = std::stoi (v[5]);
              int mode_function = std::stoi (v[6]);
              float parameter  = std::stof (v[7]);

              for(int idx = 8; idx < v.size(); idx += 4){
                cv::Vec4f tmp {std::stof(v[idx]), std::stof(v[idx+1]),
                               std::stof(v[idx+2]), std::stof(v[idx+3])};
                points->push_back(tmp);
              }
              if(base->manipulate_transformation(id, mode_distance, mode_function, parameter, points, offset, stride)){
                std::cout << "\t > mode_distance: " << mode_distance << "\n";
                std::cout << "\t > mode_function: " << mode_function << "\n";
                std::cout << "\t > parameter: " << parameter << "\n";
                std::cout << "\t > points: [";
                for(unsigned int i = 0; i < points->size(); i++){
                  std::cout << (*points)[i] << ", ";
                }
                std::cout << "]\n";
              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }
            }
            else if(typ_i == 8 /*Singleimage*/){
              std::string file_path = v[5];
              if(base->manipulate_transformation(id, file_path)){
                std::cout << "\t > transformation id: " << id << "\n";
                std::cout << "\t > typ: " << typ_i << "\n";
                std::cout << "\t > file path: " << file_path << "\n";

              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }
            }
            else if(typ_i == 9 /*Paint*/){
              std::shared_ptr<std::vector<ColorCoords>> colorTimes = std::make_shared<std::vector<ColorCoords>>();

              for(unsigned int idx = 5; idx < v.size(); idx += 5){
                ColorCoords tmp;
                tmp.color = cv::Vec3f(std::stof(v[idx]), std::stof(v[idx+1]), std::stof(v[idx+2]));
                tmp.start = std::stoi(v[idx+3]);
                tmp.end = std::stoi(v[idx+4]);
                colorTimes->push_back(tmp);
              }

              if(base->manipulate_transformation(id, colorTimes, offset, stride)){
                std::cout << "\t > transformation id: " << id << "\n";
                std::cout << "\t > typ: " << typ_i << "\n";
                std::cout << "\t > ColorTimes: [";
                for(unsigned int i = 0; i < colorTimes->size(); i++){
                  std::cout << (*colorTimes)[i].color << ", " << (*colorTimes)[i].start << ", " << (*colorTimes)[i].end << ", ";
                }
                std::cout << "]\n";              }
              else{
                std::cout << "\t > manipulate transformation id: " << id << " failed. \n";
              }

            }
            else {
              std::cout<<"not yet implemented manipulation\n";
            }
          }
          else{
            std::cout << "manipulate either segment or transformation. \n";
          }
        }
        else if(v[0] == "connect") {
          int id_segment        = std::stoi (v[1]);
          int id_transformation = std::stoi (v[2]);

          if(base->connect(id_segment, id_transformation)) {
            std::cout << "\t > Segment id: " << id_segment << " now connected with transformation id: "<< id_transformation << "\n\n";
          }
          else {
            std::cout<<" \t connect() failed. (Wrong id's) \n";
          }
        }
        else if(v[0] == "work_size") {
             int i = std::stoi (v[1]);
             base->set_work_size(i);
        }
        else if(v[0] == "delete") {
             int id = std::stoi (v[1]);

             if(!base->delete_segment(id)){
               std::cout << "\t > delete_segment with id: " << id << "\n";
             }
             else{
               std::cout << "\t delete_segment() failed. \n";
             }
        }
        else if(v[0] == "save") {
             if(base->save("out"+v[1]+".png")){
               std::cout << "Saved as ./build/out" << v[1] << ".png \n";
             }
             else{
               std::cout << "save() failed. \n";
             }
        }
    }

    std::cout << "done!" << std::endl;
    imwrite( "./final.png", base->get_result());
  }
  else {
    std::cout<<"Videopfad angeben?\n";
  }

  return 0;
}
