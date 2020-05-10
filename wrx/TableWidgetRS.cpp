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

#include "TableWidgetRS.h"
#include "Logger.h"

#include <QResizeEvent>

TableWidgetRS::TableWidgetRS(QWidget *parent)
             : QTableWidget(parent) {
}


// virtual
void TableWidgetRS::resizeEvent(QResizeEvent * event) {

  QTableWidget::resizeEvent(event);

QSize size = event->size();
int   width = size.width() - 8;
int   col2  = 80;
int   col1  = ((width - col2) * 60) / 100;
int   col0  = width - col1 - col2;

  // logger.debug("TabWidgetRS::resizeEvent()", __LINE__, "size = %i - %i", size.width(), size.height());

  setColumnWidth(2, col2);
  setColumnWidth(1, col1);
  setColumnWidth(0, col0);
}
