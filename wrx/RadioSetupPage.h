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

#ifndef RADIOSETUPPAGE_H
#define RADIOSETUPPAGE_H

#include <QDialog>

#include "ui_RadioSetupPage.h"
#include "Radio.h"

class RadioSetupPage : public QDialog, protected Ui::RadioSetupPage {

  Q_OBJECT
  
public:
  explicit RadioSetupPage(QWidget *parent = 0, Radio * pRadio = nullptr);
  ~RadioSetupPage();
  
protected :

  Radio * radio;
  QString  SoundCardName;
  QString  SoundCardAlsaDev;
  QString  SoundCardPhysicalPath;
  QString  SerialNr;

protected slots :
  void on_pbtFindSoundcard_clicked();
  void on_rbtPhysicalMount_toggled(bool);
  void on_rbtSoundcardName_toggled(bool);
  void on_rbtAlsaDevNo_toggled(bool);
  void on_rbtSerialNo_toggled(bool);
public :

  void updateRadio();

};

#endif // RADIOSETUPPAGE_H
