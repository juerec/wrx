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

#include "Radio.h"
#include "Options.h"
#include "Logger.h"
#include "Frequency.h"
#include "RadioSetup.h"

#include <string.h>
#include <QProcess>

QList<Radio *> Radio::lstRadios;

Radio Radio::Dummy;

Radio::Radio()
     : SoundDeviceInfo() {
  Id                     = 999;
  Enable                 = false;
  Name                   = "dummy";
  NameLong               = "Dummy";;
  CmdStart               = "";
  CmdCheck               = "";
  CmdStop                = "";
  SoundCardAlsaDev       = "0";
}


Radio::Radio(int id, QString name, QStringList & lstSetup)
     : SoundDeviceInfo(lstSetup) {

  Id                     = id;
  Enable                 = false;
  Name                   = name;
  NameLong               = "dummy";
  CmdStart               = "";
  CmdCheck               = "";
  CmdStop                = "";
  SoundCardAlsaDev       = "0";

  init(lstSetup);
}

Radio::~Radio() {
}

// static
void Radio::init(QList<OptionsList> & lstConfig, QList<OptionsList> & lstSetup) {
int nRadios, iRadio, i, n;
bool   load;
QString  Name;
QStringList  lstDummy;

  clear();
  nRadios = lstConfig.count();
  for (iRadio = 0; iRadio < nRadios; iRadio++) {
    QStringList & lstConf = lstConfig[iRadio].ArgList;
    load = false;
    n = lstConf.count();
    for (i = 0; i < n; i++) {
      QString & str = lstConf[i];
      if (str == "Load=Y")
        load = true;
      if (str.startsWith("Name=") && str.length() > 5)
        Name = str.mid(5);
    }
    if (load) {
      n = lstSetup.count();
      for(i = 0; i < n; i++)
        if (lstSetup[i].Key == Name)
          break;
      Radio * radio = new Radio(iRadio, Name, (i == n ? lstDummy : lstSetup[i].ArgList));
      lstRadios.append(radio);
      logger.debug("Radio::init()",  __LINE__, "Adding Radio %s", qPrintable(radio->NameLong));
    }
  }

}


// static
void Radio::clear() {
  for (int i = 0; i < lstRadios.count(); i++)
    delete lstRadios[i];
  lstRadios.clear();
}


// static
Radio * Radio::find(QString name) {

  for (int i = 0; i < lstRadios.count(); i++)
    if (lstRadios[i]->Name == name) {
      return lstRadios[i];
    }
  return &Dummy;
}


// static
bool Radio::exists(QString name) {

  for (int i = 0; i < lstRadios.count(); i++)
    if (lstRadios[i]->Name == name) {
      return true;
    }
  return false;
}


// static
void Radio::saveRadios() {
QList<OptionsList> lstSetup;

  for (int i = 0; i < lstRadios.count(); i++) {
    QStringList lstValues;
    lstRadios[i]->valuesToSave(lstValues);
    lstSetup.append(OptionsList(lstRadios[i]->Name, lstValues));
  }
  options.saveRadios(lstSetup);
}


void Radio::init(QStringList & lstSetup) {
int  i, n;

  n = lstSetup.length();
  for (i = 0; i < n; i++) {
    QString & str = lstSetup[i];
    if (str.startsWith("Enable="))
      Enable = str[7] == 'Y';
    if (str.startsWith("NameLong="))
      NameLong = str.mid(9);
    if (str.startsWith("CmdStart="))
      CmdStart = str.mid(9);
    if (str.startsWith("CmdCheck="))
      CmdCheck = str.mid(9);
    if (str.startsWith("CmdStop="))
      CmdStop = str.mid(8);
  }
}


void Radio::valuesToSave(QStringList & lstValues) {

  lstValues.append(QString("Enable=") + (Enable ? "Y" : "N"));
  lstValues.append("Name=" + Name);
  lstValues.append("NameLong=" + NameLong);
  SoundDeviceInfo::valuesToSave(lstValues);
  lstValues.append("CmdStart=" + CmdStart);
  lstValues.append("CmdCheck=" + CmdCheck);
  lstValues.append("CmdStop=" + CmdStop);
}


QString Radio::SoundCardNrFromSerial() {
QString     Cmd;
QProcess    Process;
QStringList CmdArgs;
QByteArray  allStdOut;
QByteArray  allStdErr;
char      * S;

  if (SerialNr.isEmpty() || options.ProgFindCardNr.isEmpty())
    return "";

  Cmd = options.ProgFindCardNr;
  Cmd.replace("%serial", SerialNr);
  CmdArgs.append("-c");
  CmdArgs.append(Cmd);
  Process.start("sh", CmdArgs);
  if (!Process.waitForStarted()) {
    logger.error("Radio::SoundCardNrFromSerial()", __LINE__, "Error starting Cmd: %s", qPrintable(Cmd));
    return "";
  }
  if (!Process.waitForFinished()) {
    logger.error("Radio::SoundCardNrFromSerial()", __LINE__, "Error runing Cmd: %s", qPrintable(Cmd));
    return "";
  }

  allStdErr = Process.readAllStandardError();
  S = allStdErr.data();
  if (*S != 0) {
    logger.error("Radio::SoundCardNrFromSerial()", __LINE__, "Error runing Cmd: %s   Error; %s", qPrintable(Cmd), S);
    return "";
  }
  allStdOut = Process.readAllStandardOutput();
  S = allStdOut.data();
  if (S == nullptr)
    return "";
  return S;
}


