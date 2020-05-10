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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui_MainWindow.h"

#include "SchedTableModel.h"
#include "ComboBoxDelegate.h"
#include "Settings.h"
#include "ReceiveJob.h"
#include "BrowserPage.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow * pMainWindow;

public slots :
    void onProgressInfo(int Progress, int JobId);
    void onTextInfo(QString RjMsg, int JobId);

protected slots :
    void on_pbtSched_clicked();
    void on_pbtRadio_clicked();
    void on_pbtBrowser_clicked();
    void on_pbtSettings_clicked();
    void SignalTimeout();
    void on_pTableModel_radioChanged();
    void on_tvwSched_clicked(const QModelIndex & index);
    void on_tvwSched_customContextMenuRequested(const QPoint & pos);


protected :
    virtual void closeEvent(QCloseEvent * event);
    void checkTimeConflicts();
    void showEvent(QShowEvent *ev);


    SchedTableModel    tableModel;
    SchedTableModel  * pTableModel;
    Settings         * twgSettings;
    BrowserPage      * wdgBrowser;
    QTimer           * pTimer;

signals :
    void Progress(int Progress, int JobId);
};

#endif // MAINWINDOW_H
