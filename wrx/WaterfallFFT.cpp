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

#include "WaterfallFFT.h"
#include "WindowFunc.h"
#include <math.h>
#include <QMutex>

#include "Logger.h"

extern QMutex  FFTwInitMutex;

typedef struct tagColorRef {
  int   Value;
  unsigned int RGB;
} ColorRef;

static
ColorRef ColorRefTable[96] = {
{      16, 0xFF000000 },
{      17, 0xFF000010 },
{      20, 0xFF000020 },
{      22, 0xFF000030 },
{      25, 0xFF000040 },
{      28, 0xFF000050 },
{      31, 0xFF000060 },
{      35, 0xFF000070 },
{      40, 0xFF000080 },
{      45, 0xFF000090 },
{      50, 0xFF0000A0 },
{      56, 0xFF0000B0 },
{      63, 0xFF0000C0 },
{      71, 0xFF0000D0 },
{      80, 0xFF0000E0 },
{      89, 0xFF0000F0 },
{     100, 0xFF0000FF },
{     113, 0xFF0010FF },
{     127, 0xFF0020FF },
{     142, 0xFF0030FF },
{     160, 0xFF0040FF },
{     179, 0xFF0050FF },
{     201, 0xFF0060FF },
{     226, 0xFF0070FF },
{     253, 0xFF0080FF },
{     284, 0xFF0090FF },
{     319, 0xFF00A0FF },
{     358, 0xFF00B0FF },
{     401, 0xFF00C0FF },
{     450, 0xFF00D0FF },
{     505, 0xFF00E0FF },
{     567, 0xFF00F0FF },
{     636, 0xFF00FFFF },
{     714, 0xFF00FFF0 },
{     801, 0xFF00FFE0 },
{     899, 0xFF00FFD0 },
{    1009, 0xFF00FFC0 },
{    1132, 0xFF00FFB0 },
{    1270, 0xFF00FFA0 },
{    1426, 0xFF00FF90 },
{    1600, 0xFF00FF80 },
{    1795, 0xFF00FF70 },
{    2014, 0xFF00FF60 },
{    2260, 0xFF00FF50 },
{    2535, 0xFF00FF40 },
{    2845, 0xFF00FF30 },
{    3192, 0xFF00FF20 },
{    3581, 0xFF00FF10 },
{    4019, 0xFF00FF00 },
{    4509, 0xFF10FF00 },
{    5059, 0xFF20FF00 },
{    5677, 0xFF30FF00 },
{    6369, 0xFF40FF00 },
{    7146, 0xFF50FF00 },
{    8018, 0xFF60FF00 },
{    8997, 0xFF70FF00 },
{   10095, 0xFF80FF00 },
{   11327, 0xFF90FF00 },
{   12709, 0xFFA0FF00 },
{   14260, 0xFFB0FF00 },
{   16000, 0xFFC0FF00 },
{   17952, 0xFFD0FF00 },
{   20142, 0xFFE0FF00 },
{   22600, 0xFFF0FF00 },
{   25358, 0xFFFFFF00 },
{   28452, 0xFFFFF000 },
{   31924, 0xFFFFE000 },
{   35819, 0xFFFFD000 },
{   40190, 0xFFFFC000 },
{   45094, 0xFFFFB000 },
{   50596, 0xFFFFA000 },
{   56770, 0xFFFF9000 },
{   63697, 0xFFFF8000 },
{   71469, 0xFFFF7000 },
{   80189, 0xFFFF6000 },
{   89974, 0xFFFF5000 },
{  100953, 0xFFFF4000 },
{  113271, 0xFFFF3000 },
{  127092, 0xFFFF2000 },
{  142600, 0xFFFF1000 },
{  160000, 0xFFFF0000 },
{  179523, 0xFFFF0010 },
{  201427, 0xFFFF0020 },
{  226005, 0xFFFF0030 },
{  253582, 0xFFFF0040 },
{  284524, 0xFFFF0050 },
{  319242, 0xFFFF0060 },
{  358195, 0xFFFF0070 },
{  401901, 0xFFFF0080 },
{  450940, 0xFFFF0090 },
{  505964, 0xFFFF00A0 },
{  567701, 0xFFFF00B0 },
{  636971, 0xFFFF00C0 },
{  714693, 0xFFFF00D0 },
{  801899, 0xFFFF00E0 },
{  899745, 0xFFFF00F0 }
};

typedef struct tagColorRefIndex {
  int Value;
  int ColorIndex;
} ColorRefIndex;


static
ColorRefIndex ColorRefTableIndex[6] = {
  {        100,   0 },
  {        636,  16 },
  {       4019,  32 },
  {      25358,  48 },
  {     160000,  64 },
  { 1000000000,  80 }
};



WaterfallFFT::WaterfallFFT() {
  nWindowSize     = 0;
  Window          = nullptr;
  OutBuffer       = nullptr;
  InBufferReal    = nullptr;
  InBufferComplex = nullptr;
  plan            = nullptr;
  isComplex       = false;
//  iBinLeft        = 0;
//  nBins           = 0;
  xchg            = false;
}

