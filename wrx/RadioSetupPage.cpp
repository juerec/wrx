#include "RadioSetupPage.h"
#include "ui_RadioSetupPage.h"
#include "SoundCard.h"
#include "SoundcardSelector.h"

RadioSetupPage::RadioSetupPage(QWidget *parent, Radio * pRadio)
          : QDialog(parent), radio(pRadio) {
  setupUi(this);

  SoundCardName         = radio->SoundCardName;
  SoundCardAlsaDev      = radio->SoundCardAlsaDev;
  SoundCardPhysicalPath = radio->SoundCardPhysicalPath;
  SerialNr              = radio->SerialNr;

  ledRadioName->setText(pRadio->Name);
  ledRadioNameLong->setText(pRadio->NameLong);
  chbRadioActivated->setChecked(pRadio->Enable);
  QString & AudioIdentType = pRadio->AudioIdentType;
  if (AudioIdentType == SoundCard::AudioIdentType_CARDNR) {
    ledSoundcardIdent->setText(SoundCardAlsaDev);
    rbtAlsaDevNo->setChecked(true);
  }
  if (AudioIdentType == SoundCard::AudioIdentType_CARDNAME) {
    ledSoundcardIdent->setText(SoundCardName);
    rbtSoundcardName->setChecked(true);
  }
  if (AudioIdentType == SoundCard::AudioIdentType_PHYSICALPATH) {
    ledSoundcardIdent->setText(SoundCardPhysicalPath);
    rbtPhysicalMount->setChecked(true);
  }
  if (AudioIdentType == SoundCard::AudioIdentType_SERIALNR) {
    ledSoundcardIdent->setText(SerialNr);
    rbtSerialNo->setChecked(true);
  }
  ledStartCommand->setText(pRadio->CmdStart);
  ledCheckCommand->setText(pRadio->CmdCheck);
  ledStopCommand->setText(pRadio->CmdStop);
}


RadioSetupPage::~RadioSetupPage() {
}


void RadioSetupPage::on_pbtFindSoundcard_clicked() {
SoundcardSelector dlg(this);
QString AudioIdentType("");
QString currAudioIdent = ledSoundcardIdent->text();

  if (rbtAlsaDevNo->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_CARDNR;
  if (rbtSoundcardName->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_CARDNAME;
  if (rbtPhysicalMount->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_PHYSICALPATH;
  if (rbtSerialNo->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_SERIALNR;

  dlg.init(AudioIdentType, currAudioIdent);
  if (dlg.exec() == QDialog::Accepted) {
    QString str = dlg.currIdent();
    ledSoundcardIdent->setText(str);

    if (AudioIdentType == SoundCard::AudioIdentType_CARDNR)
      SoundCardAlsaDev = str;
    if (AudioIdentType == SoundCard::AudioIdentType_CARDNAME)
      SoundCardName = str;
    if (AudioIdentType == SoundCard::AudioIdentType_PHYSICALPATH)
      SoundCardPhysicalPath = str;
    if (AudioIdentType == SoundCard::AudioIdentType_SERIALNR)
      SerialNr = str;
  }
}


void RadioSetupPage::on_rbtPhysicalMount_toggled(bool isChecked) {
  if (isChecked)
    ledSoundcardIdent->setText(SoundCardPhysicalPath);
}


void RadioSetupPage::on_rbtSoundcardName_toggled(bool isChecked) {
  if (isChecked)
    ledSoundcardIdent->setText(SoundCardName);
}


void RadioSetupPage::on_rbtAlsaDevNo_toggled(bool isChecked) {
  if (isChecked)
    ledSoundcardIdent->setText(SoundCardAlsaDev);
}

void RadioSetupPage::on_rbtSerialNo_toggled(bool isChecked) {
  if (isChecked)
    ledSoundcardIdent->setText(SerialNr);
}


void RadioSetupPage::updateRadio() {
QString AudioIdentType;

  radio->CmdCheck = ledCheckCommand->text();
  radio->CmdStart = ledStartCommand->text();
  radio->CmdStop  = ledStopCommand->text();
  radio->NameLong = ledRadioNameLong->text();
  radio->Enable   = chbRadioActivated->isChecked();
  if (rbtPhysicalMount->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_PHYSICALPATH;
  if (rbtSoundcardName->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_CARDNAME;
  if (rbtAlsaDevNo->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_CARDNR;
  if (rbtSerialNo->isChecked())
    AudioIdentType = SoundCard::AudioIdentType_SERIALNR;
  radio->AudioIdentType        = AudioIdentType;
  radio->SoundCardName         = SoundCardName;
  radio->SoundCardAlsaDev      = SoundCardAlsaDev;
  radio->SoundCardPhysicalPath = SoundCardPhysicalPath;
  radio->SerialNr              = SerialNr;
}
