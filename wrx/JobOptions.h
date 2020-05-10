#ifndef JOBOPTIONS_H
#define JOBOPTIONS_H

#include <QString>

class RadioSetup;

class JobOptions {

public:

  JobOptions();
  JobOptions(JobOptions * opt);
  // JobOptions(bool isDecoder, bool isDecoderRecord, bool isWaveRecord, bool isRunPostCommand);
  // static JobOptions startOptions(bool isDecoder, bool isDecoderRecord, bool isWaveRecord, bool runPostCommand);
  void clear();
  // bool isEmpty();

  int          JobId;
  int          JobType;
  int          Duration;
  bool         isWaterfall;
  bool         isDecoder;
  bool         isWaveRecord;
  bool         isDecoderRecord;
  bool         isRunPostCommand;
  
  QString      WaveRecordFileName;
//  QString      ManualWaveRecordFileName;
  QString      DecoderRecordFileName;
  QString      PostCommand;
  RadioSetup * pRadioSetup;
};


class JobOptionsModify {

public:

  enum Action {
    Start,
    Stop,
    Keep
  };

  JobOptionsModify();
  JobOptionsModify(Action aDecoder, Action aDecoderRecord, Action aWaveRecord, Action aRunPostCommand);
  bool  isEmpty();
  void  clear();

  Action   aDecoder;
  Action   aDecoderRecord;
  Action   aWaveRecord;
  Action   aRunPostCommand;

  QString  WaveRecordFileName;
  QString  DecoderRecordFileName;
};

#endif // JOBOPTIONS_H
