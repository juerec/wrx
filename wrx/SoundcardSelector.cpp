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

#include "SoundcardSelector.h"
#include "ui_SoundcardSelector.h"
#include "SoundCard.h"


SoundcardSelector::SoundcardSelector(QWidget *parent)
                 : QDialog(parent) {
  setupUi(this);
}

SoundcardSelector::~SoundcardSelector() {

}


void SoundcardSelector::init(QString & AudioIdentType, QString & currAudioIdent) {
QStringList lst;
QListWidgetItem * item;
int  i, n;

  if (AudioIdentType == SoundCard::AudioIdentType_CARDNR) {
    lst = SoundCard::listSoundCardAlsaDevs();
  }
  if (AudioIdentType == SoundCard::AudioIdentType_CARDNAME) {
    lst = SoundCard::listSoundCardNames();
  }
  if (AudioIdentType == SoundCard::AudioIdentType_PHYSICALPATH) {
    lst = SoundCard::listSoundCardPhysicalPaths();
  }
  if (AudioIdentType == SoundCard::AudioIdentType_SERIALNR) {
    lst = SoundCard::listSoundCardSerials();
  }
  liwSoundcards->addItems(lst);
  n = liwSoundcards->count();
  for (i = 0; i < n; i++) {
    item = liwSoundcards->item(i);
    if (item->text() == currAudioIdent) {
      liwSoundcards->setCurrentRow(i);
      break;
    }
  }
}

QString SoundcardSelector::currIdent() {
QListWidgetItem * item = liwSoundcards->currentItem();

  return item != nullptr ? item->text() : "";
}
