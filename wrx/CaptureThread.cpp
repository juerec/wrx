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

#include "CaptureThread.h"

#include "SamplesBlock.h"
#include "SamplesFiFo.h"
#include "Logger.h"
#include "Decoder.h"
#include "ReceiveJobMessage.h"


#define CAPTURE_THREAD_CMD_NOTHING                0x00000000
#define CAPTURE_THREAD_CMD_NEW_JOB                0x00010000
#define CAPTURE_THREAD_CMD_MODIFY_JOB             0x00020000
#define CAPTURE_THREAD_CMD_STOP_JOB               0x00040000
#define CAPTURE_THREAD_CMD_TERM                   0x40000000


class CaptureThreadInfo {
public :

  CaptureThreadInfo();

  int          ThreadCmd;
  JobOptions * pBetweenJobOptions;
  bool         isRadio;
  bool         isCapture;
  bool         isWaterfall;
  bool         isWaveRecord;
  bool         isDecoder;
  bool         isRecordTextDecoder;
  bool         isBetweenJob;
  bool         xyz;
  int          SamplesBufSize;
  int          AlsaFrames;
  short      * SamplesBuf;
  int          readSamples;
  int          SampleRate;
  int          Channels;
  int          DurationSamples;
  int          iProgress;
  int          lastProgress;
  JobOptions * pCurrentJobOptions;
  ReceiveJob::JobType  JobType;
};


CaptureThreadInfo::CaptureThreadInfo() {
    ThreadCmd           = 0;
    pBetweenJobOptions  = nullptr;
    isRadio             = false;
    isCapture           = false;
    isWaterfall         = false;
    isWaveRecord        = false;
    isDecoder           = false;
    isRecordTextDecoder = false;
    isBetweenJob        = false;
    SamplesBufSize      = 0;
    AlsaFrames          = 0;
    SamplesBuf          = nullptr;
    readSamples         = 0;
    SampleRate          = 0;
    Channels            = 0;
    DurationSamples     = 0;
    iProgress           = 0;
    lastProgress        = 0;
    pCurrentJobOptions  = nullptr;
    JobType             = ReceiveJob::JOB_TYPE_NOJOB;
}


CaptureThread::CaptureThread(SamplesFiFo * _pWaterfallFiFo,
                             SamplesFiFo * _pTextDecoderFiFo,
                             WaterfallWorker * _pWaterfallWorker,
                             TextDecoderWorker * _pTextDecoderWorker,
                             Radio * _pRadio)
             : QThread(nullptr),
               pWaterfallFiFo(_pWaterfallFiFo),
               pTextDecoderFiFo(_pTextDecoderFiFo),
               pWaterfallWorker(_pWaterfallWorker),
               pTextDecoderWorker(_pTextDecoderWorker),
               pRadio(_pRadio) {
  ThreadCmd = 0;
  pNewJobOptions = nullptr;
}

// ===========================  public interface ===================================
// The public version. Does register for start only
bool CaptureThread::startJob(JobOptions * newJobOptions) {
bool ret = false;

  mutexJobInfo.lock();
  if (pNewJobOptions == nullptr ||
      // newReceiveJob may still set from prevois startJob() call, which ist not handled by thread, yet
      // timed jobs will not be overwritten, but a new timed job does overwrite test jobs or between jobs
      (newJobOptions->JobType == ReceiveJob::JOB_TYPE_TIMED && pNewJobOptions->JobType != ReceiveJob::JOB_TYPE_TIMED)) {
    if (pNewJobOptions)
      delete pNewJobOptions;
    pNewJobOptions = newJobOptions;
    ThreadCmd = (ThreadCmd & CAPTURE_THREAD_CMD_TERM) | CAPTURE_THREAD_CMD_NEW_JOB;
    ret = true;
  } else {
    logger.info("CaptureThread::startJob()", __LINE__, "There was a unhandled job start overwritten.");
  }
  mutexJobInfo.unlock();
  signalThread();
  return ret;
}


