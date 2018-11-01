#include <../include/segment.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <string.h>
#include <math.h>
#include <chrono>

Segment::Segment(int start_frame, int last_frame, double intensity_local, double intensity_global, std::shared_ptr<Base> mother, int id):
  m_mother{mother},
  m_frame_start_destin{start_frame},
  m_frame_last_destin{last_frame},
  m_frame_start_actual{-1},
  m_frame_last_actual{-1},
  m_intensity_local_destin{intensity_local},
  m_intensity_global_destin{intensity_global},
  m_intensity_local_actual{intensity_local},
  m_intensity_global_actual{intensity_global},
  m_values_abs{cv::Mat(mother->get_width(), mother->get_height(), mother->get_img_type(), cv::Scalar(0,0,0))},
  m_values_fac{cv::Mat(mother->get_width(), mother->get_height(), CV_64FC1, cv::Scalar(0))},
  m_uni_fac{0.0f},
  m_interpretation{std::make_shared<Average>(mother,-8)}, //default id: -8!?
  m_mutex_soll{},
  m_mutex_state{},
  m_percent{0.0f},
  m_work_done{true},
  m_new_interpretation{NULL},
  m_needs_reset{false},
  m_id{id} {
    m_interpretation->add_connection(id, this);
    ready_to_work();
  }

Segment::~Segment(){
  std::cout << "delete segment. \n";
}

void Segment::ready_to_work(){
  m_mutex_state.lock();
  bool state = m_work_done;
  m_mutex_state.unlock();

  if(state) {
    m_mutex_state.lock();
    m_work_done = false;
    m_mutex_state.unlock();
    m_mother->add_work(this);
  }
}

void Segment::reset(){
  std::cout << "reset segment. \n";
  m_frame_start_actual = -1;
  m_frame_last_actual = -1;
  revert_influence();
  m_uni_fac = 0;
}

void Segment::revert_influence(){
  m_mother->add_to_values_abs(-1 * (m_values_abs * ((float) 1 / ((float) m_uni_fac))) * m_intensity_local_actual * m_intensity_global_actual);
  m_mother->add_to_uni_fac( -m_intensity_global_actual);
}

void Segment::upload_influence(){
  m_mother->add_to_values_abs((m_values_abs * ((float) 1 / ((float) m_uni_fac))) * m_intensity_local_actual * m_intensity_global_actual);
  m_mother->add_to_uni_fac(m_intensity_global_actual);
}

bool Segment::work(int& work_size){
  //ist soll gleich ist stand?
  m_mutex_state.lock();
  bool state = m_work_done;
  m_mutex_state.unlock();

  update_intensity();
  m_mutex_soll.lock();

  //set new interpetation:
  if(m_new_interpretation != NULL) {
      m_interpretation->delete_connection(m_id);
      m_interpretation = m_new_interpretation;
      m_interpretation->add_connection(m_id, this);
      m_new_interpretation = NULL;
      reset();
  }
  //interpretation need reset:
  if(m_needs_reset) {
    reset();
    m_needs_reset = false;
  }

  m_mutex_soll.unlock();

  if(work_size > 0) {
    state = interpret_sized(work_size);
    m_mutex_soll.lock();
    float soll = m_frame_last_destin - m_frame_start_destin;
    float ist = abs(m_frame_last_actual - m_frame_last_destin) + abs(m_frame_start_actual - m_frame_start_destin);

    float percentage = 100 - 100.0 * ( ist / soll );

    if( percentage < 0 ){
      percentage = 0;
    }

    m_mutex_soll.unlock();

    m_mutex_state.lock();
    m_work_done = state;
    m_percent = percentage;
    m_mutex_state.unlock();
  }
  if(state) {
    std::string out_file = std::string("out_seg_id") + std::to_string(m_id) + ".jpg";
    imwrite(out_file, m_values_abs * ((float) 1 / ((float) m_uni_fac)) * m_intensity_local_actual);
  }

  return state;
}

