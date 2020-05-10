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
