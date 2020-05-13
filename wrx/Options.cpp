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

#include "RadioService.h"
#include "Options.h"
#include "Logger.h"

Options::Options() {

    WindowXY               = QPoint(32, 24);
    WindowWH               = QSize(800, 600);
    WindowFormat           = "normal";
    confirmQuit            = true;
    FontSizeJobList        = 14;
    loadAllJobs            = false;
    showTimeAsUTC          = false;

    const char * home = getenv("HOME");
    if (home == nullptr)
      home = ".";
    QString SettingsPath = QString(home) + QDir::separator() + ".wrx";
    QDir dir(SettingsPath);
    if (!dir.exists(SettingsPath))
      dir.mkdir(SettingsPath);
    FileNamePreferences  = SettingsPath + QDir::separator() + "wrx-preferences.ini";
    FileNameConfig      = SettingsPath + QDir::separator() + "wrx-config.ini";
    isTest = false;
}
// ============================================ Options::Options()

Options::~Options() {
}
// ============================================ Options::~Options()


void Options::readIniBlock(QList<OptionsList> & lst, QString Section, QString IniFileName) {
QSettings settings(IniFileName, QSettings::IniFormat);
int  i, n;

  lst.clear();
  settings.setIniCodec("UTF-8");
  settings.beginGroup(Section);
  QStringList allKeys = settings.allKeys();
  n = allKeys.count();
  for (i = 0; i < n; i++) {
    QString & key = allKeys[i];
    lst.append(OptionsList(key, settings.value(key).toStringList()));
  }
}
// ============================================ Options::readIniBlock()

void Options::readIniKeyConfig(QStringList & Values, QString Section, QString Key) {
QSettings settings(FileNameConfig, QSettings::IniFormat);

  settings.setIniCodec("UTF-8");
  settings.beginGroup(Section);
  Values = settings.value(Key, "").toStringList();
}
// ============================================ Options::readIniKeyConfig()

void Options::init() {
QList<OptionsList> lstConfig;
QList<OptionsList> lstSetup;

  checkDebug();
  readPreferences();

  readIniBlock(lstConfig, "Radios", FileNameConfig);
  readIniBlock(lstSetup,  "Radios", FileNamePreferences);
  Radio::init(lstConfig, lstSetup);

  readIniBlock(lstConfig, "Frequencies", FileNameConfig);
  Frequency::init(lstConfig);

  readIniBlock(lstConfig, "RadioSetup", FileNameConfig);
  RadioSetup::init(lstConfig);

  readIniBlock(lstConfig, "ReceiveJobs", FileNameConfig);
  readIniBlock(lstSetup,  "ReceiveJobs", FileNamePreferences);
  ReceiveJob::init(lstConfig, lstSetup, isTest);


  // readJobs();
  RadioService::initServices();
}
// ============================================ Options::init()

void Options::clear() {
  ReceiveJob::clear();
  RadioSetup::clear();
  Radio::clear();
  Frequency::clear();
}
// ============================================ Options::~Options()


void Options::checkDebug() {
QString FileNameDebug = FileNamePreferences;

  FileNameDebug.replace("wrx-preferences.ini", "wrx-test-preferences.ini");
QSettings settings(FileNameDebug, QSettings::IniFormat);
  QString str = settings.value("Application/test", "N").toString();
  if (str == "Y" || str == "y" || str == "J" || str == "j") {
    isTest = true;
    FileNameConfig      = FileNameConfig.replace("wrx-config.ini", "wrx-test-config.ini");
    FileNamePreferences = FileNamePreferences.replace("wrx-preferences.ini", "wrx-test-preferences.ini");
  }
}
// ============================================ Options::checkDebug()


