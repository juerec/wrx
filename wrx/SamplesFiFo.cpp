#include "SamplesFiFo.h"
#include "Logger.h"

SamplesFiFo::SamplesFiFo() {
  maxBlocks = 100;
}

SamplesFiFo::~SamplesFiFo() {
  clear();
}

void SamplesFiFo::append(SamplesBlock * Block) {
SamplesBlock * sb;

  mutex.lock();
  lst.append(Block);
  while (lst.count() > maxBlocks) {
    logger.error("SamplesFiFo::append()", __LINE__, "Overflow!");
    sb = lst[0];
    lst.removeFirst();
    delete sb;
  }
  mutex.unlock();
  // wakeup waterfall thread
}


SamplesBlock * SamplesFiFo::remove() {
SamplesBlock * sb = nullptr;

  mutex.lock();
  if (lst.count() > 0) {
    sb = lst[0];
    lst.removeFirst();
  }
  mutex.unlock();
  return sb;
}


void SamplesFiFo::clear() {
  mutex.lock();
  for (int i = 0; i < lst.count(); i++)
    delete lst[i];
  lst.clear();
  mutex.unlock();
}
