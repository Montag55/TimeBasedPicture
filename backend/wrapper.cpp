#include <node.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include "dirent.h"

#include "base.hpp"
#include "segment.hpp"
#include "interpretation.hpp"
#include "average.hpp"
#include "transferfunction.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/cvconfig.h>

bool log0 = false;  //To log more information about opencv

std::shared_ptr<Base> base;

/**
* init: expects a video(/imagefolder)-path[string]
* and inits the TBP-SYS
* returns: true (base_properties?: frame_number,...)
* false sth wrong with videofile
*/
void init(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value param1( args[0]->ToString() );
  std::string video_path = std::string( *param1 );
  auto msg = v8::String::NewFromUtf8( isolate , "true" );

  if( log0 ) {
    std::cout << "Using OpenCV version " << CV_VERSION << "\n";
    std::cout << cv::getBuildInformation();
    std::cout << "Video_path: " << video_path << "\n";
  }

  // might change image to video
  cv::VideoCapture vid = cv::VideoCapture(video_path, cv::CAP_IMAGES /*cv::CAP_GSTREAMER*/ /*CAP_FFMPEG*/);

  if( !vid.isOpened()) {
    msg = v8::String::NewFromUtf8( isolate , "0 0 0 0" ); //this shows failure
    std::cout << "!!! Failed to open video." << std::endl;
    std::cout<<"TBS-initialisation: failed-wrong videopath?\n";
  }
  else{
    base =  std::make_shared<Base>(video_path);
    std::string output = "1 " + std::to_string(vid.get(CV_CAP_PROP_FRAME_COUNT)) + " " +
                         std::to_string(vid.get(CV_CAP_PROP_FRAME_WIDTH)) + " " +
                         std::to_string(vid.get(CV_CAP_PROP_FRAME_HEIGHT));
    msg = v8::String::NewFromUtf8( isolate , output.c_str());
    std::cout<< "TBS-initialisation: success \n";
    vid.release();
  }

  args.GetReturnValue().Set(msg);
}

/**
* add_segment: expects a segment definition(start[int],end[int],local[float],global[float])
* and Initializes it
* returns: true and seg-id[int]
*/
void add_segment(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //for all properties check if values fit!?
  int start =       args[0]->IntegerValue();
  int end =         args[1]->IntegerValue();
  float local_i =   args[2]->NumberValue();
  float global_i =  args[3]->NumberValue();

  int id = base->add_segment(start, end, local_i, global_i);
  if(id < 0 ){
    std::cout << "add_segment() failed. \n";
  }

  auto msg = v8::Number::New(isolate , id);
  args.GetReturnValue().Set(msg);
}

/**
* delete_segment: expects an existing segment id[int])
* returns: true
*/
void delete_segment(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  int id = args[0]->IntegerValue();
  std::string correct = "true";
  if(!base->delete_segment(id)){
    correct = "false";
  }
  else{
    std::cout << "delete_segment() failed. \n";
  }

  auto msg = v8::String::NewFromUtf8( isolate , correct.c_str() );
  args.GetReturnValue().Set(msg);
}

/**
* manipulate_segment: expects segm. id[int] and all properties
* (start[int],end[int],local[float],global[float])
* and start calculation
* returns: boolean (but not used yet)
*/
void manipulate_segment(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //check if values fit!?
  int id =          args[0]->IntegerValue();
  int start =       args[1]->IntegerValue();
  int end =         args[2]->IntegerValue();
  float local_i =   args[3]->NumberValue();
  float global_i =  args[4]->NumberValue();
  std::string correct = "true";

  if(!base->manipulate_segment(id, start, end, local_i, global_i)){
    correct = "false";
    std::cout << "manipulate_segment() failed. \n";
  }

  auto msg = v8::String::NewFromUtf8( isolate , correct.c_str() );
  args.GetReturnValue().Set(msg);
}

 /**
 ype 0: in  (int type)
 * Type 1: in  (int id, int start, float weight1, float weight2, ...)
 * Type 2: in  (int id)
 * Type 3: in  (int id, int ref_id, float threshhold)
 *             (int id, string file_path, float threshhold)
 * Type 4: in  (int id, float threshhold, float color_R, float color_G, float color_B, ... )
 * Type 5: in  (int id, int ref_id, float threshhold)
 *             (int id, string file_path, float threshhold)
 */