void Options::readPreferences() {
int       i;
QSettings settings(FileNamePreferences, QSettings::IniFormat);
static const int  SchedColumnWidthDef[SCHED_COLUMNS] = { 32,32, 120,120, 300, 200, 32, 32 };
static const int  ReceiveJobResultColWidthDef[2] = { 600, 48};
static const int  ReceiveJobMessagesColWidthDef[2] = { 200, 120};
static const int  ReceiveJobInfoColWidthDef[5] = { 140, 140, 140, 140, 140 };


  WindowXY         = settings.value("MainWindow/xy", QPoint(32, 24)).toPoint();
  WindowWH         = settings.value("MainWindow/wh", QSize(800, 600)).toSize();
  WindowFormat     = settings.value("MainWindow/format", "normal").toString();

  SchedulerColWidth = ListIntFromString(settings.value("Sched/ColWidth", "32,32, 120,120, 300, 200, 32, 32").toString(), SCHED_COLUMNS, SchedColumnWidthDef);
  loadAllJobs = boolFromYN(settings.value("Sched/loadAllJobs").toString());
  FontSizeJobList = settings.value("Sched/FontSizeJobList", QVariant(14)).toInt();

// Logger Level
  LogLevel = settings.value("Logger/Level", "ERROR").toString();
  logger.setLevel(qPrintable(LogLevel));

  runTimedJobs = settings.value("Application/runTimedJobs", true).toBool();

// Other Dialogs
  ReceiveJobResultColWidth = ListIntFromString(settings.value("ReceiveJobResultDlg/ColWidth", "320, 32").toString(), 2, ReceiveJobResultColWidthDef);
  ReceiveJobResultSize = settings.value("ReceiveJobResultDlg/DlgSize", QSize(800, 400)).toSize();

  ReceiveJobMessagesColWidth = ListIntFromString(settings.value("ReceiveJobMessagesDlg/ColWidth", "200, 120").toString(), 2, ReceiveJobMessagesColWidthDef);
  ReceiveJobMessagesSize = settings.value("ReceiveJobMessagesDlg/DlgSize", QSize(800, 400)).toSize();
  ReceiveJobMessagesSplitter = settings.value("ReceiveJobMessagesDlg/Splitter").toByteArray();

  ReceiveJobInfoSize = settings.value("ReceiveJobInfoDlg/DlgSize", QSize(800, 400)).toSize();
  ReceiveJobInfoColWidth = ListIntFromString(settings.value("ReceiveJobInfoDlg/ColWidth", "140, 140, 140, 140").toString(), 5, ReceiveJobInfoColWidthDef);

// settings pages
  // common
  confirmQuit   = boolFromYN(settings.value("MainWindow/confirmQuit").toString());
  runTimedJobs  = boolFromYN(settings.value("Application/runTimedJobs").toString());
  showTimeAsUTC = boolFromYN(settings.value("Application/showTimeAsUTC").toString());
  showTimeAsUTC_new = showTimeAsUTC;

  // scheduler
  // Root path for output
  if (!settings.contains("Paths/ReceiveResultPath")) {
    logger.error("Options::readPreferences()", __LINE__, "No definition found for \"Paths/ReceiveResultPath\"");
    ReceiveResultPath = QDir::homePath() + "/wrx";
  } else {
    ReceiveResultPath = settings.value("Paths/ReceiveResultPath").toString();
  }

  // Radio
  ProgListIds = settings.value("Radio/ProgListIds", "").toString();
  ProgFindCardNr = settings.value("Radio/ProgFindCardNr", "").toString();

  // Browser
  QStringList lstStr = settings.value("Browsers/Viewer").toStringList();
  for (i = 0; i < lstStr.count(); i++) {
    QStringList l = lstStr[i].split(">");
    if (l.count() == 3) {
      lstBrowserCommands.append(BrowserCommand(l[0], l[2], boolFromYN(l[1])));
    }
  }
}
// ============================================ Options::readPreferences()

void Options::write() {
  writePreferences();
}
// ============================================ Options::write()

void Options::writePreferences() {
QSettings settings(FileNamePreferences, QSettings::IniFormat);
QList<OptionsList> lstSetup;
int i, n;

  settings.setValue("MainWindow/xy", WindowXY);
  settings.setValue("MainWindow/wh", WindowWH);
  settings.setValue("MainWindow/format", WindowFormat);
  settings.setValue("Sched/ColWidth", ListIntToString(SchedulerColWidth));
  settings.setValue("Logger/Level", LogLevel);

  ReceiveJob::valuesToSave(lstSetup);
  n = lstSetup.count();
  for (i = 0; i < n; i++)
    settings.setValue("ReceiveJobs/" + lstSetup[i].Key, lstSetup[i].ArgList);

// Other Dialogs
  settings.setValue("ReceiveJobResultDlg/ColWidth", ListIntToString(ReceiveJobResultColWidth));
  settings.setValue("ReceiveJobResultDlg/DlgSize", ReceiveJobResultSize);

  settings.setValue("ReceiveJobMessagesDlg/ColWidth", ListIntToString(ReceiveJobMessagesColWidth));
  settings.setValue("ReceiveJobMessagesDlg/DlgSize", ReceiveJobMessagesSize);
  settings.setValue("ReceiveJobMessagesDlg/Splitter", ReceiveJobMessagesSplitter);

  settings.setValue("ReceiveJobInfoDlg/ColWidth", ListIntToString(ReceiveJobInfoColWidth));
  settings.setValue("ReceiveJobInfoDlg/DlgSize", ReceiveJobInfoSize);

// settings pages
  saveOptCommon();
  saveOptScheduler();
  saveOptRadio();
  saveOptBrowser();
}
// ============================================ Options::writePreferences()

