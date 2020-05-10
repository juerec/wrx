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
