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
#include "WaterfallWorker.h"
#include "Logger.h"


#define WFWORKER_THREAD_CMD_TERM             0x40000000
#define WFWORKER_THREAD_CMD_FFT_INIT         0x00000004
#define WFWORKER_THREAD_CMD_COMPLEX          0x00000008
#define WFWORKER_THREAD_CMD_WATERFALL_INFO   0x00000010



WaterfallWorker::WaterfallWorker(SamplesFiFo * _pWaterfallFiFo,
                                 RGBLineFiFo * _pRGBLineFiFo)
                                 // const QString & rRadioName)
               : pWaterfallFiFo(_pWaterfallFiFo),
                 pRGBLineFiFo(_pRGBLineFiFo) {
                 // RadioName(rRadioName) {
  ThreadCmd  = 0;
  SampleRate = 0;
  Channels   = -1;
}

// this call may kill the thread!!!
// if the thread waiting for data it gets a NULL pointer
void WaterfallWorker::signalThread() {
  mutexCond.lock();
  waitCondWWThread.wakeAll();
  mutexCond.unlock();
}


void WaterfallWorker::termThread() {
  ThreadCmd = WFWORKER_THREAD_CMD_TERM;
  signalThread();
}


void WaterfallWorker::setupWaterfall(WaterfallInfo wi) {
  mutexJobInfo.lock();
  this->wi = wi;
  ThreadCmd |= WFWORKER_THREAD_CMD_WATERFALL_INFO;
  mutexJobInfo.unlock();
}


void WaterfallWorker::activateReceiveJob(int SampleRate, int Channels) {

  mutexJobInfo.lock();
  this->SampleRate = SampleRate;
  this->Channels = Channels;
  isComplex = Channels != 1;
  ThreadCmd |= WFWORKER_THREAD_CMD_COMPLEX;
  mutexJobInfo.unlock();
}

