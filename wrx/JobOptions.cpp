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
