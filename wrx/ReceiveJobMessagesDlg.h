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

#ifndef RECEIVEJOBMESSAGESDLG_H
#define RECEIVEJOBMESSAGESDLG_H

#include "ui_ReceiveJobMessagesDlg.h"
#include "ReceiveJob.h"

class ReceiveJobMessagesDlg : public QDialog, private Ui::ReceiveJobMessagesDlg
{
    Q_OBJECT

public:
    explicit ReceiveJobMessagesDlg(QWidget *parent = nullptr);
    void setData(ReceiveJob * pJob);
public slots :

    void on_twgMessages_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void saveSize(int);
};

#endif // RECEIVEJOBMESSAGESDLG_H
