#ifndef WATERFALLINFO_H
#define WATERFALLINFO_H

class WaterfallInfo {

public :

  WaterfallInfo() { nBins = 512; binLeft = 0; Zoom = 1; Gain = 0.03f; }
  WaterfallInfo(int nbins, int binleft, int zoom, float gain)
             : nBins(nbins), binLeft(binleft), Zoom(zoom), Gain(gain) {}

  int     nBins;
  int     binLeft;
  int     Zoom;
  float   Gain;

};

#endif // WATERFALLINFO_H
