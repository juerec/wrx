#include "RadioPage.h"
#include "ui_RadioPage.h"
#include "Logger.h"
#include "Options.h"
#include "WaterfallInfo.h"
#include "Frequency.h"

// #include <QList>

QList<RadioPage *> RadioPage::lstRadioPages;


RadioPage::RadioPage(QWidget *parent, QString RadioName)
         : QWidget (parent), TestJob(777) {

  setupUi(this);
  this->RadioName = RadioName;
  pRadioService = RadioService::findRadioService(RadioName);
  if (pRadioService == nullptr) {
    logger.error("RadioPage::RadioCommon()", __LINE__, "Cannot find service for radio named \"%s\"", qPrintable(RadioName));
    return;
  }

  for (int i = 0; i < Frequency::lstFrequencies.count(); i++) {
    Frequency * freq = Frequency::lstFrequencies[i];
    QString     lstRadios = "|" + freq->Radios + "|";
    QString     radio = "|" + RadioName + "|";
    if (lstRadios.indexOf(radio) >= 0)
      cbxSender->addItem(freq->Name, freq->Callsign);
  }
  QString CallSign = cbxSender->itemData(cbxSender->currentIndex()).toString();
  TestJob.activeRadioSetup = RadioSetup::find(CallSign, RadioName);
  TestJob.Duration = 0;
  TestJob.Id = 777;
  TestJob.Enable = true;
  TestJob.Type = ReceiveJob::JOB_TYPE_TEST;
  TestJob.TimeConflict = false;
  pCurrentRadioSetup = nullptr;

  pRadioService->connectWaterfall(frmWaterfall);
  pRadioService->connectTextDecoder(tedDecodedText);
  pRadioService->connectCaptureThreadStartStop(this);
  lstRadioPages.append(this);

  int Zoom     =  2;
  int Gain     = -4;
  int binLeft  =  0;

  QStringList lst = options.readWaterfallPrefs(RadioName);
  for (int i = 0; i < lst.count(); i++) {
    QString str = lst[i];
    if (str.startsWith("WaterfallZoom=")) {
      Zoom = str.mid(14).toInt();
    }
    if (str.startsWith("WaterfallGain=")) {
      Gain = str.mid(14).toInt();
    }
    if (str.startsWith("WaterfallIBinLeft=")) {
      binLeft = str.mid(18).toInt();
    }
    if (str.startsWith("Center="))
      chbCenterReception->setChecked(str.mid(7).toInt());
    if (str.startsWith("WaveRecord="))
      chbWaveRecord->setChecked(str.mid(11).toInt());
    if (str.startsWith("Decoder="))
      chbDecoder->setChecked(str.mid(8).toInt());
    if (str.startsWith("DecoderRecord="))
      chbDecoderRecord->setChecked(str.mid(14).toInt());
    if (str.startsWith("PostCommand="))
      chbRunPostCommand->setChecked(str.mid(12).toInt());
  }

  if (Zoom < 1 || Zoom > 8)
    Zoom = 2;
  if (Gain < -5 || Gain > 5)
    Gain = -4;
  if (binLeft < 0 || binLeft > 512 * (Zoom - 1))
    binLeft = 0;

  sldGain->setValue(Gain);
  sldZoom->setValue(Zoom);
  scbWaterfallRange->setValue(binLeft);
}


RadioPage::~RadioPage() {

  QStringList lst;
  lst.append("WaterfallZoom=" + QString::number(sldZoom->sliderPosition()));
  lst.append("WaterfallGain=" + QString::number(sldGain->sliderPosition()));
  lst.append("WaterfallIBinLeft=" + QString::number(scbWaterfallRange->sliderPosition()));
  lst.append("Center=" + QString::number(chbCenterReception->isChecked()));
  lst.append("WaveRecord=" + QString::number(chbWaveRecord->isChecked()));
  lst.append("Decoder=" + QString::number(chbDecoder->isChecked()));
  lst.append("DecoderRecord=" + QString::number(chbDecoderRecord->isChecked()));
  lst.append("PostCommand=" + QString::number(chbRunPostCommand->isChecked()));

  options.writeWaterfallPrefs(lst, RadioName);
}


void RadioPage::setupWaterfall(int Zoom, int Gain/*, int Range*/) {
WaterfallInfo wi;

  setupWaterfallHeader();
  wi.nBins   = 512;
  wi.binLeft = scbWaterfallRange->sliderPosition();
  wi.Gain    = Gain >= 0 ? float(1 << Gain) : 1.0f / float(64 >> (6 + Gain));
  wi.Zoom    = Zoom;
  pRadioService->setupWaterfall(wi);
}


