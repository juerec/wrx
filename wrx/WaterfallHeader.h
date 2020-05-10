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

#ifndef WATERFALLHEADER_H
#define WATERFALLHEADER_H

#include <QWidget>
#include "OpMode.h"

class WaterfallHeader : public QWidget
{
  Q_OBJECT
public:
  explicit WaterfallHeader(QWidget *parent = 0);

  void setParams(OpMode::OPMODE_ID mode, int left, int center, int right);

protected :
  virtual void paintEvent(QPaintEvent * e);
  virtual void resizeEvent(QResizeEvent * event);

  QPixmap Pixmap;
  QColor  BgColor;
  QColor  LightColor;
  QColor  NormalColor;

signals:
  
public slots:
  
};

#endif // WATERFALLHEADER_H