bool CaptureThread::modifyJob(ReceiveJob::JobType whichJob, JobOptionsModify newOptions) {
bool ret = false;

  logger.debug("CaptureThread::modifyJob()", __LINE__, "VOR : mutexJobInfo.lock();");
  mutexJobInfo.lock();
  if (modifyOptions.isEmpty()) {
    modifyType = whichJob;
    modifyOptions = newOptions;
    ThreadCmd |= CAPTURE_THREAD_CMD_MODIFY_JOB;
    ret = true;
  }
  mutexJobInfo.unlock();
  signalThread();
  logger.debug("CaptureThread::modifyJob()", __LINE__, "NACH: mutexJobInfo.lock();");
  return ret;
}


void CaptureThread::stopJob(ReceiveJob::JobType whichJob) {

  // logger.debug("CaptureThread::stopJob()", __LINE__, "VOR : mutexJobInfo.lock();");
  mutexJobInfo.lock();
  ThreadCmd |= CAPTURE_THREAD_CMD_STOP_JOB;
  modifyType = whichJob;
  mutexJobInfo.unlock();
  signalThread();
  // logger.debug("CaptureThread::stopJob()", __LINE__, "NACH: mutexJobInfo.lock();");
}


void CaptureThread::termThread() {

  // logger.debug("CaptureThread::termThread()", __LINE__, "VOR : mutexJobInfo.lock();");
  mutexJobInfo.lock();
  ThreadCmd = CAPTURE_THREAD_CMD_TERM;
  mutexJobInfo.unlock();
  signalThread();
  // logger.debug("CaptureThread::termThread()", __LINE__, "NACH: mutexJobInfo.lock();");
  signalThread();
}

// ===========================  internal functions  ===================================

void CaptureThread::signalThread() {
  mutexCond.lock();
  waitCondCaptureThread.wakeAll();
  mutexCond.unlock();
}


// bool CaptureThread::startRadio(CaptureThreadInfo * pInfo, ReceiveJob * pJob) {
bool CaptureThread::startRadio(CaptureThreadInfo * pInfo, JobOptions * pJobOptions) {
QString ErrorDesc("");

  if (pInfo->isRadio) {
    logger.warning("CaptureThread::startRadio()", __LINE__, "Calling startRadio() while radio is running.");
    pRadio->turnOff();
    pInfo->isRadio = false;
  }
  pInfo->isRadio = pRadio->turnOn(pJobOptions->pRadioSetup, ErrorDesc);
  if (pInfo->isRadio) {
    emit Progress(0, pJobOptions->JobId);
  } else {
    logger.error("CaptureThread::startRadio()", __LINE__, "Radio = %s  error turn on", pRadio == nullptr ? "NULL" : qPrintable(pRadio->Name));
    emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "%s", qPrintable(ErrorDesc)), pJobOptions->JobId);
    emit Progress(PROGRESS_ERROR, pJobOptions->JobId);
  }
  return pInfo->isRadio;
}


void CaptureThread::stopRadio(CaptureThreadInfo * pInfo) {
  pRadio->turnOff();
  pInfo->isRadio = false;
}


bool CaptureThread::startCapture(CaptureThreadInfo * pInfo, JobOptions * pJobOptions) {
bool ok = false;

  logger.debug("CaptureThread::startCapture()", __LINE__, "pInfo->isCapture = %s", pInfo->isCapture ? "on" : "off");
  if (pInfo->isCapture) {
    logger.warning("CaptureThread::startCapture()", __LINE__, "Calling startCapture while capturing is active.");
    pRadio->closeCapture();
    pInfo->isCapture = false;
  }
  RadioSetup * rs = pJobOptions->pRadioSetup;
  if ((ok = pRadio->openCapture(rs->SampleRate, rs->Channels, rs->ChannelMode, rs->SubDevice, rs->ResamplerQuality))) {
    pInfo->SamplesBufSize = rs->SampleRate * rs->Channels >> 3;
    pInfo->AlsaFrames     = pInfo->SamplesBufSize / rs->Channels;
    pInfo->SamplesBuf     = new short [size_t(pInfo->SamplesBufSize)];
    pInfo->readSamples    = 0;
    pInfo->SampleRate     = rs->SampleRate;
    pInfo->Channels       = rs->Channels;
    pInfo->isCapture      = true;
  } else {
    logger.error("CaptureThread::startCapture()", __LINE__, "Error open capture: Radio = %s.", pRadio == nullptr ? "NULL" : qPrintable(pRadio->Name));
    emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "Error open capture: Radio = %s.", pRadio == nullptr ? "NULL" : qPrintable(pRadio->Name)), pJobOptions->JobId);
    emit Progress(PROGRESS_ERROR, pJobOptions->JobId);
  }
  return ok;
}


