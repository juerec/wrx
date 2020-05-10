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

#ifndef RECEIVEJOBRESULTDLG_H
#define RECEIVEJOBRESULTDLG_H

#include "ui_ReceiveJobResultDlg.h"
#include "ReceiveJobResultModel.h"

class ReceiveJobResultDlg : public QDialog, private Ui::ReceiveJobResultDlg {
    Q_OBJECT

public:

  explicit ReceiveJobResultDlg(QWidget *parent = nullptr);

  void setData(const QStringList & lstResultFileNames);
  void showResult(int iResult);

public slots:

  void saveSize(int);
  void on_tvwResults_clicked(const QModelIndex &);
  void on_tvwResults_doubleClicked(const QModelIndex &);

protected :

  ReceiveJobResultModel Model;

};

#endif // RECEIVEJOBRESULTDLG_H
