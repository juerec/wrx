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

#ifndef TEXTDECODERWORKER_H
#define TEXTDECODERWORKER_H

#include <QtCore>
#include <QMutex>
#include <QWaitCondition>

#include "ReceiveJob.h"
#include "SamplesFiFo.h"


class TextDecoderWorker : public QThread {

  Q_OBJECT

public:

  TextDecoderWorker(SamplesFiFo * _pTextDecoderFiFo, const QString &rRadioName);
  void signalThread();
  void termThread();
  bool setupDecoder(RadioSetup *pRadioSetup);
  bool startRecord(QString FileName);
  void stopRecord();
  bool openDecoderFile(const QString & FileName);
  void closeDecoderFile();

  QMutex         mutexCond;
  QWaitCondition waitCondTDThread;

  SamplesFiFo * pTextDecoderFiFo;

signals :

  void decodedText(QString);

protected :

  QMutex         mutexJobInfo;

  virtual void run();
  int           ThisThreadCmd;

  char          DecoderType[16];
  int           SampleRate;
  int           FFTSize;
  int           iWindowFunc;
  int           WindowSize;
  int           CenterFreq;
  int           Shift;
  bool          xchg;

  QString       RadioName;
  QString       FileName;
  FILE        * fDecoderRecord;

};

#endif // TEXTDECODERWORKER_H
