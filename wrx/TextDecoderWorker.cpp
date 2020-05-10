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

#include "TextDecoderWorker.h"
#include "TextDecoderWorkerIntern.h"
#include "SamplesBlock.h"

#include "Logger.h"


#define TDWORKER_THREAD_CMD_TERM          0x08000000
#define TDWORKER_THREAD_CMD_STOP          0x00000001
#define TDWORKER_THREAD_CMD_NEW_JOB       0x00000002
#define TDWORKER_THREAD_CMD_OPEN_RECORD   0x00000004
#define TDWORKER_THREAD_CMD_CLOSE_RECORD  0x00000008


TextDecoderWorker::TextDecoderWorker(SamplesFiFo * _pTextDecoderFiFo, const QString &rRadioName)
                 : pTextDecoderFiFo(_pTextDecoderFiFo),
                   RadioName(rRadioName) {
  ThisThreadCmd = 0;
  fDecoderRecord = nullptr;
}


void TextDecoderWorker::signalThread() {
//  logger.debug("TextDecoderWorker::signalThread()", __LINE__, "-");
  mutexCond.lock();
  waitCondTDThread.wakeAll();
  mutexCond.unlock();
}


void TextDecoderWorker::termThread() {
  logger.debug("TextDecoderWorker::termThread()", __LINE__, "-");
  ThisThreadCmd = TDWORKER_THREAD_CMD_TERM;
  signalThread();
}


bool TextDecoderWorker::setupDecoder(RadioSetup * pRadioSetup) {
bool ret = true;

  logger.debug("TextDecoderWorker::setupDecoder()", __LINE__, "OpModeIdMode = %s  SamplRate = %i   Channels = %i   CenterFreq = %i   Shift = %i",
               pRadioSetup->frequency->opMode.ModeName(),
               pRadioSetup->SampleRate,
               pRadioSetup->Channels,
               pRadioSetup->CenterFreq,
               pRadioSetup->frequency->opMode.Shift);

  mutexJobInfo.lock();

  ThisThreadCmd = TDWORKER_THREAD_CMD_STOP;
  if (pRadioSetup != nullptr) {
    ThisThreadCmd |= TDWORKER_THREAD_CMD_NEW_JOB;

    DecoderType[0] = 0;
    switch(pRadioSetup->frequency->opMode.IdMode) {
    case OpMode::FAX :
    case OpMode::SPEACH_AM :
    case OpMode::SPEACH_SSB :
    case OpMode::SPEACH_NFM :
    case OpMode::SPEACH_WFM :
    case OpMode::SPEACH :
    case OpMode::OTHER :
    // default :        // invalid modes
      ThisThreadCmd = 0;
      logger.warning("TextDecoderWorker::setupDecoder()", __LINE__, "Calling setupDecoder() with invalid Op-Mode %s", OpMode::Names[pRadioSetup->frequency->opMode.IdMode]);
      ret = false;
      break;
    case OpMode::RTTY_85 :
    case OpMode::RTTY_450 :
      strcpy(DecoderType, "rtty");
      WindowSize  = pRadioSetup->WindowSize;
      break;
    case OpMode::NAVTEX :
      strcpy(DecoderType, "navtex");
      WindowSize  = pRadioSetup->WindowSize;
    }

    if (ThisThreadCmd != 0) {
      if (pRadioSetup->Channels == 2)
        strcat(DecoderType, "-iq");
      SampleRate  = pRadioSetup->SampleRate;
      FFTSize     = pRadioSetup->FFTSize;
      iWindowFunc = pRadioSetup->iWindowFunc;
      CenterFreq  = pRadioSetup->CenterFreq;
      Shift       = pRadioSetup->frequency->opMode.Shift;
      xchg        = false;
    }
  }
  mutexJobInfo.unlock();
  if (ThisThreadCmd != 0)
    signalThread();
  return ret;
}


