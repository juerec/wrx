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

#include "SamplesBlock.h"

#include <string.h>

SamplesBlock::SamplesBlock(short int * Samples, int nSamples, int SampleRate, int Channels) {
  this->Samples = new short int [nSamples];
  memmove(this->Samples, Samples, nSamples * sizeof(short));
  this->nSamples = nSamples;
  this->SampleRate = SampleRate;
  this->Channels = Channels;
}

SamplesBlock::~SamplesBlock() {
  delete [] Samples;
}
