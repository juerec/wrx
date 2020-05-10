#ifndef SAMPLESFIFO_H
#define SAMPLESFIFO_H

#include <QtCore>
#include "SamplesBlock.h"

class SamplesFiFo {
public:
  SamplesFiFo();
  ~SamplesFiFo();
  void append(SamplesBlock * Block);
  SamplesBlock * remove();
  int  maxBlocks;
  void clear();

protected :

  QList<SamplesBlock *> lst;
  QMutex  mutex;

};

#endif // SAMPLESFIFO_H