bool TextDecoderWorker::startRecord(QString newFileName) {
bool ret = false;

  mutexJobInfo.lock();
  if (!FileName.isEmpty()) {
    logger.error("TextDecoderWorker::startRecord()", __LINE__, "Cannot start decoders record, output file is still in use.");
  } else {
    FileName = newFileName;
    ret = true;
    ThisThreadCmd |= TDWORKER_THREAD_CMD_OPEN_RECORD;
  }
  mutexJobInfo.unlock();
  if (ret)
    signalThread();
  return ret;
}


void TextDecoderWorker::stopRecord() {
  ThisThreadCmd |= TDWORKER_THREAD_CMD_CLOSE_RECORD;
  signalThread();
}


void TextDecoderWorker::run() {
SamplesBlock * sb;
TextDecoderWorkerIntern Worker;
int  ThreadCmd = 0;

  logger.debug("TextDecoderWorker::run()", __LINE__, "Is started. Radio: %s ", qPrintable(RadioName));

  while (!(ThreadCmd & TDWORKER_THREAD_CMD_TERM )) {
    if (ThisThreadCmd) {
      mutexJobInfo.lock();
      ThreadCmd = ThisThreadCmd;

      if (ThreadCmd & (TDWORKER_THREAD_CMD_TERM |
                       TDWORKER_THREAD_CMD_STOP |
                       TDWORKER_THREAD_CMD_NEW_JOB |
                       TDWORKER_THREAD_CMD_CLOSE_RECORD))
        if (fDecoderRecord != nullptr) {
          fclose(fDecoderRecord);
          fDecoderRecord = nullptr;
          FileName = "";
        }

      if (ThreadCmd & TDWORKER_THREAD_CMD_TERM)
        break;

      if (ThreadCmd & TDWORKER_THREAD_CMD_STOP)
        Worker.cleanUp();

      if (ThreadCmd & TDWORKER_THREAD_CMD_NEW_JOB) {
        logger.debug("TextDecoderWorker::run()", __LINE__, "VOR:  Worker.init(DecoderType=%s, SampleRate=%i, FFTSize=%i, iWindowFunc=%i, WindowSize=%i, CenterFreq=%i, Shift=%i)",
                     this->DecoderType,this->SampleRate, this->FFTSize, this->iWindowFunc, this->WindowSize, this->CenterFreq, this->Shift);
        Worker.init(this->DecoderType,
                    this->SampleRate,
                    this->FFTSize,
                    this->iWindowFunc,
                    this->WindowSize,
                    this->CenterFreq,
                    this->Shift,
                    this->xchg);
      }

      if (ThreadCmd & TDWORKER_THREAD_CMD_OPEN_RECORD) {
        if (fDecoderRecord != nullptr) {
          fclose(fDecoderRecord);
          fDecoderRecord = nullptr;
        }
        fDecoderRecord = fopen(qPrintable(FileName), "w");
      }

      ThisThreadCmd &= TDWORKER_THREAD_CMD_TERM;
      mutexJobInfo.unlock();
      ThreadCmd = 0;
    }

   //logger.debug("TextDecoderWorker::run()", __LINE__, "VOR:  pTextDecoderFiFo->remove()");
    if ((sb = pTextDecoderFiFo->remove()) == nullptr) {
      //logger.debug("TextDecoderWorker::run()", __LINE__, "VOR:  waitCondTDThread.wait(&mutexCond) (sb == NULL)");
      mutexCond.lock();
      waitCondTDThread.wait(&mutexCond);
      mutexCond.unlock();
      //logger.debug("TextDecoderWorker::run()", __LINE__, "NACH: waitCondTDThread.wait(&mutexCond)  ThreadCmd = 0x%X", ThreadCmd);
      continue;
    }
   //logger.debug("TextDecoderWorker::run()", __LINE__, "NACH: pTextDecoderFiFo->remove() sb != NULL");

    QString str = Worker.processSamples(sb->Samples, sb->nSamples);
    delete sb;
    if (!str.isEmpty()) {
      emit decodedText(str);
      if (fDecoderRecord != nullptr) {
        fputs(qPrintable(str), fDecoderRecord);
        fflush(fDecoderRecord);
      }
    }
  }
  Worker.cleanUp();
  logger.debug("TextDecoderWorker::run()", __LINE__, "Is ended.");
}
