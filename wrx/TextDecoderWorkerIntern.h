#ifndef TEXTDECODERWORKERINTERN_H
#define TEXTDECODERWORKERINTERN_H

#include <QString>

#include "FSKDecoder.h"
#include "SamplesBlock.h"

class TextDecoderWorkerIntern {

public :

  TextDecoderWorkerIntern();
  ~TextDecoderWorkerIntern();

  void     init(const char * DecoderType,
                int          SampleRate,
                int          FFTSize,
                int          iWindowFunc,
                int          WindowSize,
                int          CenterFreq,
                int          Shift,
                bool         xchg);
  QString  processSamples(short * Samples, int nSamples);
  void     cleanUp();

protected :

  FSKDecoderOptions DecoderOptions;
  FSKDecoderHandle  DecoderHandle;

//  static FSKDecoderType findDecoderType(const char * DecoderType);

};

#endif // TEXTDECODERWORKERINTERN_H