RadioPage * RadioPage::findPage(QString & RadioName) {
int nPages = lstRadioPages.count();
RadioPage * rp;

  for (int i = 0; i < nPages; i++) {
    rp = lstRadioPages[i];
    if (rp->RadioName == RadioName)
      return rp;
  }
  logger.error("RadioPage::findPage()", __LINE__, "No RadioPage found for \"%s\"", qPrintable(RadioName));
  return nullptr;
}


void RadioPage::on_sldZoom_valueChanged(int value) {
  scbWaterfallRange->setRange(0, 512 * (value -1));
  scbWaterfallRange->setPageStep(512);
  setupWaterfall(value, sldGain->sliderPosition()); //, scbWaterfallRange->sliderPosition());
}


void RadioPage::on_sldGain_valueChanged(int value) {
  setupWaterfall(sldZoom->sliderPosition(), value); //, scbWaterfallRange->sliderPosition());
}


void RadioPage::on_scbWaterfallRange_valueChanged(int /*value*/) {
  setupWaterfall(sldZoom->sliderPosition(), sldGain->sliderPosition()); //, value);
}


void RadioPage::on_pbtStart_clicked() {
QString CallSign = cbxSender->itemData(cbxSender->currentIndex()).toString();

  on_pbtStop_clicked();
  pCurrentRadioSetup = RadioSetup::find(CallSign, RadioName);
  TestJob.activeRadioSetup = pCurrentRadioSetup;
  if (TestJob.activeRadioSetup == nullptr) {
    logger.error("RadioPage::findPage()", __LINE__, "No RadioSetup found for \"%s\" + \"%s\"", qPrintable(CallSign), qPrintable(RadioName));
  } else {
    if (TestJob.canStart()) {
      bool canDecode = pCurrentRadioSetup->frequency->opMode.IdMode == OpMode::RTTY_85 ||
                       pCurrentRadioSetup->frequency->opMode.IdMode == OpMode::RTTY_450 ||
                       pCurrentRadioSetup->frequency->opMode.IdMode == OpMode::NAVTEX;
      JobOptions * jo = TestJob.currentJobOptions();
      pCurrentRadioSetup = TestJob.activeRadioSetup;
      jo->isDecoder = canDecode && chbDecoder->isChecked();
      jo->isWaveRecord = chbWaveRecord->isChecked();
      if (jo->isWaveRecord)
        jo->WaveRecordFileName = TestJob.buildManualWaveRecordFileName(jo->pRadioSetup);
      jo->isDecoderRecord = canDecode && jo->isDecoder && chbDecoderRecord->isChecked();
      if (jo->isDecoderRecord)
        jo->DecoderRecordFileName = TestJob.buildManualDecoderRecordFileName(jo->pRadioSetup);
      jo->isRunPostCommand = jo->isWaveRecord && chbRunPostCommand->isChecked();
      if (jo->isRunPostCommand)
        jo->PostCommand = TestJob.buildPostCommand(jo->WaveRecordFileName);
      pRadioService->startTestJob(jo);
      setupWaterfall(sldZoom->sliderPosition(), sldGain->sliderPosition());
      TestJob.State = ReceiveJob::JOB_STATE_RUNNING;
    }
  }
}


void RadioPage::on_pbtStop_clicked() {
  pRadioService->stopTestJob();
  TestJob.State = ReceiveJob::JOB_STATE_WAITING;
}


void RadioPage::on_chbDecoder_stateChanged(int state) {

  logger.debug("RadioPage::on_chbDecoder_stateChanged()", __LINE__, "state %c= Qt::Checked", state == Qt::Checked ? '=' : '!');
  if (TestJob.State == ReceiveJob::JOB_STATE_RUNNING) {
    JobOptionsModify jom;
    jom.aDecoder        = state == Qt::Checked ? JobOptionsModify::Start : JobOptionsModify::Stop;
    jom.aDecoderRecord  = state == Qt::Checked ? (chbDecoderRecord->checkState() == Qt::Checked ? JobOptionsModify::Start : JobOptionsModify::Keep) : JobOptionsModify::Stop;
    jom.aWaveRecord     = JobOptionsModify::Keep;
    jom.aRunPostCommand = JobOptionsModify::Keep;
    if (jom.aDecoderRecord == JobOptionsModify::Start)
      jom.DecoderRecordFileName = ReceiveJob::buildManualDecoderRecordFileName(pCurrentRadioSetup);
    pRadioService->modifyTestJob(jom);
  }
}


