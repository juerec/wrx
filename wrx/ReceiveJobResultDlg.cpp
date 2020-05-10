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

#include "ReceiveJobResultDlg.h"
#include "Options.h"
#include "Logger.h"

ReceiveJobResultDlg::ReceiveJobResultDlg(QWidget *parent) :
    QDialog(parent) {
    setupUi(this);

  connect(this, SIGNAL(finished(int)), this, SLOT(saveSize(int)));
  resize(options.ReceiveJobResultSize);
}

void ReceiveJobResultDlg::setData(const QStringList & lstResultFileNames) {
int nResultFileNames = lstResultFileNames.count();
int iResultFileName;

  for (iResultFileName = 0; iResultFileName < nResultFileNames; iResultFileName++)
    Model.lstResults.append(QFileInfo(lstResultFileNames[iResultFileName]));
  tvwResults->setModel(&Model);
  // must be done after the model is set
  QHeaderView * hv = tvwResults->horizontalHeader();
  hv->setMinimumSectionSize(28);
  hv->resizeSection(0, options.ReceiveJobResultColWidth[0]);
  hv->resizeSection(1, options.ReceiveJobResultColWidth[1]);
}

void ReceiveJobResultDlg::showResult(int iResult) {
const QFileInfo & fi = Model.lstResults[iResult];
QString FileName     = fi.absoluteFilePath();
QString Cmd          = options.browserCommand(FileName);

  logger.debug("ReceiveJobResultDlg::showResult()", __LINE__, "FileName = %s", qPrintable(FileName));

  if (!Cmd.isEmpty()) {
    Cmd = Cmd.replace("%f", FileName);
    logger.debug("ReceiveJobResultDlg::showResult()", __LINE__, "Cmd      = %s", qPrintable(Cmd));
    // QProcess * pProcess = new QProcess(this);
    // pProcess->start(Cmd,  QProcess::NotOpen);
    /* int ret = */ QProcess::startDetached(Cmd);
    // logger.debug("BrowserPage::showFile()", __LINE__, "exit code = %i", ret);
  }
/*
  if (fi.suffix() == "txt")
    return;
  if (fi.suffix() == "wav")
    return;
  if (fi.suffix() == "bmp")
    return;
*/
}

void ReceiveJobResultDlg::saveSize(int) {

  options.ReceiveJobResultSize = size();
  options.ReceiveJobResultColWidth[0] = tvwResults->columnWidth(0);
  options.ReceiveJobResultColWidth[1] = tvwResults->columnWidth(1);
}


void ReceiveJobResultDlg::on_tvwResults_clicked(const QModelIndex & index) {
  if (index.isValid() && index.column() == 1)
    showResult(index.row());
}

void ReceiveJobResultDlg::on_tvwResults_doubleClicked(const QModelIndex & index) {
  if (index.isValid())
    showResult(index.row());
}
