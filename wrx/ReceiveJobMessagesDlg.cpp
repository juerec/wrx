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