const OpMode * Options::findOpMode(const char * Name) {
const OpMode * mode;

  for (int i = 0; i < lstOpMode.count(); i++) {
    mode = lstOpMode[i];
    if (!strcmp(Name, mode->ModeName()))
      return mode;
  }
  if (lstOpMode.count() > 0)
    return lstOpMode[0];
  return nullptr;
}

const OpMode * Options::findOpMode(OpMode::OPMODE_ID Id) {
const OpMode * mode;

  for (int i = 0; i < lstOpMode.count(); i++) {
    mode = lstOpMode[i];
    if (Id == mode->IdMode)
      return mode;
  }
  if (lstOpMode.count() > 0)
    return lstOpMode[0];
  return nullptr;
}

QString Options::browserCommand(QString & FileName) {
int len;

  if (FileName.isEmpty())
    return FileName;

  len = FileName.length();
  for (int i = 0; i < lstBrowserCommands.count(); i++) {
    BrowserCommand & bc = lstBrowserCommands[i];
    if (FileName.indexOf(bc.Pattern) == len - bc.Pattern.length())
      return bc.Command;
  }
  logger.error("Options::browserCommand()", __LINE__, "No browser command defined for: \"%s\"", qPrintable(FileName));
  return "";
}


QStringList Options::readWaterfallPrefs(QString RadioName) {
QSettings settings(FileNamePreferences, QSettings::IniFormat);

  return settings.value("RadioPage/" + RadioName, "normal").toStringList();
}

void Options::writeWaterfallPrefs(QStringList lst, QString RadioName) {
QSettings settings(FileNamePreferences, QSettings::IniFormat);

  settings.setValue("RadioPage/" + RadioName, lst);
}


void Options::saveOptCommon() {
QSettings settings(FileNamePreferences, QSettings::IniFormat);

  settings.setValue("MainWindow/confirmQuit", (confirmQuit ? "Y" : "N"));
  settings.setValue("Application/runTimedJobs", runTimedJobs);
  settings.setValue("Application/showTimeAsUTC", (showTimeAsUTC_new ? "Y" : "N"));
}

void Options::saveOptScheduler() {
QSettings settings(FileNamePreferences, QSettings::IniFormat);

  settings.setValue("Paths/ReceiveResultPath", ReceiveResultPath);
}

// called at program shotdown
void Options::saveOptRadio() {
//QSettings settings(FileNamePreferences, QSettings::IniFormat);

//  settings.setValue("Radio/ProgListIds", ProgListIds);
//  settings.setValue("Radio/ProgFindCardNr", ProgFindCardNr);
}

// called by Save-button
void Options::saveRadios(QList<OptionsList> & lstSetup) {
QSettings settings(FileNamePreferences, QSettings::IniFormat);
int iRadio, nRadios;

  nRadios = lstSetup.count();
  for (iRadio = 0; iRadio < nRadios; iRadio++) {
    QStringList & lstValues = lstSetup[iRadio].ArgList;
    QString key = "Radios/" + lstSetup[iRadio].Key;
    settings.setValue(key, lstValues);
  }
  settings.setValue("Radio/ProgListIds", ProgListIds);
  settings.setValue("Radio/ProgFindCardNr", ProgFindCardNr);
}

void Options::saveOptBrowser() {
int  nCmd = lstBrowserCommands.count();
int  i;
QStringList lst;
QSettings settings(FileNamePreferences, QSettings::IniFormat);

  for (i = 0; i < nCmd; i++) {
    BrowserCommand & bc = lstBrowserCommands[i];
    lst.append(bc.Pattern + (bc.active ? ">Y>" : ">N>") + bc.Command);
  }
  settings.setValue("Browsers/Viewer", lst);
}


// static
bool Options::boolFromYN(const QString & str) {

  if (str.isEmpty())
    return false;
  QChar Y = str[0].toUpper();
  return str == "true" || Y == 'Y' || Y == 'J';
}

// static
QString Options::ListIntToString(const QList<int> & lstInt) {
QString str("");
int     nLst, iLst;

  nLst = lstInt.count();
  for (iLst = 0; iLst < nLst; iLst++) {
    if (iLst > 0)
      str += ",";
    str += QString::number(lstInt[iLst]);
  }
  return str;
}


// static
QList<int> Options::ListIntFromString(const QString & str, int nInitElm, const int InitElm[]) {
QStringList lstStr = str.split(",", QString::SkipEmptyParts);
QList<int>  lstInt;
int         nLst, iLst;

  nLst = lstStr.count();
  for (iLst = 0; iLst < nLst; iLst++)
    lstInt.append(lstStr[iLst].toInt());
  if (nInitElm > 0)
    for ( ; iLst < nInitElm; iLst++)
      lstInt.append(InitElm == nullptr ? 0 : InitElm[iLst]);
  return lstInt;
}
