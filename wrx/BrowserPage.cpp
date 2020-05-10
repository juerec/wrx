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

#include "BrowserPage.h"
#include "Options.h"
#include <QtCore>
#include "Logger.h"

BrowserPage::BrowserPage(QWidget *parent)
           : QWidget(parent), Ui::BrowserPage() {
QListWidgetItem * lwi;

  setupUi(this);
  stwBrowser->setCurrentWidget(stwBrowserPageDir);

  ParentFolderIcon.addFile(":/img/folder-parent.png", QSize(), QIcon::Normal, QIcon::On);
  FolderIcon.addFile(":/img/folder.png", QSize(), QIcon::Normal, QIcon::On);
  // AsciiIcon.addFile(":/img/ascii.png", QSize(), QIcon::Normal, QIcon::On);
  AsciiIcon.addFile(":/img/text_on.xpm", QSize(), QIcon::Normal, QIcon::On);
  // FaxIcon.addFile(":/img/fax.png", QSize(), QIcon::Normal, QIcon::On);
  FaxIcon.addFile(":/img/fax_on.xpm", QSize(), QIcon::Normal, QIcon::On);
  SoundIcon.addFile(":/img/speaker_on.xpm", QSize(), QIcon::Normal, QIcon::On);

  lwgFiles->setIconSize(QSize(24,28));

  lwi = new QListWidgetItem(lwgFiles);
  lwi->setIcon(ParentFolderIcon);
  lwi->setText("Verzeichnis ^");

  lwi = new QListWidgetItem(lwgFiles);
  lwi->setIcon(FolderIcon);
  lwi->setText("Verzeichnis ABC");

  lwi = new QListWidgetItem(lwgFiles);
  lwi->setIcon(AsciiIcon);
  lwi->setText("Datei.txt");

  lwi = new QListWidgetItem(lwgFiles);
  lwi->setIcon(FaxIcon);
  lwi->setText("Fax.bmp");

  lwi = new QListWidgetItem(lwgFiles);
  lwi->setIcon(SoundIcon);
  lwi->setText("Wetterbericht.wav");

  CurrentDir = options.ReceiveResultPath;

  QDir dir(CurrentDir);
  QFileInfoList fil = dir.entryInfoList(QDir::AllDirs | QDir::NoDot, QDir::Name);
  if (fil.count()) {
    QFileInfo fi = fil[fil.count() - 1];
    CurrentDir += QDir::separator() + fi.fileName();
    listFolder(CurrentDir);
    lwgFiles->setCurrentRow(lwgFiles->count() - 1);
  }

}

BrowserPage::~BrowserPage() {

}

void BrowserPage::on_pbtHome_clicked() {
  logger.debug("BrowserPage::on_pbtHome_clicked()", __LINE__, "BrowserPage::on_pbtHome_clicked()");
  CurrentDir = options.ReceiveResultPath;
  listFolder(CurrentDir);
}

void BrowserPage::on_pbtDirUp_clicked() {
QString currDir;
QListWidgetItem * lwi;

  int x = CurrentDir.lastIndexOf(QDir::separator());
  if (x > 0) {
    currDir = CurrentDir.mid(x + 1);
    CurrentDir = CurrentDir.left(x);
  } else {
    CurrentDir = QDir::separator();
  }
  listFolder(CurrentDir);
  if (!currDir.isEmpty())
    for (int i = 0; i < lwgFiles->count(); i++) {
      lwi = lwgFiles->item(i);
      if (lwi->text() == currDir) {
        lwgFiles->setCurrentItem(lwi);
      }
    }
}

void BrowserPage::on_pbtPrev_clicked() {
  int y = lwgFiles->currentRow();
  if (y > 0)
    lwgFiles->setCurrentRow(y - 1);
}

void BrowserPage::on_pbtNext_clicked() {
  int n = lwgFiles->count();
  int y = lwgFiles->currentRow();
  if (y < n - 1)
    lwgFiles->setCurrentRow(y + 1);
}

void BrowserPage::on_pbtReload_clicked() {
  listFolder(CurrentDir);
  lwgFiles->setCurrentRow(lwgFiles->count() - 1);
}

void BrowserPage::on_pbtShow_clicked() {
  QListWidgetItem * item = lwgFiles->currentItem();
  if (item != nullptr)
    on_lwgFiles_itemDoubleClicked(item);
}

void BrowserPage::on_lwgFiles_itemDoubleClicked(QListWidgetItem * item) {
QString dir = item->text();
int x;

  if (dir.isEmpty()) {
    x = CurrentDir.lastIndexOf(QDir::separator());
    if (x == 0)
      dir = QDir::separator();
    else
      on_pbtDirUp_clicked();
  } else {
    dir = CurrentDir + QDir::separator() + dir;
    QFileInfo fi(dir);
    if (fi.isDir())
      listFolder(dir);
    else
      showFile(dir);
  }
}

void BrowserPage::listFolder(QString Folder) {
QDir dir(Folder);
QListWidgetItem * lwi;

  CurrentDir = Folder;
  ledCurrentDir->setText(CurrentDir);

  lwgFiles->clear();
  QFileInfoList fil = dir.entryInfoList(QDir::AllDirs | QDir::NoDot | QDir::Files, QDir::Name | QDir::DirsFirst);
  for (int i = 0; i < fil.count(); i++) {
    QFileInfo & fi = fil[i];

    if (fi.fileName() == "..") {
      lwi = new QListWidgetItem(lwgFiles);
      lwi->setIcon(ParentFolderIcon);
      lwi->setText("");
      continue;
    }
    if (fi.isDir()) {
      lwi = new QListWidgetItem(lwgFiles);
      lwi->setIcon(FolderIcon);
      lwi->setText(fi.fileName());
       continue;
    }
    QString Name = fi.fileName();
    if (Name.endsWith(".txt")) {
      lwi = new QListWidgetItem(lwgFiles);
      lwi->setIcon(AsciiIcon);
      lwi->setText(Name);
      continue;
    }
    if (Name.endsWith(".bmp")) {
      lwi = new QListWidgetItem(lwgFiles);
      lwi->setIcon(FaxIcon);
      lwi->setText(Name);
      continue;
    }
    if (Name.endsWith(".ssb.wav") || Name.endsWith(".am.wav")) {
      lwi = new QListWidgetItem(lwgFiles);
      lwi->setIcon(SoundIcon);
      lwi->setText(Name);
      continue;
    }
  }
}


void BrowserPage::showFile(QString FileName) {

  logger.debug("BrowserPage::showFile()", __LINE__, "FileName = %s", qPrintable(FileName));

  QString Cmd = options.browserCommand(FileName);
  if (!Cmd.isEmpty()) {
    Cmd = Cmd.replace("%f", FileName);
    logger.debug("BrowserPage::showFile()", __LINE__, "Cmd = %s", qPrintable(Cmd));
    // QProcess * pProcess = new QProcess(this);
    // pProcess->start(Cmd,  QProcess::NotOpen);
    int ret = QProcess::startDetached(Cmd);
    logger.debug("BrowserPage::showFile()", __LINE__, "exit code = %i", ret);
  }
}



void BrowserPage::showEvent(QShowEvent * /*event*/) {
  // logger.debug("BrowserPage::showEvent()", __LINE__, ":: :: :: ::");
  on_pbtReload_clicked();
}
