#include "../include/utils.hpp"

namespace utils {

  float dE2000(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh){

    float l1 = color1[0];
    float l2 = color2[0];

    float a1 = color1[1];
    float a2 = color2[1];

    float b1 = color1[2];
    float b2 = color2[2];

    float L_ = (l1 + l2) / 2;
    float delta_L = l2 - l1;

    float c1 = sqrt(pow(a1, 2) + pow(b1, 2));
    float c2 = sqrt(pow(a2, 2) + pow(b2, 2));
    float C_ = (c1 + c2) / 2;

    float a1_ = a1 + (a1/2) * (1 - pow(C_, 7) / sqrt(pow(C_, 7) + pow(25, 7)));
    float a2_ = a2 + (a2/2) * (1 - pow(C_, 7) / sqrt(pow(C_, 7) + pow(25, 7)));

    float C1_ = sqrt(pow(a1_, 2) + pow(b1, 2));
    float C2_ = sqrt(pow(a2_, 2) + pow(b2, 2));
    float C__ = (C1_ + C2_) / 2;
    float delta_C__ = C2_ - C1_;

    float h1_ = fmod(atan2(b1, a1_), (360 * (M_PI / 180.0)));
    float h2_ = fmod(atan2(b2, a2_), (360 * (M_PI / 180.0)));

    float delta_h_ = -1;
    if(abs(h2_ - h1_) <=  (180 * (M_PI / 180.0))){
      delta_h_ = h2_ - h1_;
    }
    else if(abs(h2_ - h1_) > (180 * (M_PI / 180.0)) && h2_ <= h1_){
      delta_h_ = h2_ - h1_ + (360 * (M_PI / 180.0));
    }
    else if(abs(h2_ - h1_) > (180 * (M_PI / 180.0)) && h2_ > h1_){
      delta_h_ = h2_ - h1_ - (360 * (M_PI / 180.0));
    }

    float delta_H_ = 2 * sqrt(C1_ * C2_) * sin(delta_h_ / 2);
    float H_ = -1;
    if(abs(h2_ - h1_) <=  (180 * (M_PI / 180.0))){
      H_ = (h2_ - h1_) / 2;
    }
    else if(abs(h2_ - h1_) > (180 * (M_PI / 180.0)) && (h2_ + h1_) < (360 * (M_PI / 180.0))){
      H_= (h2_ - h1_ + (360 * (M_PI / 180.0))) / 2;
    }
    else if(abs(h2_ - h1_) > (180 * (M_PI / 180.0)) && (h2_ + h1_) >= (360 * (M_PI / 180.0))){
      H_ = (h2_ - h1_ - (360 * (M_PI / 180.0))) / 2;
    }

    float T = 1 - 0.17 * cos(H_ * 1 - (30 * (M_PI / 180.0))) +
                  0.24 * cos(H_ * 2) +
                  0.32 * cos(H_ * 3  + (6 * (M_PI / 180.0))) +
                  0.20 * cos(H_ * 4 - (63 * (M_PI / 180.0)));

    float Sl = 1 + (0.015 * pow(L_ - 50, 2)) / sqrt(20 + pow(L_ - 50, 2));
    float Sc = 1 + 0.045 * C__;
    float Sh = 1 + 0.015 * C__ * T;
    float RT = -2 * sqrt(pow(C__, 7) /pow(C__, 7) + pow(25, 7)) *
                    sin((60 * (M_PI / 180.0)) * exp(-1 * pow(((H_ - (275 * (M_PI / 180.0))) / (25 * (M_PI / 180.0))), 2)));

    float deltaE2000 = sqrt(pow( delta_L / (kl * Sl), 2) +
                            pow( delta_C__ / (kc * Sc), 2) +
                            pow( delta_H_ / (kh * Sh), 2) +
                            RT * (delta_C__ / (kc * Sc)) * (delta_H_ / (kh * Sh)));

    return deltaE2000;
  }

};