void RadioPage::on_chbDecoderRecord_stateChanged(int state) {
  if (TestJob.State == ReceiveJob::JOB_STATE_RUNNING)
    if (chbDecoder->checkState() == Qt::Checked) {
      JobOptionsModify jom;
      jom.aDecoder        = JobOptionsModify::Keep;
      jom.aDecoderRecord  = state == Qt::Checked ? JobOptionsModify::Start : JobOptionsModify::Stop;
      if (jom.aDecoderRecord == JobOptionsModify::Start)
        jom.DecoderRecordFileName = ReceiveJob::buildManualDecoderRecordFileName(pCurrentRadioSetup);
      jom.aWaveRecord     = JobOptionsModify::Keep;
      jom.aRunPostCommand = JobOptionsModify::Keep;
      pRadioService->modifyTestJob(jom);
    }
}


void RadioPage::on_chbWaveRecord_stateChanged(int state) {
  if (TestJob.State == ReceiveJob::JOB_STATE_RUNNING) {
    JobOptionsModify jom;
    jom.aDecoder        = JobOptionsModify::Keep;
    jom.aDecoderRecord  = JobOptionsModify::Keep;
    jom.aWaveRecord     = state == Qt::Checked ? JobOptionsModify::Start : JobOptionsModify::Stop;
    if (jom.aWaveRecord == JobOptionsModify::Start)
      jom.WaveRecordFileName = ReceiveJob::buildManualWaveRecordFileName(pCurrentRadioSetup);
    if (state == Qt::Checked && chbRunPostCommand->isChecked())
      jom.aRunPostCommand = JobOptionsModify::Start;
    else
      jom.aRunPostCommand = JobOptionsModify::Keep;
    pRadioService->modifyTestJob(jom);
  }
}


void RadioPage::on_chbRunPostCommand_stateChanged(int state) {
  if (TestJob.State == ReceiveJob::JOB_STATE_RUNNING)
    if (chbWaveRecord->isChecked()) {
      JobOptionsModify jom;
      jom.aDecoder        = JobOptionsModify::Keep;
      jom.aDecoderRecord  = JobOptionsModify::Keep;
      jom.aWaveRecord     = JobOptionsModify::Keep;
      jom.aRunPostCommand = state == Qt::Checked ? JobOptionsModify::Start : JobOptionsModify::Stop;
      pRadioService->modifyTestJob(jom);
    }
}


void RadioPage::on_cbxSender_currentIndexChanged(int) {
QString CallSign = cbxSender->itemData(cbxSender->currentIndex()).toString();
bool    canDecode;

  pCurrentRadioSetup = RadioSetup::find(CallSign, RadioName);
  canDecode = pCurrentRadioSetup->frequency->opMode.IdMode == OpMode::RTTY_85 ||
              pCurrentRadioSetup->frequency->opMode.IdMode == OpMode::RTTY_450 ||
              pCurrentRadioSetup->frequency->opMode.IdMode == OpMode::NAVTEX;
  chbDecoder->setEnabled(canDecode);
  chbDecoderRecord->setEnabled(canDecode);
}


void RadioPage::onJobStarted(RadioSetup * pRadioSetup, int JobType) {

  if (JobType == ReceiveJob::JOB_TYPE_TIMED) {
    pbtStart->setEnabled(false);
    pbtStop->setEnabled(false);
    QString & Callsign = pRadioSetup->Callsign;
    cbxSender->setEnabled(false);
    cbxSender->hidePopup();
    int n = cbxSender->count();
    for (int i = 0; i < n; i++)
      if (cbxSender->itemData(i).toString() == Callsign) {
        cbxSender->setCurrentIndex(i);
        break;
      }
  }
  pCurrentRadioSetup = pRadioSetup;
  setupWaterfall(sldZoom->sliderPosition(), sldGain->sliderPosition());
}


void RadioPage::onJobFinished() {
  cbxSender->setEnabled(true);
  pbtStart->setEnabled(true);
  pbtStop->setEnabled(true);
  pCurrentRadioSetup = nullptr;
}


