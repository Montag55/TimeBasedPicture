#include <chrono>
#include <thread>
#include <../include/interpretation.hpp>
#include <../include/average.hpp>
#include <../include/base.hpp>
#include <memory>


#include <string.h>
#include <vector>

Average::Average(std::shared_ptr<Base> mother, int id):
Interpretation{mother, id},
m_test{0}
{
  std::cout<<"average\n";
}

Average::~Average(){
  std::cout<<"deleting interpretation-avg\n";
}
int Average::getTypenumber(){
  return 0;
}

void Average::calc(int id, int start, int length, int sign, Mat& result, float& factor) {
  std::cout<<"AVERAGE: with work_size:"<<length<<" took ";
  auto start_time=std::chrono::high_resolution_clock::now();

  //CPU:
  std::cout<<"sign........"<<sign<<"\n";
  Mat tmp_frame;
  Mat tmp_frame_d;
  m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);

  for(int i=0; i<length; i++)
  {
    m_video->read(tmp_frame);
    if(tmp_frame.empty())
    {
      std::cout<<"empty\n";
    }
    tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?
    if(sign>0){
      factor+=1;
      result+=tmp_frame_d;
    }else{
      factor-=1;
      result-=tmp_frame_d;
    }
  }

  auto end_time=std::chrono::high_resolution_clock::now();
  auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
  std::cout<<duration<<" milli-seconds in computation\n";
}
