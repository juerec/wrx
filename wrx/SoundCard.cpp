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

#include "SoundCard.h"
#include "Radio.h"
#include "Logger.h"
#include "Options.h"

const char  SoundCard::AudioIdentType_CARDNR[] = "alsa.cardnr";
const char  SoundCard::AudioIdentType_CARDNAME[] = "card.name";
const char  SoundCard::AudioIdentType_PHYSICALPATH[] = "physical.path";
const char  SoundCard::AudioIdentType_SERIALNR[] = "serial.no";

const char AccessType[] = "hw";

QList<SoundCardInfo> SoundCard::lstSoundCardInfo;



SoundCard::SoundCard() {
  HwDeviceName[0] = 0;
  capture_handle  = nullptr;
  isResample      = false;
  InBufferLR      = nullptr;
  InBufferLRSize  = 0;
}

SoundCard::SoundCard(int CardNr) {
  if (CardNr < 0)
    HwDeviceName[0] = 0;
  else
    sprintf(HwDeviceName, "%s:%i", AccessType, CardNr);
  capture_handle  = nullptr;
  isResample      = false;
  InBufferLR      = nullptr;
  InBufferLRSize  = 0;
}

SoundCard::SoundCard(int CardNr, int DevNr) {
  if (CardNr < 0 || DevNr < 0)
    HwDeviceName[0] = 0;
  else
    sprintf(HwDeviceName, "%s:%i,%i", AccessType, CardNr, DevNr);
  capture_handle  = nullptr;
  isResample      = false;
  InBufferLR      = nullptr;
  InBufferLRSize  = 0;
}

SoundCard::SoundCard(QString & CardNr, int SubDevice) {
  if (CardNr.isEmpty() || CardNr.toInt() < 0)
    HwDeviceName[0] = 0;
  else {
    if (CardNr.indexOf("hw") > 0)
      strcpy(HwDeviceName, qPrintable(CardNr));
    else
      sprintf(HwDeviceName, "%s:%i,%i", AccessType, CardNr.toInt(), SubDevice);
  }
  capture_handle  = nullptr;
  isResample      = false;
  InBufferLR      = nullptr;
  InBufferLRSize  = 0;
}

SoundCard::~SoundCard() {
  closeCapture();
}

SoundCard SoundCard::findByCardNr(QString & CardNr, int SubDevice) {
QDir      SysClassSoundCardX("/sys/class/sound/card" + CardNr);

  if (SysClassSoundCardX.exists())
    return SoundCard(CardNr, SubDevice);
  else {
    logger.warning("SoundCard::findByPhysicalPath()", __LINE__, "No sound card found with number %s", qPrintable(CardNr));
    return SoundCard();
  }
}

SoundCard SoundCard::findByCardName(QString CardName, int SubDevice) {
QDir                 SysClassSound("/sys/class/sound");
QStringList          NameFilterLst;
QStringList          FilesInSysClassSound;
int                  nCards, err, dev;
snd_ctl_card_info_t *info;
snd_pcm_info_t      *pcminfo;
snd_ctl_t           *handle;
int                  CardNr = -1;

  NameFilterLst.append("card*");
  FilesInSysClassSound = SysClassSound.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
  nCards = FilesInSysClassSound.count();
  if (nCards < 1) {
    logger.warning("SoundCard::readSoundCards()", __LINE__, "No sound cards found in /sys/class/sound.");
    return SoundCard();
  }

  snd_ctl_card_info_malloc(&info);
  snd_pcm_info_malloc(&pcminfo);
  bool found = false;
  for (int i = 0; !found && i < nCards; i++) {
    QString DirName = FilesInSysClassSound[i];
    // QDir dir(QString("/sys/class/sound/") + DirName);
    // QString canoncial = dir.canonicalPath();
    // printf("Karte %i: Canoncial Path = %s\n", i, qPrintable(canoncial));

    CardNr = DirName.mid(4).toInt();
    char HwCardNr[32];
    sprintf(HwCardNr, "hw:%d", CardNr);
    if ((err = snd_ctl_open(&handle, HwCardNr, 0)) < 0) {
      logger.error("SoundCard::findByCardName()", __LINE__, "snd_ctl_open(handle, %s, 0) : %s", HwCardNr, snd_strerror(err));
      continue;
    }
    if ((err = snd_ctl_card_info(handle, info)) < 0) {
      logger.error("SoundCard::findByCardName()", __LINE__, "snd_ctl_card_info() %s : %s", HwCardNr, snd_strerror(err));
      snd_ctl_close(handle);
      continue;
    }
    dev = -1;
    while (snd_ctl_pcm_next_device(handle, &dev) >= 0 && dev >= 0) {
      snd_pcm_info_set_device(pcminfo, uint(dev));
      snd_pcm_info_set_subdevice(pcminfo, 0);
      snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_CAPTURE);
      if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
        if (err != -ENOENT)
          logger.error("SoundCard::findByCardName()", __LINE__, "CardName = \"%s\" : %s", qPrintable(CardName), snd_strerror(err));
        continue;
      }
      QString iCardName(snd_ctl_card_info_get_name(info));
      if (iCardName == CardName) {
        found = true;
        break;
      }

    }
    snd_ctl_close(handle);
  }
  free(info);
  free(pcminfo);
  if (!found) {
    logger.warning("SoundCard::findByPhysicalPath()", __LINE__, "No sound card found with name \"%s\"", qPrintable(CardName));
    return SoundCard();
  }
  return SoundCard(CardNr, /* dev */  SubDevice);
}

