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

#ifndef RADIOSERVICE_H
#define RADIOSERVICE_H

#include <QtCore>
/*
#include "WaterfallWorker.h"
#include "TextDecoderWorker.h"
#include "ReceiveJob.h"
*/
#include "CaptureThread.h"


class QWidget;

class RadioService {

public:

  RadioService(Radio * pRadio);
  ~RadioService();

  static QList<RadioService *> lstRadioService;
  static void initServices();
  static RadioService * findRadioService(QString radio);

  void   activateReceiveJob(JobOptions * pJobOptions) {  CaptureThread.startJob(pJobOptions); }

  void   startTestJob(JobOptions * pJobOptions) {  CaptureThread.startJob(pJobOptions); }
  void   stopTestJob() { CaptureThread.stopJob(ReceiveJob::JOB_TYPE_TEST); }
/*
  bool   modifyTestJob(JobOptionsModify::Action aDecoder, JobOptionsModify::Action aRecordDecoder, JobOptionsModify::Action aRecordWave, JobOptionsModify::Action aRunExternalDecoder) {
             return CaptureThread.modifyJob(ReceiveJob::JOB_TYPE_TEST, JobOptionsModify(aDecoder, aRecordDecoder, aRecordWave, aRunExternalDecoder));
         }
*/
  bool   modifyTestJob(JobOptionsModify & jom) { return CaptureThread.modifyJob(ReceiveJob::JOB_TYPE_TEST, jom); }
  void   startBetweenJob(JobOptions * pJobOptions) {  CaptureThread.startJob(pJobOptions); }
  bool   modifyBetweenJob(JobOptionsModify::Action aDecoder, JobOptionsModify::Action aRecordDecoder, JobOptionsModify::Action aRecordWave, JobOptionsModify::Action aRunExternalDecoder) {
              return CaptureThread.modifyJob(ReceiveJob::JOB_TYPE_BETWEEN, JobOptionsModify(aDecoder, aRecordDecoder, aRecordWave, aRunExternalDecoder));
         }
  void   stopBetweenJob() { CaptureThread.stopJob(ReceiveJob::JOB_TYPE_BETWEEN); }

  // static void staticConnectProgress(QObject * pMainWindow); renamed to:
  static void staticConnectCaptureThreads(QObject * pMainWindow);
  static void staticTermThreads();

  // void connectProgress(QObject * pMainWindow); renamed to:
  void connectCaptureThread(QObject * pMainWindow);
  void setupWaterfall(WaterfallInfo wi) { WaterfallWorker.setupWaterfall(wi); }
  void connectWaterfall(QObject * pRadioTabPage);
  void connectTextDecoder(QObject * pEditText);
  void connectCaptureThreadStartStop(QObject * pRadioPage);

  void onWaterfallZoom(int newZoom);
  void onWaterfallGain(int newGain);
  void onWaterfallRange(int newRange);


protected :

  void termThreads();
  void waitThreads();

  Radio * pRadio;
  class RGBLineFiFo       RGBLineFiFo;
  class SamplesFiFo       WaterfallFiFo;
  class SamplesFiFo       TextDecoderFiFo;
  class WaterfallWorker   WaterfallWorker;
  class TextDecoderWorker TextDecoderWorker;
  class CaptureThread     CaptureThread;
};

#endif // RADIOSERVICE_H
