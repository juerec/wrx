#include "ReceiveJob.h"
#include "Options.h"
#include "Logger.h"

QList<ReceiveJob *> ReceiveJob::lstReceiveJobs;


ReceiveJob::ReceiveJob(int id) {
  Id                 = id;
  Name               = "Dummy Receive Job";
  StartUTC           = QTime(0, 0, 0);
  Duration           = 0;
  Start              = "00:00:00";
  End                = "00:00:00";
  Progress           = PROGRESS_WAIT;  // grey square ==> not started yet
  Enable             = false;
  activeRadioSetup   = nullptr;
  State              = JOB_STATE_INVALID;
  lastStart          = QDateTime(QDate(2000,1,1));
  iStart             = 0;
  TimeConflict       = true;
  Type               = JOB_TYPE_TIMED;
  nResults           = -1;
}


ReceiveJob::ReceiveJob(ReceiveJob * job)
          : Id(job->Id),
            Name(job->Name),
            StartUTC(job->StartUTC),
            Duration(job->Duration),
            Start(job->Start),
            End(job->End),
            Progress(job->Progress),
            FileName(job->FileName),
            lstFrequencies(job->lstFrequencies),
            Enable(job->Enable),
            activeRadioSetup(job->activeRadioSetup),
            State(job->State),
            lastStart(job->lastStart),
            iStart(job->iStart),
            TimeConflict(job->TimeConflict),
            Type(job->Type),
            nResults(job->nResults) {
}


// static
ReceiveJob * ReceiveJob::createJob(QStringList & lstConfig, QStringList & lstSetup, int Id, bool isTest) {
QString           Name               = "Dummy Receive Job";
QTime             StartUTC           = QTime(0, 0, 0);
int               Duration           = 0;
QString           Start              = "00:00:00";
QString           End                = "00:00:00";
QString           FileName           = "Dummy-File";
bool              Enable             = false;
RadioSetup *      activeRadioSetup   = nullptr;
int               iConfig, nConfig;
int               iSetup, nSetup;
int               iCallsign, nCallsign;
QString           currFreq, currRadio;
QString           strFrequencies;
QList<Frequency*> lstFrequencies;
ReceiveJob      * pJob               = nullptr;

  nConfig = lstConfig.count();
  for (iConfig = 0; iConfig < nConfig; iConfig++) {
    QString & str = lstConfig[iConfig];
    if (str.startsWith("Start=") && str.length() > 6) {
      if (isTest) { // interprets the start point as offset from now
        StartUTC = calcTestStart(str.mid(6));
      } else {
        StartUTC = QTime::fromString(str.mid(6), "hh:mm:ss");
      }
    }
    if (str.startsWith("Duration=") && str.length() > 9)
      Duration = str.mid(9).toInt();
    if (str.startsWith("Name=") && str.length() > 5)
      Name = str.mid(5);
    if (str.startsWith("FileName=") && str.length() > 9)
      FileName = str.mid(9);
    if (str.startsWith("Frequencies=") && str.length() > 12)
      strFrequencies = str.mid(12);
  }
  Start = timeFromUTC(StartUTC, "hh:mm:ss");
  End   = timeFromUTC(StartUTC.addSecs(Duration), "hh:mm:ss");
  nSetup = lstSetup.count();
  for (iSetup = 0; iSetup < nSetup; iSetup++) {
    QString & str = lstSetup[iSetup];
    if (str.startsWith("Enable="))
      Enable = str == "Enable=Y";
    if (str.startsWith("Frequency=") && str.length() > 10)
      currFreq = str.mid(10);
    if (str.startsWith("Radio=") && str.length() > 6)
      currRadio = str.mid(6);
  }
  // Make a list of frequencies for which a radio page is loaded
  QStringList lstCallsign = strFrequencies.split("|");
  if (lstCallsign.count() < 1) {
    logger.error("ReceiveJob::createJob()", __LINE__, "No Frequency defined for Receive Job rj-%03i", Id);
    return pJob;
  }
  nCallsign = lstCallsign.count();
  for (iCallsign = 0; iCallsign < nCallsign; iCallsign++) {
    const QString & Callsign = lstCallsign[iCallsign];
    if (RadioSetup::existsRadio(Callsign)) {
      Frequency * pFreq = Frequency::find(Callsign);
      if (pFreq != nullptr)
        lstFrequencies.append(pFreq);
    }
  }
  if (lstFrequencies.count() == 0) {
    logger.error("ReceiveJob::createJob()", __LINE__, "No usable Frequency defined for Receive Job rj-%03i", Id);
    return pJob;
  }
  if (!currFreq.isEmpty() && !currRadio.isEmpty())
    activeRadioSetup = RadioSetup::find(currFreq, currRadio);
  // if currFreq or currRadio are not specified or no RadioSetup exists for these pair,
  // we look for the first working pair
  nCallsign = lstFrequencies.count();
  for (iCallsign = 0; activeRadioSetup == nullptr && iCallsign < nCallsign; iCallsign++)
    activeRadioSetup = RadioSetup::findFirst(lstFrequencies[iCallsign]->Callsign);
  if (activeRadioSetup == nullptr) {
    logger.error("ReceiveJob::createJob()", __LINE__, "No usable Frequency defined for Receive Job rj-%03i", Id);
    return pJob;
  }
  pJob                   = new ReceiveJob(Id);
  pJob->Name             = Name;
  pJob->StartUTC         = StartUTC;
  pJob->Duration         = Duration;
  pJob->Start            = Start;
  pJob->End              = End;
  pJob->FileName         = FileName;
  pJob->lstFrequencies   = lstFrequencies;
  pJob->Enable           = Enable;
  pJob->activeRadioSetup = activeRadioSetup;
  pJob->State            = JOB_STATE_WAITING;
  if (StartUTC.isValid())
    pJob->iStart = (StartUTC.hour() * 3600 + StartUTC.minute() * 60 + StartUTC.second()) % (3600 * 24);
  else
    pJob->iStart = 60 * 60 * 24 + 1;   // > 24h ==> will be never started
  pJob->readInResults(); // sets: nResults, lstResultsWithPath, lstResultsWithoutPath
  return pJob;
}


