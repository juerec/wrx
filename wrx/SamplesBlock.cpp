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
