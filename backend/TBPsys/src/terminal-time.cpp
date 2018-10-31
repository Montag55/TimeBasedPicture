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

#include "base.hpp"
#include "average.hpp"
#include "interpretation.hpp"
#include "transferfunction.hpp"

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
      std::cout<<"no path specified: default=./../../../videos/street/frame%01d.jpg\n";
      file_path = "./../../../videos/street/frame%01d.jpg";
    }
    else {
      file_path = argv[1];
    }

    base = std::make_shared<Base>(file_path);
    bool exit_status = false;
    std::cout<<"expecting a segment definition: \n";

    while(!exit_status) {

        std::cout<<"expecting input\n";
        std::string command;
        std::vector<std::string> v;
        getline (std::cin, command);
        split( command, v, ' ' );

        if(v[0]      == "addsegment") {
          std::cout<<"addsegment\n";
          int startframe  = std::stoi (v[1]);
          int endframe    = std::stoi (v[2]);
          int local_i     = std::stof (v[3]);
          int global_i    = std::stof (v[4]);

          int id = base->add_segment(startframe, endframe,local_i, global_i);
          if(id < 0 ){
            std::cout << "add_segment() failed. \n";
          }
        }
        else if(v[0] == "addinterpretation") {
          std::cout << "addinterpretation \n";
          int typ_i         = std::stoi (v[1]);
          int interpret_id  = base->add_interpretation(typ_i);

          if(interpret_id >= 0 ) {
            std::cout << "\n\t > intpretation id: "<< interpret_id << "\n";
            std::cout << "\n\t > typ: ";
          }
          else {
            std::cout << "add_interpretation() failed. \n";
          }

        }
        else if(v[0] == "manipulate") {
          std::cout << "manipulate: ";

          if(v[1] == "segment"){
            int id          = std::stoi(v[2]);
            int start       = std::stoi(v[3]);
            int end         = std::stoi(v[4]);
            float local_i   = std::stof(v[5]);
            float global_i  = std::stof(v[6]);

            if(base->manipulate_segment(id, start, end, local_i, global_i)){
              std::cout<<"Segment id: " << id << "\n";
            }
            else{
              std::cout << "manipulate_segment() failed. \n";
            }
          }
          // else if(v[1] == "interpretation") {
          //   //all segments need to recalculate!
          //   int id = std::stoi(v[2]);
          //
          //   if(interpretations[id]->getTypenumber() == 0 /*average*/ ) {
          //       std::cout<<"The Average-interpretation needs no manipulation. \n";
          //   }
          //   else if(interpretations[id]->getTypenumber() == 1 /*transferfunction*/ ) {
          //     int start  = std::stoi (v[3]);
          //     int length = v.size();
          //     std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>();
          //
          //     for (int i=4; i<length; i++) {
          //       std::istringstream iss(v[i]);
          //       float val;
          //       iss>> noskipws >> val;
          //       if(iss.eof() && !iss.fail()){   //to compensate input errors with to much blanks
          //         std::cout<<"weight"<<val<<"\n";
          //         weights->push_back(val);
          //       }
          //     }
          //     //ist folgende zeile in ordnung?!:
          //     Transferfunction& x = dynamic_cast<Transferfunction&>(*interpretations[id]);
          //     x.set_weights(start, weights);
          //   }
          //   else {
          //     std::cout<<"not yet implemented manipulation\n";
          //   }
          // }
          else{
            std::cout << "manipulate either segment or interpretation. \n";
          }

        }
        else if(v[0] == "connect") {
          int id_segment        = std::stoi (v[1]);
          int id_interpretation = std::stoi (v[2]);

          if(base->connect(id_segment, id_interpretation)) {
            std::cout << "Segment id: " << id_segment << " -> interpretation id: "<< id_interpretation << "\n";
          }
          else {
            std::cout<<"connect() failed. (Wrong id's) \n";
          }
        }
        else if(v[0] == "work_size") {
             int i = std::stoi (v[1]);
             base->set_work_size(i);
        }
        else if(v[0] == "delete") {
             int id = std::stoi (v[1]);

             if(!base->delete_segment(id)){
               std::cout << "delete_segment with id: " << id << "\n";
             }
             else{
               std::cout << "delete_segment() failed. \n";
             }
        }
        else if(v[0] == "save") {
             if(base->save("out"+v[1]+".jpg")){
               std::cout << "Saved as ./build/out" << v[1] << ".jpg \n";
             }
             else{
               std::cout << "save() failed. \n";
             }
        }
    }

    std::cout << "done!" << endl;
    imwrite( "./final.jpg", base->get_result());
  }
  else {
    std::cout<<"Videopfad angeben?\n";
  }

  return 0;
}
