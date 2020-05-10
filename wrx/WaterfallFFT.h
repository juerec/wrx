#ifndef WATERFALLFFT_H
#define WATERFALLFFT_H

// #include <complex.h>
#include <fftw3.h>
#include "RGBLine.h"

class WaterfallFFT {

public:
  WaterfallFFT();
  ~WaterfallFFT();
  void init(int nWindowSize, int iWindowFunc, bool isComplex, bool xchg);
  void setValue(const short * pSample, int y);
  RGBLine * calculate(int binLeft, int nBins, float Gain);
  void cleanUp();

protected :


  // unsigned int     calcRGB(float complex bin, float Gain);
  unsigned int     calcRGB(fftwf_complex bin, float Gain);

  int              nWindowSize;
  float          * Window;
  float          * InBufferReal;
  // float complex  * InBufferComplex;
  fftwf_complex  * InBufferComplex;
  // float complex  * OutBuffer;
  fftwf_complex  * OutBuffer;
  fftwf_plan       plan;
  bool             isComplex;
  bool             xchg;

};

#endif // WATERFALLFFT_H
