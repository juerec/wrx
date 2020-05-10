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
