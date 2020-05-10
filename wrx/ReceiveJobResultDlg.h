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
