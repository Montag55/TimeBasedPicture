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
#include "interpretation.hpp"

#include <opencv2/opencv.hpp>

int main (int argc, char **argv){

#ifndef true //Common Variables:
  std::string file_path = "/home/lucas/Videos/TBP-Footage/forest/frame%01d.png";
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

#ifndef true  //Computation:
  int   startframe  = 9000;
  int   endframe    = 9020;
  int   local_i     = 1;
  int   global_i    = 1;
  base->add_segment(startframe, endframe, local_i, global_i);

  int   typ_i       = 3;
  int   ref_id      = -1;
  int   offset      = 0;
  int   stride      = 0;
  float threshhold  = 0;



  std::cout<<"here\n";
//  base->add_interpretation(typ_i, offset, stride, ref_id, threshhold);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  std::cout<<"here\n";

  int segment_id    = 0;
  int interpret_id  = 0;
  base->connect(segment_id, interpret_id);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000)); //safety reason
  std::cout<<"here\n";

  for (int i= 0; i<5; i++){
    std::cout<<"cycle: "<<i<<"\n";

    //base->manipulate_interpretation(interpret_id, ref_id, threshhold++, offset, stride);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //safety reason
    bool wait=true;
    float progress=0;
    while(wait)
    {

      progress= (base->get_segment_progress(0));
      if(progress<100){
        std::cout<<i<<"Segment["<<0<<"] Progress: "<<progress<<"\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));//update rate
      }
      else{
        std::cout<<"Segment["<<0<<"] Progress: "<<progress<<" ->save\n";
        base->save("boost_cyc_"+std::to_string(i)+".png");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
        wait= false;
      }
    }
  }
  // //
  // typ_i       = 5;
  // ref_id      = -1;
  // threshhold  = 10;
  // base->add_interpretation(typ_i, ref_id, threshhold);
  // std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
  //

  typ_i       = 5;
  ref_id      = -1;
  threshhold  = 10;
  offset      = 0;
  stride      = 0;

  //base->add_interpretation(typ_i, ref_id, threshhold);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

  segment_id    = 0;
  interpret_id  = 1;
//  base->connect(segment_id, interpret_id);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason

  for (int i= 1; i<=20; i++){
    std::cout<<"cycle: "<<i<<"\n";
    //base->manipulate_interpretation(interpret_id, ref_id, threshhold*i, offset, stride);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000)); //safety reason
    bool wait=true;
    float progress=0;
    while(wait)
    {

      progress= (base->get_segment_progress(0));
      if(progress<100){
        std::cout<<"Segment["<<0<<"] Progress: "<<progress<<"\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));//update rate
      }
      else{
        std::cout<<"Segment["<<0<<"] Progress: "<<progress<<" ->save\n";
        base->save("reduce_cyc_"+std::to_string(i*10)+".png");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //safety reason
        wait= false;
      }
    }
  }

#endif


  std::cout << "testlab completed!\n";
  return 0;
}
