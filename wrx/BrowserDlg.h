#ifndef BROWSERDLG_H
#define BROWSERDLG_H

#include <QDialog>

namespace Ui {
  class BrowserDlg;
}

class BrowserDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit BrowserDlg(QWidget *parent = 0);
  ~BrowserDlg();
  
private:
  Ui::BrowserDlg *ui;
};

#endif // BROWSERDLG_H
