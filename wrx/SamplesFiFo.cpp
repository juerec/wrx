/****************************************************************************
**
**  This file is a part of the program "WRX".
**
**  Copyright © 2020 Jürgen Reche
**
** WRX is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** WRX is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

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
