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

#include "ReceiveJobResultModel.h"

ReceiveJobResultModel::ReceiveJobResultModel(QObject *parent)
    : QAbstractTableModel(parent),
      pixmapTxt(":/img/text_on.xpm"),
      pixmapWav(":/img/speaker_on.xpm"),
      pixmapBmp(":/img/fax_on.xpm")
      /* ,pixmapDelete(""), pixmapSpectrum("") */ {
}


QVariant ReceiveJobResultModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    switch (section) {
    case COL_RESULT :
      return tr("Result file");
    case COL_STARTVIEW :
      return tr("View");
    }
  return QVariant();
}


int ReceiveJobResultModel::rowCount(const QModelIndex & /*parent*/) const {
  return lstResults.count();
}


int ReceiveJobResultModel::columnCount(const QModelIndex & /*parent*/) const {
  return 2;
}


QVariant ReceiveJobResultModel::data(const QModelIndex &index, int role) const {

  if (!index.isValid() || lstResults.count() < 1)
    return QVariant();

  const QFileInfo & fi = lstResults[index.row()];
  switch (role) {
  case Qt::DisplayRole :
    switch (index.column()) {
    case COL_RESULT :
      return fi.fileName();
    default :
      ;
    }
    break;
  case Qt::DecorationRole :
    switch (index.column()) {
    case COL_STARTVIEW :
      if (fi.suffix() == "txt")
        return pixmapTxt;
      if (fi.suffix() == "wav")
        return pixmapWav;
      if (fi.suffix() == "bmp")
        return pixmapBmp;
      break;
    default :
      ;
    }
    break;
  case Qt::SizeHintRole :
    switch (index.column()) {
    case COL_STARTVIEW :
      return QSize(21, 21);
    default :
      ;
    }
    break;
  case Qt::ToolTipRole :
    switch (index.column()) {
    case COL_STARTVIEW :
      if (fi.suffix() == "txt")
        return tr("View with text viewer for RTTY/NAVTEX");
      if (fi.suffix() == "wav")
        return tr("Play sound");
      if (fi.suffix() == "bmp")
        return tr("Show with picture viwer");
      break;
    default :
      ;
    }
    break;
  default :
    ;
  }
  return QVariant();
}
