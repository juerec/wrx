/****************************************************************************
**
**  This file is a part of the program "WRX".
**
**  Copyright © 2020 Jürgen Reche
**
** WRX is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** WRX is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

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
