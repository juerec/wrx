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

#include "RadioSetup.h"
#include "Frequency.h"
#include "Logger.h"

QList<RadioSetup *> RadioSetup::lstRadioSetup;


RadioSetup::RadioSetup() {
  init();
}


RadioSetup::RadioSetup(QStringList & lst) {

  init();
  for (int i = 0; i < lst.length(); i++) {
    QString & str = lst[i];
    if (str.isEmpty())
      continue;
    if (str.startsWith("Callsign=")) {
      Callsign = str.mid(9);
      continue;
    }
    if (str.startsWith("Radio=")) {
      pRadio = Radio::find(str.mid(6));
      continue;
    }
    if (str.startsWith("RunType=")) {
      QString rt = str.mid(8);
      if (rt == "extern") {
        RunType = RUN_TYPE_EXTERN;
      } else {
        RunType = RUN_TYPE_INTERN;
        if (rt != "intern")
          logger.warning("RadioSetup::RadioSetup()", __LINE__, "Illegal value for RunType: \"%s\"! Set to \"intern\"", qPrintable(rt));
      }
      continue;
    }
    if (str.startsWith("DecoderType=")) {
      QString dt = str.mid(12);
      if (dt == "extern") {
        DecoderType = DECODER_EXTERN;
      } else {
        if (dt == "intern") {
          DecoderType = DECODER_INTERN;
        } else {
          DecoderType = DECODER_NONE;
          if (dt != "none")
            logger.warning("RadioSetup::RadioSetup()", __LINE__, "Illegal value for DecoderType: \"%s\"! Set to \"none\"", qPrintable(dt));
        }
      }
      continue;
    }
    if (str.startsWith("CenterFreq=")) {
      CenterFreq = str.mid(11).toInt();
      continue;
    }
    if (str.startsWith("Offset=")) {
      Offset = str.mid(7).toInt();
      continue;
    }
    if (str.startsWith("SampleRate=")) {
      SampleRate = str.mid(11).toInt();
      continue;
    }
    if (str.startsWith("Channels=")) {
      Channels = str.mid(9).toInt();
      ChannelMode = 0;
      if (Channels == 0) {
        QChar c = str[9];
        if (c == 'L' || c == 'l') {
          Channels = 1;
          ChannelMode = 1;
        }
        if (c == 'R' || c == 'r') {
          Channels = 1;
          ChannelMode = 2;
        }
      }
      continue;
    }
    if (str.startsWith("SubDevice=")) {
      SubDevice = str.mid(10).toInt();
      continue;
    }
    if (str.startsWith("ResamplerQuality=")) {
      QString q = str.mid(17);
      if (q == "SRC_SINC_BEST_QUALITY")
        ResamplerQuality = 0;
      if (q == "SRC_SINC_MEDIUM_QUALITY")
        ResamplerQuality = 1;
      if (q == "SRC_SINC_FASTEST")
        ResamplerQuality = 2;
      if (q == "SRC_ZERO_ORDER_HOLD")
        ResamplerQuality = 3;
      if (q == "SRC_LINEAR")
        ResamplerQuality = 4;
      continue;
    }
    if (str.startsWith("DecodedFileExt=")) {
      DecodedFileExt = str.mid(15);
      continue;
    }
    if (str.startsWith("FFTSize=")) {
      FFTSize = str.mid(8).toInt();
      continue;
    }
    if (str.startsWith("WindowSize=")) {
      WindowSize = str.mid(11).toInt();
      continue;
    }
    if (str.startsWith("WindowFunc=")) {
      iWindowFunc = str.mid(11).toInt();
      continue;
    }
    if (str.startsWith("isWaterfall=")) {
      isWaterfall = Options::boolFromYN(str.mid(12));
      continue;
    }
    if (str.startsWith("isWaveRecord=")) {
      isWaveRecord = Options::boolFromYN(str.mid(13));
      continue;
    }
    if (str.startsWith("isDecoder=")) {   // run internal decoder, output to radio page
      isDecoder = str.mid(10) == "true";
      continue;
    }
    if (str.startsWith("isDecoderRecord=")) {   // record internal decoders output
      isDecoderRecord = str.mid(16) == "true";  // additional to post command
      continue;
    }
    if (str.startsWith("isRunPostCommand=")) {
      isRunPostCommand = str.mid(17) == "true";
      continue;
    }
    if (str.startsWith("ExternStopType=")) {
      QString est = str.mid(15);
      if (est == "command") {
        ExternStopType = EXTERN_STOP_BY_COMMAND;
      } else {
        if (est == "wait") {
          ExternStopType = EXTERN_STOP_WAIT_TERM;
        } else {
          ExternStopType = EXTERN_STOP_BY_SIGTERM;
          if (est != "SIGTERM")
            logger.warning("RadioSetup::RadioSetup()", __LINE__, "Illegal value for ExternStopType: \"%s\"! Set to \"SIGTERM\"", qPrintable(est));
        }
      }
      continue;
    }
    if (str.startsWith("ExternStopCommand=")) {
      ExternStopCommand = str.mid(18) == "true";
      continue;
    }

    if (str.startsWith("PostCommand=")) {
      PostCommand = str.mid(12);
      continue;
    }
    if (str.startsWith("StartParameter=")) {
      StartParameter = str.mid(15);
      continue;
    }
    if (str.startsWith("ParameterSet=")) {
      QStringList ParameterSet;
      options.readIniKeyConfig(ParameterSet, "ParameterSet", str.mid(13));
      for (int ii = ParameterSet.length() - 1; ii >= 0;  ii--)
        lst.insert(i + 1, ParameterSet[ii]);
      continue;
    }
  }
  frequency = Frequency::find(Callsign);
  // set flags which are mandotary for decoder type
  if (RunType == RUN_TYPE_INTERN) {
    switch (DecoderType) {
    case DECODER_NONE :
      isWaveRecord = true;
      isDecoder = false;
      isDecoderRecord = false;
        break;
    case DECODER_INTERN :
      break;
    case DECODER_EXTERN :
      isWaveRecord = true;
      isRunPostCommand = true;
    }
  }
  // log any missing input
  if (isRunPostCommand && PostCommand.isEmpty())
    logger.error("RadioSetup::RadioSetup()", __LINE__, "Run external program is switched on, but no external command is specified");
  if (ExternStopType == EXTERN_STOP_BY_COMMAND && ExternStopCommand.isEmpty())
    logger.error("RadioSetup::RadioSetup()", __LINE__, "Run external stop command is switched on, but no command is specified");
  if (RunType == RUN_TYPE_INTERN) {
    if (frequency == nullptr)
      logger.error("RadioSetup::RadioSetup()", __LINE__, "Invalid frequency specified.");
    else
      if (DecoderType == DECODER_INTERN && !(frequency->opMode.IdMode == OpMode::RTTY_85 || frequency->opMode.IdMode == OpMode::RTTY_450 || frequency->opMode.IdMode == OpMode::NAVTEX))
        logger.error("RadioSetup::RadioSetup()", __LINE__, "Configuration error: No internal decoder available for %s", frequency->opMode.ModeName());
  }
}


