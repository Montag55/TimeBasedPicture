#include <../include/base.hpp>
#include <../include/segment.hpp>


#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>
#include <string.h>
#include <vector>
#include <sys/stat.h>

Base::Base(std::string const& video_name) :
  m_in_calculation{false},
  m_file {video_name},
  m_work_size{10},
  m_video{std::make_shared<cv::VideoCapture>( video_name, cv::CAP_IMAGES)},  /*!when changing to ffmpeg, change set framepos!*/
  m_img_type{CV_32FC3},//http://ninghang.blogspot.de/2012/11/list-of-mat-type-in-opencv.html
  m_img_delta{3},
  m_frame_start{0},
  m_intensity{1.0f},
  m_uni_fac{0.0f},
  m_worker{&Base::thread_calc_loop, this},
  m_mutex_update{},
  m_mutex_result{},
  m_segments{std::vector<std::shared_ptr<Segment>>()},
  m_interpretations{std::vector<std::shared_ptr<Interpretation>>()}
  {
    m_pnt_min = cv::Point(0, 0);
    m_pnt_max = cv::Point(m_video->get(CV_CAP_PROP_FRAME_WIDTH), m_video->get(CV_CAP_PROP_FRAME_HEIGHT));
    m_frame_last = m_video->get(CV_CAP_PROP_FRAME_COUNT) - 1;
    m_values_abs = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
    m_values_fac = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
    m_result = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));

    std::cout<<"NEW BASE\n";
    std::cout<<"video:"<<m_file<<"\n";
    std::cout<<"pnt_min:"<<m_pnt_min<<"\n";
    std::cout<<"pnt_max:"<<m_pnt_max<<"\n";
    std::cout<<"frame_start:"<<m_frame_start<<"\n";
    std::cout<<"frame_last:"<<m_frame_last<<"\n";
    std::cout<<"intensity:"<<m_intensity<<"\n";
  }

Base::~Base(){
  std::cout << "Base-Destruction: \n";
  m_interpretations.clear();
  m_segments.clear();
}

void Base::compressVideoFiles(){

  struct stat info;
  std::string directory = "./previewImages";

  if(stat(directory.data() , &info)){
    system(("mkdir " + directory).data());
  }
  else{
    system(("rm -rf " + directory).data());
    system(("mkdir " + directory).data());
  }

  m_video->set(CV_CAP_PROP_POS_MSEC, 0);
  cv::Mat tmp_frame;
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
  compression_params.push_back(20);

  auto start_time = std::chrono::high_resolution_clock::now();


  for(int i = 0; i < m_frame_last + 1; i++) {
    m_video->read(tmp_frame);
    if(tmp_frame.empty()){
      std::cout<<"empty\n";
    }
    cv::imwrite(directory + "/frame" + std::to_string(i) + ".jpg" , tmp_frame, compression_params);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  std::cout << "\t\t + time: \t" << duration << std::endl;
}

void Base::thread_calc_loop(){ //waits for work and makes calculataion
  int wait_for_work_time = 10;
  int wait_while_work = 10;
  bool save_state = true;
  m_mutex_update.lock();
  bool in_calc = m_in_calculation;
  m_mutex_update.unlock();

  while(true) {
    while(in_calc) {
      auto start_time=std::chrono::high_resolution_clock::now();

      std::this_thread::sleep_for(std::chrono::milliseconds(wait_while_work));
      continue_work();  //with size?
      m_mutex_update.lock();
      in_calc = m_in_calculation;
      m_mutex_update.unlock();
      save_state = false;

      auto end_time = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
      #ifdef show_time
          std::cout << "\t # Base_full_calc_cycle_time: \t" << duration << "\n\n";
      #endif
    }

    if(!save_state){
      save_state = true;
      this->save("fin_state.png");
    }

    m_mutex_update.lock();
    in_calc = m_in_calculation;
    m_mutex_update.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_for_work_time));
  }
}

int Base::add_segment(int start, int end, float local_i, float global_i){
  int id = m_segments.size();
  double inten_loc = local_i;
  double inten_glo = global_i;
  end++;

  std::shared_ptr<Base> ptr = shared_from_this();
  m_segments.push_back(std::make_shared<Segment>(start, end, inten_loc, inten_glo, ptr, id));
  return id;
}

bool Base::delete_segment(int id){
  bool exit_status;

  if(id < m_segments.size()){
    m_segments[id]->delete_seg();
    exit_status = true;
    std::cout<< "Deleting segment with id: " << id << "\n";
  }
  else{
    exit_status = false;
    std::cout<<"error, wrong segment id!?\n";
  }

  return exit_status;
}

