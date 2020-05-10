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