void manipulate_interpretation(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();
  int id = args[0]->IntegerValue();
  int typ_i = base->get_typ_i(id);
  std::string correct = "true";

  if(typ_i == 0){
    std::cout<<"\t > you fool. The Average-interpretation needs no manipulation. \n";
  }
  else if(typ_i == 1){
    std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>();
    int start = args[1]->IntegerValue();

    for(int idx = 2; idx < args.Length(); idx++){
      weights->push_back(args[idx]->NumberValue());
    }

    if(!base->manipulate_interpretation(id, start, weights)){
      correct = "false";
      std::cout << "manipulate_segment() failed. \n";
    }
  }
  else if(typ_i == 3 || typ_i == 5){
    if(args[1].IsNumber()){
      int ref_id = args[1]->IntegerValue();
      float threshhold = args[2]->NumberValue();

      if(!base->manipulate_interpretation(id, ref_id, threshhold)){
        correct = "false";
        std::cout << "manipulate_segment() failed. \n";
      }
    }
    else{
      v8::String::Utf8Value param1( args[1]->ToString() );
      std::string ref_file_path = std::string( *param1 );
      float threshhold = args[2]->NumberValue();

      if(!base->manipulate_interpretation(id, ref_file_path, threshhold)){
        correct = "false";
        std::cout << "manipulate_segment() failed. \n";
      }
    }
  }
  else if(typ_i == 4){
    std::shared_ptr<std::vector<float>> colors = std::make_shared<std::vector<float>>();
    float threshhold = args[1]->NumberValue();

    for(int idx = 2; idx < args.Length(); idx++){
      colors->push_back(args[idx]->NumberValue());
    }

    if(!base->manipulate_interpretation(id, threshhold, colors)){
      correct = "false";
      std::cout << "manipulate_segment() failed. \n";
    }
  }
  else{
    std::cout<<"not yet implemented manipulation\n";
  }

  auto msg = v8::String::NewFromUtf8( isolate , correct.c_str() );
  args.GetReturnValue().Set(msg);

}

/**
* get_segment_progress: expects the segment id
* returns: current progress [float] between 0.0 and 100.0
*/
void get_segment_progress(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //check if values fit!?
  int id = args[0]->IntegerValue();
  float progress = base->get_segment_progress(id);
  std::string output = std::to_string(progress);

  if(progress < 0 ){
    std::cout << "progress: "<<progress<<" \n";
    std::cout << "get_segment_progress() failed. \n";
  }

  auto msg = v8::String::NewFromUtf8( isolate , output.c_str());
  args.GetReturnValue().Set(msg);
}

/**
* add_interpretation: expects a full interpretation definition(type dependet)
* returns: interpretation id
* Type 0: in  (int type)
* Type 1: in  (int type, int start, float weight1, float weight2, ...)
* Type 2: in  (int type)
* Type 3: in  (int type, int ref_id, float threshhold)
*             (int type, string file_path, float threshhold)
* Type 4: in  (int type, float threshhold, float color_R, float color_G, float color_B, ... )
* Type 5: in  (int type, int ref_id, float threshhold)
*             (int type, string file_path, float threshhold)
*/
void add_interpretation(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //check if values fit!?
  int interpret_id = -1;
  int typ_i = args[0]->IntegerValue();

  if(typ_i == 0 || typ_i == 2){
    interpret_id = base->add_interpretation(typ_i);
  }
  else if(typ_i == 1){
    std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>();
    float start  = args[1]->NumberValue();

    for(int idx = 2; idx < args.Length(); idx++){
      weights->push_back(args[idx]->NumberValue());
    }

    interpret_id = base->add_interpretation(typ_i, start, weights);
  }
  else if(typ_i == 3){
    int ref_id        = args[1]->IntegerValue();
    float threshhold  = args[2]->NumberValue();
    interpret_id = base->add_interpretation(typ_i, ref_id, threshhold);
  }
  else if(typ_i == 4){
    std::shared_ptr<std::vector<float>> colors = std::make_shared<std::vector<float>>();
    float threshhold  = args[1]->NumberValue();

    for(int idx = 2; idx < args.Length(); idx++){
      colors->push_back(args[idx]->NumberValue());
    }

    if(colors->size()%3 == 0)
      interpret_id = base->add_interpretation(typ_i, threshhold, colors);
    else
      std::cout << "\t too few arguments (tpye, float, [r, g, b]*n) \n";
  }
  else if(typ_i == 5){
    int ref_id        = args[1]->IntegerValue();
    float threshhold  = args[2]->NumberValue();
    interpret_id = base->add_interpretation(typ_i, ref_id, threshhold);
  }

  if(interpret_id >= 0 ) {
    std::cout<<"\n\t > intpretation id: "<< interpret_id << "\n";
  }
  else {
    std::cout << "add_interpretation() failed. \n";
  }

  auto msg = v8::Number::New(isolate, interpret_id);
  args.GetReturnValue().Set(msg);
}

/**
* conect: expects a segment's id first and an interpretaton's id second
* connects the two for computation
* one segment can have only one interpretations
* one interpretation can have mutliple segments
* returns: true or false
*/
void connect(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //check if values fit!?
  int id_segment        = args[0]->IntegerValue();
  int id_interpretation = args[1]->IntegerValue();
  bool correct = base->connect(id_segment, id_interpretation);

  if(!correct){
    std::cout << "connect() failed. \n";
  }

  auto msg = v8::Number::New(isolate , correct);
  args.GetReturnValue().Set(msg);
}

void Initialize(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "add_segment", add_segment);
  NODE_SET_METHOD(exports, "delete_segment", delete_segment);
  NODE_SET_METHOD(exports, "manipulate_segment", manipulate_segment);
  NODE_SET_METHOD(exports, "manipulate_interpretation", manipulate_interpretation);
  NODE_SET_METHOD(exports, "get_segment_progress", get_segment_progress);
  NODE_SET_METHOD(exports, "add_interpretation", add_interpretation);
  NODE_SET_METHOD(exports, "connect", connect);
  //Todo: NODE_SET_METHOD(exports, "manipulate_interpretation", manipulate_interpretation);
}

NODE_MODULE(wrapper, Initialize)
