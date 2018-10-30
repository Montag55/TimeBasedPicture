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

std::vector<std::shared_ptr<Segment>>         segments;
std::vector<std::shared_ptr<Interpretation>>  interpretations;
std::shared_ptr<Base>                         base;

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
  String message;

  if( log0 ) {
    std::cout <<"Using OpenCV version " << CV_VERSION << "\n" << std::endl;
    std::cout << getBuildInformation();
    std::cout<<"Video_path: "<<video_path<<"\n";
  }

  // might change image to video
  cv::VideoCapture vid=cv::VideoCapture(video_path, cv::CAP_IMAGES /*cv::CAP_GSTREAMER*/ /*CAP_FFMPEG*/);

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

  //SEGMENT PROPERTIES:
  int id;
  int end;
  int start;
  float local_i;
  float global_i;

  //check if values fit!?
  start =     args[0]->IntegerValue();
  end =       args[1]->IntegerValue();
  local_i =   args[2]->NumberValue();
  global_i =  args[3]->NumberValue();

  int exposure = end - start + 1;
  id = segments.size();
  segments.push_back(base->add_segment(start, end, local_i, global_i, id));

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
  
  if(id < segments.size()){
    segments[id]->delete_seg();
    std::cout<< "Deleting segment with id: " << id << "\n";
  }
  else{
    std::cout<<"error, wrong segment id!?\n";
  }

  auto msg = v8::String::NewFromUtf8( isolate , "true" );
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

  //SEGMENT PROPERTIES:
  int start;
  int end;
  float local_i;
  float global_i;
  int id;

  //check if values fit!?
  id =        args[0]->IntegerValue();
  start =     args[1]->IntegerValue();
  end =       args[2]->IntegerValue();
  local_i =   args[3]->NumberValue();
  global_i =  args[4]->NumberValue();
  segments[id]->manipulate(start,end,local_i,global_i);

  auto msg = v8::String::NewFromUtf8( isolate , "true" );
  args.GetReturnValue().Set(msg);
}

/**
* get_segment_progress: expects the segment id
* returns: current progress [float] between 0.0 and 100.0
*/
void get_segment_progress(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //SEGMENT PROPERTIES:
  int id;

  //check if values fit!?
  id = args[0]->IntegerValue();
  float progress= segments[id]->get_progress();
  std::string output =std::to_string(progress);

  auto msg = v8::String::NewFromUtf8( isolate , output.c_str());
  args.GetReturnValue().Set(msg);
}

/**
* add_interpretation: expects a full interpretation definition(type dependet)
* returns: interpretation id
*/
void add_interpretation(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();

  //SEGMENT PROPERTIES:
  int typ_i;

  //check if values fit!?
  typ_i = args[0]->IntegerValue();
  bool correct = true;
  int interpret_id = interpretations.size();
  std::cout<<"addinterpretation: ";

  if(typ_i == 0 /*averaging*/){
    std::cout<<"Average\n";
    interpretations.push_back(std::make_shared<Average>(base, interpret_id));
  }
  else if (typ_i == 1 /*transferfunktion*/){
    std::cout<<"Transferfunction (not implemnted) \n";
    correct = false;
    /*
    args.size() ? alternativ einen vector? oder einen parameter, der length bestimmt
    int start  = args[1]->IntegerValue();
    int length =args.size();
    std:shared_ptr<std::vector<float>> weights= std::make_shared<std::vector<float>>();
    for (int i=2; i<length; i++){
      std::istringstream iss(v[i]);
      weights->push_back(((float)args[i]->NumberValue()));
      }
    }
    interpretations.push_back(std::make_shared<Transferfunction>(base,interpret_id, start, weights));
    */
  }
  else if (typ_i == 2 /*overplott*/){
    correct = false;
    std::cout<<"Overplott (not implemnted) \n";
  }
  else if (typ_i == 3 /*boost*/){
    correct = false;
    std::cout<<"Boost not (not implemnted)\n";
  }
  else{
    correct = false;
    std::cout<<"wrong typ?\n";
  }

  if(correct) {
    std::cout<<"\n\t > intpretation id: "<< interpret_id << "\n";
  }
  else {
    interpret_id =-42;
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

  //SEGMENT PROPERTIES:
  int id_segment;
  int id_interpretation;
  bool success;

  //check if values fit!?
  id_segment        =     args[0]->IntegerValue();
  id_interpretation =     args[1]->IntegerValue();

  if((id_segment <= segments.size() - 1) && (id_interpretation <= interpretations.size() - 1)) {
    segments[id_segment]->set_interpretation(interpretations[id_interpretation]);
    std::cout<< "Segment " << id_segment << " is interpreted with interpretation " << id_interpretation << "\n";
    success = true;
  }
  else{
    std::cout<<"wrong id's?\n";
    success=false;
  }

  auto msg = v8::Number::New(isolate ,success);
  args.GetReturnValue().Set(msg);
}

void Initialize(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "add_segment", add_segment);
  NODE_SET_METHOD(exports, "delete_segment", delete_segment);
  NODE_SET_METHOD(exports, "manipulate_segment", manipulate_segment);
  NODE_SET_METHOD(exports, "get_segment_progress", get_segment_progress);
  NODE_SET_METHOD(exports, "add_interpretation", add_interpretation);
  NODE_SET_METHOD(exports, "connect", connect);
  //Todo: NODE_SET_METHOD(exports, "manipulate_interpretation", manipulate_interpretation);
}

NODE_MODULE(wrapper, Initialize)
