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

#ifndef RADIO_H
#define RADIO_H

#include <QtCore>

#include "Options.h"
#include "SoundDeviceInfo.h"
#include "SoundCard.h"
#include "Options.h"

class RadioSetup;

class Radio : public SoundDeviceInfo {

protected :

  Radio();
  Radio(int Id, QString name, QStringList & lstSetup);
  ~Radio();

  static Radio  Dummy;

public:

  static QList<Radio *> lstRadios;

  static void    init(QList<OptionsList> & lstConfig, QList<OptionsList> & lstSetup);
  static void    clear();
  static Radio * find(QString name);
  static bool    exists(QString name);
  static void    saveRadios();

  void           init(QStringList & lstSetup);
  void           valuesToSave(QStringList & values);

  QString        SoundCardNrFromSerial();
  bool           runCmd(QString Cmd, RadioSetup * radioSetup, QString & stdOut);
  bool           turnOn(RadioSetup * radioSetup, QString &ErrorDesc);
  void           turnOff();
  bool           openCapture(int Samplerate, int Channels, int ChannelMode, int SubDevice, int ResamplerQuality);
  void           closeCapture();
  bool           readCapture(short * Buffer, int max);

  int      Id;
  bool     Enable;
  QString  Name;
  QString  NameLong;
  QString  CmdStart;
  QString  CmdStop;
  QString  CmdCheck;

protected :

  class SoundCard SoundCard;

};

#endif // RADIO_H