void RadioPage::setupWaterfallHeader() {
int CenterFreq = 2000;
int SampleRate = 48000;
int Channels   = 2;
int Shift      = 450;
int left, center, right;
int Zoom       = sldZoom->sliderPosition();
int sliderPos  = scbWaterfallRange->sliderPosition();
int newSliderPos;

  if (pCurrentRadioSetup != nullptr) {
      CenterFreq = pCurrentRadioSetup->CenterFreq;
      SampleRate = pCurrentRadioSetup->SampleRate;
      Channels   = pCurrentRadioSetup->Channels;
      Shift      = pCurrentRadioSetup->frequency->opMode.Shift;

      logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "Zoom = %i  Minimum = %i   Maximum = %i   sliderPos = %i", Zoom, scbWaterfallRange->minimum(), scbWaterfallRange->maximum(), sliderPos);

      if (chbCenterReception->checkState() == Qt::Checked) {
        if (Channels == 1) {
          newSliderPos = (CenterFreq * 2 * 512 * Zoom) / SampleRate - 256;
          if (newSliderPos < 0)
            newSliderPos = 0;
          if (newSliderPos >= 512 * (Zoom - 1))
            newSliderPos = 512 * (Zoom - 1) - 1;

          logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "CenterFreq = %i   SampleRate = %i   newSliderPos = %i", CenterFreq, SampleRate, newSliderPos);
          center = (512 * 2 * Zoom * CenterFreq) / SampleRate - newSliderPos;
          left   = (512 * 2 * Zoom * (CenterFreq - (Shift >> 1))) / SampleRate - newSliderPos;
          right  = (512 * 2 * Zoom * (CenterFreq + (Shift >> 1))) / SampleRate - newSliderPos;
          logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "Channels = %i   center = %i   left = %i   right = %i   newSliderPos = %i",
                                                                     Channels, center, left, right, newSliderPos);
        } else {
          newSliderPos = ((SampleRate / 2 + CenterFreq) * 512 * Zoom / SampleRate) - 256;
          if (newSliderPos < 0)
            newSliderPos = 0;
          if (newSliderPos >= 512 * (Zoom - 1))
            newSliderPos = 512 * (Zoom - 1) - 1;

          // logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "CenterFreq = %i   SampleRate = %i   newSliderPos = %i", CenterFreq, SampleRate, newSliderPos);
          center = (256 + (512 * CenterFreq) / SampleRate) * Zoom - newSliderPos;
          left   = (256 + (512 * (CenterFreq - (Shift >> 1))) / SampleRate) * Zoom - newSliderPos;
          right  = (256 + (512 * (CenterFreq + (Shift >> 1))) / SampleRate) * Zoom - newSliderPos;

          // logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "Zoom   = %4i   sliderPos = %4i   newSliderPos = %4i", Zoom, sliderPos, newSliderPos);
          // logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "center = %4i   left      = %4i   right        = %4i", center, left, right);
        }
        if (newSliderPos != sliderPos)
          scbWaterfallRange->setSliderPosition(newSliderPos);
      } else {
        //  logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "CenterFreq = %i   SampleRate = %i", CenterFreq, SampleRate);
        // kein Zentrieren
        if (Channels == 1) {
          center = 0;
          left   = ((512 * (CenterFreq - (Shift >> 1))) / SampleRate) * Zoom * 2 - sliderPos;
          right  = ((512 * (CenterFreq + (Shift >> 1))) / SampleRate) * Zoom * 2 - sliderPos;
        } else {
          center = (256 + (512 * CenterFreq) / SampleRate) * Zoom - sliderPos;
          left   = (256 + (512 * (CenterFreq - (Shift >> 1))) / SampleRate) * Zoom - sliderPos;
          right  = (256 + (512 * (CenterFreq + (Shift >> 1))) / SampleRate) * Zoom - sliderPos;
        }
        // logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "Zoom   = %4i   sliderPos = %4i", Zoom, sliderPos);
        // logger.debug("RadioPage::setupWaterfallHeader()", __LINE__, "center = %4i   left      = %4i   right        = %4i", center, left, right);
      }
      wdgWaterfallHeader->setParams(pCurrentRadioSetup->frequency->opMode.IdMode, left, center, right);
    }
}


void RadioPage::on_chbCenterReception_stateChanged(int state) {
  logger.debug("RadioPage::on_chbCenterReception_stateChanged()", __LINE__, "state = %i", state);

  scbWaterfallRange->setEnabled(state != Qt::Checked);

  if (state == Qt::Checked) {
    setupWaterfall(sldZoom->sliderPosition(), sldGain->sliderPosition());
    if (TestJob.activeRadioSetup != nullptr) {
      logger.error("RadioPage::on_chbCenterReception_stateChanged()", __LINE__, "Job.activeRadioSetup == NULL");
    } else {
      const char * state;
      switch (TestJob.State) {
      case ReceiveJob::JOB_STATE_WAITING :
        state = "JOB_STATE_WAITING";
        break;
      case ReceiveJob::JOB_STATE_ERROR :
        state = "JOB_STATE_ERROR";
        break;
      case ReceiveJob::JOB_STATE_SOON :
        state = "JOB_STATE_SOON";
        break;
      case ReceiveJob::JOB_STATE_RUNNING :
        state = "JOB_STATE_RUNNING";
        break;
      case ReceiveJob::JOB_STATE_SUCCESS :
        state = "JOB_STATE_SUCCESS";
        break;
      default :
        state = "JOB_STATE_????";
      }
      logger.debug("RadioPage::on_chbCenterReception_stateChanged()", __LINE__, "Job.State = %s", state);
    }
  }
}


void RadioPage::on_tedDecodedText_textChanged() {
  tedDecodedText->ensureCursorVisible();
}
