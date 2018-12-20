#include <../include/segment.hpp>
#include <../include/base.hpp>

#include <opencv2/opencv.hpp>
#include <string.h>
#include <math.h>
#include <chrono>
#include <omp.h>


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
  m_values_abs{cv::Mat(mother->get_height(), mother->get_width(), mother->get_img_type(), cv::Scalar(0,0,0))},
  m_values_fac{cv::Mat(mother->get_height(), mother->get_width(), mother->get_img_type(), cv::Scalar(0,0,0))},
  m_mask{cv::Mat(mother->get_height(), mother->get_width(), CV_32FC1, cv::Scalar(0))},
  m_uni_fac{0.0f},
  m_interpretation{std::make_shared<Average>(mother, -1, 0, 0)},
  m_mutex_soll{},
  m_mutex_state{},
  m_percent{0.0f},
  m_work_done{true},
  m_hasMask_actual{false},
  m_hasMask_destin{false},
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
  //std::cout << "\t > reset segment. \n";
  m_frame_start_actual = -1;
  m_frame_last_actual = -1;
  m_values_abs = cv::Mat(m_mother->get_height(), m_mother->get_width(), m_mother->get_img_type(), cv::Scalar(0,0,0));
  m_values_fac = cv::Mat(m_mother->get_height(), m_mother->get_width(), m_mother->get_img_type(), cv::Scalar(0,0,0));
  m_uni_fac = 0;
  m_percent= 0;
}

void Segment::revert_influence(){
  //should be muted from higher levl
  float intensity = -1;
  bool skip = false;
  cv::Mat factors = m_values_fac.clone();
  cv::Mat influence = m_values_abs.clone();


  if( m_interpretation->getTypenumber() == 0 || m_interpretation->getTypenumber() == 1 || m_interpretation->getTypenumber() == 8){
    if(m_uni_fac != 0)
      influence = (m_values_abs * ((float) 1 / ((float) m_uni_fac))) * m_intensity_local_actual * m_intensity_global_actual;
    else
      skip = true;
    intensity = m_intensity_global_actual;
  }
  else if(m_interpretation->getTypenumber() == 2 || m_interpretation->getTypenumber() == 3 || m_interpretation->getTypenumber() == 4 || m_interpretation->getTypenumber() == 5 || m_interpretation->getTypenumber() == 6  || m_interpretation->getTypenumber() == 7 || m_interpretation->getTypenumber() == 9){
    normalize_factor(influence, factors);
    influence = influence * m_intensity_local_actual * m_intensity_global_actual;
    factors = factors * m_intensity_global_actual;
    intensity = m_uni_fac;
  }
  else{
    std::cout<< "revert influence is not allowed yet " << m_interpretation->getTypenumber() << "\n";
  }


  if(m_hasMask_actual){
    std::cout<<"XXXXXXXXXXrevert influence of mask\n";
    cv::Mat tmp_mask = cv::Mat(m_mother->get_max_Point().y, m_mother->get_max_Point().x, CV_32FC3, cv::Scalar(0,0,0));
    int from_to[] = { 0,0, 0,1, 0,2};
    cv::mixChannels(&m_mask, 1, &tmp_mask, 1, from_to, 3);
    factors = factors.mul(tmp_mask);
    influence = influence.mul(tmp_mask);
    if( m_interpretation->getTypenumber() == 0 || m_interpretation->getTypenumber() == 1 || m_interpretation->getTypenumber() == 8){
      intensity = 0;
      factors = tmp_mask * m_intensity_global_actual;
    }
  }

  if(!skip){
    m_mother->add_to_values_abs(-influence);
    m_mother->add_to_values_fac(-factors);
    m_mother->add_to_uni_fac(-intensity);
  }
}

