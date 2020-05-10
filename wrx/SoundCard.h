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

#ifndef SOUNDCARD_H
#define SOUNDCARD_H

#include "QtCore"
#include <alsa/asoundlib.h>
#include <samplerate.h>
#include "SoundDeviceInfo.h"

class SoundCardInfo {

public :

  SoundCardInfo() {}
  SoundCardInfo(int cardNr, QString & cardName, QString & physicalPath)
       : CardNr(cardNr), CardName(cardName), PhysicalPath(physicalPath), SerialNr("") {}

  int      CardNr;
  QString  CardName;
  QString  PhysicalPath;
  QString  SerialNr;
};


class SoundCard {

public :

  SoundCard();
  SoundCard(int CardNr);
  SoundCard(int CardNr, int DevNr);
  SoundCard(QString & CardNr, int SubDevice);
  ~SoundCard();
  static SoundCard findByCardNr(QString & CardNr, int SubDevice);
  static SoundCard findByCardName(QString name, int SubDevice);
  static SoundCard findByPhysicalPath(QString path, int SubDevice);
  static SoundCard findBySerialNr(QString & SerialNr, QString & RadioName, int SubDevice);
  static SoundCard findCard(SoundDeviceInfo & devInfo, QString & RadioName, int SubDevice);

  static QList<SoundCardInfo> listSoundCardInfo();
//  static void refreshSoundCardInfo();

  static QStringList listSoundCardAlsaDevs();
  static QStringList listSoundCardNames();
  static QStringList listSoundCardPhysicalPaths();
  static QStringList listSoundCardSerials();

  static QList<SoundCardInfo> lstSoundCardInfo;

  static const char  AudioIdentType_CARDNR[];
  static const char  AudioIdentType_CARDNAME[];
  static const char  AudioIdentType_PHYSICALPATH[];
  static const char  AudioIdentType_SERIALNR[];

/*
  static QString findCardName(int CardNr);
  static QString findPhysicalPath(int CardNr);
  static QList<int> findCards();
*/
  void clear();

  bool isValid();
  bool openCapture(uint Samplerate, int Channels, int ChannelMode, snd_pcm_format_t Format, int ResamplerQuality);
  bool readCapture(short * Buffer, int OutBufferSize);
  void closeCapture();

protected :

  bool        initResampler(uint SampleRate, int Channels, double ResamplRatio, int ResamplerQuality);
  void        freeResampler();
  bool        readCaptureDirect(short * Buffer, long OutBufferSize);
  bool        readCaptureResample(short * Buffer, int OutBufferSize);

  char        HwDeviceName[32];
  snd_pcm_t * capture_handle;
  uint        SampleRate;         // rate delivered to client
  uint        actualSampleRate;   // rate on capture device
  int         Channels;
  int         ChannelMode;        // 0: capt. channels
                                  // 1: capt. stereo, use L
                                  // 2: capt. stereo, use R
  bool        isResample;
  short     * InBufferResample;
  SRC_STATE * src_state;
  SRC_DATA    src_data;
  float     * data_in;            // a writeable shadow for the readonly src_data.data_in
  long        ResampleOutputPos;
  short     * InBufferLR;
  long        InBufferLRSize;
};


#endif // SOUNDCARD_H
