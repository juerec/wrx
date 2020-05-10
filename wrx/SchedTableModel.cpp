#include "SchedTableModel.h"
#include "ReceiveJob.h"
#include "Logger.h"
#include "Options.h"

#include <qpixmap.h>

SchedTableModel::SchedTableModel(QObject *parent)
               : QAbstractTableModel(parent) {

  lstPixmaps.append(QPixmap(":/img/square-21-red-X.xpm"));
  lstPixmaps.append(QPixmap(":/img/square-21-grey-R.xpm"));
  lstPixmaps.append(QPixmap(":/img/square-21-timeconflict.xpm"));
  lstPixmaps.append(QPixmap(":/img/square-21-grey-X.xpm"));
  lstPixmaps.append(QPixmap(":/img/square-21-grey.xpm"));
  lstPixmaps.append(QPixmap(":/img/square-21-yellow.xpm"));
  lstPixmaps.append(QPixmap(":/img/square-21-red.xpm"));
  StartBluePi = lstPixmaps.count();
  lstPixmaps.append(QPixmap(":/img/circle-21-000deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-009deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-018deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-027deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-036deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-045deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-054deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-063deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-072deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-081deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-090deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-099deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-108deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-117deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-126deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-135deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-144deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-153deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-162deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-171deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-180deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-189deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-198deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-207deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-216deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-225deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-234deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-243deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-252deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-261deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-270deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-279deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-288deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-297deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-306deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-315deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-324deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-333deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-342deg-blue.xpm"));
  lstPixmaps.append(QPixmap(":/img/circle-21-351deg-blue.xpm"));
  StartOk  = lstPixmaps.count();
  lstPixmaps.append(QPixmap(":/img/circle-21-green.xpm"));
  lstPixmaps.append(QPixmap(":/img/decoder-ok-21.png"));
  lstPixmaps.append(QPixmap(":/img/square-21-green.xpm"));
  StartResult = lstPixmaps.count();
  lstPixmaps.append(QPixmap(":/img/text_off.xpm"));
  lstPixmaps.append(QPixmap(":/img/text_on.xpm"));
  lstPixmaps.append(QPixmap(":/img/fax_off.xpm"));
  lstPixmaps.append(QPixmap(":/img/fax_on.xpm"));
  lstPixmaps.append(QPixmap(":/img/speaker_off.xpm"));
  lstPixmaps.append(QPixmap(":/img/speaker_on.xpm"));
  StartMessage = lstPixmaps.count();
  lstPixmaps.append(QPixmap(":/img/msg_none.xpm"));
  lstPixmaps.append(QPixmap(":/img/msg_info.xpm"));
  lstPixmaps.append(QPixmap(":/img/msg_warn.xpm"));
  lstPixmaps.append(QPixmap(":/img/msg_error.xpm"));

  for (int i = 0; i < lstPixmaps.count(); i++)
    if (lstPixmaps[i].isNull())
      logger.error("SchedTableModel::SchedTableModel()", __LINE__, "lstPixmaps[%i].isNull()!", i);
}


int SchedTableModel::rowCount(const QModelIndex & /* parent */) const {
  return ReceiveJob::lstReceiveJobs.count();
}


int SchedTableModel::columnCount(const QModelIndex & /* parent */ ) const {
  return SCHED_COLUMNS;
}


QPixmap SchedTableModel::ProgressPixmap(int Percent) const {
int iPixmap;

  switch (Percent) {
  case PROGRESS_CONFIG_ERROR :
    iPixmap = StartBluePi - 7;
    break;
  case PROGRESS_RADIO_OFF :
    iPixmap = StartBluePi - 6;  // grey square with red R
    break;
  case PROGRESS_TIMECONFLICT :  // grey square timeconflict
    iPixmap = StartBluePi - 5;
    break;
  case PROGRESS_DISABLED :  // grey square with red X
    iPixmap = StartBluePi - 4;
    break;
  case PROGRESS_WAIT :  // gey square
    iPixmap = StartBluePi - 3;
    break;
  case PROGRESS_SOON :  // yellow square
    iPixmap = StartBluePi - 2;
    break;
  case PROGRESS_ERROR :  // red square
    iPixmap = StartBluePi - 1;
    break;
  case PROGRESS_RECORD_OK : // green circle - record ok, ext. decoder still running
    iPixmap = StartOk;
      break;
  case PROGRESS_POSTPROC_OK :  // green circle with mark - post command finished
    iPixmap = StartOk + 1;
    break;
  case PROGRESS_OK : // green square - Ok, iternal decoder or recording only, no post command
    iPixmap = StartOk + 2;
      break;
  default :
    if (Percent < (0 - StartBluePi) || Percent > 102) // invalid value!  Keep program stable, return a valide pointer
      iPixmap = PROGRESS_DISABLED;
    else
      iPixmap = (Percent * 2) / 5 + StartBluePi;
  }
  return lstPixmaps[iPixmap];
}