void CaptureThread::stopCapture(CaptureThreadInfo * pInfo) {

  logger.debug("CaptureThread::stopCapture()", __LINE__, "pInfo->isCapture = %s", pInfo->isCapture ? "on" : "off");
  pInfo->isCapture = false;
  pRadio->closeCapture();
  if (pInfo->SamplesBuf) {
    delete[] pInfo->SamplesBuf;
    pInfo->SamplesBuf = nullptr;
    pInfo->SamplesBufSize = 0;
    pInfo->AlsaFrames = 0;
    pInfo->readSamples = 0;
  }
}


bool CaptureThread::startWaterfall(CaptureThreadInfo * pInfo) {
  pInfo->isWaterfall = true;
  return pInfo->isWaterfall;
}


void CaptureThread::stopWaterfall(CaptureThreadInfo * pInfo) {
  pInfo->isWaterfall = false;
}


bool CaptureThread::startWaveRecord(CaptureThreadInfo * pInfo, JobOptions * pJobOptions) {
bool ok = false;

  logger.debug("CaptureThread::startWaveRecord()", __LINE__, "JobId = %i", pJobOptions->JobId);
  if (pInfo->isWaveRecord) {
    logger.warning("CaptureThread::startWaveRecord()", __LINE__, "Calling startWaveRecord() while wave record is running.");
    pInfo->isWaveRecord = false;
    Wave.close();
  }
  RadioSetup * rs = pJobOptions->pRadioSetup;
  if (pJobOptions->WaveRecordFileName.isEmpty()) {
    logger.warning("CaptureThread::startWaveRecord()", __LINE__, "WaveRecordFileName is empty. JobId = %i", pJobOptions->JobId);
  } else {
    if ((ok = Wave.create(qPrintable(pJobOptions->WaveRecordFileName), rs->SampleRate, rs->Channels, 16))) {
      // pInfo->ExternCommand = pJob->buildExternCommand(pInfo->WaveRecordFileName);
      pInfo->isWaveRecord = true;
    } else {
      logger.error("CaptureThread::startWaveRecord()", __LINE__, "Error open wave file \"%s\" ", qPrintable(pJobOptions->WaveRecordFileName));
      emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "Error open wave file \"%s\" ", qPrintable(pJobOptions->WaveRecordFileName)), pJobOptions->JobId);
      emit Progress(PROGRESS_ERROR, pJobOptions->JobId);
    }
  }
  return ok;
}


void CaptureThread::stopWaveRecord(CaptureThreadInfo * pInfo) {

  logger.debug("CaptureThread::stpoWaveRecord()", __LINE__, "pInfo->isWaveRecord = %s", pInfo->isWaveRecord ? "on" : "off");
  Wave.close();
  pInfo->isWaveRecord = false;
}


bool CaptureThread::startDecoder(CaptureThreadInfo * pInfo, JobOptions * pJobOptions) {

  logger.debug("CaptureThread::startDecoder()", __LINE__, "JobId = %i", pJobOptions->JobId);
  pInfo->isDecoder = pTextDecoderWorker->setupDecoder(pJobOptions->pRadioSetup);
  if (!pInfo->isDecoder) {
    logger.error("CaptureThread::startDecoder()", __LINE__, "Error initialising text decoder for Job %i ", pJobOptions->JobId);
    emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "Error initialising text decoder for Job %i", pJobOptions->JobId), pJobOptions->JobId);
    emit Progress(PROGRESS_ERROR, pJobOptions->JobId);
  }
  return pInfo->isDecoder;
}

