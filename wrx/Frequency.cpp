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

#include "Frequency.h"
#include "Logger.h"


QList<Frequency*>  Frequency::lstFrequencies;
Frequency Frequency::Dummy;

Frequency::Frequency() {

  Callsign  = "UNKNOW";
  Name      = "dummy";
  Freq      = 500000;       // Hz
  opMode    = OpMode::find("OTHER");
  Radios    = "dummy";
}

Frequency::Frequency(QStringList & lst) {

  Callsign   = "";
  Name       = "";
  // NameShort  = "";
  Freq       = 0;
  Radios     = "";

  for (int i = 0; i < lst.length(); i++) {
    QString & str = lst[i];
    if (str.startsWith("Callsign="))
      Callsign = str.mid(9);
    if (str.startsWith("Name="))
      Name = str.mid(5);
    if (str.startsWith("Frequency="))
      Freq = str.mid(10).toInt();
    if (str.startsWith("OpMode="))
      opMode = OpMode::find(str.mid(7));
    if (str.startsWith("Radios="))
      Radios = str.mid(7);
  }
  // Remove radios which not loaded
  QStringList lstRadios = Radios.split("|");
  Radios = "";
  int i, n;
  n = lstRadios.count();
  for (i = 0; i < n; i++)
    if (Radio::exists(lstRadios[i])) {
      if (Radios.length() > 0)
        Radios += "|";
      Radios += lstRadios[i];
    }
  if (!isValid())
    logger.error("Frequency::Frequency()", __LINE__, "Frequency is invalid: Callsign=%s, Name=%s, Frequency=%i, OpMode=%s, Radios=%s",
                                                     qPrintable(Callsign), qPrintable(Name), Freq, opMode.ModeName(), qPrintable(Radios));
}


Frequency::~Frequency() {
}


void Frequency::init(QList<OptionsList> & lstValues) {
int i, nParams;

  nParams = lstValues.count();
  for (i = 0; i < nParams; i++) {
    Frequency * f = new Frequency(lstValues[i].ArgList);
    if (f->isValid())
      lstFrequencies.append(f);
    else
      delete f;
  }
}


void  Frequency::clear() {
int i;

  for (i = 0; i < lstFrequencies.count(); i++)
    delete lstFrequencies[i];
  lstFrequencies.clear();
}


Frequency * Frequency::find(QString Callsign) {
int i;

  for (i = 0; i < lstFrequencies.count(); i++)
    if (Callsign == lstFrequencies[i]->Callsign)
      return lstFrequencies[i];

  logger.error("Frequency::find()", __LINE__, "No Frequency found for Callsign %s", qPrintable(Callsign));
  return &Dummy;
}


bool Frequency::isValid() {

  return !Callsign.isEmpty() &&
         Freq > 9 &&
         opMode.IdMode != OpMode::OTHER &&
         !Radios.isEmpty();
}


const char * Frequency::OpModeName() {
  return opMode.ModeName();
}


const char * Frequency::OpModeModulationType() {
  return opMode.ModulationName();
}
