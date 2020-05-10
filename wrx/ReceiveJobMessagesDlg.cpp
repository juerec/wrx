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

#include "ReceiveJobMessagesDlg.h"

ReceiveJobMessagesDlg::ReceiveJobMessagesDlg(QWidget *parent)
                     : QDialog(parent) {

    setupUi(this);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    connect(this, SIGNAL(finished(int)), this, SLOT(saveSize(int)));

    resize(options.ReceiveJobMessagesSize);
    splitter->restoreState(options.ReceiveJobMessagesSplitter);
}


void ReceiveJobMessagesDlg::setData(ReceiveJob * pJob) {
int nMessages = pJob->lstMessages.count();

  twgMessages->setRowCount(nMessages);
  for (int iMsg = 0; iMsg < nMessages; iMsg++) {
    const ReceiveJobMessage & rjm = pJob->lstMessages[iMsg];
    QTableWidgetItem * twi = new QTableWidgetItem(rjm.Created.toString(tr("dd-MM-yyyy HH:mm:ss")));
    twi->setData(Qt::UserRole, rjm.Message);
    twgMessages->setItem(iMsg, 0, twi);
    twgMessages->setItem(iMsg, 1, new QTableWidgetItem(rjm.TypeName()));
    if (iMsg == 0)
      tedMessage->setText(rjm.Message);
  }
  QHeaderView * hv = twgMessages->horizontalHeader();
  hv->setMinimumSectionSize(28);
  hv->resizeSection(0, options.ReceiveJobMessagesColWidth[0]);
  hv->resizeSection(1, options.ReceiveJobMessagesColWidth[1]);
}


void ReceiveJobMessagesDlg::on_twgMessages_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem * /* previous */) {
  tedMessage->setText(current->data(Qt::UserRole).toString());
}


void ReceiveJobMessagesDlg::saveSize(int) {

  options.ReceiveJobMessagesSize = size();
  options.ReceiveJobMessagesColWidth[0] = twgMessages->columnWidth(0);
  options.ReceiveJobMessagesColWidth[1] = twgMessages->columnWidth(1);
  options.ReceiveJobMessagesSplitter = splitter->saveState();
}
