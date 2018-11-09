/*
test lab to generate fast output of different settings
*/

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>

#include "base.hpp"

#include <opencv2/opencv.hpp>



int main (int argc, char **argv){

#ifndef true //Common Variables:
  std::string file_path = "./../../../videos/street/frame%01d.jpg";
  bool exit_status = true;
  std::shared_ptr<Base> base ;
#endif

#ifndef true //Specific Variables:
  //nothing yet
#endif

#ifndef true  //Initialize:
  base= std::make_shared<Base>(file_path);
#endif

#ifndef true  //Computation:
  int   startframe  = 20;
  int   endframe    = 50;
  int   local_i     = 1;
  int   global_i    = 1;
  base->add_segment(startframe, endframe, local_i, global_i);

  int   typ_i       = 3;
  int   ref_id      = -1;
  float threshhold  = 10;
  base->add_interpretation(typ_i, ref_id, threshhold);

  int segment_id    = 0;
  int interpret_id  = 0;
  base->connect(segment_id, interpret_id);

  for (int i= 0; i<100; i++){
    std::cout<<"cycle: "<<i<<"\n";
    base->manipulate_interpretation(interpret_id, ref_id, threshhold*i);
    //work size off
    //base->compute_and_save("out"+std::to_string(i)+".jpg");
  }
#endif


  std::cout << "testlab completed!\n";
  return 0;
}
