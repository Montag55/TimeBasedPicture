/*
a simple terminal interface:
-input video via parameter
##
addsegment       [int]startframe [int] exposure [float] local_i [float] global_i //makes a new segment

% delete segment [int]id
startframe      [int]id   [int]frame
%endframe       [int]id   [int]frame
move            [int]id   [int]delta
exposure        [int]id   [int]imgs
exposure_delta  [int]id   [int]delta
##
-always copmpute full segment
-erstmal nur avg
*/

#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "base.hpp"
#include "interpretation.hpp"
#include "average.hpp"
#include "transferfunction.hpp"
using namespace cv;
using namespace std;

/*
seems to not allow multiple events like this.. so no long calculations pls!
*/
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch){
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos +1 ) );

    return strs.size();
}

int main (int    argc, char **argv){

  std::vector<std::shared_ptr<Segment>>         segments;
  std::vector<std::shared_ptr<Interpretation>>  interpretations;

  if((argc==2)||((argc==1)))   //file parameter
  {
      std::string file;
    if(argc==1)
    {
      std::cout<<"no path specified: default=./../../../videos/street/frame%01d.jpg\n";
      file="./../../../videos/street/frame%01d.jpg";
    }else{
      file=argv[1];
    }
    Mat image;

    std::shared_ptr<Base> base = std::make_shared<Base>(file);
    bool fin=false;
    std::cout<<"expecting a segment definition:\n";
    int i=0;
    while(!fin)
    {
        string command;
        std::cout<<"expecting input\n";
        getline (cin,command);
        std::vector<std::string> v;
        split( command, v, ' ' );
        if(v[0]=="addsegment")
        {
          std::cout<<"addsegment\n";
          int startframe  =std::stoi (v[1]);
          int endframe   =std::stoi (v[2]);
          int local_i   =std::stof (v[3]);
          int global_i   =std::stof (v[4]);
          int seg_id=segments.size();
          segments.push_back(base->add_segment(startframe, endframe,local_i, global_i,seg_id));
        }if(v[0]=="addinterpretation")
        {
          std::cout<<"addinterpretation\n";
          int i_typ  =std::stoi (v[1]);
          bool correct= true;
          int interpret_id= interpretations.size();
          std::cout<<"typ: ";
          if(i_typ==0)
          {
            std::cout<<"averaging\n";
            interpretations.push_back(std::make_shared<Average>(base,interpret_id));
          }else if (i_typ==1){
            std::cout<<"transferfunction\n";
            int start  =std::stoi (v[2]);
            int length =v.size();
            std:shared_ptr<std::vector<float>> weights= std::make_shared<std::vector<float>>();
            for (int i=3; i<length; i++){
              std::istringstream iss(v[i]);
              float val;
              iss>> noskipws >> val;
              if(iss.eof() && !iss.fail()){   //to compensate input errors with to much blanks
                std::cout<<"weight"<<val<<"\n";
                weights->push_back(val);
              }
            }
            interpretations.push_back(std::make_shared<Transferfunction>(base,interpret_id, start, weights));
          }else if (i_typ==2){
            std::cout<<"overpaint\n";
          }else if (i_typ==3){
            std::cout<<"boost\n";
          }else{
            correct=false;
            std::cout<<"wrong typ?\n";
          }
          if(correct)
          {
            std::cout<<"id:"<<interpret_id<<"\n";
          }
        //  int endframe   =std::stoi (v[2]);
        //  int local_i   =std::stof (v[3]);
        //  int global_i   =std::stof (v[4]);

        //segments.push_back(base->add_segment(startframe, endframe,local_i, global_i));
      }else if(v[0]=="manipulate")
        {
          std::cout<<"manipulate: ";
          if(v[1]=="segment"){
            int id=std::stoi(v[2]);
            int start=std::stoi(v[3]);
            int end=std::stoi(v[4]);
            float local_i=std::stof(v[5]);
            float global_i=std::stof(v[6]);
            std::cout<<"segment "<<id<<"\n";
            segments[id]->manipulate(start,end,local_i,global_i);
          }else if(v[1]=="interpretation"){
            //all segments need to recalculate!
            int id=std::stoi(v[2]);

            if(interpretations[id]->getTypenumber()==0)
            {//average
                std::cout<<"an average interpretation needs no manipulation\n";
            }else if(interpretations[id]->getTypenumber()==1)
            {//transferfunction:
              int start  =std::stoi (v[3]);
              int length =v.size();
              std::shared_ptr<std::vector<float>> weights= std::make_shared<std::vector<float>>();
              for (int i=4; i<length; i++){
                std::istringstream iss(v[i]);
                float val;
                iss>> noskipws >> val;
                if(iss.eof() && !iss.fail()){   //to compensate input errors with to much blanks
                  std::cout<<"weight"<<val<<"\n";
                  weights->push_back(val);
                }
              }
              //ist folgende zeile in ordnung?!:
               Transferfunction& x = dynamic_cast<Transferfunction&>(*interpretations[id]);
              x.set_weights(start, weights);
            }else{
              std::cout<<"not yet implemented manipulation\n";
            }
          }else{
            std::cout<<"either segment or interpretation ?!!\n";
          }

        }else if(v[0]=="connect"){
          int id_segments       =std::stoi (v[1]);
          int id_interpretation =std::stoi (v[2]);
          if(id_segments<=segments.size()-1 && id_interpretation <= interpretations.size()-1)
          {
            // set calculation to zero, if interpretation is new
            segments[id_segments]->set_interpretation(interpretations[id_interpretation]);
            std::cout<<"Segment "<<id_segments<<" is interpreted with interpretation "<<id_interpretation<<"\n";
          }else{
            std::cout<<"wrong id's?\n";
          }
          //segments[id]->move_in_time(delta);
        }else if(v[0]=="move"){
          int id        =std::stoi (v[1]);
          int delta     =std::stoi (v[2]);

          segments[id]->move_in_time(delta);
        }else if(v[0]=="exposure"){
          //seems to work
          int id        =std::stoi (v[1]);
          int imgs      =std::stoi (v[2]);
          //std::cout<<"move, id:"<<id<<", delta: "<<delta<<"\n";
          //add avg segment
           //base->set_exposure(id, imgs);

         }else if(v[0]=="startframe"){
           //seems to work
           int id        =std::stoi (v[1]);
           int frame      =std::stoi (v[2]);
           //std::cout<<"move, id:"<<id<<", delta: "<<delta<<"\n";
           //add avg segment
            //base->set_startframe(id, frame);
          }else if(v[0]=="endframe"){
            //seems to work
            int id          =std::stoi (v[1]);
            int frame      =std::stoi (v[2]);
            //std::cout<<"move, id:"<<id<<", delta: "<<delta<<"\n";
            //add avg segment
             //base->set_endframe(id, frame);
           }else if(v[0]=="exposure_delta"){
             //seems to work
             int id        =std::stoi (v[1]);
             int delta      =std::stoi (v[2]);
             //std::cout<<"move, id:"<<id<<", delta: "<<delta<<"\n";
             //add avg segment
              //base->set_exposure_delta(id, delta);
            }else if(v[0]=="inten_local"){
              //seems to work
              int id        =std::stoi (v[1]);
              float i      =std::stof (v[2]);
              segments[id]->set_local_intensity(i);

            }else if(v[0]=="inten_global"){
               //does not work
               int id        =std::stoi (v[1]);
               float i      =std::stof (v[2]);
               segments[id]->set_global_intensity(i);
            }else if(v[0]=="work_size"){
                 //does not work
                 int i      =std::stoi (v[1]);
                 base->set_work_size(i);
            }else if(v[0]=="delete"){
                 //does not work
                 int id      =std::stoi (v[1]);
                 std::cout<<"del\n";
                 segments[id]->delete_seg();
            }else if(v[0]=="save"){
                 //does not work
                 std::cout<<"save\n";
                 base->save("out"+v[1]+".jpg");
            }
    }
    std::cout << "done!" << endl;
    imwrite( "./final.jpg", base->get_result());

  }else{
    std::cout<<"videopfad angeben?\n";
  }

  return 0;
}