RadioSetup::~RadioSetup() {
}


void RadioSetup::init() {
  Callsign           = "UNKNOW";
  frequency          = nullptr;
  pRadio             = Radio::find("dummy");
  RunType            = RUN_TYPE_INTERN;
  DecoderType        = DECODER_NONE;
  CenterFreq         = 1000;
  Offset             = 0;
  SampleRate         = 8000;
  Channels           = 2;
  ChannelMode        = 0;
  SubDevice          = 0;
  ResamplerQuality   = 2;  // SRC_SINC_FASTEST
  DecodedFileExt     = ".txt";
  FFTSize            = 0;
  WindowSize         = 0;
  iWindowFunc        = 0;
  isWaterfall        = true;
  isWaveRecord       = false;
  isDecoder          = false;
  isDecoderRecord    = false;
  isRunPostCommand   = false;
  PostCommand        = "";
  ExternStopType     = EXTERN_STOP_WAIT_TERM;
  ExternStopCommand  = "";
}


// static
void RadioSetup::init(QList<OptionsList> & lst) {
int iParam, nParams, i, n;

  nParams = lst.count();
  for (iParam = 0; iParam < nParams; iParam++) {
    QStringList & lstParams = lst[iParam].ArgList;
    n = lstParams.count();
    for (i = 0; i < n; i++) {
      QString str = lstParams[i];
      if (str.startsWith("Radio="))
        if (Radio::exists(str.mid(6))) {
          RadioSetup * rs = new RadioSetup(lstParams);
          lstRadioSetup.append(rs);
          break;
        }
    }
  }
}


// static
void RadioSetup::clear() {
  for (int i = 0; i < lstRadioSetup.count(); i++)
    delete lstRadioSetup[i];
  lstRadioSetup.clear();
}


// static
RadioSetup * RadioSetup::find(QString Callsign, QString RadioName) {
RadioSetup * rs;

  for (int i = 0; i < lstRadioSetup.count(); i++) {
    rs = lstRadioSetup[i];
    if (Callsign == rs->Callsign && rs->pRadio != nullptr && RadioName == rs->pRadio->Name)
      return rs;
  }
  logger.warning("RadioSetup::find()", __LINE__, "No RadioSetup found for: Callsign = %s   Radio = %s", qPrintable(Callsign), qPrintable(RadioName));
  return nullptr;
}


// static
RadioSetup * RadioSetup::findFirst(QString Callsign) {
RadioSetup * rs;

  for (int i = 0; i < lstRadioSetup.count(); i++) {
    rs = lstRadioSetup[i];
    if (Callsign == rs->Callsign && rs->pRadio != nullptr)
      return rs;
  }
  return nullptr;
}

// static
bool RadioSetup::existsRadio(QString Callsign) {
RadioSetup * rs;

  for (int i = 0; i < lstRadioSetup.count(); i++) {
    rs = lstRadioSetup[i];
    if (Callsign == rs->Callsign && rs->pRadio != nullptr)
      return true;
  }
  return false;
}


// static
QStringList RadioSetup::findRadios(QString Callsign) {
QStringList ret;
RadioSetup * rs;
int  i, n;

  n = lstRadioSetup.count();
  for (i = 0; i < n; i++) {
    rs = lstRadioSetup[i];
    if (rs->Callsign == Callsign)
      ret.append(rs->pRadio->Name);
  }
  return ret;
}


//  static
RadioSetup * RadioSetup::findByName(QString CallsignRadio) {
RadioSetup * rs;
int   i, n;

  n = lstRadioSetup.count();
  for (i = 0; i < n; i++) {
    rs = lstRadioSetup[i];
    if (rs->Name() == CallsignRadio)
      return rs;
  }
  return nullptr;
}


QString RadioSetup::Name() {
  return Callsign + " - " + pRadio->NameLong + (pRadio->Enable ? "" : " *");
}


int RadioSetup::tuningFreq() {
  return (this == nullptr) ? 0 : frequency->Freq + Offset - CenterFreq;
}
