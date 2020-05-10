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

#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <QtCore>

#include "OpMode.h"
#include "RadioSetup.h"


class Frequency {

protected :

    Frequency();
    Frequency(QStringList & lst);

    static Frequency Dummy;

public :
    ~Frequency();

    static QList<Frequency*>  lstFrequencies;

public:

    static void init(QList<OptionsList> & lst);
    static void clear();
    static Frequency * find(QString Callsign);
    bool        isValid();
    const char * OpModeName();
    const char * OpModeModulationType();

    QString            Callsign;
    QString            Name;
    // QString            NameShort;
    int                Freq;       // Hz
    OpMode             opMode;
    QString            Radios;

};

#endif // FREQUENCY_H
