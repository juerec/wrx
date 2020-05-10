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