bool Radio::runCmd(QString Cmd, RadioSetup * radioSetup, QString & stdOut) {
char        s[16];
int         Frequency = radioSetup->tuningFreq();
OpMode      mode      = radioSetup == nullptr ? OpMode::find("UNKNOWN") : radioSetup->frequency->opMode;
QStringList CmdArgs;
QProcess    Process;
QByteArray  allStdOut;
QByteArray  allStdErr;
bool        ret;

  if (Cmd.indexOf("%MHz") > 0) {
    sprintf(s, "%i.%06i", Frequency / 1000000, Frequency % 1000000);
    Cmd.replace("%MHz", s);
  }
  if (Cmd.indexOf("%kHz") > 0) {
    sprintf(s, "%i.%03i", Frequency / 1000, Frequency % 1000);
    Cmd.replace("%kHz", s);
  }
  if (Cmd.indexOf("%Hz") > 0) {
    sprintf(s, "%i", Frequency);
    Cmd.replace("%Hz", s);
  }
  if (Cmd.indexOf("%serial") > 0)
    Cmd.replace("%serial", SerialNr);

  if (Cmd.indexOf("%mode") > 0)
    Cmd.replace("%mode", mode.ModulationName());

  logger.debug("Radio::runCmd()", __LINE__, "Cmd = %s\n", qPrintable(Cmd));

  CmdArgs.append("-c");
  CmdArgs.append(Cmd);
  Process.start("sh", CmdArgs);
  if (!Process.waitForStarted()) {
    logger.error("Radio::runCmd()", __LINE__, "Error starting Cmd: %s", qPrintable(Cmd));
    return false;
  }
  if (!Process.waitForFinished()) {
    logger.error("Radio::runCmd()", __LINE__, "Error runing Cmd: %s", qPrintable(Cmd));
    return false;
  }
  allStdErr = Process.readAllStandardError();
  QString StdErr(allStdErr);
  StdErr = StdErr.replace(QChar(10), " ");

  allStdOut = Process.readAllStandardOutput();
  QString StdOut(allStdOut);
  StdOut = StdOut.replace(QChar(10), " ");

  int ExitCode = Process.exitCode();
  QProcess::ExitStatus es = Process.exitStatus();
  const char * esDesc = "QProcess::NormalExit";
  switch (es) {
  case QProcess::NormalExit :
    esDesc = "QProcess::NormalExit";
    break;
  case QProcess::CrashExit :
    esDesc = "QProcess::CrashExit";
    break;
  }
  logger.debug("Radio::runCmd()", __LINE__, "ExitStatus = %s   ExitCode = %i   stderr = \"%s\"   stdout = \"%s\"", esDesc, ExitCode, qPrintable(StdErr), qPrintable(StdOut));

  // Error is : ExitCode != 0 OR any output to stderr
  ret = (es == QProcess::NormalExit) && (ExitCode == 0) && StdErr.isEmpty();
  if (!ret) {
    QString ReturnOrCrash;
    if (es == QProcess::NormalExit)
      ReturnOrCrash = QObject::tr("return code: ") + QString::number(ExitCode);
    else
      ReturnOrCrash = QObject::tr("The command did crash");
    stdOut = QObject::tr("Error runing Cmd: ") + Cmd +
             "\n" + ReturnOrCrash +
             "\nstderr: " + (StdErr.isEmpty() ? "" : StdErr) +
             "\nstdout: " + (StdOut.isEmpty() ? "" : StdOut);
    logger.error("Radio::runCmd()", __LINE__, "%s", qPrintable(stdOut));
  } else {
    stdOut = StdOut;
  }
  return ret;
}


bool Radio::turnOn(RadioSetup * radioSetup, QString & ErrorDesc) {
QString stdOut;
int     fx, fy;

  ErrorDesc = "";
  if (radioSetup == nullptr) {
    logger.error("Radio::turnOn()", __LINE__, "No valid RadioSetup exists. Check your configuration.");
    ErrorDesc = QObject::tr("No valid RadioSetup exists. Check your configuration.");
    return false;
  }
  if (CmdStart.isEmpty()) {
    logger.debug("Radio::turnOn()", __LINE__, "No CmdStart ==> ok.");
    return true;
  }

  if (!runCmd(CmdStart + " " + radioSetup->StartParameter, radioSetup, stdOut)) {
    ErrorDesc = stdOut;
    return false;
  }
  if (CmdCheck.isEmpty())
    return true;
  if (!runCmd(CmdCheck, radioSetup, stdOut)) {
    ErrorDesc = stdOut;
    return false;
  }

  fx = stdOut.toInt();
  fy = radioSetup->tuningFreq();
  if (fx != fy) {
    logger.error("Radio::turnOn()", __LINE__, "Error starting radio \"%s\"   requested frequncy = %i    reported frequency = %i", qPrintable(NameLong), fy, fx);
    ErrorDesc = QObject::tr("Error starting radio") + " " + NameLong + " - " +
                QObject::tr("requested frequncy") + " = " + QString::number(fy) + "   " +
                QObject::tr("reported frequency") + " = " + QString::number(fx);
    return false;
  }
  return true;
}


void Radio::turnOff() {
QString stdOut;

  logger.debug("Radio::turnOff()", __LINE__, "Radio = %s", qPrintable(NameLong));

  if (!CmdStop.isEmpty()) {
    runCmd(CmdStart, nullptr, stdOut);
  }
}


bool Radio::openCapture(int Samplerate, int Channels, int ChannelMode, int SubDevice, int ResamplerQuality) {

  SoundCard = SoundCard::findCard(*this, Name, SubDevice);
  return SoundCard.isValid() &&
         SoundCard.openCapture(uint(Samplerate), Channels, ChannelMode, SND_PCM_FORMAT_S16_LE, ResamplerQuality);
}


void Radio::closeCapture() {
  SoundCard.closeCapture();
}


bool Radio::readCapture(short * Buffer, int max) {
  return SoundCard.readCapture(Buffer, max);
}
