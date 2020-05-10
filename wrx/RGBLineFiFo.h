#ifndef RGBLINEFIFO_H
#define RGBLINEFIFO_H

#include <QtCore>
#include "RGBLine.h"

class RGBLineFiFo {

public:

  RGBLineFiFo();
  ~RGBLineFiFo();

  void append(RGBLine * Line);
  RGBLine * remove();
  int  maxLines;
  void clear();

protected :

  QList<RGBLine *> lst;
  QMutex  mutex;

};

#endif // RGBLINEFIFO_H
