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
