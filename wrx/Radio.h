#ifndef RADIO_H
#define RADIO_H

#include <QtCore>

#include "Options.h"
#include "SoundDeviceInfo.h"
#include "SoundCard.h"
#include "Options.h"

class RadioSetup;

class Radio : public SoundDeviceInfo {

protected :

  Radio();
  Radio(int Id, QString name, QStringList & lstSetup);
  ~Radio();

  static Radio  Dummy;

public:

  static QList<Radio *> lstRadios;

  static void    init(QList<OptionsList> & lstConfig, QList<OptionsList> & lstSetup);
  static void    clear();
  static Radio * find(QString name);
  static bool    exists(QString name);
  static void    saveRadios();

  void           init(QStringList & lstSetup);
  void           valuesToSave(QStringList & values);

  QString        SoundCardNrFromSerial();
  bool           runCmd(QString Cmd, RadioSetup * radioSetup, QString & stdOut);
  bool           turnOn(RadioSetup * radioSetup, QString &ErrorDesc);
  void           turnOff();
  bool           openCapture(int Samplerate, int Channels, int ChannelMode, int SubDevice, int ResamplerQuality);
  void           closeCapture();
  bool           readCapture(short * Buffer, int max);

  int      Id;
  bool     Enable;
  QString  Name;
  QString  NameLong;
  QString  CmdStart;
  QString  CmdStop;
  QString  CmdCheck;

protected :

  class SoundCard SoundCard;

};

#endif // RADIO_H
