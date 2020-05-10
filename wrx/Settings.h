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
