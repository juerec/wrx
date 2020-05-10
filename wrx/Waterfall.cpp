#include "Waterfall.h"
#include "int_fft.h"

#include <qpainter.h>

#include <assert.h>

SoundData::SoundData(short * samples, int count) {
  this->count = count;
  this->samples = new short[count];
  memmove(this->samples, samples, count * sizeof(short));
}
SoundData::~SoundData() {
  delete [] samples;
}


Waterfall::Waterfall(QWidget * parent, Qt::WindowFlags f)
         : QFrame(parent, f),
           Pixmap(WINDOWWIDTH, WINDOWHEIGHT),
           CurrLineImage(WINDOWWIDTH, 1, QImage::Format_RGB32) {

  nFFTLen = FFTSIZE;
  Data = new short [ nFFTLen ];
  DataPos  = 0;
  DataCycle = 4000;
  InputPos  = 0;
  CurrLine = 0;
  memset(Data, 0, nFFTLen);
  aReal = NULL;
  aImag = NULL;
  aColorTable = NULL;
  nColorTableLen = 0;
  loudbuf = NULL;
  setFFTSize(nFFTLen);
  fix_fft_initialize();
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&InputListMutex, NULL);
  RefreshId = 0;
  RefreshIdOld = 0;
}
// ============================================ Waterfall::Waterfall()

Waterfall::~Waterfall() {
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&InputListMutex);
  delete [] Data;
  setFFTSize(0);
  while (!InputList.isEmpty())
     delete InputList.takeFirst();
}
// ============================================ Waterfall::~Waterfall()

void Waterfall::setFFTSize(int fftSize) {

  if (aReal != NULL) {
    delete [] aReal;
    aReal = NULL;
  }
  if (loudbuf != NULL) {
    delete [] loudbuf;
    loudbuf = NULL;
  }
  if (fftSize > 0) {
    aReal = new short [fftSize << 1];
    aImag = aReal + fftSize;
    memset(aReal, 0, fftSize << 1);
    loudbuf = new int [fftSize];
    memset(loudbuf, 0, sizeof(int) * fftSize);
    nColorTableLen = 6 * 31 + 1;
    aColorTable = new unsigned int [nColorTableLen];
    CreatePowerColorGradientSimple(5, aColorTable, nColorTableLen);
  }
}
// ============================================ Waterfall::setFFTSize()

void Waterfall::processData() {
bool  bOverloaded = false;
int   i, ii;
int   nWidth   = nFFTLen >> 1;
int   iDecibelsThreshold	= 10 << 16;
int   iDecibelsRange		= 80; //  75;
int   iReal;
int   iImag;

  memmove(aReal, Data, sizeof(short) * nFFTLen);
  memset(aImag, 0, sizeof(short) * nFFTLen);
  for (i = 0; i < nFFTLen; i++) {
    if (aReal[i] < -32766 || aReal[i] > 32766) {
      bOverloaded = true;
      break;
    }
  }
  window_hamming(aReal, nFFTLen);
  sorensen_realfft_radix2(aReal, nFFTLen);

  *loudbuf = (*aReal) * (*aReal);
  for (i = 1; i < nWidth; ++ i) {
    iReal = aReal[i];
    iImag = aReal[nFFTLen - i];
    loudbuf[i] = db_from_ampl2_2(iReal * iReal + iImag * iImag);
  }
  for (i = 0, ii = 0; i < nWidth && ii < WINDOWWIDTH; ++ i) {
    int loud  = (loudbuf[i] - iDecibelsThreshold) / iDecibelsRange;
    int color = (loud * nColorTableLen + 32768) >> 16;
    if (color >= nColorTableLen)
      color = nColorTableLen - 1;
    if (color < 0)
      color = 0;
    CurrLineImage.setPixel(ii++, 0, aColorTable[color]);
  }

  pthread_mutex_lock(&mutex);
  CurrLine--;
  if (CurrLine < 0)
    CurrLine = WINDOWHEIGHT - 1;
  QPainter painter(&Pixmap);
  painter.drawImage(0, CurrLine, CurrLineImage, 0, 0, 512, 1);
  RefreshId++;
  pthread_mutex_unlock(&mutex);
}
// ============================================ Waterfall::processData()