SoundCard SoundCard::findByPhysicalPath(QString path, int SubDevice) {
QString PhysPath = "/sys/devices" + path + "/sound/";
QStringList          NameFilterLst;
QStringList          FilesInSysClassSound;
int                  nCards, i, CardNr = -1;
QDir                 SysClassSound("/sys/class/sound");
bool                 found = false;

  NameFilterLst.append("card*");
  FilesInSysClassSound = SysClassSound.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
  nCards = FilesInSysClassSound.count();
  if (nCards < 1) {
    logger.warning("SoundCard::readSoundCards()", __LINE__, "No sound cards found in /sys/class/sound.");
    return SoundCard();
  }
  for (i = 0; i < nCards; i++) {
    QString DirName = FilesInSysClassSound[i];
    QDir dir(PhysPath + DirName);
    if (dir.exists()) {
      CardNr = DirName.mid(4).toInt();
      // sprintf(sc.HwDeviceName, "hw:%i", DirName.mid(4).toInt());
      found = true;
      break;
    }
  }
  if (!found)
    logger.warning("SoundCard::findByPhysicalPath()", __LINE__, "No sound card found in path: %s", qPrintable(path));
  return SoundCard(CardNr, SubDevice);
}

SoundCard SoundCard::findBySerialNr(QString & SerialNr, QString & RadioName, int SubDevice) {
QString     Cmd;
QProcess    Process;
QStringList CmdArgs;
QByteArray  allStdOut;
QByteArray  allStdErr;
SoundCard   sc;
char      * S;

  if (SerialNr.isEmpty() || options.ProgFindCardNr.isEmpty()) {
    if (SerialNr.isEmpty())
      logger.error("SoundCard::findBySerialNr()", __LINE__, "Configuration problem: No serial no. given for radio %s.", qPrintable(RadioName));
    if (options.ProgFindCardNr.isEmpty())
        logger.error("SoundCard::findBySerialNr()", __LINE__, "Configuration problem: Missing external program to find sound card no. Radio: %s.", qPrintable(RadioName));
    return sc;
  }
  Cmd = options.ProgFindCardNr;
  Cmd.replace("%serial", SerialNr);
  Cmd.replace("%radio", RadioName);
  Process.start(Cmd,QIODevice::ReadOnly);
  if (!Process.waitForStarted()) {
    logger.error("SoundCard::SoundCardNrFromSerial()", __LINE__, "Error starting Cmd: %s", qPrintable(Cmd));
    return sc;
  }
  if (!Process.waitForFinished()) {
    logger.error("SoundCard::SoundCardNrFromSerial()", __LINE__, "Error runing Cmd: %s", qPrintable(Cmd));
    return sc;
  }

  allStdErr = Process.readAllStandardError();
  S = allStdErr.data();
  if (*S != 0) {
    logger.error("SoundCard::SoundCardNrFromSerial()", __LINE__, "Error runing Cmd: %s   Error; %s", qPrintable(Cmd), S);
    return sc;
  }
  allStdOut = Process.readAllStandardOutput();
  QString str(allStdOut);
  return SoundCard(str, SubDevice);
}