WaterfallFFT::~WaterfallFFT() {
  cleanUp();
}

void WaterfallFFT::init(int nWindowSize, int iWindowFunc, bool isComplex, bool xchg) {
  cleanUp();
  this->nWindowSize = nWindowSize;
  this->isComplex   = isComplex;
  this->xchg        = xchg;

  xchg = true;

  // logger.debug("WaterfallFFT::init()", __LINE__, "nWindowSize = %i   iWindowFunc = %i   isComplex = %s   xchg = %s",
  //                                                 nWindowSize, iWindowFunc, isComplex ? "true" : "false", xchg ? "true" : "false");

  Window = new float[nWindowSize];
  for (int i = 0; i < nWindowSize; i++)
    Window[i] = 1.0f;
  WindowFunc(iWindowFunc, nWindowSize, Window);

  if (isComplex) {
    InBufferComplex = fftwf_alloc_complex(nWindowSize);
  } else {
    InBufferReal    = fftwf_alloc_real(nWindowSize);
  }
  OutBuffer = fftwf_alloc_complex(nWindowSize + 1);

  // Initialization of FFTw is NOT thread save!
  // We must protect the fftwf_plan_xxx call application wide.
  FFTwInitMutex.lock();
  if (isComplex)
    plan = fftwf_plan_dft_1d(nWindowSize, InBufferComplex, OutBuffer, FFTW_FORWARD, FFTW_MEASURE);
  else
    plan = fftwf_plan_dft_r2c_1d(nWindowSize, InBufferReal, OutBuffer, FFTW_MEASURE);
  FFTwInitMutex.unlock();
}

void WaterfallFFT::setValue(const short * pSample, int y) {
float real;
float imag;

  if (y < 0 || y >= nWindowSize) {
    logger.error("WaterfallFFT::setValue()", __LINE__, "y = %i     nWindowSize = %i", y, nWindowSize);
    return;
  }

  if (isComplex) {
    if (!xchg) {
      real = float(*pSample);
      imag = float(*(pSample + 1));
    } else {
      real = float(*(pSample + 1));
      imag = float(*pSample);
    }
    // InBufferComplex[y] = (float(*pSample) + float(*(pSample + 1)) * I) * Window[y];
    InBufferComplex[y][0] = real * Window[y] /* - imag * 0.0 */;
    InBufferComplex[y][1] = /* real * 0.0 + */ Window[y] * imag;
  } else {
    InBufferReal[y] = float(*pSample) * Window[y];
  }
}


RGBLine * WaterfallFFT::calculate(int binLeft, int nBins, float Gain) {
RGBLine * line;
int       iBin, nBin, binRight = binLeft + nBins;
int       iRGB = 0;

  // logger.debug("WaterfallFFT::calculate()", __LINE__, "FFT %c   nBins = %i   binLeft = %i   binRight = %i", isComplex ? 'C' : 'R', nBins, binLeft, binRight);
  fftwf_execute(plan);
  line = new RGBLine(nBins);
  // logger.debug("WaterfallFFT::calculate()", __LINE__, "FFT %c   binLeft = %i   binRight = %i", isComplex ? 'C' : 'R', binLeft, binRight);
  if (!isComplex) {
    for (iBin = binLeft; iBin < binRight; iBin++)
      line->set(calcRGB(OutBuffer[iBin], Gain), iRGB++);
  } else {
    nBin = binLeft + (nWindowSize >> 1);
    for (iBin = binLeft; iBin < binRight; iBin++)
      line->set(calcRGB(OutBuffer[nBin++ % nWindowSize], Gain), iRGB++);
  }
  return line;
}


void WaterfallFFT::cleanUp() {
  if (Window != nullptr) {
    delete [] Window;
    Window = nullptr;
  }
  if (plan != nullptr) {
    fftwf_destroy_plan(plan);
    plan = nullptr;
  }
  if (InBufferReal != nullptr) {
    fftwf_free(InBufferReal);
    InBufferReal = nullptr;
  }
  if (InBufferComplex != nullptr) {
    fftwf_free(InBufferComplex);
    InBufferComplex = nullptr;
  }
  if (OutBuffer != nullptr) {
    fftwf_free(OutBuffer);
    OutBuffer = nullptr;
  }
}


unsigned int WaterfallFFT::calcRGB(fftwf_complex bin, float Gain) {
long Power;
int    i = 0;

  // Power = (long)(sqrtf(creal(bin) * creal(bin) + cimag(bin) * cimag(bin)) * Gain);
  Power = (long)(sqrtf(bin[0] * bin[0] + bin[1] * bin[1]) * Gain);
  while (i < 5 && Power > ColorRefTableIndex[i].Value)
    i++;
  i = ColorRefTableIndex[i].ColorIndex;

  while (i < 95 && Power > ColorRefTable[i].Value)
    i++;

  return ColorRefTable[i].RGB;
}
