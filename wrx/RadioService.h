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
