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

#ifndef SOUNDCARDSELECTOR_H
#define SOUNDCARDSELECTOR_H

#include <QDialog>

#include "ui_SoundcardSelector.h"

class SoundcardSelector : public QDialog, protected Ui::SoundcardSelector {

  Q_OBJECT
  
public:
  explicit SoundcardSelector(QWidget *parent = 0);
  ~SoundcardSelector();

  void init(QString & AudioIdentType, QString & currAudioIdent);
  QString currIdent();
  
};

#endif // SOUNDCARDSELECTOR_H
