#ifndef SOUNDDEVICEINFO_H
#define SOUNDDEVICEINFO_H

#include <QStringList>

class SoundDeviceInfo {
public:
  SoundDeviceInfo();
  SoundDeviceInfo(QStringList & values);

  void     init(QStringList & values);
  void     valuesToSave(QStringList & values);
  QString  AudioIdent();

  QString  AudioIdentType;
  QString  SoundCardName;
  QString  SoundCardAlsaDev;
  QString  SoundCardPhysicalPath;
  QString  SerialNr;

};

#endif // SOUNDDEVICEINFO_H
