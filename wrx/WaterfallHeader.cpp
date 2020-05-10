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

#include "WaterfallHeader.h"
#include <QPainter>
#include <QResizeEvent>
#include "Logger.h"

WaterfallHeader::WaterfallHeader(QWidget *parent)
               : QWidget(parent),
                 BgColor(64, 128, 128),
                 LightColor(128, 128, 255),
                 NormalColor(32, 32, 195) {
}

void WaterfallHeader::setParams(OpMode::OPMODE_ID mode, int left, int center, int right) {
QPainter p(&Pixmap);

  // logger.debug("WaterfallHeader::setParams()", __LINE__, "left = %i   center = %i   right = %i", left, center, right);

  Pixmap.fill(BgColor);
  switch (mode) {
  case OpMode::SPEACH_AM :
  case OpMode::SPEACH_SSB :
  case OpMode::SPEACH :
    p.fillRect(left, 6, right - left, Pixmap.height() - 6, NormalColor);
    p.fillRect(center - 1, 0, 3, Pixmap.height(), LightColor);
    break;
  case OpMode::SPEACH_NFM :
  case OpMode::SPEACH_WFM :
    p.fillRect(left, 6, right - left, Pixmap.height() - 6, NormalColor);
    break;
  case OpMode::RTTY_450 :
  case OpMode::RTTY_85 :
  case OpMode::NAVTEX :
  case OpMode::FAX :
    p.fillRect(left, 6, right - left, Pixmap.height() - 6, NormalColor);
    p.fillRect(left - 1, 0, 3, Pixmap.height(), LightColor);
    p.fillRect(right - 1, 0, 3, Pixmap.height(), LightColor);
    break;
  case OpMode::OTHER :
    ;
  }
  update();
}

void WaterfallHeader::paintEvent(QPaintEvent *) {
QPainter p(this);

  p.drawPixmap(0, 0, Pixmap);
}

void WaterfallHeader::resizeEvent(QResizeEvent * event) {
  Pixmap = QPixmap(event->size());
  Pixmap.fill(BgColor);
}
