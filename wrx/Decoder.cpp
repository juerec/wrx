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

#include "Decoder.h"
#include "Logger.h"
#include <QThread>


Decoder * pDecoder;

Decoder::Decoder() {
  pDecoder = this;
  pMainWindow = nullptr;
}

Decoder::~Decoder() {
  logger.debug("Decoder::~Decoder()", __LINE__, "IN:Decoder::~Decoder()\n");
}

void Decoder::startDecoder(QString Cmd, int JobId) { // ReceiveJob * pJob) {

  DecoderThread * dt = new DecoderThread(Cmd, JobId);

  mutex.lock();
  lstDecoder.append(dt);
  mutex.unlock();
  QObject::connect(dt, SIGNAL(TextInfo(QString, int)), pMainWindow, SLOT(onTextInfo(QString, int)));
  QObject::connect(dt, SIGNAL(Progress(int, int)), pMainWindow, SLOT(onProgressInfo(int, int)));
  dt->start();
}


bool Decoder::findTerminatedDecoder(int * pJobId, bool * success) {
int  i;

  for (i = lstDecoder.count() - 1; i >= 0; i--) {
    DecoderThread * dt = lstDecoder[i];
    if (dt->isFinished()) {
//      logger.debug("Decoder::findTerminatedDecoder()", __LINE__, "JobId = %i   success = %s   Cmd = %s", dt->JobId, dt->success ? "true" : "false", qPrintable(dt->Cmd));
      *pJobId = dt->jobId();
      *success = dt->success;
      mutex.lock();
      lstDecoder.removeAt(i);
      mutex.unlock();
      delete dt;
      return true;
    }
  }
  return false;
}


// Clean up terminated objects
// Called from inside MainWindow::SignalTimeout()
void Decoder::deleteTerminatedDecoders() {
int  i;

  for (i = lstDecoder.count() - 1; i >= 0; i--) {
    DecoderThread * dt = lstDecoder[i];
    if (dt->isFinished()) {
      mutex.lock();
      lstDecoder.removeAt(i);
      mutex.unlock();
      delete dt;
    }
  }
}
