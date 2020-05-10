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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMenu>
#include <QMessageBox>

#include "RadioPage.h"
#include "Settings.h"
#include "ReceiveJobInfoDlg.h"
#include "ReceiveJobMessagesDlg.h"
#include "ReceiveJobResultDlg.h"

#include "Options.h"
#include "Logger.h"
#include "Decoder.h"

MainWindow * MainWindow::pMainWindow;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), Ui::MainWindow(), tableModel(nullptr) {
    setupUi(this);

    pMainWindow = this;
    pTableModel = &tableModel;
    for (int i = Radio::lstRadios.count() -1; i >= 0; i--) {
      Radio * radio = Radio::lstRadios[i];
      twgRadios->insertTab(0, new RadioPage(twgRadios, radio->Name), radio->NameLong);
    }
    while (twgRadios->count() > Radio::lstRadios.count())
      twgRadios->removeTab(Radio::lstRadios.count());

    twgRadios->setCurrentIndex(0);
    tvwSched->setModel(&tableModel);
    tvwSched->verticalHeader()->hide();
    ComboBoxDelegate * delegate = new ComboBoxDelegate(tvwSched);
    tvwSched->setItemDelegate(delegate);
    tvwSched->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    QHeaderView * hv = tvwSched->horizontalHeader();
    hv->setMinimumSectionSize(28);
    for (int i = 0; i < options.SchedulerColWidth.count(); i++)
      hv->resizeSection(i, options.SchedulerColWidth[i]);

    QFont Font(tvwSched->font());
    Font.setPointSize(options.FontSizeJobList);
    tvwSched->setFont(Font);
    tvwSched->setContextMenuPolicy(Qt::CustomContextMenu);

    twgSettings = new Settings(stwPageSettings);
    twgSettings->init();
    verticalLayout_3->addWidget(twgSettings);

    wdgBrowser = new BrowserPage(stwPageBrowser);
    verticalLayout_Browser->addWidget(wdgBrowser);

    pTimer = new QTimer(this);
    pTimer->start(1000);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(SignalTimeout()));
    connect(this, SIGNAL(Progress(int, int)), this, SLOT(onProgressInfo(int, int)));
    connect(&tableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), tvwSched, SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(&tableModel, SIGNAL(radioChanged()), this, SLOT(on_pTableModel_radioChanged()));
    RadioService::staticConnectCaptureThreads(this); // does connect the capture threads

    stackedWidget->setCurrentWidget(stwPageSched);
    checkTimeConflicts();

    setWindowIcon(QIcon(":/img/wrx.xpm"));
}


MainWindow::~MainWindow() {
}


void MainWindow::onProgressInfo(int Progress, int JobId) {
int nJobs = ReceiveJob::lstReceiveJobs.count();
int iJob;

  for (iJob = 0; iJob < nJobs; iJob++) {
    ReceiveJob * rj = ReceiveJob::lstReceiveJobs[iJob];
    if (rj->Id == JobId) {
      QModelIndex index = tableModel.index(iJob, COL_PROGRESS);
      tableModel.setData(index, QVariant(Progress), Qt::DecorationRole);
      return;
    }
  }
}


void MainWindow::onTextInfo(QString RjMsg, int JobId) {
int nJobs = ReceiveJob::lstReceiveJobs.count();
int iJob;

  logger.debug("MainWindow::onTextInfo()", __LINE__, "JobId = %i    RjMsg = %s", JobId, qPrintable(RjMsg));
  for (iJob = 0; iJob < nJobs; iJob++) {
    ReceiveJob * rj = ReceiveJob::lstReceiveJobs[iJob];
    if (rj->Id == JobId) {
      QModelIndex index = tableModel.index(iJob, COL_ERRORS);
      tableModel.setData(index, QVariant(RjMsg), Qt::DecorationRole);
      return;
    }
  }
}


void MainWindow::on_pbtSched_clicked() {
  stackedWidget->setCurrentWidget(stwPageSched);
}


void MainWindow::on_pbtRadio_clicked() {
  stackedWidget->setCurrentWidget(stwPageRadio);
}


void MainWindow::on_pbtBrowser_clicked() {
  stackedWidget->setCurrentWidget(stwPageBrowser);
}


void MainWindow::on_pbtSettings_clicked() {
  stackedWidget->setCurrentWidget(stwPageSettings);
}


