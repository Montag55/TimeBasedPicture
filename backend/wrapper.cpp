#include <node.h>
//#include <string.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>

#include "base.hpp"
#include "segment.hpp"
#include "interpretation.hpp"
#include "average.hpp"
#include "transferfunction.hpp"

#include "dirent.h"
#include <opencv2/opencv.hpp>
#include <opencv2/cvconfig.h>
using namespace cv;
typedef Point3_<uint8_t> Pixel;


//////////////////////////////////
bool log0=false;
std::vector<std::shared_ptr<Segment>>   segments;
std::vector<std::shared_ptr<Interpretation>> interpretations;
std::shared_ptr<Base> base;

std::vector<int> picture(1920 * 1080 * 4);

void init(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  init: expects a video(/imagefolder)-path[string]
        and inits the TBP-SYS
  returns:
      true (base_properties?: frame_number,...)
      false sth wrong with videofile
  */
  v8::Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value param1( args[0]->ToString() );
  std::string video_path= std::string( *param1 );
  auto msg = v8::String::NewFromUtf8( isolate , "true" );
  String message;

  if( log0 )
  {
    std::cout <<"Using OpenCV version " << CV_VERSION << "\n" << std::endl;
    std::cout << getBuildInformation();
    std::cout<<"Video_path: "<<video_path<<"\n";
  }
  cv::VideoCapture vid=cv::VideoCapture(video_path, cv::CAP_IMAGES /*cv::CAP_GSTREAMER*/ /*CAP_FFMPEG*/);

  if( !vid.isOpened() ) //just testing videopath
  {

    msg = v8::String::NewFromUtf8( isolate , "0 0 0 0" ); //this shows failure

    std::cout << "!!! Failed to open video." << std::endl;
    std::cout<<"TBS-initialisation: failed-wrong videopath?\n";

    //vid=cv::VideoCapture(video_path, cv::CAP_IMAGES /*cv::CAP_GSTREAMER*CAP_FFMPEG*/);

  }else{

    base =  std::make_shared<Base>(video_path);
    if( log0 )
    {
      std::cout << "Video seems to work." << std::endl;
    }
    std::string output ="1 "+std::to_string(vid.get(CV_CAP_PROP_FRAME_COUNT))+" "+std::to_string(vid.get(CV_CAP_PROP_FRAME_WIDTH))+" "+std::to_string(vid.get(CV_CAP_PROP_FRAME_HEIGHT));
    msg = v8::String::NewFromUtf8( isolate , output.c_str());
    std::cout<<"TBS-initialisation: success\n";
    vid.release();
  }
  //status
  //"0"    pathwron
  //"1"   framecount res.x res.y
  args.GetReturnValue().Set(msg);
}

void add_segment(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  add_segment: expects a segment definition(start[int],end[int],local[float],global[float])
              and Initializes it
  returns:
      true and seg-id[int]
  */
  v8::Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value param1( args[0]->ToString() );

  //SEGMENT PROPERTIES:
  int start;
  int end;
  float local_i;
  float global_i;
  int id;

  //check if values fit!?
  start =     args[0]->IntegerValue();
  end =       args[1]->IntegerValue();
  local_i=    args[2]->NumberValue();
  global_i =  args[3]->NumberValue();


  int exposure = end-start + 1;
  id=segments.size();
  segments.push_back(base->add_segment(start, end, local_i, global_i, id));

  std::cout<<"adding segment, start:"<<start<<" ,end:"<<end<<", local_i:"<<local_i<<", global_i:"<<global_i<<", id:"<<id<<"\n";

  auto msg = v8::Number::New(isolate , id);
  args.GetReturnValue().Set(msg);
}

void delete_segment(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  delete_segment: expects an existing segment id[int])
  returns:
      true
  */
  v8::Isolate* isolate = args.GetIsolate();
  int id=args[0]->IntegerValue();
  if(id<segments.size()){
    segments[id]->delete_seg();
    std::cout<<"delete segment: "<<id<<"\n";
  }else{
    std::cout<<"error, wrong segment id!?\n";
  }
  auto msg = v8::String::NewFromUtf8( isolate , "true" );
  args.GetReturnValue().Set(msg);
}

void manipulate_segment(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  manipulate_segment: expects segm. id[int] and all properties
                      (start[int],end[int],local[float],global[float])
                     and start calculation
  returns:
      nothing yet
  */
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
  local_i=    args[3]->NumberValue(); //float works?
  global_i =  args[4]->NumberValue();
  segments[id]->manipulate(start,end,local_i,global_i);
  std::cout<<"manipulate segment, start:"<<start<<" ,end:"<<end<<", local_i:"<<local_i<<", global_i:"<<global_i<<", id:"<<id<<"\n";

  auto msg = v8::String::NewFromUtf8( isolate , "true" );
  args.GetReturnValue().Set(msg);
}

