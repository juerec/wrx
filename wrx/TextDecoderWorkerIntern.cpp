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

#include "TextDecoderWorkerIntern.h"

#include <string.h>
#include <QMutex>

extern QMutex  FFTwInitMutex;

TextDecoderWorkerIntern::TextDecoderWorkerIntern() {

  fskDecoderInitOptions(&DecoderOptions);
  DecoderHandle = nullptr;
}


TextDecoderWorkerIntern::~TextDecoderWorkerIntern() {
  cleanUp();
}


void TextDecoderWorkerIntern::init(const char * DecoderType,
                                   int          SampleRate,
                                   int          FFTSize,
                                   int          iWindowFunc,
                                   int          WindowSize,
                                   int          CenterFreq,
                                   int          Shift,
                                   bool         xchg) {

  cleanUp();

  DecoderOptions.Shift          = Shift;
  DecoderOptions.CenterFreq     = CenterFreq;
  DecoderOptions.WindowSize     = WindowSize;     // real samples used
  DecoderOptions.FFTSize        = FFTSize;        // >= WindowSize. If FFTSize > WindowSize, difference is zero padded
  DecoderOptions.WindowFunction = iWindowFunc;
  DecoderOptions.SampleRate     = SampleRate;
  DecoderOptions.xchgIQ         = xchg;
  DecoderOptions.DecoderType    = fskDecoderTypeFromString(DecoderType);
  DecoderOptions.verbose        = 0;

  FFTwInitMutex.lock();
  DecoderHandle = fskDecoderInit(&DecoderOptions);
  FFTwInitMutex.unlock();
}


QString TextDecoderWorkerIntern::processSamples(short * Samples, int nSamples) {
FSKDecodedText  * decodedText;

  fskDecoderProcessSamples(DecoderHandle, Samples, nSamples);
  decodedText = fskDecoderOutput(DecoderHandle);
  QString str = QString(decodedText->Text);
  fskDecoderFreeOutput(decodedText);
  return str;
}


void TextDecoderWorkerIntern::cleanUp() {
  // fskDecoderInitOptions(&DecoderOptions);
  if (DecoderHandle != nullptr) {
    fskDecoderCleanUp(DecoderHandle);
    DecoderHandle = nullptr;
  }
}
