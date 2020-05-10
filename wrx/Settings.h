#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_Settings.h"
#include "QTabWidget"

class Radio;
class QResizeEvent;


class Settings : public QTabWidget, private Ui::twgSettings {
  Q_OBJECT
public:

  Settings(QWidget *parent = 0);
  void init();

protected :
  void optRadioShowSettings(Radio * radio);

protected slots :

  void on_optCommonPbtSave_clicked();

  void on_optSchedulerPbtRootPath_clicked();
  void on_optSchedulerPbtSave_clicked();

  void on_optRadioPbtSave_clicked();

  void on_optBrowserPbtViewerNew_clicked();
  void on_optBrowserPbtViewerDelete_clicked();
  void on_optBrowserPbtSave_clicked();

};

#endif // SETTINGS_H
