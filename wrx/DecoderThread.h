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

#ifndef DECODERTHREAD_H
#define DECODERTHREAD_H

#include <QThread>
#include "ReceiveJob.h"

class DecoderThread : public QThread {
  Q_OBJECT

public:

  DecoderThread(QString & cmd, int JobId);

  int jobId() { return JobId; }
  bool    success;

protected :

  QString Cmd;
  // ReceiveJob * pJob;
  int JobId;

  virtual void run();

signals :

  void TextInfo(QString, int);
  void Progress(int, int);
  
};

#endif // DECODERTHREAD_H
