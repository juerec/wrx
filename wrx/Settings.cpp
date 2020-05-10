#include "Settings.h"

#include "Radio.h"
#include "Options.h"
#include "Logger.h"
#include "RadioSetupPage.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFileDialog>

#include <QResizeEvent>

Settings::Settings(QWidget *parent)
        : QTabWidget(parent) {
  setupUi(this);
}

void Settings::init() {
int i = 0;
// Common

  optCommonLabVersion->setText("3.1.1");
  optCommonLabBuildDate->setText(QString(__DATE__) + " " + __TIME__);
  optCommonCbxConfirmToQuit->setChecked(options.confirmQuit);
  optCommonCbxRunTimedJobs->setChecked(options.runTimedJobs);
  optCommonRbtTimeUTC->setChecked(options.showTimeAsUTC_new);
  optCommonRbtTimeLocal->setChecked(!options.showTimeAsUTC_new);
  optCommomCbxLogLevel->setCurrentIndex(optCommomCbxLogLevel->findText(options.LogLevel));

// Scheduler

  optSchedulerLedRootPath->setText(options.ReceiveResultPath);

// Radio

  for (i = 0; i < Radio::lstRadios.count(); i++)
    optRadioTwgRadios->addTab(new RadioSetupPage(this, Radio::lstRadios[i]), Radio::lstRadios[i]->NameLong);

  optRadioLedProgListIds->setText(options.ProgListIds);
  optRadioLedProgFindCardNr->setText(options.ProgFindCardNr);

// Browser
  optBrowserTwgViewer->setRowCount(options.lstBrowserCommands.count());
  optBrowserTwgViewer->setColumnCount(3);
  for (i = 0; i < options.lstBrowserCommands.count(); i++) {
    BrowserCommand & bc = options.lstBrowserCommands[i];
    QTableWidgetItem * twi = new QTableWidgetItem(bc.Pattern);
    optBrowserTwgViewer->setItem(i, 0, twi);
    twi = new QTableWidgetItem(bc.Command);
    optBrowserTwgViewer->setItem(i, 1, twi);
    twi = new QTableWidgetItem();
    twi->setCheckState(bc.active ? Qt::Checked :Qt::Unchecked);
    optBrowserTwgViewer->setItem(i, 2, twi);
  }
}


void Settings::on_optCommonPbtSave_clicked() {
  // logger.debug("Settings::on_optCommonPbtSave_clicked()", __LINE__, "");
  options.confirmQuit = optCommonCbxConfirmToQuit->isChecked();
  options.runTimedJobs = optCommonCbxRunTimedJobs->isChecked();
  options.showTimeAsUTC_new = optCommonRbtTimeUTC->isChecked();
  options.LogLevel = optCommomCbxLogLevel->currentText();
  options.saveOptCommon();
}


void Settings::on_optSchedulerPbtRootPath_clicked() {
QString str = optSchedulerLedRootPath->text();

  str = QFileDialog::getExistingDirectory(this, tr("WRX - New Root Path for Receptions") , str);
  if (!str.isEmpty())
    optSchedulerLedRootPath->setText(str);
}


void Settings::on_optSchedulerPbtSave_clicked() {
QString str = optSchedulerLedRootPath->text();
QDir    dir(str);

  if (!dir.makeAbsolute()) {
    QMessageBox::critical(this, tr("WRX - Illegal path"), tr("The path provided is invalid"), QMessageBox::Ok);
    return;
  }
  str = dir.path();
  optSchedulerLedRootPath->setText(str);
  options.ReceiveResultPath = str;
  options.saveOptScheduler();
}


void Settings::on_optRadioPbtSave_clicked() {
int  n;

  options.ProgListIds = optRadioLedProgListIds->text();
  options.ProgFindCardNr = optRadioLedProgFindCardNr->text();
  n = optRadioTwgRadios->count();
  for (int i = 0; i < n; i++) {
    RadioSetupPage * rsp = (RadioSetupPage *)optRadioTwgRadios->widget(i);
    rsp->updateRadio();
  }
  Radio::saveRadios();
}

void Settings::on_optBrowserPbtViewerNew_clicked() {
int nRows = optBrowserTwgViewer->rowCount();

  optBrowserTwgViewer->setRowCount(nRows + 1);
  QTableWidgetItem * twi = new QTableWidgetItem(".txt");
  optBrowserTwgViewer->setItem(nRows, 0, twi);
  twi = new QTableWidgetItem("command %f");
  optBrowserTwgViewer->setItem(nRows, 1, twi);
  twi = new QTableWidgetItem();
  twi->setCheckState(Qt::Unchecked);
  optBrowserTwgViewer->setItem(nRows, 2, twi);
}


void Settings::on_optBrowserPbtViewerDelete_clicked() {
QList<QTableWidgetItem *> lst = optBrowserTwgViewer->selectedItems();
int * Rows;
int   nRows = optBrowserTwgViewer->rowCount();
int   i;

  if (lst.count() == 0)
    return;
  Rows = new int [nRows];
  for (i = 0; i < nRows; i++)
    Rows[i] = 0;

  for (i = 0; i < lst.count(); i++) {
    QTableWidgetItem * twi = lst[i];
    Rows[twi->row()] = 1;
  }

  for (i = nRows - 1; i >= 0; i--)
    if (Rows[i] == 1)
      optBrowserTwgViewer->removeRow(i);
  delete [] Rows;
}


void Settings::on_optBrowserPbtSave_clicked() {
int   nRows = optBrowserTwgViewer->rowCount();
int   i;

  options.lstBrowserCommands.clear();
  for (i = 0; i < nRows; i++) {
    QString Pattern = optBrowserTwgViewer->item(i, 0)->text();
    QString Commad  = optBrowserTwgViewer->item(i, 1)->text();
    bool    active  = optBrowserTwgViewer->item(i, 2)->checkState() == Qt::Checked;
    options.lstBrowserCommands.append(BrowserCommand(Pattern, Commad, active));
  }
  options.saveOptBrowser();
}
