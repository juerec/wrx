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

#include "OpMode.h"
#include "Logger.h"

const char * OpMode::Names[] = { "OTHER", "RTTY-450", "RTTY-85", "FAX", "NAVTEX", "SPEACH-AM", "SPEACH-NFM", "SPEACH-WFM", "SPEACH-SSB", "SPEACH" };
const char * OpMode::ModulationTypes[] = { "UNKNOWN", "AM", "USB", "LSB", "NFM", "WFM" };

OpMode  OpMode::modes[10] = { OpMode(OpMode::OTHER, OpMode::UNKNOWN, 0),
                              OpMode(OpMode::RTTY_450, OpMode::USB, 450),
                              OpMode(OpMode::RTTY_85, OpMode::USB, 85),
                              OpMode(OpMode::FAX, OpMode::USB, 850),
                              OpMode(OpMode::NAVTEX, OpMode::USB, 170),
                              OpMode(OpMode::SPEACH_AM, OpMode::AM, 0),
                              OpMode(OpMode::SPEACH_NFM, OpMode::NFM, 0),
                              OpMode(OpMode::SPEACH_WFM, OpMode::WFM, 0),
                              OpMode(OpMode::SPEACH_SSB, OpMode::USB, 0),
                              OpMode(OpMode::SPEACH, OpMode::NF, 0)
                            };

OpMode::OpMode() {
  IdMode       = OTHER;
  IdModulation = UNKNOWN;
  Shift        = 0;
}

OpMode::OpMode(OPMODE_ID IdMode, MODULATION_ID IdModulation, int Shift) {
  this->IdMode       = IdMode;
  this->IdModulation = IdModulation;
  this->Shift        = Shift;
}

const char * OpMode::ModeName() const {
  return Names[(int)IdMode];
}

const char * OpMode::ModulationName() const {
  return ModulationTypes[(int)IdModulation];
}

const OpMode OpMode::find(const QString & Mode) {

  for (uint i = 0; i < sizeof(modes)/sizeof(OpMode); i++) {
    if (Mode == modes[i].ModeName())
      return modes[i];
  }
  logger.error("OpMode::find()", __LINE__, "Invalid mode name \"%s\"! Check configuration in ~/wrx/wrx-config.ini.", qPrintable(Mode));
  return modes[0];
}

const char * OpMode::ModeType() const {
  switch (IdMode) {
  case OTHER :
    return "xxx";
  case RTTY_450 :
  case RTTY_85 :
    return "rtty";
  case FAX :
    return "fax";
  case NAVTEX :
    return "navtex";
  case SPEACH_AM :
    return "am";
  case SPEACH_NFM :
    return "nfm";
  case SPEACH_WFM :
    return "wfm";
  case SPEACH_SSB :
    return "ssb";
  case SPEACH :
    return "nf";
  }
  return "???";
}