bool Base::manipulate_segment(int id, int start, int end, float local_i, float global_i, bool hasMask){
  m_segments[id]->manipulate(start, end, local_i, global_i, hasMask);
  return true;
}

bool Base::manipulate_interpretation(int id, std::string file_path){
  cv::Mat image = cv::imread(file_path);

  if(image.empty()){
    image = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
    std::cout << "image not loaded. Loading empty Image\n";
  }
  else {
    image.convertTo(image, m_img_type );   //do this for the whole video right at the start!?
  }

  if(m_interpretations[id]->getTypenumber() == 8 /*SingleImage*/){
    Singleimage& interpretation = dynamic_cast<Singleimage&>(*m_interpretations[id]);
    interpretation.manipulate(image);
  }

  return true;
}

bool Base::manipulate_interpretation(int id, int offset, int stride){
  if(m_interpretations[id]->getTypenumber() == 0){
    Average& interpretation = dynamic_cast<Average&>(*m_interpretations[id]);
    interpretation.manipulate(offset, stride);
  }
  return true;
}

bool Base::manipulate_interpretation(int id, std::shared_ptr<std::vector<ColorCoords>> colorTimes, int offset, int stride){
  if(m_interpretations[id]->getTypenumber() == 9){
    Paint& interpretation = dynamic_cast<Paint&>(*m_interpretations[id]);
    interpretation.manipulate(colorTimes, offset, stride);
  }
  return true;
}

bool Base::manipulate_interpretation(int id, int ref_id, float threshhold, int modi, int offset, int stride){

  std::string path = " ";

  if(ref_id < 0){
    path = "./ref.png";
  }
  else{
    path = "./out_seg_id" + std::to_string( ref_id ) + ".png";
  }

  cv::Mat ref_img = cv::imread(path);

  if(ref_img.empty()){
    ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
    std::cout << "reference image not loaded. Loading empty Image\n";
  }
  else {
    ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
  }

  if(m_interpretations[id]->getTypenumber() == 2 /*Overplott*/){
    Overplott& interpretation = dynamic_cast<Overplott&>(*m_interpretations[id]);
    interpretation.manipulate(ref_img, threshhold, modi, offset, stride);
  }
  else if(m_interpretations[id]->getTypenumber() == 3 /*Boost*/){
    Boost& interpretation = dynamic_cast<Boost&>(*m_interpretations[id]);
    interpretation.manipulate(ref_img, threshhold, modi, offset, stride);
  }
  else if(m_interpretations[id]->getTypenumber() == 5 /*Reduce*/){
    Reduce& interpretation = dynamic_cast<Reduce&>(*m_interpretations[id]);
    interpretation.manipulate(ref_img, threshhold, modi, offset, stride);
  }

  return true;
}

bool Base::manipulate_interpretation(int id, std::string ref_file_path, float threshhold, int modi, int offset, int stride){

  cv::Mat ref_img = cv::imread(ref_file_path);

  if(ref_img.empty()){
    ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
    std::cout << "reference image not loaded. Loading empty Image\n";
  }
  else {
    ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
  }

  if(m_interpretations[id]->getTypenumber() == 2 /*Overplott*/){
    Overplott& interpretation = dynamic_cast<Overplott&>(*m_interpretations[id]);
    interpretation.manipulate(ref_img, threshhold, modi, offset, stride);
  }
  else if(m_interpretations[id]->getTypenumber() == 3 /*Boost*/){
    Boost& interpretation = dynamic_cast<Boost&>(*m_interpretations[id]);
    interpretation.manipulate(ref_img, threshhold, modi, offset, stride);
  }
  else if(m_interpretations[id]->getTypenumber() == 5 /*Reduce*/){
    Reduce& interpretation = dynamic_cast<Reduce&>(*m_interpretations[id]);
    interpretation.manipulate(ref_img, threshhold, modi, offset, stride);
  }

  return true;
}

bool Base::manipulate_interpretation(int id, float threshhold, int modi, std::shared_ptr<std::vector<float>> values, int offset, int stride){

  if(m_interpretations[id]->getTypenumber() == 1){
    Transferfunction& interpretation = dynamic_cast<Transferfunction&>(*m_interpretations[id]);
    interpretation.manipulate(threshhold, values, offset, stride);
  }
  else if(m_interpretations[id]->getTypenumber() == 4){
    BoostColor& interpretation = dynamic_cast<BoostColor&>(*m_interpretations[id]);
    interpretation.manipulate(threshhold, modi, values, offset, stride);
  }
  return true;
}

