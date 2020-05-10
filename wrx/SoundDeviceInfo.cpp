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

#include "SoundDeviceInfo.h"
#include "SoundCard.h"

SoundDeviceInfo::SoundDeviceInfo() {
QStringList lstDummy;

  init(lstDummy);
}

SoundDeviceInfo::SoundDeviceInfo(QStringList & lstValues) {
  init(lstValues);
}

void SoundDeviceInfo::init(QStringList & lstValues) {
int i;

  AudioIdentType = "";
  SoundCardName  = "";
  SoundCardAlsaDev = "";
  SoundCardPhysicalPath = "";
  SerialNr = "";

  for (i = 0; i < lstValues.length(); i++) {
    QString & str = lstValues[i];
    if (str.startsWith("AudioIdentType=")) {
      AudioIdentType = str.mid(15);
      if (AudioIdentType.isEmpty() || (AudioIdentType != SoundCard::AudioIdentType_CARDNR &&
                                       AudioIdentType != SoundCard::AudioIdentType_CARDNAME &&
                                       AudioIdentType != SoundCard::AudioIdentType_PHYSICALPATH &&
                                       AudioIdentType != SoundCard::AudioIdentType_SERIALNR)) {

         AudioIdentType = SoundCard::AudioIdentType_CARDNR;
      }
    }
    if (str.startsWith("SoundCardName="))
      SoundCardName = str.mid(14);
    if (str.startsWith("SoundCardAlsaDev="))
      SoundCardAlsaDev = str.mid(17).toInt();
    if (str.startsWith("SoundCardPhysicalPath="))
      SoundCardPhysicalPath = str.mid(22);
    if (str.startsWith("SerialNr="))
      SerialNr = str.mid(9);
  }
}

void SoundDeviceInfo::valuesToSave(QStringList & lstValues) {
  lstValues.append("AudioIdentType=" + AudioIdentType);
  lstValues.append("SoundCardName=" + SoundCardName);
  lstValues.append("SoundCardAlsaDev=" + SoundCardAlsaDev);
  lstValues.append("SoundCardPhysicalPath=" + SoundCardPhysicalPath);
  lstValues.append("SerialNr=" + SerialNr);
}

QString SoundDeviceInfo::AudioIdent() {
  if (AudioIdentType == SoundCard::AudioIdentType_CARDNR)
    return SoundCardAlsaDev;
  if (AudioIdentType == SoundCard::AudioIdentType_CARDNAME)
    return SoundCardName;
  if (AudioIdentType == SoundCard::AudioIdentType_PHYSICALPATH)
    return SoundCardPhysicalPath;
  if (AudioIdentType == SoundCard::AudioIdentType_SERIALNR)
    return SerialNr;
  return "Invalid AudioIdentType \"" + AudioIdentType + "\"";
}
