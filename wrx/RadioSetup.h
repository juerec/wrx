#ifndef RADIOSETUP_H
#define RADIOSETUP_H

#include <QtCore>
#include "Options.h"
#include "Radio.h"

class Frequency;

class RadioSetup {

protected :

  RadioSetup();
  RadioSetup(QStringList & lst);
  ~RadioSetup();
  void   init();
  static QList<RadioSetup *> lstRadioSetup;

public :

  static void init(QList<OptionsList> & lst);
  static void clear();
  static RadioSetup * find(QString Callsign, QString RadioName);
  static RadioSetup * findFirst(QString Callsign);
  static bool         existsRadio(QString Callsign);
  static QStringList  findRadios(QString Callsign);
  static RadioSetup * findByName(QString CallsignRadio);
  QString             Name();
  int                 tuningFreq();

  enum RunType {
    RUN_TYPE_INTERN,
    RUN_TYPE_EXTERN,
    RUN_TYPE_NORUN           // for invalid coditions
  };

  enum DecoderType {
    DECODER_NONE,
    DECODER_INTERN,
    DECODER_EXTERN
  };

  enum ExternStopType {
    EXTERN_STOP_BY_COMMAND,
    EXTERN_STOP_BY_SIGTERM,
    EXTERN_STOP_WAIT_TERM
  };

  QString        Callsign;
  Frequency    * frequency;
  Radio        * pRadio;
  RunType        RunType;
  DecoderType    DecoderType;
  int            CenterFreq;
  int            Offset;
  int            SampleRate;
  int            Channels;
  int            ChannelMode;
  int            SubDevice;
  int            ResamplerQuality;
  QString        DecodedFileExt;
  int            FFTSize;
  int            WindowSize;
  int            iWindowFunc;
  bool           isWaterfall;
  bool           isWaveRecord;
  bool           isDecoder;
  bool           isDecoderRecord;
  bool           isRunPostCommand;
  QString        PostCommand;
  ExternStopType ExternStopType;
  QString        ExternStopCommand;
  QString        StartParameter;
};

#endif // RADIOSETUP_H