SoundCard SoundCard::findCard(SoundDeviceInfo & devInfo, QString & RadioName, int SubDevice) {
SoundCard sc;

  if (devInfo.AudioIdentType == SoundCard::AudioIdentType_CARDNR)
    sc =findByCardNr(devInfo.SoundCardAlsaDev, SubDevice);
  if (devInfo.AudioIdentType == SoundCard::AudioIdentType_CARDNAME)
    sc =findByCardName(devInfo.SoundCardName, SubDevice);
  if (devInfo.AudioIdentType == SoundCard::AudioIdentType_PHYSICALPATH)
    sc =findByPhysicalPath(devInfo.SoundCardPhysicalPath, SubDevice);
  if (devInfo.AudioIdentType == SoundCard::AudioIdentType_SERIALNR)
    sc = findBySerialNr(devInfo.SerialNr, RadioName, SubDevice);

  if (!sc.isValid()) {
    logger.error("SoundCard::findCard()", __LINE__, "Error: No sound card found for %s - %s", qPrintable(devInfo.AudioIdentType), qPrintable(devInfo.AudioIdent()));
  }
  return sc;
}


void SoundCard::clear() {
  closeCapture();
  HwDeviceName[0] = 0 ;
}

bool SoundCard::isValid() {
unsigned int i;

  if (HwDeviceName[0] != 0) {
    QString DirName("/sys/class/sound/card");
    i = strlen(AccessType) + 1;
    while (i < sizeof(HwDeviceName) && HwDeviceName[i] >= '0' && HwDeviceName[i] <= '9') {
      DirName += HwDeviceName[i];
      i++;
    }
    QDir SysClassSoundCard(DirName);
    return SysClassSoundCard.exists();
  }
  return false;
}

bool SoundCard::openCapture(uint Samplerate, int channels, int channelMode, snd_pcm_format_t Format, int ResamplerQuality) {
int        err;
snd_pcm_hw_params_t *hw_params;
bool      ret = false;
int       dir = 0;
double    ResampleRatio = 1.0;

  logger.debug("SoundCard::openCapture()", __LINE__, "HwDeviceName = \"%s\"   Samplerate = %i   Channels = %i\n", HwDeviceName, Samplerate, channels);

  SampleRate = Samplerate;   // this->SampleRate
  actualSampleRate = Samplerate;
  Channels = channels;
  ChannelMode = channelMode;

  if ((err = snd_pcm_open (&capture_handle, HwDeviceName, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    logger.error("SoundCard::openCapture()", __LINE__, "cannot open audio device %s (%s)\n", HwDeviceName, snd_strerror(err));
    return false;
  }

  if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
    logger.error("SoundCard::openCapture()", __LINE__, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
    return false;
  }

  while (true) {
    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
      break;
    }
    if (ChannelMode)
      channels = 2;
    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, uint(channels))) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot set channels to %i   (%s)\n", channels, snd_strerror (err));
      break;
    }
    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot set access type (%s)\n", snd_strerror (err));
      break;
    }

    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, Format)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot set sample format (%s)\n", snd_strerror (err));
      break;
    }

    // Because the sample rate accuracy is low, we use libSamplerate for resampling if needed
    if ((err = snd_pcm_hw_params_set_rate_resample(capture_handle, hw_params, 0)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot set sample rate (%s)\n", snd_strerror (err));
      break;
    }

    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &actualSampleRate, &dir)) < 0) {
        logger.error("SoundCard::openCapture()", __LINE__, "cannot set sample rate (%s)\n", snd_strerror (err));
        break;
    }

    if ((err = snd_pcm_hw_params_get_rate(hw_params, &actualSampleRate, &dir)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot get sample rate (%s)\n", snd_strerror (err));
      break;
    }
    logger.debug("SoundCard::openCapture()", __LINE__, "actualSampleRate = %i    dir = %i", actualSampleRate, dir);

    isResample = actualSampleRate != Samplerate;
    if (isResample) {
      logger.debug("SoundCard::openCapture()", __LINE__, "sample rate mismatch: required rate = %i   actual rate = %i", Samplerate, actualSampleRate);
      ResampleRatio = double(Samplerate) / double(actualSampleRate);
      logger.debug("SoundCard::openCapture()", __LINE__, "Resampler activated, ratio = %8.3f", ResampleRatio);
    }

    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot set parameters (%s)\n", snd_strerror (err));
      break;
    }

    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
      logger.error("SoundCard::openCapture()", __LINE__, "cannot prepare audio interface for use (%s)\n", snd_strerror(err));
      break;
    }
    ret = true;
    break;
  }

  snd_pcm_hw_params_free(hw_params);

  if (!ret) {
    snd_pcm_close(capture_handle);
    capture_handle = nullptr;
  }
  logger.debug("SoundCard::openCapture()", __LINE__, "==> %s\n", ret ? "true" : "false");

  return ret && (!isResample || initResampler(Samplerate, Channels, ResampleRatio, ResamplerQuality));
}