void CaptureThread::stopDecoder(CaptureThreadInfo * pInfo) {
  logger.debug("CaptureThread::stopDecoder()", __LINE__, ". . . . ");
  pInfo->isDecoder = false;
}

bool CaptureThread::startRecordTextDecoder(CaptureThreadInfo * pInfo, JobOptions * pJobOptions) {

  logger.debug("CaptureThread::startRecordTextDecoder()", __LINE__, "pInfo->isRecordTextDecoder = ", pInfo->isRecordTextDecoder ? "on" : "off");
  // try to open the file named pJobOptions->DecoderRecordFileName, and close it atfterthem
  // If it works, we have good chance the text docoder can do also. The most error conditions should accore, if any.
  // We cannot do it self because another thread. Now, it runs asynchron and cannot return errors to us. The only error condition reported is a already opened output file.
  pInfo->isRecordTextDecoder = false;
  FILE * f = fopen(qPrintable(pJobOptions->DecoderRecordFileName), "w");
  if (f != nullptr) {
    fclose(f);
    pInfo->isRecordTextDecoder = pTextDecoderWorker->startRecord(pJobOptions->DecoderRecordFileName);
  }
  return pInfo->isRecordTextDecoder;
}


void CaptureThread::stopRecordTextDecoder(CaptureThreadInfo * pInfo) {

  logger.debug("CaptureThread::stopRecordTextDecoder()", __LINE__, "pInfo->isRecordTextDecoder = %s", pInfo->isRecordTextDecoder ? "on" : "off");
  if (pInfo->isRecordTextDecoder) {
    pTextDecoderWorker->stopRecord();
    pInfo->isRecordTextDecoder = false;
  }
}

/*
// Implementation later
void CaptureThread::stopBetweenJob(CaptureThreadInfo * pInfo) {

  if (pInfo->pCurrentJob != nullptr && pInfo->pCurrentJob == pInfo->pBetweenJob)
    stopJob(pInfo);
  pInfo->isBetweenJob = false;
}

void CaptureThread::registerBetweenJob(CaptureThreadInfo * pInfo, ReceiveJob * pNewJob) {
bool isBetweenJob = pInfo->isBetweenJob;

  stopBetweenJob(pInfo);
  pInfo->pBetweenJob = pNewJob;
  if (pNewJob != nullptr &&
      pNewJob->Type == ReceiveJob::JOB_TYPE_BETWEEN &&
      isBetweenJob) {
    startJob(pInfo, pInfo->pBetweenJob);
    pInfo->isBetweenJob = true;
  }
}
*/


// void CaptureThread::startPostCommand(CaptureThreadInfo * pInfo) {
void CaptureThread::startPostCommand(JobOptions * pJobOptions) {

  if (!pJobOptions->PostCommand.isEmpty())
    pDecoder->startDecoder(pJobOptions->PostCommand, pJobOptions->JobId);
}