void Segment::upload_influence(){
  // should be muted from above
  // intensity could be uplaoded here
  float intensity = -1;
  bool skip = false;
  cv::Mat factors = m_values_fac.clone();
  cv::Mat influence = m_values_abs.clone();

  if(m_interpretation->get_upload_specification() == 0){
    if(m_uni_fac != 0)
      influence = (m_values_abs * ((float) 1 / ((float) m_uni_fac))) * m_intensity_local_actual * m_intensity_global_actual;
    else
      skip = true;

    intensity = m_intensity_global_actual;
  }
  else if(m_interpretation->get_upload_specification() == 1){
    normalize_factor(influence, factors);
    influence = influence * m_intensity_local_actual * m_intensity_global_actual;
    factors = factors * m_intensity_global_actual;
    intensity = m_uni_fac;
  }
  else{
    std::cout<< "upload influence is not allowed yet " << m_interpretation->getTypenumber() << "\n";
  }


  if(m_hasMask_actual){

    std::cout<<"XXXXXXXXXXread mask for upload\n";
    updateMask("mask" + std::to_string(m_id) + ".png");
    cv::Mat tmp_mask = cv::Mat(m_mother->get_max_Point().y, m_mother->get_max_Point().x, CV_32FC3, cv::Scalar(0,0,0));
    int from_to[] = { 0,0, 0,1, 0,2};
    cv::mixChannels(&m_mask, 1, &tmp_mask, 1, from_to, 3);
    factors = factors.mul(tmp_mask);
    influence = influence.mul(tmp_mask);
    if( m_interpretation->getTypenumber() == 0 || m_interpretation->getTypenumber() == 1 || m_interpretation->getTypenumber() == 8){
      intensity = 0;
      factors = tmp_mask * m_intensity_global_actual;
    }
  }

  if(!skip){
    m_mother->add_to_values_abs(influence);
    m_mother->add_to_values_fac(factors);
    m_mother->add_to_uni_fac(intensity);
  }
}

