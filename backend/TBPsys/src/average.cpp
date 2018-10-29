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
  if(!m_base->use_gpu()){
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
  }else{
    //GPU:
    /*ffmpeg:
    double frameRate = m_video.get(CV_CAP_PROP_FPS);
    double frameTime = 1000.0 * m_frame_start_actual / frameRate;
    */
    GLApplication* app = m_base->getGLAPP();
    std::vector<Mat> pix_raw;
    std::vector<std::shared_ptr<std::vector<float>>> pictures;
    Mat tmp_frame;
    Mat tmp_frame_d;

    int Res_Y=  m_all_connections[id]->getY();
    int Res_X=  m_all_connections[id]->getX();
    m_video->set(CV_CAP_PROP_POS_MSEC, start/*frameTime*/);
    for(int i=0; i<length; i++)
    {
      m_video->read(tmp_frame);
      if(tmp_frame.empty())
      {
        std::cout<<"empty\n";
      }

      tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?

      pix_raw.push_back(tmp_frame_d.clone());
      std::shared_ptr<std::vector<float>> array= std::make_shared<std::vector<float>>();

      if (pix_raw.back().isContinuous()) {
        array->assign((float*)pix_raw.back().datastart, (float*)pix_raw.back().dataend);
      } else {
        std::cout << "cv::Mat to std::vector<float> failed" << std::endl;
      }
      pictures.push_back(array);
    }

    std:vector<float> tmp;
    std::vector<float> v = app->render(pictures, tmp);
    std::cout << "v-0: " << v[0] << std::endl;
    app->cleanup();
    Mat gpuimg = Mat(Res_Y, Res_X, m_img_type);
    memcpy(gpuimg.data, v.data(), v.size()*sizeof(float));

    v.clear();
    imwrite("rtddt.jpg", gpuimg);
    if(sign>0){
      result+=gpuimg;
      factor+=length;
    }else{
      factor-=length;
      result-=gpuimg;
    }
  }
  auto end_time=std::chrono::high_resolution_clock::now();
  auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
  std::cout<<duration<<" milli-seconds in computation\n";
}