QVariant SchedTableModel::data(const QModelIndex &index, int role) const {

  if (!index.isValid())
    return QVariant();
  ReceiveJob * rj = ReceiveJob::lstReceiveJobs[index.row()];
  switch (role) {
  case Qt::DisplayRole :
    switch (index.column()) {
    case COL_ENABLE :
      return "";
    case COL_PROGRESS:
    case COL_RESULTS :
    case COL_ERRORS :
      return QVariant();
    case COL_START :
      return rj->Start;
    case COL_END :
      return rj->End;
    case COL_NAME :
      return rj->Name;
    case COL_CALLSIGN_RADIO :
      return rj->activeRadioSetup->Name();
      // return "COL_CALLSIGN_RADIO";
    default :
      return "???";
    }
  case Qt::CheckStateRole :
    if (index.column() == COL_ENABLE)
      return  rj->Enable ? Qt::Checked : Qt::Unchecked;
    else
      return QVariant();
  case Qt::UserRole :
    switch (index.column()) {
    case COL_CALLSIGN_RADIO :
      return rj->RadioSetupNamen();
    }
    break;
  case Qt::DecorationRole :
    switch (index.column()) {
    case COL_PROGRESS :
      return ProgressPixmap(rj->ProgressInfo());
    case COL_RESULTS :
      return lstPixmaps[rj->ResultInfo() + StartResult];
    case COL_ERRORS :
      return lstPixmaps[rj->MessageInfo() + StartMessage];
    }
    break;
  case Qt::SizeHintRole :
    switch (index.column()) {
    case COL_PROGRESS :
      return QSize(21,21);
    }
    break;
  case Qt::ToolTipRole :
    switch (index.column()) {
    case COL_RESULTS :
      return rj->ToolTipResults();
    case COL_ERRORS :
      return rj->ToolTipErrors();
    }
    break;
  default :
    break;
  }
  return QVariant();
}


bool SchedTableModel::setData(const QModelIndex & index, const QVariant & value, int role) {

  // logger.debug("SchedTableModel::setData()", __LINE__, "SchedTableModel::setData(index(%i,%i), value, role(%i))\n", index.column(), index.row(), role);
  if ((role == Qt::EditRole || role == Qt::DecorationRole) && index.isValid()) {
    ReceiveJob * rj = ReceiveJob::lstReceiveJobs[index.row()];
    switch (index.column()) {
    case COL_ENABLE :
      // logger.debug("SchedTableModel::setData()", __LINE__, "role == Qt::EditRole    COL_ENABLE     rj->Id = %i   value.toBool() = %s", rj->Id,  value.toBool() ? "true" : "false");
      rj->Enable = value.toBool();
      return true;
    case COL_PROGRESS :
      {
        int newProgress = value.toInt();
        int row = index.row();
        QModelIndex index0 = createIndex(row, (newProgress <= 0 || newProgress == PROGRESS_POSTPROC_OK || newProgress == PROGRESS_OK) ? COL_ENABLE : COL_PROGRESS);
        QModelIndex index1 = createIndex(row, newProgress >= 100 ? COL_PROGRESS : COL_RESULTS);
        rj->setProgress(newProgress);
        emit dataChanged(index0, index1, {Qt::DecorationRole});
      }
      return true;
    case COL_CALLSIGN_RADIO :
      rj->setCallsignRadio(value.toString());
      emit radioChanged();
      return true;
    case COL_ERRORS :
      rj->addMessage(ReceiveJobMessage::fromString(value.toString()));
      emit dataChanged(index, index, {Qt::DecorationRole});
      return true;
    }
  }
  if (role == Qt::CheckStateRole && index.column() == COL_ENABLE) {
    ReceiveJob * rj = ReceiveJob::lstReceiveJobs[index.row()];
    rj->Enable = value.toBool();
    // rj->changed = true;
    QModelIndex idx = this->index(index.row(), COL_PROGRESS);
    emit dataChanged(index, idx);
    emit radioChanged();
    // logger.debug("SchedTableModel::setData()", __LINE__, " role == Qt::CheckStateRole    rj->Enable = %s", rj->Enable ? "true" : "false");
    return true;
  }
  return false;
}


QVariant SchedTableModel::headerData(int section, Qt::Orientation /* orientation */, int role) const {

  if (role == Qt::DisplayRole)
    switch (section) {
    case COL_ENABLE :
      return tr("A");
    case COL_PROGRESS :
      return tr("P");
    case COL_START :
      return tr("Start");
    case COL_END :
      return tr("End");
    case COL_NAME :
      return tr("Name");
    case COL_CALLSIGN_RADIO :
      return tr("Call Sign - Radio");
    case COL_RESULTS :
      return tr("R");
    case COL_ERRORS :
      return tr("E");
    }
  return QVariant();
}


Qt::ItemFlags SchedTableModel::flags ( const QModelIndex & index ) const {

  if (!index.isValid())
    return Qt::ItemIsEnabled;

  Qt::ItemFlags flags = Qt::NoItemFlags;
  ReceiveJob * rj = ReceiveJob::lstReceiveJobs[index.row()];
  switch (index.column()) {
  case COL_ENABLE :
    if (rj->isRunning())
      return  QAbstractTableModel::flags(index) & ~(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    flags = Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    break;
  case COL_CALLSIGN_RADIO :
    flags = Qt::ItemIsEditable;
    break;
  }
  return QAbstractTableModel::flags(index) | flags;
}
