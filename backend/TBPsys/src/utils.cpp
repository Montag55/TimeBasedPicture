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
    else
      h1_strich = fmod(atan2(b1, a1_strich), (2 * M_PI));

    float h2_strich = -1;
    if(b2 == 0 && a2_strich == 0)
      h2_strich = 0;
    else
      h2_strich = fmod(atan2(b2, a2_strich), (2 * M_PI));


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

  float not_mine(cv::Scalar color1, cv::Scalar color2, float k_L, float k_C, float k_H){
      /*
  	 * "For these and all other numerical/graphical 􏰀delta E00 values
  	 * reported in this article, we set the parametric weighting factors
  	 * to unity(i.e., k_L = k_C = k_H = 1.0)." (Page 27).
  	 */
  	const double deg360InRad = 2*M_PI;
  	const double deg180InRad = M_PI;
  	const double pow25To7 = 6103515625.0;

    float l1 = color1[0];
    float l2 = color2[0];

    float a1 = color1[1];
    float a2 = color2[1];

    float b1 = color1[2];
    float b2 = color2[2];

  	/*
  	 * Step 1
  	 */
  	/* Equation 2 */
  	double C1 = sqrt((a1 * a1) + (b1 * b1));
  	double C2 = sqrt((a2 * a2) + (b2 * b2));
  	/* Equation 3 */
  	double barC = (C1 + C2) / 2.0;
  	/* Equation 4 */
  	double G = 0.5 * (1 - sqrt(pow(barC, 7) / (pow(barC, 7) + pow25To7)));
  	/* Equation 5 */
  	double a1Prime = (1.0 + G) * a1;
  	double a2Prime = (1.0 + G) * a2;
  	/* Equation 6 */
  	double CPrime1 = sqrt((a1Prime * a1Prime) + (b1 * b1));
  	double CPrime2 = sqrt((a2Prime * a2Prime) + (b2 * b2));
  	/* Equation 7 */
  	double hPrime1;
  	if (b1 == 0 && a1Prime == 0)
  		hPrime1 = 0.0;
  	else {
  		hPrime1 = atan2(b1, a1Prime);
  		/*
  		 * This must be converted to a hue angle in degrees between 0
  		 * and 360 by addition of 2􏰏 to negative hue angles.
  		 */
  		if (hPrime1 < 0)
  			hPrime1 += deg360InRad;
  	}
  	double hPrime2;
  	if (b2 == 0 && a2Prime == 0)
  		hPrime2 = 0.0;
  	else {
  		hPrime2 = atan2(b2, a2Prime);
  		/*
  		 * This must be converted to a hue angle in degrees between 0
  		 * and 360 by addition of 2􏰏 to negative hue angles.
  		 */
  		if (hPrime2 < 0)
  			hPrime2 += deg360InRad;
  	}

  	/*
  	 * Step 2
  	 */
  	/* Equation 8 */
  	double deltaLPrime = l2 - l1;
  	/* Equation 9 */
  	double deltaCPrime = CPrime2 - CPrime1;
  	/* Equation 10 */
  	double deltahPrime;
  	double CPrimeProduct = CPrime1 * CPrime2;
  	if (CPrimeProduct == 0)
  		deltahPrime = 0;
  	else {
  		/* Avoid the fabs() call */
  		deltahPrime = hPrime2 - hPrime1;
  		if (deltahPrime < -deg180InRad)
  			deltahPrime += deg360InRad;
  		else if (deltahPrime > deg180InRad)
  			deltahPrime -= deg360InRad;
  	}
  	/* Equation 11 */
  	double deltaHPrime = 2.0 * sqrt(CPrimeProduct) *
  	    sin(deltahPrime / 2.0);

  	/*
  	 * Step 3
  	 */
  	/* Equation 12 */
  	double barLPrime = (l1 + l2) / 2.0;
  	/* Equation 13 */
  	double barCPrime = (CPrime1 + CPrime2) / 2.0;
  	/* Equation 14 */
  	double barhPrime, hPrimeSum = hPrime1 + hPrime2;
  	if (CPrime1 * CPrime2 == 0) {
  		barhPrime = hPrimeSum;
  	}
    else {
  		if (fabs(hPrime1 - hPrime2) <= deg180InRad)
  			barhPrime = hPrimeSum / 2.0;
  		else {
  			if (hPrimeSum < deg360InRad)
  				barhPrime = (hPrimeSum + deg360InRad) / 2.0;
  			else
  				barhPrime = (hPrimeSum - deg360InRad) / 2.0;
  		}
  	}
  	/* Equation 15 */
  	double T = 1.0 - (0.17 * cos(barhPrime - 30.0 * M_PI/180)) +
  	    (0.24 * cos(2.0 * barhPrime)) +
  	    (0.32 * cos((3.0 * barhPrime) + 06.0 * M_PI/180)) -
  	    (0.20 * cos((4.0 * barhPrime) - 63.0 * M_PI/180));
  	/* Equation 16 */
  	double deltaTheta = 30.0 * M_PI/180 *
  	    exp(-pow((barhPrime - (275.0 * M_PI/180)) / (25.0 * M_PI/180), 2.0));
  	/* Equation 17 */
  	double R_C = 2.0 * sqrt(pow(barCPrime, 7.0) /
  	    (pow(barCPrime, 7.0) + pow25To7));
  	/* Equation 18 */
  	double S_L = 1 + ((0.015 * pow(barLPrime - 50.0, 2.0)) /
  	    sqrt(20 + pow(barLPrime - 50.0, 2.0)));
  	/* Equation 19 */
  	double S_C = 1 + (0.045 * barCPrime);
  	/* Equation 20 */
  	double S_H = 1 + (0.015 * barCPrime * T);
  	/* Equation 21 */
  	double R_T = (-sin(2.0 * deltaTheta)) * R_C;

  	/* Equation 22 */
  	double deltaE = sqrt(
  	    pow(deltaLPrime / (k_L * S_L), 2.0) +
  	    pow(deltaCPrime / (k_C * S_C), 2.0) +
  	    pow(deltaHPrime / (k_H * S_H), 2.0) +
  	    (R_T * (deltaCPrime / (k_C * S_C)) * (deltaHPrime / (k_H * S_H))));

    return (deltaE);
  }

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

};