void Segment::set_interpretation(std::shared_ptr<Interpretation> interpret){
  if(m_interpretation->get_id() == interpret->get_id()) {
    std::cout << "the just set interpretation is already set. \n";
  }
  else {

    m_mutex_soll.lock();
    m_new_interpretation = interpret;
    m_mutex_soll.unlock();
    std::cout << "needs to recalculate segment, because of new interpretation. \n";
    ready_to_work();

  }
}

float Segment::get_progress(){
  m_mutex_state.lock();
  float out = m_percent;
  m_mutex_state.unlock();
  return out;
}

void Segment::update_interpretation(){
  //is called, when complete recomput
  m_mutex_soll.lock();
  m_needs_reset = true;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::update_intensity(){
    m_mutex_soll.lock();
    float dest_intensity_local = m_intensity_local_destin;
    float dest_intensity_global = m_intensity_global_destin;
    m_mutex_soll.unlock();

    //int delta=m_frame_last_actual-m_frame_start_actual;
    if(m_intensity_local_actual != dest_intensity_local) {
        revert_influence();
        m_intensity_local_actual = dest_intensity_local;
        upload_influence();
    }

    if(m_intensity_global_actual != dest_intensity_global) {
      revert_influence();
      m_intensity_global_actual = dest_intensity_global;
      upload_influence();
    }
}

bool Segment::interpret_sized( int & work_size){
  m_mutex_soll.lock();
  int dest_start = m_frame_start_destin;
  int dest_end = m_frame_last_destin;
  bool exit_status = false;
  m_mutex_soll.unlock();

  //int delta=m_frame_last_actual-m_frame_start_actual;
  if(m_uni_fac/*m_frame_start_actual*/ > 0) {
    revert_influence();
  }

  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;

  //shortcuts possible?
  int moves     = abs(dest_start - m_frame_start_actual) + abs(dest_end - m_frame_last_actual);
  int alt_moves = dest_end - dest_start;

  if(alt_moves <= moves) {
    m_frame_start_actual = dest_start;
    m_frame_last_actual = dest_start;
    m_values_abs -= m_values_abs;
    m_uni_fac -= m_uni_fac;
  }

  //startingpoint:
  if(m_frame_start_actual == -1) {//not yet computed!
    m_frame_start_actual = dest_start;

  }
  else if(m_frame_start_actual < dest_start) {
    int length = work_size;

    if(m_frame_start_actual + work_size>dest_start){
      length = dest_start - m_frame_start_actual;
    }

    int sign = -1;
    m_interpretation->calc(m_id, m_frame_start_actual, length, sign, m_values_abs, m_uni_fac);
    work_size -= length;
    m_frame_start_actual += length;
  }
  else if(m_frame_start_actual > dest_start) {
    int startpoint = m_frame_start_actual - work_size;
    int length = work_size;

    if(startpoint < dest_start) {
      length = m_frame_start_actual - dest_start;
      startpoint = dest_start;
    }

    int sign = 1;
    m_interpretation->calc(m_id, startpoint, length, sign, m_values_abs, m_uni_fac);
    m_frame_start_actual -= length;
  }

  //endpoint:
  if(m_frame_last_actual == -1) {//not yet computed!
    m_frame_last_actual = dest_start;
  }
  else if(m_frame_last_actual < dest_end) {
    /*ffmpeg:
    double frameRate = m_video.get(CV_CAP_PROP_FPS);
    double frameTime = 1000.0 * m_frame_last_actual / frameRate;
    */
    //m_video.set(CV_CAP_PROP_POS_MSEC, m_frame_last_actual/*frameTime*/);
    int endpoint = m_frame_last_actual + work_size;
    int length = work_size;
    if(endpoint > dest_end){
      endpoint = dest_end;
      length = dest_end - m_frame_last_actual;
    }

    int sign = 1;
    m_interpretation->calc(m_id, m_frame_last_actual, length, sign, m_values_abs, m_uni_fac);
    m_frame_last_actual += length;
  }
  else if(m_frame_last_actual > dest_end) {
    int startpoint = m_frame_last_actual - work_size;
    int length = work_size;

    if(startpoint < dest_end) {
      length = m_frame_last_actual - dest_end;
      startpoint = dest_end;
    }

    int sign =- 1;
    m_interpretation->calc(m_id, startpoint, length, sign, m_values_abs, m_uni_fac);
    m_frame_last_actual -= length;
  }

  if(m_uni_fac > 0) {
    upload_influence();
  }

  std::cout<<m_uni_fac<<"munifac..............................\n";

  if((dest_start == m_frame_start_actual) && (dest_end == m_frame_last_actual)) {
    exit_status = true;  //ist = soll
  }
  else{
    exit_status = false; //ist != soll
  }

  return exit_status;
}

//EDIT THE SEGMENT:
void Segment::move_in_time(int delta){
  m_mutex_soll.lock();
  m_frame_start_destin += delta;
  m_frame_last_destin += delta;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::set_exposure(int new_exposure){
  m_mutex_soll.lock();
  m_frame_last_destin = m_frame_start_destin + new_exposure;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::set_startframe(int imgs){
  m_mutex_soll.lock();
  m_frame_start_destin = imgs;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::set_endframe(int imgs){
  m_mutex_soll.lock();
  m_frame_last_destin = imgs;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::set_exposure_delta(int delta){
  m_mutex_soll.lock();
  m_frame_last_destin += delta;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::set_local_intensity(float i){
  m_mutex_soll.lock();
  m_intensity_local_destin = i;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::set_global_intensity(float i){
  m_mutex_soll.lock();
  m_intensity_global_destin = i;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::manipulate(int start, int end, float local_i, float global_i){
  end += 1;//including the end frame to calculation
  m_mutex_soll.lock();
  m_frame_start_destin = start;
  m_frame_last_destin = end;
  m_intensity_local_destin = local_i;
  m_intensity_global_destin = global_i;
  m_mutex_soll.unlock();

  ready_to_work();
}

void Segment::delete_seg(){
  m_mutex_soll.lock();
  m_intensity_global_destin=0;
  m_mutex_soll.unlock();

  ready_to_work();
}


/*
void Segment::boost_similarities(int power){
  //tries to kick everything out that is moving...
  //another ,but expensive, option might be a modal filter..in time?
  /*
  might be good to treshhold video errors.. litle bit shaking cam..
  and make effect stronger
  */
  /*
  double treshhold=3*255-300;
  /*another not to extreme  smoother boost function, might be nice*/
  //double treshhold=10;
  //reset influence :
  /*
  m_m_values_abs - m_values_abs;
  m_m_values_fac - m_values_fac;

  //vid
  Mat tmp_frame;
  Mat tmp_frame_d;

  /*
  Mat avg_mat=Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));

  //get avg
  m_uni_fac = 1;             //simply the number of frames
  m_video.set(CV_CAP_PROP_POS_FRAMES,m_frame_start);  //start frame

  for(unsigned int i = m_frame_start; i <= m_frame_last; i++) {
    std::cout<<"frame:"<<i<<"\n";
    m_video.read(tmp_frame);

    tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?

    avg_mat += tmp_frame_d / (m_frame_last - m_frame_start + 1);
  }

  //search for differences:
  std::cout<<"search for diff\n";
  m_video.set(CV_CAP_PROP_POS_FRAMES, m_frame_start);  //start frame
  m_values_abs += avg_mat;    //set simpl clr

  for(unsigned int i = m_frame_start; i <= m_frame_last; i++) {
    //std::cout<<"frame:"<<i<<"\n";
    m_video.read(tmp_frame);

    tmp_frame.convertTo(tmp_frame_d, m_img_type);
      for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row){
          //ptr:
          const float *ptr_tmp  =  (float*)tmp_frame_d.ptr(row);
          const float *ptr_avg  =  (float*)avg_mat.ptr(row);
          float *ptr_abs        =  (float*)m_values_abs.ptr(row);
          float *ptr_fac        =  (float*)m_values_fac.ptr(row);
          //const float *ptr_fac =  (float*)m_values_fac.ptr(row);
          for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
              //ptr:
              const float *uc_pixel_tmp = ptr_tmp;
              const float *uc_pixel_avg = ptr_avg;
              float *uc_pixel_abs       = ptr_abs;
              float *uc_pixel_fac       = ptr_fac;

              //double distance_old=abs(uc_pixel_abs[0]-uc_pixel_avg[0])+abs(uc_pixel_abs[1]-uc_pixel_avg[1])+abs(uc_pixel_abs[2]-uc_pixel_avg[2]);
              //maybe other fct?: double distance_new=exp(((3*255)-(abs(uc_pixel_tmp[0]-uc_pixel_avg[0])+abs(uc_pixel_tmp[1]-uc_pixel_avg[1])+abs(uc_pixel_tmp[2]-uc_pixel_avg[2])))-500);

              //maybe pythagoras dist?:
              double distance=(abs(uc_pixel_tmp[0]-uc_pixel_avg[0])+abs(uc_pixel_tmp[1]-uc_pixel_avg[1])+abs(uc_pixel_tmp[2]-uc_pixel_avg[2]));
              double value =pow(3*255-distance,power);
              if(distance<treshhold)
              {
                for(int c=0; c<3; c++) //allow more channel!
                {
                  uc_pixel_abs[c]+=uc_pixel_tmp[c]*value;
                }

                uc_pixel_fac[0]+=value;
              }else{
                //std::cout<<"cut\n";
              }


              ptr_tmp += 3;
              ptr_avg += 3;
              ptr_abs += 3;
              ptr_fac += 1;
          }
      }
  }
  m_m_uni_fac+=1;
  m_m_values_abs+=m_values_abs;
  m_m_values_fac+=m_values_fac;

}*/


//it boosts extraordinary influences in an image
//void Segment::boost_diff()
//{
  //some image sectors look like they are negativ.. should that happen?
  /*another not to extreme  smoother boost function, might be nice*/
/*
  double treshhold=10;
  //reset influence :
  m_m_values_abs-m_values_abs;
  m_m_values_fac-m_values_fac;
  for(unsigned int i = m_frame_start; i <= m_frame_last; i++) {

    std::cout<<"frame:"<< i <<"\n";
    m_video >> tmp_frame;
    tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?

    m_values_abs += tmp_frame_d * m_intensity_local * m_intensity_global;
  }

  m_m_values_abs  += m_values_abs;
  m_m_uni_fac     += m_uni_fac;
  //auto end=std::chrono::high_resolution_clock::now();
  //auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end-st).count();
  //std::cout<<duration<<"\n";

  //search for differences:
  std::cout<<"search for diff\n";
  m_video.set(CV_CAP_PROP_POS_FRAMES,m_frame_start);  //start frame
  m_values_abs+=avg_mat;    //set simpl clr
  for(int i=m_frame_start; i<=m_frame_last;i++)
  {
    //std::cout<<"frame:"<<i<<"\n";
    m_video.read(tmp_frame);

    tmp_frame.convertTo(tmp_frame_d, m_img_type);
      for (int row = m_pnt_min.y; row < m_pnt_max.y; ++row){
          //ptr:
          const float *ptr_tmp =        (float*)tmp_frame_d.ptr(row);
          const float *ptr_avg =        (float*)avg_mat.ptr(row);
          float *ptr_abs =  (float*)m_values_abs.ptr(row);
          //const float *ptr_fac =  (float*)m_values_fac.ptr(row);
          for (int col = m_pnt_min.x; col < m_pnt_max.x; col++)
          {
              //ptr:
              const float *uc_pixel_tmp = ptr_tmp;
              const float *uc_pixel_avg = ptr_avg;
              float *uc_pixel_abs = ptr_abs;

              double distance_old=abs(uc_pixel_abs[0]-uc_pixel_avg[0])+abs(uc_pixel_abs[1]-uc_pixel_avg[1])+abs(uc_pixel_abs[2]-uc_pixel_avg[2]);
              double distance_new=abs(uc_pixel_tmp[0]-uc_pixel_avg[0])+abs(uc_pixel_tmp[1]-uc_pixel_avg[1])+abs(uc_pixel_tmp[2]-uc_pixel_avg[2]);
              if(distance_new>treshhold && distance_old<distance_new)
              {
                for(int c=0; c<3; c++) //allow more channel!?
                {
                  uc_pixel_abs[c]=uc_pixel_tmp[c];
                }
              }

              ptr_tmp += 3;
              ptr_avg += 3;
              ptr_abs += 3;
          }
      }
  }
  m_m_uni_fac+=1;
  m_m_values_abs+=m_values_abs;
  */
//}

//it boosts extraordinary influences in an image
/*
void Segment::boost_diff() {
  /*
  /*another not to extreme  smoother boost function, might be nice*/
  /*
  double treshhold = 10;
  //reset influence :
  m_m_values_abs - m_values_abs;
  m_m_values_fac - m_values_fac;

  //vid
  Mat tmp_frame;
  Mat tmp_frame_d;


  Mat avg_mat = Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));

  //get avg
  m_uni_fac = 1;             //simply the number of frames
  m_video.set(CV_CAP_PROP_POS_FRAMES, m_frame_start);  //start frame

  for(unsigned int i = m_frame_start; i <= m_frame_last; i++) {
    std::cout<<"frame:"<<i<<"\n";
    m_video.read(tmp_frame);

    tmp_frame.convertTo(tmp_frame_d, m_img_type);   //do this for the whole video right at the start!?

    avg_mat += tmp_frame_d / (m_frame_last - m_frame_start+1);
  }

  //search for differences:
  std::cout<<"search for diff\n";
  m_video.set(CV_CAP_PROP_POS_FRAMES, m_frame_start);  //start frame
  m_values_abs += avg_mat;    //set simpl clr

  for(int i = m_frame_start; i <= m_frame_last; i++) {
    //std::cout<<"frame:"<<i<<"\n";
    m_video.read(tmp_frame);

    tmp_frame.convertTo(tmp_frame_d, m_img_type);
      for (unsigned int row = m_pnt_min.y; row < m_pnt_max.y; ++row){
          //ptr:
          const float *ptr_tmp =        (float*)tmp_frame_d.ptr(row);
          const float *ptr_avg =        (float*)avg_mat.ptr(row);
          float *ptr_abs =  (float*)m_values_abs.ptr(row);
          float *ptr_fac =  (float*)m_values_fac.ptr(row);
          //const float *ptr_fac =  (float*)m_values_fac.ptr(row);
          for (unsigned int col = m_pnt_min.x; col < m_pnt_max.x; col++) {
              //ptr:
              const float *uc_pixel_tmp = ptr_tmp;
              const float *uc_pixel_avg = ptr_avg;
              float *uc_pixel_abs = ptr_abs;
              float *uc_pixel_fac = ptr_fac;

              //double distance_old=abs(uc_pixel_abs[0]-uc_pixel_avg[0])+abs(uc_pixel_abs[1]-uc_pixel_avg[1])+abs(uc_pixel_abs[2]-uc_pixel_avg[2]);
              double distance_new=pow(abs(uc_pixel_tmp[0]-uc_pixel_avg[0])+abs(uc_pixel_tmp[1]-uc_pixel_avg[1])+abs(uc_pixel_tmp[2]-uc_pixel_avg[2]),power);
              if(distance_new>treshhold)
              {
                for(int c=0; c<3; c++) //allow more channel!
                {
                  uc_pixel_abs[c]+=uc_pixel_tmp[c]*distance_new;
                }

                uc_pixel_fac[0]+=distance_new;
              }


              ptr_tmp += 3;
              ptr_avg += 3;
              ptr_abs += 3;
              ptr_fac += 1;
          }
      }
  }
  m_m_uni_fac+=1;
  m_m_values_abs+=m_values_abs;
  m_m_values_fac+=m_values_fac;

}*/
