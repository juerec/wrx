#include "RGBLineFiFo.h"

RGBLineFiFo::RGBLineFiFo() {
  maxLines = 100;
}

RGBLineFiFo::~RGBLineFiFo() {
  clear();
}

void RGBLineFiFo::append(RGBLine * Line) {
RGBLine * line;

  mutex.lock();
  lst.append(Line);
  while (lst.count() > maxLines) {
    line = lst[0];
    lst.removeFirst();
    delete line;
  }
  mutex.unlock();
}

void RGBLineFiFo::clear() {
  mutex.lock();
  for (int i = 0; i < lst.count(); i++)
    delete lst[i];
  lst.clear();
  mutex.unlock();
}
