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

#ifndef DECODER_H
#define DECODER_H

#include <QList>
#include <QMutex>
#include <QWidget>
#include "DecoderThread.h"

class Decoder  {

public:

  Decoder();
  ~Decoder();

  void startDecoder(QString cmd, int JobId);
  bool findTerminatedDecoder(int * pJobId, bool * success);
  void deleteTerminatedDecoders();
  void setMainWindow(QWidget * pMainWindow) { this->pMainWindow = pMainWindow; }

protected :

  QList<DecoderThread *> lstDecoder;
  QMutex  mutex;
  QWidget * pMainWindow;

};

extern Decoder * pDecoder;

#endif // DECODER_H