ReceiveJob::~ReceiveJob() {
}

// static
QTime ReceiveJob::calcTestStart(QString start) {
QDateTime dt(QDateTime::currentDateTimeUtc());

  QStringList lst = start.split(":");
  if (lst.count() != 3)
    return dt.time();
  int Seconds = lst[0].toInt() * 3600 + lst[1].toInt() * 60 + lst[2].toInt();
  return dt.addSecs(Seconds).time();
}


// static
void ReceiveJob::init(QList<OptionsList> & lstConfig, QList<OptionsList> & lstSetup, bool isTest) {
int       iJob, nJobs, iParam, nParams, iJobSetup, nJobSetup, Id, i , n;
bool      load;
QStringList SetupDummy;

  //  logger.debug("ReceiveJob::init()", __LINE__, "FileNameJobs = %s   FileNameJobSetup = %s   allJobs = %s", qPrintable(FileNameJobs), qPrintable(FileNameJobSetup), (allJobs ? "true" : "false"));
  clear();
  nJobs = lstConfig.count();
  for (iJob = 0; iJob < nJobs; iJob++) {
    load = false;
    QString & Key = lstConfig[iJob].Key;
    QStringList & lstConf = lstConfig[iJob].ArgList;

    nParams = lstConf.count();
    for (iParam = 0; iParam < nParams; iParam++)
      if (lstConf[iParam] == "Load=Y") {
        load = true;
        break;
      }
    if (!load)
      continue;
    nJobSetup = lstSetup.count();
    for (iJobSetup = 0; iJobSetup < nJobSetup; iJobSetup++)
      if (Key == lstSetup[iJobSetup].Key)
        break;
    Id = Key.mid(3).toInt();
    ReceiveJob * rj = ReceiveJob::createJob(lstConf, iJobSetup == nJobSetup ? SetupDummy : lstSetup[iJobSetup].ArgList, Id, isTest);
    if (rj != nullptr) {
      if (rj->State == JOB_STATE_INVALID) {
        delete rj;
      } else {
        n = lstReceiveJobs.count();
        i = 0;
        while (i < lstReceiveJobs.count() && lstReceiveJobs[i]->Start < rj->Start)
          i++;
        if (i == n)
          lstReceiveJobs.append(rj);
        else
          lstReceiveJobs.insert(i, rj);
      }
    }
  }
}

