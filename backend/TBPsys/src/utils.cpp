#include "../include/utils.hpp"

namespace utils {

  float dE2000(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh){

    float l1 = color1[0];
    float l2 = color2[0];

    float a1 = color1[1];
    float a2 = color2[1];

    float b1 = color1[2];
    float b2 = color2[2];

    float Lbar = (l1 + l2) / 2;
    float delta_L_strich = l2 - l1;

    float c1 = sqrt(pow(a1, 2) + pow(b1, 2));
    float c2 = sqrt(pow(a2, 2) + pow(b2, 2));
    float Cbar = (c1 + c2) / 2;

    float a1_strich = a1 + (a1/2) * (1 - sqrt(pow(Cbar, 7) / (pow(Cbar, 7) + pow(25, 7))));
    float a2_strich = a2 + (a2/2) * (1 - sqrt(pow(Cbar, 7) / (pow(Cbar, 7) + pow(25, 7))));

    float C1_strich = sqrt( pow(a1_strich, 2) + pow(b1, 2) );
    float C2_strich = sqrt( pow(a2_strich, 2) + pow(b2, 2) );
    float Cbar_strich = (C1_strich + C2_strich) / 2;
    float delta_C_strich = C2_strich - C1_strich;

    float h1_strich = -1;
    if(b1 == 0 && a1_strich == 0)
      h1_strich = 0;
    else{
      h1_strich = atan2(b1, a1_strich);
      if(h1_strich < 0)
        h1_strich += (2 * M_PI);
    }

    float h2_strich = -1;
    if(b2 == 0 && a2_strich == 0)
      h2_strich = 0;
    else{
      h2_strich = atan2(b2, a2_strich);
      if(h2_strich < 0)
        h2_strich += (2 * M_PI);
    }

    float delta_h_strich = -1;
    if(abs(h1_strich - h2_strich) <=  (M_PI)){
      delta_h_strich = h2_strich - h1_strich;
    }
    else if(abs(h1_strich - h2_strich) > (M_PI) && h2_strich <= h1_strich){
      delta_h_strich = h2_strich - h1_strich + (2 * M_PI);
    }
    else if(abs(h1_strich - h2_strich) > (M_PI) && h2_strich > h1_strich){
      delta_h_strich = h2_strich - h1_strich - (2 * M_PI);
    }
    else{
      std::cout << "smth went wrong 1 \n";
    }

    float delta_H_strich = 2 * sqrt(C1_strich * C2_strich) * sin(delta_h_strich / 2);

    float Hbar_strich = -1;
    if(abs(h1_strich - h2_strich) <=  (M_PI)){
      Hbar_strich = (h1_strich + h2_strich) / 2;
    }
    else if(abs(h1_strich - h2_strich) > (M_PI) && (h1_strich + h2_strich) < (2 * M_PI)){
      Hbar_strich = (h1_strich + h2_strich + (2 * M_PI)) / 2;
    }
    else if(abs(h1_strich - h2_strich) > (M_PI) && (h1_strich + h2_strich) >= (2 * M_PI)){
      Hbar_strich = (h1_strich + h2_strich - (2 * M_PI)) / 2;
    }
    else{
      std::cout << "smth went wrong 2 \n";
    }

    float T = 1 - 0.17 * cos(Hbar_strich - (30 * (M_PI / 180))) +
                  0.24 * cos(Hbar_strich * 2) +
                  0.32 * cos(Hbar_strich * 3 + (6 * (M_PI / 180))) -
                  0.20 * cos(Hbar_strich * 4 - (63 * (M_PI / 180)));

    float Sl = 1 + (0.015 * pow(Lbar - 50, 2)) / sqrt(20 + pow(Lbar - 50, 2));
    float Sc = 1 + 0.045 * Cbar_strich;
    float Sh = 1 + 0.015 * Cbar_strich * T;
    float RT = -2 * sqrt(pow(Cbar_strich, 7) / (pow(Cbar_strich, 7) + pow(25, 7))) *
                    sin((60 * (M_PI / 180)) * exp(-1 * pow(((Hbar_strich - (275 * (M_PI / 180))) /
                    (25 * (M_PI / 180))), 2)));

    float deltaE2000 = sqrt(pow( delta_L_strich / (kl * Sl), 2) +
                            pow( delta_C_strich / (kc * Sc), 2) +
                            pow( delta_H_strich / (kh * Sh), 2) +
                            RT * (delta_C_strich / (kc * Sc)) * (delta_H_strich / (kh * Sh)));

    return deltaE2000;
  }

