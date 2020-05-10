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