// static
void ReceiveJob::clear() {
  for (int i = 0; i < lstReceiveJobs.count(); i++)
    delete lstReceiveJobs[i];
  lstReceiveJobs.clear();
}

// static
void ReceiveJob::valuesToSave(QList<OptionsList> & lstSetup) {
char key[8];

  for (int i = 0; i < lstReceiveJobs.count(); i++) {
    QStringList lstStr;
    sprintf(key, "rj-%03i", lstReceiveJobs[i]->Id);
    lstReceiveJobs[i]->valuesToSave(lstStr);
    lstSetup.append(OptionsList(key, lstStr));
  }
}


void ReceiveJob::valuesToSave(QStringList & lstSetup) {
QStringList lst;
char id[8];

  sprintf(id, "%03i", Id);
  lst.append(QString("Id=") + id);
  lst.append("Enable=" + QString(Enable ? "Y" : "N"));
  lst.append("Frequency=" + activeRadioSetup->frequency->Callsign);
  lst.append("Radio=" + activeRadioSetup->pRadio->Name);
  lstSetup.append(lst);
}


/* For showing reception result - we do not create any paths */
/* If path does not exists, we have no reception results. */
// static
QString ReceiveJob::calcOutputPath(const QDateTime &dtIn, int DaysOld) {
QDateTime  dt(dtIn.addDays(DaysOld));
// QDateTime  dt((options.showTimeAsUTC ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime()).addDays(DaysOld));
QString    ReceiveResultPath(options.ReceiveResultPath);

  if (ReceiveResultPath.isEmpty())
    ReceiveResultPath = QDir::homePath() + "/wrx";
  return ReceiveResultPath + dt.toString("/yyyy/MM-dd/");
}


