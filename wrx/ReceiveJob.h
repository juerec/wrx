#ifndef RECEIVEJOB_H
#define RECEIVEJOB_H

#include <QtCore>

#include "Radio.h"
#include "Frequency.h"
#include "RadioSetup.h"
#include "JobOptions.h"
#include "ReceiveJobMessage.h"


#define  PROGRESS_ERROR         -1
#define  PROGRESS_SOON          -2
#define  PROGRESS_WAIT          -3
#define  PROGRESS_DISABLED      -4
#define  PROGRESS_TIMECONFLICT  -5
#define  PROGRESS_RADIO_OFF     -6
#define  PROGRESS_CONFIG_ERROR  -7
#define  PROGRESS_RECORD_OK    100
#define  PROGRESS_POSTPROC_OK  101
#define  PROGRESS_OK           102

class ReceiveJob {

public :

  ReceiveJob(int id);
  ReceiveJob(ReceiveJob * job);
  ~ReceiveJob();

protected :

  static QTime calcTestStart(QString start);
  static ReceiveJob * createJob(QStringList & lstConfig, QStringList & lstSetup, int Id, bool isTest);

public :

  static void     init(QList<OptionsList> & lstConfig, QList<OptionsList> & lstSetup, bool isTest = false);
  static void     clear();
  static void     saveReceiveJobs(QList<OptionsList> lstSetup);
  static void     valuesToSave(QList<OptionsList> & lstSetup);
  void            valuesToSave(QStringList & lstSetup);
  static QString  calcOutputPath(const QDateTime & dtIn, int DaysOld);
  static QString  makeTodayPath();
protected :
  static QString  timeFromUTC(const QTime & Time, const QString & FormatStr, int DaysAgo = 0);
  static QString  buildFileName(const QTime & StartUTC, const QString & JobFileName, RadioSetup * pRadioSetup, int DaysOld);
public :
  QString     buildWaveRecordFileName() const;
  QString     buildDecoderRecordFileName() const;
  QString     buildRecordFileNamePattern(int DaysOld) const;
  static QString    buildManualWaveRecordFileName(RadioSetup *pRadioSetup);
  static QString    buildManualDecoderRecordFileName(RadioSetup * pRadioSetup);
  QString     buildPostCommand(QString WaveFileName);
  QStringList RadioSetupNamen();
  bool        canStart();
  enum RadioSetup::RunType RunType();
  void        setCallsignRadio(QString CallsignRadio);
  int         ProgressInfo();
  int         ResultInfo();
  int         MessageInfo();
  void        setProgress(int prog);
  int         progress() const { return Progress; }
  bool        isRunning() const { return Progress >= 0 && Progress <= 100; }
  QStringList ResultFileNames();  // File names with path
  QStringList Results();          // File names without path
  QString     ToolTipResults();
  QString     ToolTipErrors() const;
  void        addMessage(const ReceiveJobMessage RjMsg) { lstMessages.insert(0, RjMsg); }

  JobOptions * currentJobOptions();

  enum JobState {
    JOB_STATE_WAITING,     // Progress  -3
    JOB_STATE_SOON,        // Progress  -2
    JOB_STATE_RUNNING,     // Progress 0 .. 99
    JOB_STATE_REC_OK,      // Progress 100      record finished succesfull, post command still running
    JOB_STATE_POSTCMD_OK,  // Progress 101      post command succesfull finished too
    JOB_STATE_SUCCESS,     // Progress 102      job finished succesfull, no post command for these job
    JOB_STATE_ERROR,       // Progress  -1
    JOB_STATE_INVALID,     // Progress  -7      configuration problem, no frequency, no radio ...
    JOB_STATE_PAUSED       // for between job which is registered but not running
  };

  enum JobType {
    JOB_TYPE_NOJOB,           // not a valid job
    JOB_TYPE_TIMED,           // job with given run time, located in job page
    JOB_TYPE_TEST,            // manual started from radio page
    JOB_TYPE_BETWEEN,         // job which runs when no other jobs active
  };

  static QList<ReceiveJob *> lstReceiveJobs;

  int               Id;
  QString           Name;
  QTime             StartUTC;
  int               Duration;
  QString           Start;    // shows local time
  QString           End;      // shows local time
protected :
  int               Progress;
public :
  QString           FileName; // description part of output file
  QList<Frequency*> lstFrequencies;
  bool              Enable;
  RadioSetup      * activeRadioSetup;
  JobState          State;
  QDateTime         lastStart;
  int               iStart;      // Start time in seconds (hh * 3600 + mm * 60 + ss)
  bool              TimeConflict;
  QList<ReceiveJobMessage> lstMessages;
  JobType           Type;

protected :
  // Cache for TableView, saving cpu utilization,
  // It's queried very often, but changed 1 times at day.
  // The value of nResults is set to -1 by setProgress() called with a argument PROGRESS_ERROR, PROGRESS_RECORD_OK, PROGRESS_OK, PROGRESS_POSTPROC_OK
  // No other arguments does change the results content.
  void              readInResults();        // reads count, filenames with and without path
  int               nResults;               // set to -1 means results must read in newly
  QStringList       lstResultsWithPath;
  QStringList       lstResultsWithoutPath;

};

#endif // RECEIVEJOB_H
