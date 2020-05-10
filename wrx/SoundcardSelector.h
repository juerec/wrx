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
