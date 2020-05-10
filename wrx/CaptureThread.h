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

#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QtCore>

#include "WaterfallWorker.h"
#include "TextDecoderWorker.h"

#define WAVEFILE_WRITE
#include "WaveFile.h"

#define CT_NO_ACTION              0x00000000
#define CT_JOB_START              0x00000001         // Between only, reactivate with current actions
#define CT_JOB_STOP               0x00000002  // Between:  sleep    Test: kill
#define CT_DECODER_START          0x00000004  // ativates live decoder, all types, decoding to text window, maybe soundcard, later
#define CT_DECODER_STOP           0x00000008  // stop decoder, incl. decoder record, for timed job only if not DD
#define CT_DECODER_RECORD_START   0x00000010  // start recording decoder output. implicies decoders start
#define CT_DECODER_RECORD_STOP    0x00000020  // stop recording decoder output, decoding to text window keeps running
#define CT_WAVE_RECORD_START      0x00000040  // start record wave data to file, Test only
#define CT_WAVE_RECORD_STOP       0x00000080  // stop  record wave data to file, Test only


class CaptureThreadCommandInfo;
class CaptureThreadInfo;

class CaptureThread : public QThread {

  Q_OBJECT

signals :

  void ProgressInfo(int Progress);

public:

  CaptureThread(SamplesFiFo * _pWaterfallFiFo,
                SamplesFiFo * _pTextDecoderFiFo,
                WaterfallWorker * pWaterfallWorker,
                TextDecoderWorker * pTextDecoderWorker,
                Radio * pRadio);

  // The public version. Does register for start only
  bool startJob(JobOptions *newJobOptions);
  bool modifyJob(ReceiveJob::JobType whichJob, JobOptionsModify newOptions);
  void stopJob(ReceiveJob::JobType whichJob);
  void termThread();

protected :

//  void readNewCommand(CaptureThreadCommandInfo * pCommandInfo);
  bool startRadio(CaptureThreadInfo * pInfo, JobOptions * pJobOptions);
  void stopRadio(CaptureThreadInfo * pInfo);
  bool startCapture(CaptureThreadInfo * pInfo, JobOptions * pJobOptions);
  void stopCapture(CaptureThreadInfo * pInfo);
  bool startWaterfall(CaptureThreadInfo * pInfo);
  void stopWaterfall(CaptureThreadInfo * pInfo);
  bool startWaveRecord(CaptureThreadInfo * pInfo, JobOptions * pJobOptions);
  void stopWaveRecord(CaptureThreadInfo * pInfo);
  bool startDecoder(CaptureThreadInfo * pInfo, JobOptions * pJobOptions);
  void stopDecoder(CaptureThreadInfo * pInfo);
  bool startRecordTextDecoder(CaptureThreadInfo * pInfo, JobOptions * pJobOptions);
  bool modifyJob(CaptureThreadInfo * pInfo);
  void stopRecordTextDecoder(CaptureThreadInfo * pInfo);
/*
  bool startBetweenJob(CaptureThreadInfo * pInfo);
  void stopBetweenJob(CaptureThreadInfo * pInfo);
  void registerBetweenJob(CaptureThreadInfo * pInfo, ReceiveJob * pNewJob);
*/
  void startPostCommand(JobOptions * pJobOptions);

  // the internal version. It expects all actions are terminated
  // It will start all neccessary actions for the new job by calling the start method for each action
  bool startJob(CaptureThreadInfo * pInfo, JobOptions * pNewJobOptions);
  // It stop a running job by calling the stop method for each active action
  void stopJob(CaptureThreadInfo * pInfo);
  int  progressInfo();
  void signalThread();

  SamplesFiFo         * pWaterfallFiFo;
  SamplesFiFo         * pTextDecoderFiFo;
  WaterfallWorker     * pWaterfallWorker;
  TextDecoderWorker   * pTextDecoderWorker;
  
  // ReceiveJob   * newReceiveJob;
  JobOptions          * pNewJobOptions;

  QMutex                mutexJobInfo;
  QMutex                mutexCond;
  QWaitCondition        waitCondCaptureThread;

  int                   ThreadCmd;
  ReceiveJob::JobType   modifyType;
  JobOptionsModify      modifyOptions;

  Radio               * pRadio;
  class RadioPage     * pRadioPage;
  WaveFile              Wave;

  virtual void run();

signals :

  void     Progress(int, int);
  void     TextInfo(QString, int);
  void     JobStarted(RadioSetup *, int);
  void     JobFinished();
};

#endif // CAPTURETHREAD_H