void Segment::normalize_factor(cv::Mat& influence, cv::Mat& factors){
  influence = influence / factors;
  factors = factors / factors;
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

      revert_influence();
      reset();
      m_interpretation->delete_connection(m_id);
      m_interpretation = m_new_interpretation;
      m_interpretation->add_connection(m_id, this);
      m_new_interpretation = NULL;
  }
  //check wether mask is on/off:
  if(m_hasMask_actual!=m_hasMask_destin)
  {
    revert_influence();
    m_hasMask_actual=m_hasMask_destin;
    upload_influence();
  }
  //interpretation need reset:
  if(m_needs_reset) {
    revert_influence(); //dont know?
    reset();
    m_needs_reset = false;
  }


  m_mutex_soll.unlock();

  if(work_size > 0) {
    if(m_interpretation->get_calculation_specification() == 0){
      state = interpret_free( work_size );
    }
    else if(m_interpretation->get_calculation_specification() == 1){
      state = interpret_extending( work_size );
    }
    else if(m_interpretation->get_calculation_specification() == 2){
      state = interpret_one_way( work_size );
    }
    else{
      std::cout<<"Other interpretation traversal not implemnted yet. \n";
    }

    m_mutex_soll.lock();
    float soll = m_frame_last_destin - m_frame_start_destin;
    float ist = abs(m_frame_last_actual - m_frame_last_destin) + abs(m_frame_start_actual - m_frame_start_destin);
    float percentage = 100 - 100.0 * ( ist / soll );

    if( percentage < 0 ){
      percentage = 0;
    }

    m_mutex_soll.unlock();

    if(percentage == 100 && state) {
      save_segment_out();
      m_mutex_state.lock();
    }
    else {
      m_mutex_state.lock();
      state = false;
    }

    m_work_done = state;
    m_percent = percentage;
    m_mutex_state.unlock();
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
    std::cout << "   (needs to recalculate segment, because of new interpretation.) \n";
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

void Segment::updateMask(std::string mask_path){
  if(m_hasMask_actual){
    auto start_time = std::chrono::high_resolution_clock::now();

    cv::Mat new_mask = cv::imread("./" + mask_path);

    if(new_mask.empty()){
      new_mask = cv::Mat(m_mother->get_max_Point().y, m_mother->get_max_Point().x, CV_32FC1, cv::Scalar(0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else{
      std::cout<<"read nes mask right now\n";
      cv::cvtColor(new_mask, new_mask, CV_BGR2GRAY);
      new_mask.convertTo(new_mask, CV_32FC1);
    }

    cv::Mat tmp_mat_diff_check;
    double min = 0, max = 0;
    cv::absdiff(m_mask, new_mask, tmp_mat_diff_check);
    cv::minMaxLoc(tmp_mat_diff_check, &min, &max);

    if(max > 0){
      std::cout<<"prep to read\n";
      cv::imwrite("oldmask.png", m_mask);
      cv::imwrite("newmask.png", new_mask);
      m_mask = new_mask;
      //cv::absdiff(m_mask, new_mask, m_mask);
    }

    m_mask = m_mask / 255;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
        std::cout << "\t\t + Seg update Mask: \t" << duration << std::endl;
  }
}

bool Segment::interpret_free( int & work_size){

  m_mutex_soll.lock();
  int dest_start = m_frame_start_destin;
  int dest_end = m_frame_last_destin;
  bool exit_status = false;
  m_mutex_soll.unlock();

  //int delta=m_frame_last_actual-m_frame_start_actual;

  if(m_frame_start_actual > -1) {
    revert_influence();
  }

  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;

  //shortcuts possible?
  int moves     = abs(dest_start - m_frame_start_actual) + abs(dest_end - m_frame_last_actual);
  int alt_moves = dest_end - dest_start;

  if(alt_moves < moves && m_frame_start_actual>-1) {
    reset();
  }
  //startingpoint:
  if(m_frame_start_actual == -1) {//not yet computed!
    m_frame_start_actual = dest_start;
  }

  if(m_frame_start_actual < dest_start) {
    int length = work_size;

    if(m_frame_start_actual + work_size > dest_start){
      length = dest_start - m_frame_start_actual;
    }

    int sign = -1;
    m_interpretation->calc(m_id, m_frame_start_actual, length, sign, m_values_abs, m_uni_fac, m_values_fac);
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
    m_interpretation->calc(m_id, startpoint, length, sign, m_values_abs, m_uni_fac, m_values_fac);
    work_size -= length;
    m_frame_start_actual -= length;
  }
  //endpoint:
  if( work_size > 0 ){
    if(m_frame_last_actual == -1) {//not yet computed!
       m_frame_last_actual = dest_start;
     }

     if(m_frame_last_actual < dest_end) {
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
       m_interpretation->calc(m_id, m_frame_last_actual, length, sign, m_values_abs, m_uni_fac, m_values_fac);
       work_size -= length;
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
       m_interpretation->calc(m_id, startpoint, length, sign, m_values_abs, m_uni_fac, m_values_fac);
       work_size -= length;
       m_frame_last_actual -= length;
     }
  }

  if(m_frame_start_actual > -1) {

    upload_influence();
  }

  if((dest_start == m_frame_start_actual) && (dest_end == m_frame_last_actual)) {
    exit_status = true;  //ist = soll
  }
  else{
    exit_status = false; //ist != soll
  }

  return exit_status;
}

bool Segment::interpret_extending( int & work_size){

  //allows only enlargment.. shrinking-> reset
  m_mutex_soll.lock();
  int dest_start = m_frame_start_destin;
  int dest_end = m_frame_last_destin;
  bool exit_status = false;
  m_mutex_soll.unlock();

  //int delta=m_frame_last_actual-m_frame_start_actual;

  if(m_frame_start_actual > -1) {
    revert_influence();
  }

  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;

  //reset neccessary?
  if( m_frame_last_actual > dest_end )
  {
    reset();
  }else if(m_frame_start_actual < dest_start)
  {
    reset();
  }

  //startingpoint:
  if(m_frame_start_actual == -1) {//not yet computed!
    m_frame_start_actual = dest_start;
  }

  if(m_frame_start_actual < dest_start) {
    std::cout << "a: should not happen !? \n";

  }
  else if(m_frame_start_actual > dest_start) {
    int startpoint = m_frame_start_actual - work_size;
    int length = work_size;

    if(startpoint < dest_start) {
      length = m_frame_start_actual - dest_start;
      startpoint = dest_start;
    }

    int sign = -1;
    // -1 indicates late merge:
    // first calculate overplotting for work length
    // and then insert all pixels to result img & fac, that aren't already used
    m_interpretation->calc(m_id, startpoint, length, sign, m_values_abs, m_uni_fac, m_values_fac);
    work_size -= length;
    m_frame_start_actual -= length;
  }
  //endpoint:
  if( work_size > 0 ){
    if(m_frame_last_actual == -1) {//not yet computed!
       m_frame_last_actual = dest_start;
     }

     if(m_frame_last_actual < dest_end) {
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
       m_interpretation->calc(m_id, m_frame_last_actual, length, sign, m_values_abs, m_uni_fac, m_values_fac);
       work_size -= length;
       m_frame_last_actual += length;
     }
     else if(m_frame_last_actual > dest_end) {
       std::cout << "b: should not happen !? \n";
     }
  }

  if(m_frame_start_actual > -1) {

    upload_influence();
  }

  if((dest_start == m_frame_start_actual) && (dest_end == m_frame_last_actual)) {
    exit_status = true;  //ist = soll
  }
  else{
    exit_status = false; //ist != soll
  }

  return exit_status;
}

bool Segment::interpret_one_way( int & work_size){
  //allows only enlargment.. shrinking-> reset
  m_mutex_soll.lock();
  int dest_start = m_frame_start_destin;
  int dest_end = m_frame_last_destin;
  bool exit_status = false;
  m_mutex_soll.unlock();

  //int delta=m_frame_last_actual-m_frame_start_actual;

  if(m_frame_start_actual > -1) {
    revert_influence();
  }

  cv::Mat tmp_frame;
  cv::Mat tmp_frame_d;

  //reset neccessary?
  if( m_frame_last_actual > dest_end )
  {
    reset();
  }
  else if(m_frame_start_actual < dest_start)
  {
    reset();
  }
  else if(m_frame_start_actual > dest_start)
  {
    reset();
  }

  //startingpoint:
  if(m_frame_start_actual == -1) {//not yet computed!
    m_frame_start_actual = dest_start;
  }

  if(m_frame_start_actual < dest_start) {
    std::cout << "a: should not happen !? \n";

  }
  else if(m_frame_start_actual > dest_start) {
        std::cout << "b: should not happen !? \n";
  }
  //endpoint:
  if( work_size > 0 ){

    if(m_frame_last_actual == -1) {//not yet computed!
       m_frame_last_actual = dest_start;
     }

     if(m_interpretation->getTypenumber() == 9) {
       // std::cout<< "checking shortcut: "<< m_frame_last_actual<<"\n";
       Paint& interpretation = dynamic_cast<Paint&>(*m_interpretation);
       int new_frame = interpretation.get_time_min(m_frame_last_actual);

       if( new_frame == -1){
         m_frame_last_actual = dest_end;
       }
       else{
         m_frame_last_actual = new_frame;
       }
       // std::cout<< "now: "<< m_frame_last_actual<<"\n";
     }

     if(m_frame_last_actual < dest_end) {
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
       m_interpretation->calc(m_id, m_frame_last_actual, length, sign, m_values_abs, m_uni_fac, m_values_fac);
       work_size -= length;
       m_frame_last_actual += length;
     }
     else if(m_frame_last_actual > dest_end) {
       std::cout << "b: should not happen !? \n";
     }
  }

  if(m_frame_start_actual > -1) {

    upload_influence();
  }


  if((dest_start == m_frame_start_actual) && (dest_end == m_frame_last_actual)) {
    exit_status = true;  //ist = soll
  }
  else{
    exit_status = false; //ist != soll
  }
  return exit_status;
}

void Segment::save_segment_out(){
  auto start_time = std::chrono::high_resolution_clock::now();

  std::string out_file = std::string("out_seg_id") + std::to_string(m_id) + ".png";
  cv::imwrite(out_file, m_values_abs / (m_values_fac + cv::Scalar(m_uni_fac, m_uni_fac, m_uni_fac)) * m_intensity_local_actual);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  #ifdef show_time
      std::cout << "\t\t * Segment_save_time: \t" << duration << std::endl;
  #endif
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

void Segment::manipulate(int start, int end, float local_i, float global_i, bool hasMask){
  end += 1;//including the end frame to calculation
  m_mutex_soll.lock();
  m_frame_start_destin = start;
  if(m_frame_last_destin != end && m_interpretation->get_calculation_specification() == 2){
    m_needs_reset = true;
  }
  m_frame_last_destin = end;
  m_intensity_local_destin = local_i;
  m_intensity_global_destin = global_i;
  m_hasMask_destin = hasMask;
  m_mutex_soll.unlock();
  ready_to_work();
}

void Segment::delete_seg(){
  m_mutex_soll.lock();
  m_intensity_global_destin=0;
  m_mutex_soll.unlock();

  ready_to_work();
}

int Segment::get_start(){
  m_mutex_soll.lock();
  int start = m_frame_start_destin;
  m_mutex_soll.unlock();
  return start;
}

int Segment::get_end(){
  m_mutex_soll.lock();
  int end = m_frame_last_destin;
  m_mutex_soll.unlock();
  return end;
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
