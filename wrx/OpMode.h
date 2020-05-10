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

#ifndef OPMODE_H
#define OPMODE_H

#include <QtCore>

class OpMode {

public:
    OpMode();
public :
    void initList();

    enum OPMODE_ID {
      OTHER        = 0,
      RTTY_450     = 1,
      RTTY_85      = 2,
      FAX          = 3,
      NAVTEX       = 4,
      SPEACH_AM    = 5,
      SPEACH_NFM   = 6,
      SPEACH_WFM   = 7,
      SPEACH_SSB   = 8,
      SPEACH             // the receiver does the demodulation
    };

    enum MODULATION_ID {
      UNKNOWN = 0,
      AM      = 1,
      USB     = 2,
      LSB     = 3,
      NFM     = 4,
      WFM     = 5,
      NF      = 6        // no modulation, the receiver does the demodulation
    };

protected :
    OpMode(OPMODE_ID Id, MODULATION_ID IdModulation, int Shift = 0);

public :
    OPMODE_ID      IdMode;
    MODULATION_ID  IdModulation;
    int            Shift;

    const char * ModeName() const;
    const char * ModulationName() const;
    static const OpMode find(const QString &Mode);
    const char * ModeType() const;

    bool operator==(const OpMode &opmode) { return IdMode == opmode.IdMode && IdModulation == opmode.IdModulation; }

    static const char * Names[];        // OTHER, RTTY-450, RTTY-85, FAX, NAVTEX, SPEACH_AM, SPEACH_NFM, SPEACH_WFM
    static const char * ModulationTypes[];  // AM, USB, LSB, NFM, WFM

protected :

    static OpMode  modes[10];

};

#endif // OPMODE_H
