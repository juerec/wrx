#include "ReceiveJobInfoDlg.h"

#include "Frequency.h"
#include "Options.h"

ReceiveJobInfoDlg::ReceiveJobInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    connect(this, SIGNAL(finished(int)), this, SLOT(saveSize(int)));
    resize(options.ReceiveJobInfoSize);
}

void ReceiveJobInfoDlg::setData(ReceiveJob * pJob) {
char str[24];

  sprintf(str, "rj-%03i", pJob->Id);
  labId->setText(str);
  labActivated->setText(pJob->Enable ? "on" : "off");
  labName->setText(pJob->Name);
  labFileName->setText(pJob->FileName);
  labStartUTC->setText(pJob->StartUTC.toString("HH:mm:ss"));
  sprintf(str, "%02i:%02i:%02i - %i s", pJob->Duration / 3600, (pJob->Duration % 3600) / 60, pJob->Duration % 60, pJob->Duration);
  labDuration->setText(str);
  labTimeConflict->setText(pJob->TimeConflict ? tr("yes") : tr("no"));

  int nFreq = pJob->lstFrequencies.count();
  twgFrequencies->setRowCount(nFreq);
  for (int row = 0; row < nFreq; row++) {
    Frequency * pFreq = pJob->lstFrequencies[row];
    twgFrequencies->setItem(row, 0, new QTableWidgetItem(pFreq->Callsign));
    twgFrequencies->setItem(row, 1, new QTableWidgetItem(QString::number(pFreq->Freq)));
    twgFrequencies->setItem(row, 2, new QTableWidgetItem(pFreq->OpModeName()));
    twgFrequencies->setItem(row, 3, new QTableWidgetItem(pFreq->OpModeModulationType()));
    if (pJob->activeRadioSetup->frequency == pFreq) {
      QString RadioName = pJob->activeRadioSetup->pRadio->NameLong;
      if (!pJob->activeRadioSetup->pRadio->Enable)
        RadioName += "#";
      twgFrequencies->setItem(row, 4, new QTableWidgetItem(RadioName));
    }
  }

  QHeaderView * hv = twgFrequencies->horizontalHeader();
  hv->setMinimumSectionSize(28);
  hv->resizeSection(0, options.ReceiveJobInfoColWidth[0]);
  hv->resizeSection(1, options.ReceiveJobInfoColWidth[1]);
  hv->resizeSection(2, options.ReceiveJobInfoColWidth[2]);
  hv->resizeSection(3, options.ReceiveJobInfoColWidth[3]);
  hv->resizeSection(4, options.ReceiveJobInfoColWidth[4]);
}


void ReceiveJobInfoDlg::saveSize(int) {
  options.ReceiveJobInfoSize = size();
  options.ReceiveJobInfoColWidth[0] = twgFrequencies->columnWidth(0);
  options.ReceiveJobInfoColWidth[1] = twgFrequencies->columnWidth(1);
  options.ReceiveJobInfoColWidth[2] = twgFrequencies->columnWidth(2);
  options.ReceiveJobInfoColWidth[3] = twgFrequencies->columnWidth(3);
  options.ReceiveJobInfoColWidth[4] = twgFrequencies->columnWidth(4);
}