void Waterfall::newDataM(short * samples, int count) {
int SamplePos = 0;

  while (SamplePos < count) {
    while (InputPos < DataCycle - nFFTLen) {
      InputPos++;
      SamplePos++;
      if (SamplePos >= count)
        return;
    }
    while (SamplePos < count && DataPos < nFFTLen) {
      Data[DataPos++] = samples[SamplePos++];
    }
    if (DataPos == nFFTLen) {
      processData();
      DataPos = 0;
      InputPos = 0;
      if (DataCycle < nFFTLen) {
        memmove(Data, &Data[DataCycle], nFFTLen - DataCycle);
        DataPos = nFFTLen - DataCycle;
        InputPos = DataPos;
      }
     }
  }
}
// ============================================ Waterfall::newDataM()  ###setData()

void Waterfall::setDataM(short * samples, int count) {
  pthread_mutex_lock(&InputListMutex);
  SoundData * sd = new SoundData(samples, count);
  InputList.append(sd);
  pthread_mutex_unlock(&InputListMutex);
}
// ============================================ Waterfall::setData()

void Waterfall::TimeOut100() {
  if (pthread_mutex_trylock(&InputListMutex) == 0) {
    while (InputList.count() > 0) {
      SoundData * sd = InputList.at(0);
      newDataM(sd->samples, sd->count);
      InputList.remove(sd);
    }
    pthread_mutex_unlock(&InputListMutex);
  }

  if (RefreshId != RefreshIdOld && pthread_mutex_trylock(&mutex) == 0) {
    update();
    RefreshIdOld = RefreshId;
    pthread_mutex_unlock(&mutex);
  }
}
// ============================================ Waterfall::TimeOut100()

void Waterfall::paintEvent(QPaintEvent *) {

  QPainter p(this);

  p.drawPixmap(0, 0, Pixmap, 0, CurrLine, -1, -1);
  if (CurrLine)
    p.drawPixmap(0, WINDOWHEIGHT - CurrLine, Pixmap, 0,  0, -1, CurrLine);
}
// ============================================ Waterfall::paintEvent()

void Waterfall::CreatePowerColorGradientSimple(int nBits, unsigned int * pColorsOut, int nColorsOut) {
int i;
int iColor  = 0;
int iShift  = 8 - nBits;
int iMaxVal = 1;

  for (i = 0; i < nBits; ++ i)
    iMaxVal <<= 1;
  -- iMaxVal;

  for (i = 0; i <= iMaxVal; ++ i)
    pColorsOut[iColor ++] = QColor(0, 0, i << iShift).pixel();
  for (i = 1; i <= iMaxVal; ++ i)
    pColorsOut[iColor ++] = QColor(0, i << iShift, iMaxVal << iShift).pixel();
  for (i = 1; i <= iMaxVal; ++ i)
    pColorsOut[iColor ++] = QColor(0, iMaxVal << iShift, (iMaxVal + 1 - i) << iShift).pixel();
  for (i = 1; i <= iMaxVal; ++ i)
    pColorsOut[iColor ++] = QColor(i << iShift, iMaxVal << iShift, 0).pixel();
  for (i = 1; i <= iMaxVal; ++ i)
    pColorsOut[iColor ++] = QColor(iMaxVal << iShift, (iMaxVal + 1 - i) << iShift, 0).pixel();
  for (i = 1; i <= iMaxVal; ++ i)
    pColorsOut[iColor ++] = QColor(iMaxVal << iShift, 0, i << iShift).pixel();
}
// ============================================ Waterfall::CreatePowerColorGradientSimple()