// static
QString ReceiveJob::makeTodayPath() {
QDir dir;
QString ReceiveResultPath(options.ReceiveResultPath);
QDateTime  dt(options.showTimeAsUTC ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
QString FilePath;

  logger.debug("ReceiveJob::makeTodayPath()", __LINE__, "options.showTimeAsUTC = %s  dt = %s", options.showTimeAsUTC ? "on" : "off", qPrintable(dt.toString("yyyy-MM-dd--hh.mm.ss")));

  if (!dir.mkpath(ReceiveResultPath)) {
    logger.error("ReceiveJob::makeTodayPath()", __LINE__, "Cannot create root path for results (\"%s\")", qPrintable(ReceiveResultPath));
    ReceiveResultPath = QDir::homePath() + "/wrx";
    if (!dir.mkpath(ReceiveResultPath)) {
      logger.error("ReceiveJob::makeTodayPath()", __LINE__, "2. try! Cannot create root path for results (\"%s\") using .", qPrintable(ReceiveResultPath));
      ReceiveResultPath = dir.currentPath();
    }
  }
  FilePath = ReceiveResultPath + dt.toString("/yyyy/MM-dd/");
  if (!dir.mkpath(FilePath)) {
    logger.error("ReceiveJob::makeTodayPath()", __LINE__, "Cannot create path for results (\"%s\")", qPrintable(FilePath));
  }
  return FilePath;
}


// static
QString ReceiveJob::timeFromUTC(const QTime & StartUTC, const QString & FormatStr, int DaysAgo) {

  QDateTime dt = QDateTime::currentDateTimeUtc().addDays(DaysAgo);
  dt.setTime(StartUTC);
  if (options.showTimeAsUTC)
    return dt.toString(FormatStr);
  // logger.debug("ReceiveJob::timeFromUTC()", __LINE__, "StartUTC = %s     dt = %s", qPrintable(StartUTC.toString("yyyy-MM-dd--hh.mm.ss")), qPrintable(dt.toString("yyyy-MM-dd--hh.mm.ss")));
  QDateTime local = dt.toLocalTime();
  // logger.debug("ReceiveJob::timeFromUTC()", __LINE__, "StartUTC = %s  local = %s", qPrintable(StartUTC.toString("yyyy-MM-dd--hh.mm.ss")), qPrintable(local.toString("yyyy-MM-dd--hh.mm.ss")));
  return local.toString(FormatStr);
}

// static
QString ReceiveJob::buildFileName(const QTime & StartUTC, const QString & JobFileName, RadioSetup * pRadioSetup, int DaysOld) {

  return timeFromUTC(StartUTC, "yyyy-MM-dd--hh.mm.ss", DaysOld) + "--" + JobFileName + "-" + pRadioSetup->Callsign +
         "@" + (pRadioSetup->CenterFreq < 0 ? "" : "+") + QString::number(pRadioSetup->CenterFreq) + "." + pRadioSetup->frequency->opMode.ModeType() + ".";
}


// for automatic record
QString ReceiveJob::buildWaveRecordFileName() const {

  return makeTodayPath() +
         buildFileName(StartUTC, FileName, activeRadioSetup, 0) +
         "wav";
}


// for automatic record decoder output
QString ReceiveJob::buildDecoderRecordFileName() const {
  // TODO: The decoder output must not always be text!
  return makeTodayPath() +
         buildFileName(StartUTC, FileName, activeRadioSetup, 0) +
         "txt";
}


// For looking for results
QString ReceiveJob::buildRecordFileNamePattern(int DaysOld) const {

  return buildFileName(StartUTC, FileName, activeRadioSetup, DaysOld) + "*";
}


// for manual record of test jobs
// static
QString ReceiveJob::buildManualWaveRecordFileName(RadioSetup * pRadioSetup) {
QDateTime dt(QDateTime::currentDateTimeUtc());

  return makeTodayPath() + buildFileName(dt.time(), "", pRadioSetup, 0) + "wav";
}

// for manual record decoder output in test jobs
// static
QString ReceiveJob::buildManualDecoderRecordFileName(RadioSetup * pRadioSetup) {
QDateTime dt(QDateTime::currentDateTimeUtc());

  return makeTodayPath() + buildFileName(dt.time(), "", pRadioSetup, 0) + "txt";
}


QString ReceiveJob::buildPostCommand(QString WaveFileName) {
char s[16];

  if (activeRadioSetup->PostCommand.isEmpty())
    return QString::null;

  QString Cmd = activeRadioSetup->PostCommand;
  Cmd.replace("%f", WaveFileName);
  QString OutFileName = WaveFileName.replace(".wav", QString(".") + activeRadioSetup->DecodedFileExt);
  Cmd.replace("%o", OutFileName);
  sprintf(s, "%i", activeRadioSetup->CenterFreq);
  return Cmd.replace("%c", s);
}


QStringList ReceiveJob::RadioSetupNamen() {
QStringList ret;
int  iFreq, nFreq, iRadio, nRadios;

  nFreq = lstFrequencies.count();
  for (iFreq = 0; iFreq < nFreq; iFreq++) {
    const QString & Callsign = lstFrequencies[iFreq]->Callsign;
    QStringList lstRadios = RadioSetup::findRadios(Callsign);
    nRadios = lstRadios.count();
    for (iRadio = 0; iRadio < nRadios; iRadio++) {
      RadioSetup * rs = RadioSetup::find(Callsign, lstRadios[iRadio]);
      if (rs != nullptr)
        ret.append(rs->Name());
    }
  }
  return ret;
}


bool ReceiveJob::canStart() {
bool ret = Enable &&
           !TimeConflict &&
           activeRadioSetup != nullptr &&
           activeRadioSetup->pRadio->Enable &&
           (!activeRadioSetup->isRunPostCommand || !activeRadioSetup->PostCommand.isEmpty()); // &&
/*
           ((activeRadioSetup->RunType == RadioSetup::RUN_TYPE_INTERN) ||
            (activeRadioSetup->ExternStopType == RadioSetup::EXTERN_STOP_BY_COMMAND) ||
            (!activeRadioSetup->ExternStopCommand.isEmpty())) &&
           (State == JOB_STATE_WAITING || State == JOB_STATE_SOON || State == JOB_STATE_SUCCESS);
*/
  return ret;
}


enum RadioSetup::RunType ReceiveJob::RunType() {

  if (activeRadioSetup != nullptr) {
    if (activeRadioSetup->RunType == RadioSetup::RUN_TYPE_EXTERN)
      return RadioSetup::RUN_TYPE_EXTERN;
    if (canStart())
      return RadioSetup::RUN_TYPE_INTERN;
  }
  return RadioSetup::RUN_TYPE_NORUN;
}


void ReceiveJob::setCallsignRadio(QString CallsignRadio) {
  activeRadioSetup = RadioSetup::findByName(CallsignRadio);
}


int ReceiveJob::ProgressInfo() {
int ret = Progress;

  if (activeRadioSetup == nullptr ||
      activeRadioSetup->pRadio == nullptr ||
      lstFrequencies.count() == 0) {
    ret = PROGRESS_CONFIG_ERROR;
  } else {
    if (!Enable) {
      ret = PROGRESS_DISABLED;
    } else {
      if (!activeRadioSetup->pRadio->Enable) {
        ret = PROGRESS_RADIO_OFF;
      } else {
        if (TimeConflict)
          ret = PROGRESS_TIMECONFLICT;
      }
    }
  }
  return ret;
}


int ReceiveJob::ResultInfo() {
int ResultType = 0;

  if (activeRadioSetup != nullptr) {
    switch (activeRadioSetup->frequency->opMode.IdMode) {
    case OpMode::SPEACH_AM :
    case OpMode::SPEACH_NFM :
    case OpMode::SPEACH_WFM :
    case OpMode::SPEACH_SSB :
    case OpMode::SPEACH :
      ResultType = 4;
      break;
    case OpMode::NAVTEX :
    case OpMode::RTTY_85 :
    case OpMode::RTTY_450 :
      ResultType = 0;
      break;
    case OpMode::FAX :
      ResultType = 2;
      break;
    default :
      ;
    }
    if (nResults < 0)
      readInResults();
    if (nResults > 0)
      ResultType++;
  }
  return ResultType;
}

int ReceiveJob::MessageInfo() {
int MessageType = 0;

  for (int i = 0; i < lstMessages.count(); i++) {
    if (MessageType < lstMessages[i].Type + 1)
      MessageType = lstMessages[i].Type + 1;
  }
  return MessageType;
}

void ReceiveJob::setProgress(int prog) {

  switch (prog) {
  case PROGRESS_DISABLED :
  case PROGRESS_TIMECONFLICT :
  case PROGRESS_RADIO_OFF :
    // Do not set these values into Progress! They are states and hold on another places.
    break;
  default :
    Progress = prog;
  }
  switch (prog) {
  case PROGRESS_ERROR :
  case PROGRESS_OK :
  case PROGRESS_RECORD_OK :
  case PROGRESS_POSTPROC_OK :
    readInResults();
      break;
  default :
      break;
  }
}

// returns filenames with path
QStringList ReceiveJob::ResultFileNames() {

  if (nResults < 0)
    readInResults();

  return lstResultsWithPath;
}

// returns filenames without path
QStringList ReceiveJob::Results() {

  if (nResults < 0)
    readInResults();

  return lstResultsWithoutPath;
}


QString ReceiveJob::ToolTipResults() {
QString Results(QObject::tr("no results"));

  if (nResults < 0)
    readInResults();

  if (nResults) {
    Results = "";
    for (int iResult = 0; iResult < nResults; iResult++) {
      if (iResult > 0)
        Results += "\n";
      Results += lstResultsWithoutPath[iResult];
    }
  }
  return Results;
}


QString ReceiveJob::ToolTipErrors() const {
QString results(QObject::tr("no messages"));
int     i, nInfo = 0, nWarning = 0, nError = 0;

  for (i = 0; i < lstMessages.count(); i++) {
    const ReceiveJobMessage & rjm = lstMessages[i];
    if (rjm.Type == ReceiveJobMessage::RJ_MSG_INFO)
      nInfo++;
    if (rjm.Type == ReceiveJobMessage::RJ_MSG_WARNING)
      nWarning++;
    if (rjm.Type == ReceiveJobMessage::RJ_MSG_ERROR)
      nError++;
  }
  if ((nInfo + nWarning + nError) > 0) {
    results = "";
    if (nError > 0)
      results += QString::number(nError) + " " + QObject::tr("error(s)") + "\n";
    if (nWarning > 0)
      results += QString::number(nWarning) + " " + QObject::tr("warning(s)") + "\n";
    if (nInfo > 0)
      results += QString::number(nInfo) + " " + QObject::tr("info(s)") + "\n";
    results = results.left(results.length() -1);
  }
  return results;
}


JobOptions *ReceiveJob::currentJobOptions() {
JobOptions * jo = new JobOptions;

  jo->JobId       = Id;
  jo->JobType     = Type;
  jo->Duration    = Duration;
  jo->isWaterfall = true;
  jo->pRadioSetup = activeRadioSetup;
  if (activeRadioSetup != nullptr) {
    jo->isWaveRecord |= activeRadioSetup->isWaveRecord;
    switch (activeRadioSetup->DecoderType) {
    case RadioSetup::DECODER_INTERN :
      jo->isDecoder = true;
      jo->isDecoderRecord = true;
      jo->isWaveRecord = false;
      jo->isRunPostCommand = false;
      break;
    case RadioSetup::DECODER_EXTERN :
      jo->isRunPostCommand = true;
      jo->isWaveRecord = true;
      break;
    case RadioSetup::DECODER_NONE :
      jo->isRunPostCommand = !activeRadioSetup->PostCommand.isEmpty();
      jo->isWaveRecord = true;
    }
  }
  jo->WaveRecordFileName = buildWaveRecordFileName();
  jo->DecoderRecordFileName = buildDecoderRecordFileName();
  if (jo->isRunPostCommand)
    jo->PostCommand = buildPostCommand(jo->WaveRecordFileName);
  return jo;
}

// returns filenames with path
void ReceiveJob::readInResults() {
/* @TODO make maxDaysBack configurable */
int day, maxDaysBack = 5;
/*
QString ReceiveJob::calcOutputPath(int DaysOld) {
QDateTime  dt((options.showTimeAsUTC ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime()).addDays(DaysOld));
QString    ReceiveResultPath(options.ReceiveResultPath);

  if (ReceiveResultPath.isEmpty())
    ReceiveResultPath = QDir::homePath() + "/wrx";
  return ReceiveResultPath + dt.toString("/yyyy/MM-dd/");
}
*/
// QDateTime dt = QDateTime::currentDateTimeUtc().addDays(DaysAgo);

  int   DateDiff = 0;
  QDateTime dtUTC = QDateTime::currentDateTimeUtc();
  dtUTC.setTime(StartUTC);

  if (dtUTC > QDateTime::currentDateTimeUtc()) {
    DateDiff = -1;
    dtUTC = dtUTC.addDays(-1);
  }
  QDateTime dt(options.showTimeAsUTC ? dtUTC : dtUTC.toLocalTime());


  lstResultsWithPath.clear();
  lstResultsWithoutPath.clear();
  nResults = 0;

  for (day = 0; day < maxDaysBack; day++) {
    QDir OutputDir(calcOutputPath(dt, 0 - day));
    if (!OutputDir.exists() || (day == 0 && isRunning()))
      continue;
    QStringList FilterList;
    QString FileNamePattern(buildRecordFileNamePattern(DateDiff - day));
    FilterList.append(FileNamePattern);
    QStringList lst = OutputDir.entryList(FilterList);
    for (int iLst = 0; iLst < lst.count(); iLst++) {
      lstResultsWithPath.append(OutputDir.path() + QDir::separator() + lst[iLst]);
      lstResultsWithoutPath.append(lst[iLst]);
    }
  }
  nResults = lstResultsWithPath.count();
}
