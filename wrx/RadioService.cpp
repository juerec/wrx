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

#include "Logger.h"

QList<RadioService *> RadioService::lstRadioService;

RadioService::RadioService(Radio * _pRadio)
            : pRadio(_pRadio),
              RGBLineFiFo(),
              WaterfallFiFo(),
              TextDecoderFiFo(),
              WaterfallWorker(&WaterfallFiFo, &RGBLineFiFo),
              TextDecoderWorker(&TextDecoderFiFo, pRadio->Name),
              CaptureThread(&WaterfallFiFo, &TextDecoderFiFo, &WaterfallWorker, &TextDecoderWorker, pRadio) {

  logger.debug("RadioService::RadioService()", __LINE__, "Radio = %s", qPrintable(pRadio->Name));

  WaterfallWorker.setupWaterfall(WaterfallInfo(512, 0, 2, 0.03f));
  CaptureThread.start();
  WaterfallWorker.start();
  TextDecoderWorker.start();
}


RadioService::~RadioService() {
  WaterfallWorker.termThread();
  CaptureThread.termThread();
  WaterfallFiFo.clear();
}


RadioService * RadioService::findRadioService(QString RadioName) {

  for (int i = 0; i < lstRadioService.count(); i++) {
    RadioService * rs = lstRadioService[i];
    if (rs->pRadio->Name == RadioName)
      return rs;
  }
  return nullptr;
}


void RadioService::initServices() {

  if (lstRadioService.count() > 0) {
    logger.error("RadioService::initServices()", __LINE__, "lstRadioService is not empty.");
    return;
  }
  for (int i = 0; i < Radio::lstRadios.count(); i++) {
    Radio * pRadio =  Radio::lstRadios[i];
    RadioService * rs = new RadioService(pRadio);
    lstRadioService.append(rs);
  }
}


// static
void RadioService::staticConnectCaptureThreads(QObject * pMainWindow) {

  for (int i = 0; i < lstRadioService.count(); i++)
    lstRadioService[i]->connectCaptureThread(pMainWindow);

}


void RadioService::connectCaptureThread(QObject * pMainWindow) {
  QObject::connect((QObject*)&CaptureThread, SIGNAL(Progress(int, int)), pMainWindow, SLOT(onProgressInfo(int, int)));
  QObject::connect((QObject*)&CaptureThread, SIGNAL(TextInfo(QString, int)), pMainWindow, SLOT(onTextInfo(QString, int)));
}


void RadioService::connectWaterfall(QObject * pRadioTabPage) {
  QObject::connect(&WaterfallWorker, SIGNAL(newRGBLine(RGBLine *, int)), pRadioTabPage, SLOT(newRGBLine(RGBLine *, int)));
}


void RadioService::connectTextDecoder(QObject * pEditText) {
  QObject::connect(&TextDecoderWorker, SIGNAL(decodedText(QString)), pEditText, SLOT(insertPlainText(const QString&)));
}


void RadioService::connectCaptureThreadStartStop(QObject * pRadioPage) {
  QObject::connect(&CaptureThread, SIGNAL(JobStarted(RadioSetup *, int)), pRadioPage, SLOT(onJobStarted(RadioSetup *, int)));
  QObject::connect(&CaptureThread, SIGNAL(JobFinished()), pRadioPage, SLOT(onJobFinished()));
}


void RadioService::staticTermThreads() {
int i, nServices = lstRadioService.count();

  for (i = 0; i < nServices; i++)
    lstRadioService[i]->termThreads();

  for (i = 0; i < nServices; i++)
    lstRadioService[i]->waitThreads();
}


void RadioService::termThreads() {
  CaptureThread.termThread();
  WaterfallWorker.termThread();
}


void RadioService::waitThreads() {

  CaptureThread.wait();
  WaterfallWorker.wait();
}