bool SoundCard::initResampler(uint SampleRate, int Channels, double ResampleRatio, int ResamplerQuality) {
int error, i, ret;

  src_data.input_frames = (int((double(SampleRate) / ResampleRatio + 7.0))) >> 3;
  src_data.output_frames = (SampleRate + 7) >> 3;
  data_in           = new float[size_t(src_data.input_frames * Channels)];
  src_data.data_in  = data_in;
  src_data.data_out = new float[size_t(src_data.output_frames * Channels)];
  src_data.input_frames_used = src_data.input_frames;
  src_data.output_frames_gen = 0;
  src_data.end_of_input = 0;
  src_data.src_ratio = ResampleRatio;
  ResampleOutputPos = 0;

  src_state = src_new(ResamplerQuality, Channels, &error);
  if (src_state == nullptr) {
    logger.error("SoundCard::initResampler()", __LINE__, "SampleRate = %i   Channels = %i   ResampleRatio = %.4f", SampleRate, Channels, ResampleRatio);
    delete [] src_data.data_in;
    delete [] src_data.data_out;
    isResample = false;
  }
  if (isResample) {
    // InBufferResampleFrames = src_data_input_frames;
    InBufferResample = new short [size_t(src_data.input_frames * Channels)];

    for (i = 0; i < src_data.input_frames; i++)
      data_in[i] = 0.0f;
    ret = src_process(src_state, &src_data);
    if (ret != 0) {
      const char * S = src_strerror(ret);
      logger.error("SoundCard::initResampler()", __LINE__, "Error src_process(): %i   %s\n", ret, S == nullptr ? "no message available" : S);
      delete [] src_data.data_in;
      delete [] src_data.data_out;
      isResample = false;
      return false;
    }
    src_data.input_frames_used = src_data.input_frames;
    src_data.output_frames_gen = 0;
    ResampleOutputPos = src_data.output_frames_gen;
  }
  return isResample;
}

void SoundCard::freeResampler() {
  if (isResample) {
    delete [] src_data.data_in;
    delete [] src_data.data_out;
    delete [] InBufferResample;
    isResample = false;
  }
}

bool SoundCard::readCaptureResample(short * OutBuffer, int OutBufferFrames) {
long nFrames;
int nOutBufferFrames = 0;
int ret;
long oldFrames;

  while (nOutBufferFrames < OutBufferFrames) {

    // Are all samples from prevois call used?
    if (ResampleOutputPos >= src_data.output_frames_gen) {

      // Count of unused farmes ?
      oldFrames = src_data.input_frames - src_data.input_frames_used;
      if (oldFrames > 0) {
        memmove(data_in,
                src_data.data_in + (src_data.input_frames_used * Channels),
                size_t((src_data.input_frames - src_data.input_frames_used) * long(sizeof(float)) * long(Channels)));
      }
      if (!readCaptureDirect(InBufferResample, src_data.input_frames - oldFrames))
        return false;
      src_short_to_float_array(InBufferResample, data_in + (oldFrames * Channels), int(src_data.input_frames - oldFrames) * Channels);

      ret = src_process(src_state, &src_data);
      if (ret != 0) {
        const char * S = src_strerror(ret);
        logger.error("SoundCard::readCaptureResample()", __LINE__, "Error src_process(): %i   %s\n", ret, S == nullptr ? "no message available" : S);
        return false;
      }
      ResampleOutputPos = 0;
    }

    // available frames:
    // src_data.output_frames_gen - ResampleOutputPos

    // needed frames:
    // OutBufferFrames - nOutBufferFrames

    nFrames = src_data.output_frames_gen - ResampleOutputPos;
    if (nFrames > OutBufferFrames - nOutBufferFrames)
      nFrames = OutBufferFrames - nOutBufferFrames;

    src_float_to_short_array(src_data.data_out + (ResampleOutputPos * Channels),
                             OutBuffer + (nOutBufferFrames * Channels),
                             int(nFrames * Channels));
    ResampleOutputPos += nFrames;
    nOutBufferFrames  += nFrames;
  }
  return nOutBufferFrames == OutBufferFrames;
}

