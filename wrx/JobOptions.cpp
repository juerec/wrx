#include "JobOptions.h"

JobOptions::JobOptions() {
  clear();
}


JobOptions::JobOptions(JobOptions * opt)
          : JobId(opt->JobId),
            JobType(opt->JobType),
            Duration(opt->Duration),
            isWaterfall(opt->isWaterfall),
            isDecoder(opt->isDecoder),
            isWaveRecord(opt->isWaveRecord),
            isDecoderRecord(opt->isDecoderRecord),
            isRunPostCommand(opt->isRunPostCommand),
            WaveRecordFileName(opt->WaveRecordFileName),
            // ManualWaveRecordFileName(opt->ManualWaveRecordFileName),
            DecoderRecordFileName(opt->DecoderRecordFileName),
            PostCommand(opt->PostCommand),
            pRadioSetup(opt->pRadioSetup) {
}


void JobOptions::clear() {
  JobId                     = 0;
  Duration                  = 0;
  isWaterfall               = false;
  isDecoder                 = false;
  isDecoderRecord           = false;
  isWaveRecord              = false;
  isRunPostCommand          = false;
  WaveRecordFileName        = "";
  DecoderRecordFileName     = "";
  PostCommand               = "";
  pRadioSetup               = nullptr;
  JobType                   = 0;
}



JobOptionsModify::JobOptionsModify() {
  clear();
}


JobOptionsModify::JobOptionsModify(Action decoder, Action decoderRecord, Action waveRecord, Action runPostCommand)
                : aDecoder(decoder),
                  aDecoderRecord(decoderRecord),
                  aWaveRecord(waveRecord),
                  aRunPostCommand(runPostCommand) {
}


bool JobOptionsModify::isEmpty() {
  return aDecoder == Keep && aDecoderRecord == Keep && aWaveRecord == Keep && aRunPostCommand == Keep;
}


void JobOptionsModify::clear() {
  aDecoder            = Keep;
  aDecoderRecord      = Keep;
  aWaveRecord         = Keep;
  aRunPostCommand = Keep;
  WaveRecordFileName  = "";
}