void WaterfallWorker::run() {
int   ThreadCmd = 0;
SamplesBlock        * sb = nullptr;
SamplesFiFo         * pWaterfallFiFo = this->pWaterfallFiFo;
int nStep           = 8000;  // Zeitabstand von FFT zu FFT
int iStep           = 0;
int nSamplesBlock   = 2000;  // Blockgröße SamplesBlock
int iSamplesBlock   = 0;
int nFFTWidth       = 1024;  // nFFTWidth muss <= nStep sein
int iFFT            = 0;
int isComplex       = 1;
WaterfallFFT    fft;
RGBLine       * Line;
int             LineCnt = 0;
WaterfallInfo   wi(512, 0, 1, 0.03f);

  while (!(ThreadCmd & WFWORKER_THREAD_CMD_TERM )) {

    if (this->ThreadCmd) {
      mutexJobInfo.lock();
      ThreadCmd = (ThreadCmd & WFWORKER_THREAD_CMD_TERM) | this->ThreadCmd;

      if (ThreadCmd & WFWORKER_THREAD_CMD_COMPLEX) {
        nStep = (SampleRate >> 1) * Channels;
        if (isComplex != this->isComplex) {
          isComplex = this->isComplex;
          ThreadCmd |= WFWORKER_THREAD_CMD_FFT_INIT;
        }
        logger.debug("WaterfallWorker::run()", __LINE__, "nStep = %i", nStep);
      }

      if (ThreadCmd & WFWORKER_THREAD_CMD_WATERFALL_INFO) {
        if (wi.Zoom != this->wi.Zoom || wi.nBins != this->wi.nBins)
           ThreadCmd |= WFWORKER_THREAD_CMD_FFT_INIT;
        wi = this->wi;
      }
      this->ThreadCmd &= WFWORKER_THREAD_CMD_TERM;
      mutexJobInfo.unlock();
    }
cleanup:
    if (ThreadCmd) {

      if (ThreadCmd & (WFWORKER_THREAD_CMD_FFT_INIT | WFWORKER_THREAD_CMD_TERM)) {
        fft.cleanUp();
      }

      if (ThreadCmd & WFWORKER_THREAD_CMD_TERM) {
        if (sb)
          delete sb;
        logger.debug("WaterfallWorker::run()", __LINE__, "Thread terminates normaly by WFWORKER_THREAD_CMD_TERM");
        return;
      }

      if (ThreadCmd & WFWORKER_THREAD_CMD_FFT_INIT) {
        nFFTWidth = wi.nBins * wi.Zoom * (isComplex ? 1 : 2);
        fft.init(nFFTWidth, 2, isComplex, false /*xchg*/);
        // logger.debug("WaterfallWorker::run()", __LINE__, "fft.init(%i, 3, %s, false /*xchg*/);", nFFTWidth, isComplex ? "true" : "false");
        iSamplesBlock = 0;
        iFFT          = 0;
        iStep         = 0;
        if (sb != nullptr)
          delete sb;
        sb = nullptr;
        while ((sb = pWaterfallFiFo->remove()) == nullptr) {
          // check termination
          if (this->ThreadCmd & WFWORKER_THREAD_CMD_TERM) {
            ThreadCmd = WFWORKER_THREAD_CMD_TERM;
            goto cleanup;
          }
          // wait for data
          // logger.debug("WaterfallWorker::run()", __LINE__, "A VOR : mutexCond.lock()");
          mutexCond.lock();
          // waitErr =
            waitCondWWThread.wait(&mutexCond);
          mutexCond.unlock();
          // logger.debug("WaterfallWorker::run()", __LINE__, "A NACH: mutexCond.unlock()  waitErr = %i", waitErr);
        }
        nSamplesBlock = sb->nSamples;
      }
      ThreadCmd = 0;
    }

    while (iFFT < nFFTWidth) {
      fft.setValue(sb->ptr(iSamplesBlock), iFFT);
      iFFT++;
      iStep++;
      iSamplesBlock++;
      if (isComplex) {
        iSamplesBlock++;
        iStep++;
      }
      if (iSamplesBlock >= nSamplesBlock) {
        delete sb;
        while ((sb = pWaterfallFiFo->remove()) == nullptr) {
          // check termination
          if (this->ThreadCmd & WFWORKER_THREAD_CMD_TERM) {
            ThreadCmd = WFWORKER_THREAD_CMD_TERM;
            goto cleanup;
          }
          // wait for data
          // logger.debug("WaterfallWorker::run()", __LINE__, "B VOR : mutexCond.lock()");
          mutexCond.lock();
          waitCondWWThread.wait(&mutexCond);
          mutexCond.unlock();
          // logger.debug("WaterfallWorker::run()", __LINE__, "B NACH: mutexCond.unlock()");
        }
        nSamplesBlock = sb->nSamples;
        iSamplesBlock = 0;
      }
    }
    iFFT = 0;

    Line = fft.calculate(wi.binLeft, wi.nBins, wi.Gain);
    // logger.debug("WaterfallWorker::run()", __LINE__, "%p  emit newRGBLine(); ++++++++++");
    // logger.debug("WaterfallWorker::run()", __LINE__, "%p  emit newRGBLine(); nFFTWidth = %5i   FFT = %c   wi.binLeft = %4i   nBins = %i", this, nFFTWidth, isComplex ? 'C' : 'R', wi.binLeft, wi.nBins);
    emit newRGBLine(Line, LineCnt++);

    while (iStep < nStep) {
      int deltaStep = nStep - iStep;
      int deltaBlock = nSamplesBlock - iSamplesBlock;
      int delta = deltaStep < deltaBlock ? deltaStep : deltaBlock;

      iStep += delta;
      iSamplesBlock += delta;

      if (iSamplesBlock == nSamplesBlock) {
       delete sb;
        while ((sb = pWaterfallFiFo->remove()) == nullptr) {
          // check termination
          if (this->ThreadCmd & WFWORKER_THREAD_CMD_TERM) {
            ThreadCmd = WFWORKER_THREAD_CMD_TERM;
            goto cleanup;
          }
          // wait for data
          // logger.debug("WaterfallWorker::run()", __LINE__, "C VOR : mutexCond.lock()");
          mutexCond.lock();
          waitCondWWThread.wait(&mutexCond);
          mutexCond.unlock();
          // logger.debug("WaterfallWorker::run()", __LINE__, "C NACH: mutexCond.unlock()");
        }
        nSamplesBlock = sb->nSamples;
        iSamplesBlock = 0;
      }
    }
    iStep = 0;
    // logger.debug("WaterfallWorker::run()", __LINE__, " ========= iSamplesBlock = %i", iSamplesBlock);
  }
}
