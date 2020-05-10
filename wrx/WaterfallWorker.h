#ifndef WATERFALLWORKER_H
#define WATERFALLWORKER_H

#include <QtCore>
#include "SamplesFiFo.h"
#include "RGBLineFiFo.h"
#include "WaterfallInfo.h"


class WaterfallWorker : public QThread {

  Q_OBJECT

public :
  WaterfallWorker(SamplesFiFo * pWaterfallFiFo, RGBLineFiFo * pRGBLineFiFo); //, const QString & rRadioNAme);

  void signalThread();
  void termThread();
  void setupWaterfall(WaterfallInfo wi);
  // void activateReceiveJob(ReceiveJob * job);
  void activateReceiveJob(int SampleRate, int Channels);

  QMutex         mutexCond;
  QWaitCondition waitCondWWThread;

  SamplesFiFo * pWaterfallFiFo;
  RGBLineFiFo   * pRGBLineFiFo;
  WaterfallInfo   wi;
  int    FFTWidth;
  int    isComplex;
  int    binLeft;
  int    nPixel;   // <= FFTWidth !!
  int    SampleRate;
  int    Channels;
  // ReceiveJob * receiveJob;
  // QString RadioName;

signals :

  void newRGBLine(RGBLine *, int);

protected :

  QMutex         mutexJobInfo;

  virtual void run();
  int  ThreadCmd;

};

#endif // WATERFALLWORKER_H
