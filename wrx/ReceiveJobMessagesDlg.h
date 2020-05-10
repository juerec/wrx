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
