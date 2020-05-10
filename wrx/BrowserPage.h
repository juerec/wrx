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

#ifndef BROWSERPAGE_H
#define BROWSERPAGE_H

#include <QWidget>
#include "ui_BrowserPage.h"

namespace Ui {
class BrowserPage;
}

class BrowserPage : public QWidget, Ui::BrowserPage {
  Q_OBJECT

public:
    explicit BrowserPage(QWidget *parent = 0);
    ~BrowserPage();


protected :
  QString  CurrentDir;

  void listFolder(QString Folder);            // Foler is the absolute Path to list
  void listParentFolder();                    // lists the parent of CurrentFolder
  void listChildFolder(QString ChildFolder);  // lists CurrentFolder/ChildFolder
  void listToDayFolder();                     // lists data-root/year/month-day from todady
  void showFile(QString FileName);

  QIcon  ParentFolderIcon;
  QIcon  FolderIcon;
  QIcon  AsciiIcon;
  QIcon  FaxIcon;
  QIcon  SoundIcon;

protected slots :

  void on_pbtHome_clicked();
  void on_pbtDirUp_clicked();
  void on_pbtPrev_clicked();
  void on_pbtNext_clicked();
  void on_pbtReload_clicked();
  void on_pbtShow_clicked();
  void on_lwgFiles_itemDoubleClicked(QListWidgetItem * item);

protected :
  virtual void showEvent(QShowEvent * event);
};

#endif // BROWSERPAGE_H
