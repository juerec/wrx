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

#ifndef OPTIONS_H
#define OPTIONS_H


#include <QtCore>

#include "OpMode.h"
// #include "Frequency.h"

#define  SCHED_COLUMNS  8

class BrowserCommand {

public :

  QString  Pattern;
  QString  Command;
  bool     active;

  BrowserCommand() { active = false; }
  BrowserCommand(QString pattern, QString command, bool isActive) : Pattern(pattern), Command(command), active(isActive) {}

};


class OptionsList {
public :

  QString      Key;
  QStringList  ArgList;

  OptionsList(QString key, QStringList argList) : Key(key), ArgList(argList) {}

};


class Options {
public:
    Options();
    ~Options();
    static void readIniBlock(QList<OptionsList> & lst, QString Section, QString IniFilename);
    void readIniKeyConfig(QStringList & Values, QString Section, QString Key);
    void init();
    void clear();
    void read();
    void checkDebug();
    void readPreferences();
    void writePreferences();
    void write();
    const OpMode * findOpMode(const char * Name);
    const OpMode * findOpMode(OpMode::OPMODE_ID Id);
    QString  browserCommand(QString & FileName);
    static bool boolFromYN(const QString & str);
protected :
    static QString ListIntToString(const QList<int> & lstInt);
    static QList<int> ListIntFromString(const QString & str, int nInitElm = 0, const int InitElm[] = nullptr);

    QString       FileNameConfig;
    QString       FileNamePreferences;

public :
    // int   SchedColumnWidth[SCHED_COLUMNS];
    QList<int>    SchedulerColWidth;

  //  Preferences
    QPoint        WindowXY;
    QSize         WindowWH;
    QString       WindowFormat;   // "normal" "maximized" ("minimized" "fullscreen")

    QString       ReceiveResultPath;
    bool          confirmQuit;
    int           FontSizeJobList;
    bool          loadAllJobs;
    bool          isTest;
    bool          runTimedJobs;
    bool          showTimeAsUTC;
    bool          showTimeAsUTC_new;  // Value to save, but showTimeAsUTC is used until program's end
    QString       LogLevel;

  // Radio
    QString       ProgListIds;        // An external programm which writes line by line identifiers - if any - for each radio
    QString       ProgFindCardNr;     // An external programm, started with the ID as argument, returning the Nr of Soundcard
    void          saveRadios(QList<OptionsList> & lstSetup);


  // Browsers
    QList<BrowserCommand>  lstBrowserCommands;


  // Frequencies
    QList<OpMode*>        lstOpMode;

    QStringList   readWaterfallPrefs(QString RadioName);
    void          writeWaterfallPrefs(QStringList lst, QString RadioName);

// Save Methods
// Called at program shotdown
    void  saveOptCommon();
    void  saveOptScheduler();
    void  saveOptRadio();
    void  saveOptBrowser();

// Other Dialogs
   QList<int>  ReceiveJobResultColWidth;
   QSize       ReceiveJobResultSize;
   QList<int>  ReceiveJobMessagesColWidth;
   QSize       ReceiveJobMessagesSize;
   QByteArray  ReceiveJobMessagesSplitter;
   QList<int>  ReceiveJobInfoColWidth;
   QSize       ReceiveJobInfoSize;
};

extern Options options;

#endif // OPTIONS_H