void MainWindow::SignalTimeout() {
QDateTime now(QDateTime::currentDateTimeUtc());
QTime     time(now.time());
int       nJobs = ReceiveJob::lstReceiveJobs.count();
int       iJob;
int       iStart = time.hour() * 3600 + time.minute() * 60 + time.second();
int       JobId;
bool      success = false;


  for (iJob = 0; iJob < nJobs; iJob++) {
    ReceiveJob * rj = ReceiveJob::lstReceiveJobs[iJob];

    if (rj->RunType() == RadioSetup::RUN_TYPE_NORUN) {
//      logger.warning("MainWindow::SignalTimeout()", __LINE__, "RUN_TYPE_NORUN:  Id = %i", rj->Id);
      continue;
    }
    if (!rj->canStart())
      continue;
    int diff = rj->iStart - iStart;
    if (diff > 0) {
      // yellow square ==> starting soon
      if (diff < 3 * 60 * 60) {
        if (rj->progress() != PROGRESS_SOON)
          onProgressInfo(PROGRESS_SOON, rj->Id);
      } else {
        if (rj->progress() == PROGRESS_SOON)
          onProgressInfo(PROGRESS_WAIT, rj->Id);
      }
    } else {
      if (diff > -180 && rj->lastStart.secsTo(now) > 3 * 60 * 60)  {
        if (options.runTimedJobs) {
          rj->lastStart = now;
          RadioService * rs = RadioService::findRadioService(rj->activeRadioSetup->pRadio->Name);
          rs->activateReceiveJob(rj->currentJobOptions());
        }
      }
    }
  }
  // Clean up terminated decoder objecs.
  while (pDecoder->findTerminatedDecoder(&JobId, &success))
    pDecoder->deleteTerminatedDecoders();
}


void MainWindow::on_pTableModel_radioChanged() {
  logger.debug("MainWindow::on_pTableModel_radioChanged()", __LINE__, "* * * * *");
  checkTimeConflicts();
}


void MainWindow::on_tvwSched_clicked(const QModelIndex & index) {
  logger.debug("MainWindow::on_tvwSched_clicked()", __LINE__, "row = %i   column = %i", index.row(), index.column());
  if (index.isValid()) {
    if (index.column() == COL_RESULTS) {
      ReceiveJob * rj = ReceiveJob::lstReceiveJobs[index.row()];
      QStringList lstResultFileNames = rj->ResultFileNames();
      if (lstResultFileNames.count() > 0) {
        ReceiveJobResultDlg dlg(this);
        dlg.setData(lstResultFileNames);
        dlg.exec();
      }
    }
    if (index.column() == COL_ERRORS) {
      ReceiveJob * rj = ReceiveJob::lstReceiveJobs[index.row()];
      if (rj->lstMessages.count() > 0) {
        ReceiveJobMessagesDlg dlg(this);
        dlg.setData(rj);
        dlg.exec();
      }
    }
  }
}


void MainWindow::on_tvwSched_customContextMenuRequested(const QPoint & pos) {
QModelIndex index = tvwSched->indexAt(pos);

  if (index.isValid())
    logger.debug("MainWindow::on_tvwSched_customContextMenuRequested()", __LINE__, "row = %i   column = %i", index.row(), index.column());
  else {
    logger.debug("MainWindow::on_tvwSched_customContextMenuRequested()", __LINE__, "index is invalid");
    return;
  }

  int row = index.row();
  ReceiveJob * rj =  ReceiveJob::lstReceiveJobs[row];

  QPoint globalPos = tvwSched->mapToGlobal(pos);
  QHeaderView * horizontalHeader = tvwSched->horizontalHeader();
  globalPos.setY(globalPos.y() + horizontalHeader->height());

  QStringList lstResultFileNames = rj->ResultFileNames();

  QAction ResultAction(tr("Show results"));
  ResultAction.setEnabled(lstResultFileNames.count() > 0);
  QAction MessageAction(tr("Show messages"));
  MessageAction.setEnabled(rj->lstMessages.count() > 0);
  QAction InfoAction(tr("Show detailed info"));

  QMenu menu;
  menu.addAction(&ResultAction);
  menu.addAction(&MessageAction);
  menu.addAction(&InfoAction);

  QAction* selectedItem = menu.exec(globalPos);

  if (selectedItem == &ResultAction) {
    logger.debug("MainWindow::on_tvwSched_customContextMenuRequested()", __LINE__, "selectedItem ==>  ResultAction");
    ReceiveJobResultDlg dlg(this);
    dlg.setData(lstResultFileNames);
    dlg.exec();
  }
  if (selectedItem == &MessageAction) {
    ReceiveJobMessagesDlg dlg(this);
    dlg.setData(rj);
    dlg.exec();
  }
  if (selectedItem == &InfoAction) {
    ReceiveJobInfoDlg dlg(this);
    dlg.setData(rj);
    dlg.exec();
  }
  if (selectedItem == nullptr)
    logger.debug("MainWindow::on_tvwSched_customContextMenuRequested()", __LINE__, "selectedItem ==>  nullptr");
}