bool SoundCard::readCaptureDirect(short * OutBuffer, long OutBufferSize) {
long nToRead = OutBufferSize;
long nRead = 0;
long nOutBuffer = 0;
int i;

  if (!ChannelMode) {
    nRead = 0;
    nToRead = OutBufferSize;

    while (nOutBuffer < OutBufferSize && (nRead = snd_pcm_readi(capture_handle, OutBuffer + nOutBuffer, size_t(nToRead))) > 0) {
      nOutBuffer += nRead;
      nToRead    -= nRead;
    }
    if (nRead <= 0) {
      logger.error("SoundCard::readCapture()", __LINE__, "read from audio interface failed (%i - %s)\n", nRead, snd_strerror(int(nRead)));
      return false;
    }
    return true;
  }

  if (InBufferLRSize < OutBufferSize << 1) {
    if (InBufferLR != nullptr)
      delete [] InBufferLR;
    InBufferLRSize = OutBufferSize << 1;
    InBufferLR = new short [size_t(InBufferLRSize)];
  }

  nRead = 0;
  nToRead = OutBufferSize;

  while (nOutBuffer < OutBufferSize && (nRead = snd_pcm_readi(capture_handle, InBufferLR + nOutBuffer, size_t(nToRead))) > 0) {
    nOutBuffer += nRead;
    nToRead    -= nRead;
  }
  if (nRead <= 0) {
    logger.error("SoundCard::readCapture()", __LINE__, "read from audio interface failed (%i - %s)\n", nRead, snd_strerror(int(nRead)));
    return false;
  }
  nOutBuffer = 0;
  if (ChannelMode == 1)
    for (i = 0; i < InBufferLRSize; i++)
      if (!(i & 1))
        OutBuffer[nOutBuffer++] = InBufferLR[i];
  if (ChannelMode == 2)
    for (i = 0; i < InBufferLRSize; i++)
      if (i & 1)
        OutBuffer[nOutBuffer++] = InBufferLR[i];
  return true;
}

bool SoundCard::readCapture(short * OutBuffer, int OutBufferSize) {
  return isResample ? readCaptureResample(OutBuffer, OutBufferSize) : readCaptureDirect(OutBuffer, OutBufferSize);
}

void SoundCard::closeCapture() {
  if (capture_handle != nullptr) {
    snd_pcm_close(capture_handle);
    capture_handle = nullptr;
    freeResampler();
  }
  if (InBufferLR) {
    delete [] InBufferLR;
    InBufferLR = nullptr;
    InBufferLRSize = 0;
  }
}

QStringList SoundCard::listSoundCardAlsaDevs() {
QStringList          NameFilterLst;
QStringList          FilesInSysClassSound;
int                  nCards;
QDir                 SysClassSound("/sys/class/sound");
QStringList          lstAlsaDevs;

  NameFilterLst.append("card*");
  FilesInSysClassSound = SysClassSound.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
  nCards = FilesInSysClassSound.count();
  if (nCards < 1) {
    logger.warning("SoundCard::readSoundCards()", __LINE__, "No sound cards found in /sys/class/sound.");
  }
  for (int i = 0; i < nCards; i++) {
    // Exists a capture device?
    QString device = FilesInSysClassSound[i];
    QDir    DeviceDir("/sys/class/sound/" + device);
    NameFilterLst.clear();
    NameFilterLst.append("pcm*c");
    QStringList lstSubDevices = DeviceDir.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
    if (lstSubDevices.count() > 0)
      lstAlsaDevs.append(FilesInSysClassSound[i].replace("card", "hw:"));
  }
  return lstAlsaDevs;
}