bool CaptureThread::startJob(CaptureThreadInfo * pInfo, JobOptions * pNewJobOptions) {
bool ok = false;

  // Print a warning when any thing is working.
  // Before a call to startJob(), all action should be stopped
  if (pInfo->isRadio | pInfo->isCapture | pInfo->isWaterfall |
      pInfo->isWaveRecord | pInfo->isDecoder | pInfo->isRecordTextDecoder)
    logger.warning("CaptureThread::startJob()", __LINE__, "Calling startJob() while running activities:\n"
                   "     isRadio: %s\n"
                   "     isCapture: %s\n"
                   "     isWaterfall: %s\n"
                   "     isWaveRecord: %s\n"
                   "     isDecoder: %s\n"
                   "     isRecordTextDecoder: %s",
                   pInfo->isRadio ? "on" : "off",
                   pInfo->isCapture ? "on" : "off",
                   pInfo->isWaterfall ? "on" : "off",
                   pInfo->isWaveRecord ? "on" : "off",
                   pInfo->isDecoder ? "on" : "off",
                   pInfo->isRecordTextDecoder ? "on" : "off");

  // We do not kill running timed jobs.
  if (pInfo->pCurrentJobOptions != nullptr && pInfo->pCurrentJobOptions->JobType == ReceiveJob::JOB_TYPE_TIMED) {
    // register between job, but do not start
    if (pNewJobOptions->JobType == ReceiveJob::JOB_TYPE_BETWEEN) {
      if (pInfo->pBetweenJobOptions != nullptr)
        delete pInfo->pBetweenJobOptions;
      pInfo->pBetweenJobOptions = pNewJobOptions;
      logger.info("CaptureThread::startJob()", __LINE__, "Between job registered, but not started because a timed job is running.");
      ok = true;
    } else {
      logger.error("CaptureThread::startJob()", __LINE__, "Attempt to start a new job while a timed job is running.");
      delete pNewJobOptions;
    }
    pNewJobOptions = nullptr;
    return ok;
  }
  // Clear out all running actions.
  if (pInfo->isRadio)
    stopRadio(pInfo);
  if (pInfo->isCapture)
    stopCapture(pInfo);
  if (pInfo->isWaterfall)
    stopWaterfall(pInfo);
  if (pInfo->isWaveRecord)
    stopWaveRecord(pInfo);
  if (pInfo->isDecoder)
    stopDecoder(pInfo);
  if (pInfo->isRecordTextDecoder)
    stopRecordTextDecoder(pInfo);
  pInfo->iProgress = -1;
  pInfo->lastProgress = -1;
  if (pInfo->pCurrentJobOptions != nullptr) {
    delete pInfo->pCurrentJobOptions;
    pInfo->pCurrentJobOptions = nullptr;
  }
  // always start Radio, Capture, Waterfall
  if (startRadio(pInfo, pNewJobOptions) &&
      startCapture(pInfo, pNewJobOptions) &&
      startWaterfall(pInfo)) {
    ok = true;
    // pInfo->pCurrentJobOptions = pNewJobOptions;
    if (pNewJobOptions->isWaveRecord)
      ok = startWaveRecord(pInfo, pNewJobOptions);
    if (ok && pNewJobOptions->isDecoder)
      ok = startDecoder(pInfo, pNewJobOptions);
    if (ok && pNewJobOptions->isDecoderRecord)
      ok = startRecordTextDecoder(pInfo, pNewJobOptions);
  }
  if (ok) {
    pInfo->JobType            = (ReceiveJob::JobType)pNewJobOptions->JobType;
    pInfo->pCurrentJobOptions = pNewJobOptions;
    pInfo->DurationSamples    = pNewJobOptions->JobType != ReceiveJob::JOB_TYPE_TIMED ? 0 : pNewJobOptions->pRadioSetup->SampleRate * pNewJobOptions->pRadioSetup->Channels * pNewJobOptions->Duration;
    logger.debug("CaptureThread::startJob()", __LINE__, "New Job started:\n"
                                                        "        isRadio              = %s\n"
                                                        "        isCapture            = %s\n"
                                                        "        isWaterfall          = %s\n"
                                                        "        isWaveRecord         = %s\n"
                                                        "        isDecoder            = %s\n"
                                                        "        isRecordTextDecoder  = %s\n"
                                                        "        WaveRecordFileName   = %s\n"
                                                        "        DecoderOutFileName   = %s\n"
                                                        "        PostCommand           = %s\n",
                                                        pInfo->isRadio ? "on" : "off",
                                                        pInfo->isCapture ? "on" : "off",
                                                        pInfo->isWaterfall ? "on" : "off",
                                                        pInfo->isWaveRecord ? "on" : "off",
                                                        pInfo->isDecoder ? "on" : "off",
                                                        pInfo->isRecordTextDecoder ? "on" : "off",
                                                        qPrintable(pNewJobOptions->WaveRecordFileName),
                                                        qPrintable(pNewJobOptions->DecoderRecordFileName),
                                                        qPrintable(pNewJobOptions->PostCommand));
  } else {
    emit Progress(PROGRESS_ERROR, pNewJobOptions->JobId);
    stopDecoder(pInfo);
    stopWaveRecord(pInfo);
    stopWaterfall(pInfo);
    stopCapture(pInfo);
    stopRadio(pInfo);
  }
  return ok;
}