bool Base::manipulate_interpretation(int id, int start, int end, int mode, cv::Point mid, float radius, bool fade_direction, float parameter, int offset, int stride){

  if(m_interpretations[id]->getTypenumber() == 6){
    Circularfade& interpretation = dynamic_cast<Circularfade&>(*m_interpretations[id]);
    interpretation.manipulate(start, end, mode, mid, radius, fade_direction, parameter, offset, stride);
  }
  return true;
}

bool Base::manipulate_interpretation(int id, int mode_distance, int mode_function, float parameter, std::shared_ptr<std::vector<cv::Vec4f>> points, int offset, int stride){

  if(m_interpretations[id]->getTypenumber() == 7){
    Timefadepoints& interpretation = dynamic_cast<Timefadepoints&>(*m_interpretations[id]);
    interpretation.manipulate(mode_distance, mode_function, parameter, points, offset, stride);
  }
  return true;
}

int Base::add_interpretation(int typ_i, std::string file_path){
  int id = m_interpretations.size();
  if(typ_i == 8 /*Singleimage*/){
    cv::Mat image = cv::imread(file_path);

    if(image.empty()){
      image = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "image not loaded. Loading empty Image\n";
    }
    else {
      image.convertTo(image, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Singleimage>(shared_from_this(), id, image));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if(typ_i == 0 /*Average*/){
    std::cout<<"Average\n";
    m_interpretations.push_back(std::make_shared<Average>(shared_from_this(), id, offset, stride));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride, std::string ref_file_path, float threshhold, int modi){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if(typ_i == 2 /*Overplott*/){
    std::cout<< "Overplott \n";
    cv::Mat ref_img = cv::imread(ref_file_path);

    if(ref_img.empty()){
      ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else {
      ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Overplott>(shared_from_this(), id, ref_img, threshhold, modi, offset, stride));
  }
  else if(typ_i == 3 /*Boost*/){
    std::cout<< "Boost \n";

    cv::Mat ref_img = cv::imread(ref_file_path);

    if(ref_img.empty()){
      ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else {
      ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Boost>(shared_from_this(), id, ref_img, threshhold, modi, offset, stride));
  }
  else if(typ_i == 5 /*Reduce*/){
    std::cout<< "Reduce \n";

    cv::Mat ref_img = cv::imread(ref_file_path);

    if(ref_img.empty()){
      ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else {
      ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Reduce>(shared_from_this(), id, ref_img, threshhold, modi, offset, stride));
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride, int ref_id, float threshhold, int modi){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if(typ_i == 2 /*Overplott*/){
    std::cout<< "Overplott \n";
    std::string path = " ";
    if(ref_id < 0){
      path = "./ref.png";
    }
    else{
      path = "./out_seg_id" + std::to_string( ref_id ) + ".png";
    }
    cv::Mat ref_img = cv::imread(path);

    if(ref_img.empty()){
      ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else {
      ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Overplott>(shared_from_this(), id, ref_img, threshhold, modi, offset, stride));
  }
  else if(typ_i == 3 /*Boost*/){
    std::cout<< "Boost \n";

    std::string path = " ";
    if(ref_id < 0){
      path = "./ref.png";
    }
    else{
      path = "./out_seg_id" + std::to_string( ref_id ) + ".png";
    }
    cv::Mat ref_img = cv::imread(path);

    if(ref_img.empty()){
      ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else {
      ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Boost>(shared_from_this(), id, ref_img, threshhold, modi, offset, stride));
  }
  else if(typ_i == 5 /*Reduce*/){
    std::cout<< "Reduce \n";

    std::string path = " ";
    if(ref_id < 0){
      path = "./ref.png";
    }
    else{
      path = "./out_seg_id" + std::to_string( ref_id ) + ".png";
    }
    cv::Mat ref_img = cv::imread(path);

    if(ref_img.empty()){
      ref_img = cv::Mat(m_pnt_max.y, m_pnt_max.x, m_img_type, cv::Scalar(0,0,0));
      std::cout << "reference image not loaded. Loading empty Image\n";
    }
    else {
      ref_img.convertTo( ref_img, m_img_type );   //do this for the whole video right at the start!?
    }

    m_interpretations.push_back(std::make_shared<Reduce>(shared_from_this(), id, ref_img, threshhold, modi, offset, stride));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride, std::shared_ptr<std::vector<ColorCoords>> colorTimes){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if(typ_i == 9 /*Paint*/){
    std::cout<< "Paint \n";
    m_interpretations.push_back(std::make_shared<Paint>(shared_from_this(), id, colorTimes, offset, stride));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride, float threshhold, int modi, std::shared_ptr<std::vector<float>> values){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if (typ_i == 1 ){
    std::cout<<"Transferfunction \n";
    m_interpretations.push_back(std::make_shared<Transferfunction>(shared_from_this(), id, threshhold, values, offset, stride));
  }
  else if(typ_i == 4 && values->size()%3 == 0){
    std::cout << "BoostColor \n";
    m_interpretations.push_back(std::make_shared<BoostColor>(shared_from_this(), id, threshhold, modi, values, offset, stride));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride, int start, int end, int mode, cv::Point mid, float radius, bool fade_direction, float parameter){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if (typ_i == 6 ){
    std::cout<<"CircularFade \n";

    if(start > end){
      std::cout << "\t ! WARNING: start > end; swaping end and start." << std::endl;
      int tmp = end;
      end = start;
      start = tmp;
    }

    m_interpretations.push_back(std::make_shared<Circularfade>(shared_from_this(), id, start, end, mode, mid, radius, fade_direction, parameter, offset, stride));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

int Base::add_interpretation(int typ_i, int offset, int stride, int mode_distance, int mode_function, float param, std::shared_ptr<std::vector<cv::Vec4f>> points){
  std::cout<<"\t > interpretation: ";
  int id = m_interpretations.size();

  if (typ_i == 7){
    std::cout<<"TimeFadePoints \n";
    m_interpretations.push_back(std::make_shared<Timefadepoints>(shared_from_this(), id, mode_distance, mode_function, param, points, offset, stride));
  }
  else{
    id = -1;
    std::cout<< "Wrong interpretation. \n";
  }

  return id;
}

bool Base::connect(int id_segment, int id_interpretation){
  bool correct = false;
  if((id_segment <= m_segments.size() - 1) &&
     (id_interpretation <= m_interpretations.size() - 1)) {

    m_segments[id_segment]->set_interpretation(m_interpretations[id_interpretation]);
    correct = true;
  }

  return correct;
}

bool Base::save(std::string file){
  auto start_time = std::chrono::high_resolution_clock::now();

  cv::Mat out = get_result();
  bool exit_status = false;

  if(cv::imwrite(file, out)){
    exit_status = true;
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  #ifdef show_time
      std::cout << "\t\t * Base_save_to_file: \t" << duration << std::endl;
  #endif
  return exit_status;
}

void Base::add_to_values_abs(cv::Mat new_values){
  m_values_abs+=new_values;
}

void Base::add_to_values_fac(cv::Mat new_values){
  m_values_fac += new_values;
}

void Base::add_to_uni_fac(float new_value){
  m_uni_fac += new_value;
}

bool Base::work_to_do() {
  m_mutex_update.lock();
  bool out = m_in_calculation;
  m_mutex_update.unlock();

  return out;
}

void Base::continue_work() {
  auto start_time = std::chrono::high_resolution_clock::now();

  int work_size = m_work_size;
  m_mutex_result.lock();

  m_seg_in_calc.erase(std::remove_if(m_seg_in_calc.begin(), m_seg_in_calc.end(), [&work_size](Segment*& o) { return o->work(work_size); }),m_seg_in_calc.end());

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  #ifdef show_time
      std::cout << "\t\t * Base_work_que_time: \t" << duration << std::endl;
  #endif
  update_result();


  if(!m_seg_in_calc.size()) {
    m_in_calculation=false;
  }

  m_mutex_result.unlock();

  this->save("state_tmp.png");

  }
/*
we might set the following functions protected?
*/
void Base::add_work(Segment* new_seg){
  //it is intended that a segments uses this functions
  //to tell the base, that it has work to do...
  m_mutex_result.lock();
  m_seg_in_calc.push_back(new_seg);
  m_mutex_result.unlock();

  m_mutex_update.lock();
  m_in_calculation=true;
  m_mutex_update.unlock();
}

void Base::update_result(){
  auto start_time = std::chrono::high_resolution_clock::now();

  m_result = m_values_abs / (m_values_fac + cv::Scalar(m_uni_fac, m_uni_fac, m_uni_fac));

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count();
  #ifdef show_time
      std::cout << "\t\t * Base_update_time: \t" << duration << std::endl;
  #endif
}

#ifndef true //getter
  float Base::get_segment_progress(int id) {
    return m_segments[id]->get_progress();
  }

  cv::Mat Base::get_result() {
    m_mutex_result.lock();
    cv::Mat out=m_result.clone();
    m_mutex_result.unlock();

    return out; //copy contructor?
  }

  bool Base::get_result(cv::Mat& out) {
    m_mutex_result.lock();
    if(m_new_output)
    {
      out=m_result.clone();
      m_new_output=false;
      m_mutex_result.unlock();
      return true;
    }else{
      m_mutex_result.unlock();
      return false;

    }
  }

  int Base::get_typ_i(int id){
    int i_id = -1;
    if(m_interpretations.size() < id || id < 0) {
      std::cout << "\t > interpretation id doesn't exist. \n";
    }
    else{
      i_id = m_interpretations[id]->getTypenumber();
    }
    return i_id;
  }

  cv::Mat const& Base::get_frame(int i) {
    m_video->set(CV_CAP_PROP_POS_FRAMES,i);
    cv::Mat output;
    m_video->read(output);
    return output;//!watchout, output is not a special mat typ!
  }

  int Base::get_img_type() {
    return m_img_type;
  }

  int Base::get_width() {
    return  m_pnt_max.x;
  }

  int Base::get_height() {
    return m_pnt_max.y;
  }

  std::string Base::get_videopath() {
    return m_file;
  }

  std::shared_ptr<cv::VideoCapture> Base::get_videocap() {
    return m_video;
  }

  cv::Point Base::get_max_Point(){
    return m_pnt_max;
  }

  cv::Point Base::get_min_Point(){
    return m_pnt_min;
  }

  int Base::get_start_frame(){
    return m_frame_start;
  }

  int Base::get_last_frame(){
    return m_frame_last;
  }

  float Base::get_intensity(){
    return m_intensity;
  }

  int Base::get_img_delta() {
    return m_img_delta;
  }

  int Base::get_seg_start(int id){
    return m_segments[id]->get_start();
  }

  int Base::get_seg_end(int id){
    return m_segments[id]->get_end();
  }

  void Base::set_work_size(int i) {
    m_work_size = i;
  }
#endif

/*  some getter...
std::string const& Base::get_file() const{
  return m_file;
}

int const& Base::get_img_type() const{
  return m_img_type;
}

Point const& Base::get_pnt_min() const{
  return m_pnt_min;
}

Point const& Base::get_pnt_max() const{
  return m_pnt_max;
}

int const Base::get_frame_start() const{
    return m_frame_start;
}

int const Base::get_frame_last() const{
  return m_frame_last;
}

float const Base::get_intensity() const{
  return m_intensity;
}

Mat& Base::get_values_abs(){
    return m_values_abs;
}

Mat& Base::get_values_fac(){
  return m_values_fac;
}

double& Base::get_uni_fac(){
  return m_uni_fac;
}



void                Base::add_segment(int start, int end){
  double inten_loc=1.0;
  double inten_glo=1.0;
  m_segments.push_back(Segment(m_file,m_img_type,m_pnt_min,m_pnt_max, start, end,inten_loc, inten_glo,m_values_abs,m_values_fac,m_uni_fac));
  m_seg_in_calc.push_back(&m_segments.back());
  m_segments.back().in_work=true;
  m_in_calculation=true;
}
void                Base::move_segment(int id, int delta){
  //m_segments.push_back(Segment(m_file,m_img_type,m_pnt_min,m_pnt_max, start, end,inten_loc, inten_glo,m_values_abs,m_values_fac,m_uni_fac));
  //set soll wert
  m_segments[id].move_in_time(delta);
  m_seg_in_calc.push_back(&m_segments[id]);
  m_segments[id].in_work=true;
  m_in_calculation=true;
}

void                Base::set_exposure(int id, int imgs){
  //set soll wert von für end
  m_segments[id].set_exposure(imgs);
  m_seg_in_calc.push_back(&m_segments[id]);
  m_segments[id].in_work=true;
  m_in_calculation=true;
}

void                Base::set_startframe(int id, int frame){
  m_segments[id].set_startframe(frame);
  m_seg_in_calc.push_back(&m_segments[id]);
  m_segments[id].in_work=true;
  m_in_calculation=true;
}

void                Base::set_endframe(int id, int frame){
  m_segments[id].set_endframe(frame);
  m_seg_in_calc.push_back(&m_segments[id]);
  m_segments[id].in_work=true;
  m_in_calculation=true;
}

void                Base::set_exposure_delta(int id, int delta){
  m_segments[id].set_exposure_delta(delta);
  m_seg_in_calc.push_back(&m_segments[id]);
  m_segments[id].in_work=true;
  m_in_calculation=true;
}

void                Base::delete_segment(){  //id
  //delete a segment:
  //-> m_segments
  //-> m_in_calc_seg
  //free mem!!
}

void                Base::run(){             //routine
  bool lucas_is_clever=false;
  int sleep_time=100;

  while(!lucas_is_clever)
  {
    //any changes?
    if(!m_in_calculation)
    {//no calculation available
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }else{
      //calculation
      //forloop with that deletes elements if calc done
      //https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
      //rechange to vec!

    }
  }
}
*/