QStringList SoundCard::listSoundCardNames() {
QDir                 SysClassSound("/sys/class/sound");
QStringList          NameFilterLst;
QStringList          FilesInSysClassSound;
int                  nCards, err, dev;
snd_ctl_card_info_t *info;
snd_pcm_info_t      *pcminfo;
snd_ctl_t           *handle;
QStringList          lstCardNames;

  NameFilterLst.append("card*");
  FilesInSysClassSound = SysClassSound.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
  nCards = FilesInSysClassSound.count();
  if (nCards < 1) {
    logger.warning("SoundCard::listSoundCardNames()", __LINE__, "No sound cards found in /sys/class/sound.");
    return lstCardNames;
  }

  snd_ctl_card_info_malloc(&info);
  snd_pcm_info_malloc(&pcminfo);
  for (int i = 0; i < nCards; i++) {
    QString DirName = FilesInSysClassSound[i];
    int CardNr = DirName.mid(4).toInt();
    char HwCardNr[32];
    sprintf(HwCardNr, "hw:%d", CardNr);
    if ((err = snd_ctl_open(&handle, HwCardNr, 0)) < 0) {
      logger.error("SoundCard::listSoundCardNames()", __LINE__, "snd_ctl_open(handle, %s, 0) : %s", HwCardNr, snd_strerror(err));
      continue;
    }
    if ((err = snd_ctl_card_info(handle, info)) < 0) {
      logger.error("SoundCard::listSoundCardNames()", __LINE__, "snd_ctl_card_info() %s : %s", HwCardNr, snd_strerror(err));
      snd_ctl_close(handle);
      continue;
    }
    dev = -1;
    while (snd_ctl_pcm_next_device(handle, &dev) >= 0 && dev >= 0) {
      snd_pcm_info_set_device(pcminfo, uint(dev));
      snd_pcm_info_set_subdevice(pcminfo, 0);
      snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_CAPTURE);
      if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
        if (err != -ENOENT)
          logger.error("SoundCard::listSoundCardNames()", __LINE__, "%s", snd_strerror(err));
        continue;
      }
      lstCardNames.append(snd_ctl_card_info_get_name(info));
    }
    snd_ctl_close(handle);
  }
  free(info);
  free(pcminfo);
  return lstCardNames;
}

QStringList SoundCard::listSoundCardPhysicalPaths() {
QStringList          NameFilterLst;
QStringList          FilesInSysClassSound;
int                  nCards, i, x;
QDir                 SysClassSound("/sys/class/sound");
QStringList          lstPhysPaths;

  NameFilterLst.append("card*");
  FilesInSysClassSound = SysClassSound.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
  nCards = FilesInSysClassSound.count();
  for (i = 0; i < nCards; i++) {
    QString DirName = FilesInSysClassSound[i];
    QDir dir(QString("/sys/class/sound/") + DirName);
    QString canoncial = dir.canonicalPath();
    x = canoncial.indexOf("/sound/card");
    if (x > 0)
      canoncial = canoncial.left(x);

    // Exists a capture device?
    // We can search in /sys/class/sound/ too, card* ars sybolic links, the resulut is the same.
    QString device = FilesInSysClassSound[i];
    QDir    DeviceDir("/sys/class/sound/" + device);
    NameFilterLst.clear();
    NameFilterLst.append("pcm*c");
    QStringList lstSubDevices = DeviceDir.entryList(NameFilterLst, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive, QDir::Name);
    if (lstSubDevices.count() > 0)
      lstPhysPaths.append(canoncial.mid(12));
  }
  return lstPhysPaths;
}


QStringList SoundCard::listSoundCardSerials() {
QProcess    Process;
QStringList CmdArgs;
QByteArray  allStdErr;
QByteArray  allStdOut;
const char *S;

  Process.start(options.ProgListIds, CmdArgs);
  if (!Process.waitForStarted()) {
    logger.error("SoundCard::listSoundCardSerials()", __LINE__, "Error starting Cmd: %s", qPrintable(options.ProgListIds));
    return QStringList();
  }
  if (!Process.waitForFinished()) {
    logger.error("SoundCard::listSoundCardSerials()", __LINE__, "Error runing Cmd: %s", qPrintable(options.ProgListIds));
    return QStringList();
  }
  allStdErr = Process.readAllStandardError();
  S = allStdErr.data();
  if (*S != 0) {
    logger.error("SoundCard::listSoundCardSerials()", __LINE__, "Error runing Cmd: %s   Error; %s", qPrintable(options.ProgListIds), S);
    return QStringList();
  }
  allStdOut = Process.readAllStandardOutput();
  QString stdout(allStdOut);
  return stdout.split(QChar('\n'), QString::SkipEmptyParts);
}
