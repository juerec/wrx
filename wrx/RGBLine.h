#ifndef RGBLINE_H
#define RGBLINE_H


class RGBLine {
public:
  RGBLine(int nPixel);
  ~RGBLine();

  unsigned int * Pixels;
  int            nPixel;

  void set(unsigned int Pixel, int iPixel) { Pixels[iPixel] = Pixel; }

};

#endif // RGBLINE_H