bool CaptureThread::modifyJob(CaptureThreadInfo * pInfo) {
bool ret = false;

  switch(modifyType) {
  case ReceiveJob::JOB_TYPE_TIMED :
    logger.error("CaptureThread::modifyJob()", __LINE__, "modifyType = ReceiveJob::JOB_TYPE_TIMED");
    // Is a job running?
    if (pInfo->pCurrentJobOptions != nullptr && pInfo->isCapture) {
      ret = true;
      // only internal Decoder is supported
      if (modifyOptions.aDecoder == JobOptionsModify::Start) {
        if (!pInfo->isDecoder) {
          pInfo->pCurrentJobOptions->isDecoder = true;
          ret = startDecoder(pInfo, pInfo->pCurrentJobOptions);
        }
      }
      if (modifyOptions.aDecoder == JobOptionsModify::Stop) {
        pInfo->pCurrentJobOptions->isDecoder = false;
        stopDecoder(pInfo);
      }
    }
    break;
  case ReceiveJob::JOB_TYPE_TEST :
    logger.error("CaptureThread::modifyJob()", __LINE__, "modifyType = ReceiveJob::JOB_TYPE_TEST");
    // Is a job running?
    if (pInfo->pCurrentJobOptions != nullptr && pInfo->isCapture) {
      ret = true;
      // internal Decoder
      if (modifyOptions.aDecoder == JobOptionsModify::Start) {
        if (!pInfo->isDecoder) {
          pInfo->pCurrentJobOptions->isDecoder = true;
          ret = startDecoder(pInfo, pInfo->pCurrentJobOptions);
        }
      }
      if (modifyOptions.aDecoder == JobOptionsModify::Stop) {
        pInfo->pCurrentJobOptions->isDecoder = false;
        stopDecoder(pInfo);
      }
      // manual wave record
      if (modifyOptions.aWaveRecord == JobOptionsModify::Start) {
        if (!pInfo->isWaveRecord) {
          pInfo->pCurrentJobOptions->WaveRecordFileName = modifyOptions.WaveRecordFileName;
          pInfo->pCurrentJobOptions->isWaveRecord = true;
          ret = startWaveRecord(pInfo, pInfo->pCurrentJobOptions);
        }
      }
      if (modifyOptions.aWaveRecord == JobOptionsModify::Stop) {
        stopWaveRecord(pInfo);
        pInfo->pCurrentJobOptions->isWaveRecord = false;
      }
      if (modifyOptions.aDecoderRecord == JobOptionsModify::Start) {
        pInfo->pCurrentJobOptions->DecoderRecordFileName = modifyOptions.DecoderRecordFileName;
        pInfo->pCurrentJobOptions->isDecoderRecord = true;
        ret = startRecordTextDecoder(pInfo, pInfo->pCurrentJobOptions);
      }
      if (modifyOptions.aDecoderRecord == JobOptionsModify::Stop) {
        pInfo->pCurrentJobOptions->isDecoderRecord = false;
        stopRecordTextDecoder(pInfo);
      }
      if (modifyOptions.aRunPostCommand == JobOptionsModify::Start &&
          !pInfo->pCurrentJobOptions->PostCommand.isEmpty() &&
          pInfo->isWaveRecord) {
        pInfo->pCurrentJobOptions->isRunPostCommand = true;
      }
      if (modifyOptions.aRunPostCommand == JobOptionsModify::Stop) {
        pInfo->pCurrentJobOptions->isRunPostCommand = false;
      }
    }
    break;
  case ReceiveJob::JOB_TYPE_BETWEEN :
    logger.info("CaptureThread::modifyJob()", __LINE__, "Modifying between job options is not implemented, yet");
    ret = false;
    break;
  case ReceiveJob::JOB_TYPE_NOJOB :
    logger.warning("CaptureThread::modifyJob()", __LINE__, "Modifying job options, but no job is running");
    ret = false;
  }
  modifyOptions.clear();
  return ret;
}

