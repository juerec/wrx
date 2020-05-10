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
