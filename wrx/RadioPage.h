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

#ifndef RADIOPAGE_H
#define RADIOPAGE_H

#include "ui_RadioPage.h"
#include "RadioService.h"
#include "ReceiveJob.h"


#define RP_SHOW_DSP   1
#define RP_SHOW_VOL   2


class RadioPage : public QWidget, protected Ui::RadioPage {

  Q_OBJECT

public:
  RadioPage(QWidget *parent = nullptr, QString RadioName = "???");
  ~RadioPage();
  static RadioPage * findPage(QString & RadioName);

//  void   aktivateReceiveJob(ReceiveJob * rj);
//  void   newJobActivated(ReceiveJob * pNewJob);

public slots :

  void setupWaterfall(int Zoom, int Gain /*, int Range */);
  void on_sldZoom_valueChanged(int value);
  void on_sldGain_valueChanged(int value);
  void on_scbWaterfallRange_valueChanged(int value);
  void on_pbtStart_clicked();
  void on_pbtStop_clicked();
  void on_chbCenterReception_stateChanged(int);
  void on_tedDecodedText_textChanged();
  void on_chbDecoder_stateChanged(int state);
  void on_chbDecoderRecord_stateChanged(int state);
  void on_chbWaveRecord_stateChanged(int state);
  void on_chbRunPostCommand_stateChanged(int state);
  void on_cbxSender_currentIndexChanged(int index);

  void onJobStarted(RadioSetup *, int);
  void onJobFinished();

protected :

  void  setupWaterfallHeader();
  void  startTestJob(bool isDecoder, bool isRecordDecoder, bool isRecordWave);

  QString RadioName;
  static  QList<RadioPage *> lstRadioPages;

  RadioService * pRadioService;
  ReceiveJob     TestJob;
//  ReceiveJob   * currentRunningJob;
  RadioSetup   * pCurrentRadioSetup;
};

#endif // RADIOPAGE_H
