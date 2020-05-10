#include "BrowserDlg.h"
#include "ui_BrowserDlg.h"

BrowserDlg::BrowserDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::BrowserDlg)
{
  ui->setupUi(this);
}

BrowserDlg::~BrowserDlg()
{
  delete ui;
}