void MainWindow::closeEvent(QCloseEvent * e) {
int iCol;

  if (!options.confirmQuit || !QMessageBox::warning(this, tr("WRX"), tr("Really exit WRX?"), "Ok", tr("Cancel"), QString::null, 0, -1)) {
    e->accept();
    if (!(isMinimized() || isMaximized() || isFullScreen())) {
      options.WindowXY = pos();
      options.WindowWH = size();
    }
    QHeaderView * hv = tvwSched->horizontalHeader();
    for (iCol = 0; iCol < options.SchedulerColWidth.count() && iCol < SCHED_COLUMNS; iCol++)
      options.SchedulerColWidth[iCol] = hv->sectionSize(iCol);
    for ( ; iCol < SCHED_COLUMNS; iCol++)
      options.SchedulerColWidth.append(hv->sectionSize(iCol));
    RadioService::staticTermThreads();
  } else {
    e->ignore();
  }
}


void MainWindow::checkTimeConflicts() {
int  iRadio, nRadios;
Radio  * radio;
int  iJob, nJobs;
ReceiveJob * job;
ReceiveJob * job0;
ReceiveJob * jobY;
QList<ReceiveJob*> lstJobs;

  logger.debug("MainWindow::checkTimeConflicts()", __LINE__, "- - -");

  nRadios = Radio::lstRadios.count();
  for (iRadio = 0; iRadio < nRadios; iRadio++) {
    radio = Radio::lstRadios[iRadio];

    lstJobs.clear();  // clear list with jobs per radio
    nJobs = ReceiveJob::lstReceiveJobs.count();
    // fill lstJobs with jobs assigned to this radio
    for (iJob = 0; iJob < nJobs; iJob++) {
      job = ReceiveJob::lstReceiveJobs[iJob];
      if (job->activeRadioSetup->pRadio != radio)
        continue;
      // job dissabled by checkbox?
      if (!job->Enable)
        continue;

      // job is active
      lstJobs.append(job);
    }

    nJobs = lstJobs.count();
    logger.debug("MainWindow::checkTimeConflicts()", __LINE__, "Radio = %s   nJobs = %i", qPrintable(radio->NameLong), nJobs);
    for (iJob = 0; iJob < nJobs; iJob++) {
      bool isConflict = false;
      job0 = lstJobs[iJob];
      int  job0Start = job0->iStart;
      int  job0End = (job0Start + job0->Duration) % (24 * 3600);

      for (int y = 0; y < nJobs; y++) {
        jobY = lstJobs[y];
        if (jobY == job0 || jobY->TimeConflict)
          continue;
        int jobYStart = jobY->iStart;
        int jobYEnd = (jobYStart + jobY->Duration) % (24 * 3600);

        if (job0Start < job0End) {
          if (jobYEnd >= job0Start && (jobYStart <= job0Start || jobYStart > jobYEnd))
            isConflict = true;
        } else {
          // job0 runs over midnight
          if (jobYStart <= job0Start && (jobYEnd >= job0Start || jobYEnd < jobYStart))
            isConflict = true;
        }
      }
      job0->TimeConflict = isConflict;
      if (isConflict) {
        onProgressInfo(PROGRESS_TIMECONFLICT, job0->Id);
        logger.warning("MainWindow::checkTimeConflicts()", __LINE__, "Time conflict! JobY: %s - %s = %s", qPrintable(jobY->Start), qPrintable(jobY->End), qPrintable(jobY->Name));
        logger.warning("MainWindow::checkTimeConflicts()", __LINE__, "Time conflict! Job0: %s - %s = %s", qPrintable(job0->Start), qPrintable(job0->End), qPrintable(job0->Name));
      }
    }
  }
}


/* This is neccesary to enable painting waterfall on tab pages that not clicked visible, yet */
void MainWindow::showEvent(QShowEvent *ev) {

  QMainWindow::showEvent(ev);
  QPixmap pm = twgRadios->grab();
  for (int iTab = 0; iTab < twgRadios->count(); iTab++)
    pm = twgRadios->widget(iTab)->grab();
}
