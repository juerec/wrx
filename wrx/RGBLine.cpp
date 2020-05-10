#include "RGBLine.h"

RGBLine::RGBLine(int nPixel) {
  this->nPixel = nPixel;
  Pixels = new unsigned int [nPixel];
}

RGBLine::~RGBLine() {
  delete [] Pixels;
}