  float CIE76(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh){
    return sqrt(kl * pow(color1[0] - color2[0], 2) + kc * pow(color1[1] - color2[1], 2) + kh * pow(color1[2] - color2[2], 2));
  }

  float CIE94(cv::Scalar color1, cv::Scalar color2, float kl, float kc, float kh){
    float L_delta = color1[0] -  color2[0];
    float c1 = sqrt(pow(color1[1], 2) + pow(color1[2], 2));
    float c2 = sqrt(pow(color2[1], 2) + pow(color2[2], 2));
    float C_delta = c1 - c2;
    float H_delta = sqrt(pow(color1[1] - color2[1], 2) + pow(color1[2] - color2[2], 2) - pow(C_delta, 2));
    float sl = 1 * kl;
    float sc = 1 + kc * c1;
    float sh = 1 + kh * c1;

    return sqrt(pow(L_delta / kl * sl, 2) + pow(C_delta / (kc * sc), 2) + pow(H_delta / (kh * sh), 2));
  }

  // https://www.easyrgb.com/en/math.php
  cv::Scalar rgb2lab(float r, float g, float b){
    float x, y, z;
    r = r / 255.0f;
    g = g / 255.0f;
    b = b / 255.0f;

    if(r > 0.04045)
      r = r / 12.92;
    else
      r = pow((r + 0.055) / 1.055, 2.4);

    if(g > 0.04045)
      g = g / 12.92;
    else
      g = pow((g + 0.055) / 1.055, 2.4);

    if(b > 0.04045)
      b = b / 12.92;
    else
      b = pow((b + 0.055) / 1.055, 2.4);

    r = r * 100;
    g = g * 100;
    b = b * 100;

    x = (r * 0.4124564 + g * 0.3575761 + b * 0.1804375) / 0.95047;
    y = (r * 0.2126729 + g * 0.7151522 + b * 0.0721750) / 1.00000;
    z = (r * 0.0193339 + g * 0.1191920 + b * 0.9503041) / 1.08883;

    if(x > 0.008856)
      x = (7.787 * x) + 16/116;
    else
      x = pow(x, 1/3);

    if(y > 0.008856)
      y = (7.787 * y) + 16/116;
    else
      y = pow(y, 1/3);

    if(z > 0.008856)
      z = (7.787 * z) + 16/116;
    else
      z = pow(z, 1/3);

    return cv::Scalar((116 * y) - 16, 500 * (x - y), 200 * (y - z));
  }

  float sigmoid(float x, float exponent, float mid){
    return (1/(1+exp(-exponent*(x-mid))));
  }

  std::shared_ptr<std::vector<float>> pointsToWeights(std::shared_ptr<std::vector<float>> points, int length){

    int frame=0;
    std::shared_ptr<std::vector<float>> weights = std::make_shared<std::vector<float>>();

    for(unsigned int i = 0; i < points->size() - 2; i+=2){

      int x = ((int)((*points)[i] * length)) + 1;
      float delta = x - (*points)[i] * length;
      float m = ((*points)[i+3] - (*points)[i+1]) / ((*points)[i+2] * length - (*points)[i] * length);

      while( frame < x ){
        weights->push_back(0);
        frame++;
      }

      while(frame < (((int)((*points)[i+2] * length))+1)){
        weights->push_back(m * delta + (*points)[i+1]);
        delta++;
        frame++;
      }
    }

    return weights;
  }
};
