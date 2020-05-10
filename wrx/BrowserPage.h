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