void base_get_result_image(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  base_resulting_image: expects nothing

  returns:
      current image result_image of calculation
  */
  v8::Isolate* isolate = args.GetIsolate();
  Mat output;
  auto start_time=std::chrono::high_resolution_clock::now();
  if(base->get_result(output))
  {
    std::cout<<"new img availabe!\n";
    int i=0;
    for (int r = 0; r < output.rows; r++)
    {
      // Loop over all columns
      for ( int c = 0; c < output.cols; c++)
      {
        // Obtain pixel at (r, c)
        //for (int j=0; j<3; j++)
        //{collor correction for nodejs
          picture[i]=((int)(output.at<Vec3f>(r, c)[2]))%255;
          i++;
          picture[i]=((int)(output.at<Vec3f>(r, c)[1]))%255;
          i++;
          picture[i]=((int)(output.at<Vec3f>(r, c)[0]))%255;
          i++;
        //}
        picture[i]=255;
        i++;
      }
    }
  }else{
    std::cout<<"no new img available\n";
  }
  //imwrite( "./current_state.jpg", output);
  //super langsame konvertierung.. muss erstmal reichen
  // alternative: http://answers.opencv.org/question/145214/convert-cvmat-to-stdvector-without-copying/



  //if (output.isContinuous())
//    picture_2.assign((float*)output.datastart, (float*)output.dataend);
    /*
  for(unsigned int i=0; i < picture_2.size(); i++){
    std::cout << " vector_p2: " << i << ": " << picture_2[i] << std::endl;
  }
  */

  auto arraybuffer = v8::ArrayBuffer::New(isolate, picture.size() * sizeof(int));
  memcpy(arraybuffer->GetContents().Data(), picture.data(), picture.size() * sizeof(int));
  auto buffer = v8::Int32Array::New(arraybuffer, 0, picture.size() );
  args.GetReturnValue().Set(buffer);
}


void get_segment_progress(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  get_segment_progress: expects the segment id

  returns:
      current progress [float] between 0.0 and 100.0
  */
  v8::Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value param1( args[0]->ToString());

  //SEGMENT PROPERTIES:
  int id;

  //check if values fit!?
  id =     args[0]->IntegerValue();
  float progress= segments[id]->get_progress();
  //std::cout<<"segment "<<id<<" progress: "<<progress<<"%\n";

  std::string output =std::to_string(progress);
  auto msg = v8::String::NewFromUtf8( isolate , output.c_str());
  args.GetReturnValue().Set(msg);
}

void add_interpretation(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  add_interpretation: expects a full interpretation definition(type dependet)

  returns:
      interpretation id
  */

  v8::Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value param1( args[0]->ToString() );

  //SEGMENT PROPERTIES:
  int typ;

  //check if values fit!?
  typ =     args[0]->IntegerValue();

  std::cout<<"addinterpretation\n";
  bool correct= true;
  int interpret_id= interpretations.size();
  if(typ==0)
  {
    std::cout<<"averaging\n";
    interpretations.push_back(std::make_shared<Average>(base,interpret_id));
  }else if (typ==1){
    std::cout<<"transferfunction, not ready yet\n";
    correct=false;
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
  }else if (typ==2){
    correct=false;
    std::cout<<"overpaint noty yet implemented\n";
  }else if (typ==3){
    correct=false;
    std::cout<<"boost not yet implemnted\n";
  }else{
    correct=false;
    std::cout<<"wrong typ?\n";
  }

  if(correct)
  {
    std::cout<<"interetation id:"<<interpret_id<<"\n";
  }else{
    interpret_id=-42;
  }

  auto msg = v8::Number::New(isolate ,interpret_id);
  args.GetReturnValue().Set(msg);
}

void connect(const v8::FunctionCallbackInfo<v8::Value>& args){
  /*
  conect: expects a segment's id first and an interpretaton's id second

  connects the two for computation
  one segment can have only one interpretations
  one interpretation can have mutliple segments

  returns:
      true or false
  */

  v8::Isolate* isolate = args.GetIsolate();
  v8::String::Utf8Value param1( args[0]->ToString() );

  //SEGMENT PROPERTIES:
  int id_segment ;
  int id_interpretation;
  bool success;
  //check if values fit!?
  id_segment =     args[0]->IntegerValue();
  id_interpretation =     args[1]->IntegerValue();

  if(id_segment<=segments.size()-1 && id_interpretation <= interpretations.size()-1)
  {
    // set calculation to zero, if interpretation is new
    segments[id_segment]->set_interpretation(interpretations[id_interpretation]);
    std::cout<<"Segment "<<id_segment<<" is interpreted with interpretation "<<id_interpretation<<"\n";
    success=true;
  }else{
    std::cout<<"wrong id's?\n";
    success=false;
  }

  auto msg = v8::Number::New(isolate ,success);
  args.GetReturnValue().Set(msg);
}


void Initialize(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "add_segment", add_segment);
  NODE_SET_METHOD(exports, "get_segment_progress", get_segment_progress);
  NODE_SET_METHOD(exports, "delete_segment", delete_segment);
  NODE_SET_METHOD(exports, "manipulate_segment", manipulate_segment);
  NODE_SET_METHOD(exports, "add_interpretation", add_interpretation);
  NODE_SET_METHOD(exports, "connect", connect);
  //NODE_SET_METHOD(exports, "manipulate_interpretation", manipulate_interpretation);
  NODE_SET_METHOD(exports, "base_get_result_image", base_get_result_image);
  /*
  -events:
    -update current segment_calculation progress (soll- & ist-stand)
  */
}

NODE_MODULE(wrapper, Initialize)