void CaptureThread::stopJob(CaptureThreadInfo * pInfo) {

  stopDecoder(pInfo);
  stopWaterfall(pInfo);
  stopCapture(pInfo);
  stopRadio(pInfo);
  stopWaveRecord(pInfo);
  stopRecordTextDecoder(pInfo);
  // if between is running, pCurrentJobOptions is copy of pBetweenJobOptions!
  if (pInfo->pCurrentJobOptions != pInfo->pBetweenJobOptions)
    delete pInfo->pCurrentJobOptions;
  pInfo->pCurrentJobOptions = nullptr;
}


void CaptureThread::run() {
CaptureThreadInfo Info;

  if (pWaterfallFiFo == nullptr ||
      pTextDecoderFiFo == nullptr ||
      pWaterfallWorker == nullptr ||
      pTextDecoderWorker == nullptr ||
      pRadio == nullptr) {
    logger.error("CaptureThread::run()", __LINE__, "Invalid configuration, cannot run CaptureThread.");
    return;
  }

  while (!(ThreadCmd & CAPTURE_THREAD_CMD_TERM)) {
    // go sleeping when nothing to do
    if (!Info.isCapture && !this->ThreadCmd) {
      // logger.debug("CaptureThread::run()", __LINE__, "CaptureThread ==> wait (%s)\n", radio != nullptr ? qPrintable(radio->Name) : "NULL");
      mutexCond.lock();
      // CaptureThread::startJob() and CaptureThread::termThread() does signal a wake up, no others
      waitCondCaptureThread.wait(&mutexCond);
      mutexCond.unlock();
      // logger.debug("CaptureThread::run()", __LINE__, "CaptureThread ==> awake (%s)\n", radio != nullptr ? qPrintable(radio->Name) : "NULL");
    }

    // program termination?
    if (ThreadCmd & CAPTURE_THREAD_CMD_TERM)
      // skip the working part of while loop
      continue;

    // Stop a running (test or between) job
    if (ThreadCmd & CAPTURE_THREAD_CMD_STOP_JOB) {
      if (Info.pCurrentJobOptions != nullptr && !(Info.pCurrentJobOptions->JobType ==  ReceiveJob::JOB_TYPE_TIMED)) {
        if (Info.pCurrentJobOptions->JobType == modifyType)  // modifyType could be JOB_TYPE_BETWEEN while a test job is runnig
          stopJob(&Info);
      }
      ThreadCmd &= CAPTURE_THREAD_CMD_TERM; // clear all bits, except TERM
      modifyType = ReceiveJob::JOB_TYPE_NOJOB;
    }

    // new Job ?  (all types)
    if (ThreadCmd & CAPTURE_THREAD_CMD_NEW_JOB) {
      mutexJobInfo.lock();
      ThreadCmd &= CAPTURE_THREAD_CMD_TERM; // clear all bits, except TERM

      if (startJob(&Info, pNewJobOptions))
        emit JobStarted(Info.pCurrentJobOptions->pRadioSetup, int(Info.JobType));
      pNewJobOptions = nullptr;
      mutexJobInfo.unlock();
    }
    // change options from running job
    if (ThreadCmd & CAPTURE_THREAD_CMD_MODIFY_JOB) {
      mutexJobInfo.lock();
      ThreadCmd &= CAPTURE_THREAD_CMD_TERM; // clear all bits, except TERM
      modifyJob(&Info);
      logger.debug("CaptureThread::run()", __LINE__, "NACH: modifyJob()  ThreadCmd = 0X%08X", ThreadCmd);
      mutexJobInfo.unlock();
    }

    // nothing to do?       program termination?
    if (!Info.isCapture || (ThreadCmd & CAPTURE_THREAD_CMD_TERM))
      // skip the working part of while loop
      continue;

    // END of administrative part
    // ============================================================
    // START of working part

    if (!pRadio->readCapture(Info.SamplesBuf, Info.AlsaFrames)) {
            // errors already logged by radio->readCapture()
      emit Progress(PROGRESS_ERROR, Info.pCurrentJobOptions->JobId);
      emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "%s", qPrintable(tr("Error reading from sound device."))), Info.pCurrentJobOptions->JobId);
      stopJob(&Info);
      continue;
    }
    if (Info.JobType == ReceiveJob::JOB_TYPE_TIMED)
      Info.readSamples += Info.SamplesBufSize;

    if (Info.isWaterfall) {
      SamplesBlock * sb = new SamplesBlock(Info.SamplesBuf, Info.SamplesBufSize, Info.SampleRate, Info.Channels);
        // logger.debug("CaptureThread::run()", __LINE__, "NACH new SamplesBlock(SamplesBuf, SamplesBufSize=%i, SampleRate=%i, Channels=%i)", SamplesBufSize, SampleRate, ChannelsWork);
      pWaterfallFiFo->append(sb);
      pWaterfallWorker->signalThread();
        // logger.debug("CaptureThread::run()", __LINE__, "radio = %s   NACH : pWaterfallWorker->signalThread()", qPrintable(radio->Name));
    }

    if (Info.isDecoder) {
      SamplesBlock * sb = new SamplesBlock(Info.SamplesBuf, Info.SamplesBufSize, Info.SampleRate, Info.Channels);
      // logger.debug("CaptureThread::run()", __LINE__, "NACH Info.isDecoder: new SamplesBlock(SamplesBuf, SamplesBufSize=%i, SampleRate=%i, Channels=%i)", Info.SamplesBufSize, Info.SampleRate, Info.Channels);
      pTextDecoderFiFo->append(sb);
      pTextDecoderWorker->signalThread();
    }

    if (Info.isWaveRecord) {
      Wave.writeSamples(Info.SamplesBuf, Info.SamplesBufSize);
    }

    if (Info.JobType == ReceiveJob::JOB_TYPE_TIMED) {
      // logger.debug("CaptureThread::run()", __LINE__, "Info.readSamples = %i    Info.DurationSamples = %i", Info.readSamples, Info.DurationSamples);
      float prog = float(Info.readSamples) / float(Info.DurationSamples) * 100.0f + 0.5f;
      Info.iProgress = int(prog);
      if (Info.iProgress >= 100)
        Info.iProgress = 100;     // a value of 100 indicates succesfully closed .wav ==> the next step
      if (Info.iProgress != Info.lastProgress) {
        emit Progress(Info.iProgress, Info.pCurrentJobOptions->JobId);
        // logger.debug("CaptureThread::run()", __LINE__, "emit Progress(%i, %i)", Info.iProgress, Info.pCurrentJob->Id);
        Info.lastProgress = Info.iProgress;
      }
      if (prog >= 100.0f) { // the job's end reached ?
        emit JobFinished();
        logger.debug("CaptureThread::run()", __LINE__, "prog >= 100.0f ?   prog = %8.2f", prog);
        logger.debug("CaptureThread::run()", __LINE__, "currentJobOptions: isWaveRecor = %s   isDecoder = %s   isDecoderRecord = %s   isRunPostCommand = %s",
                     Info.pCurrentJobOptions->isRunPostCommand ? "on" : "off",
                     Info.pCurrentJobOptions->isDecoder ? "on" : "off",
                     Info.pCurrentJobOptions->isDecoderRecord ? "on" : "off",
                     Info.pCurrentJobOptions->isRunPostCommand ? "on" : "off");
        if (Info.pCurrentJobOptions->isRunPostCommand)
          startPostCommand(Info.pCurrentJobOptions);
        else
          emit Progress(PROGRESS_OK, Info.pCurrentJobOptions->JobId);  // there is no post processing step
        stopJob(&Info);
      }
    }
  }
}
