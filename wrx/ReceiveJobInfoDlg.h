#ifndef RECEIVEJOBINFODLG_H
#define RECEIVEJOBINFODLG_H

#include "ui_ReceiveJobInfoDlg.h"
#include "ReceiveJob.h"

class ReceiveJobInfoDlg : public QDialog, private Ui::ReceiveJobInfoDlg
{
    Q_OBJECT

public:
    explicit ReceiveJobInfoDlg(QWidget *parent = nullptr);

    void setData(ReceiveJob * pJob);

public slots :

    void saveSize(int);

};

#endif // RECEIVEJOBINFODLG_H
