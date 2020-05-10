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

#ifndef SAMPLESBLOCK_H
#define SAMPLESBLOCK_H

/*
 *  Die Klasse wird verwendet, um Samples in einer Liste vom Audio-Input lesenden Threead
 *  an die verarbeitenden Threads - Wasserfall, Speichern, Auswerten - zu übergeben
 *
 */



class SamplesBlock {
public:

  SamplesBlock(short int * Samples, int nSamples, int SampleRate, int Channels);
  ~SamplesBlock();
  int SampleRate;
  int Channels;
  short int * Samples;
  int nSamples;

  const short * ptr(int i) { return &Samples[i]; }
};

#endif // SAMPLESBLOCK_H
