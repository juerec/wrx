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

#include "ReceiveJobMessage.h"
#include <QObject>


ReceiveJobMessage::ReceiveJobMessage(RJ_MSG_TYPE MsgType, const QString & Msg)
                 : Type(MsgType),
                   Created(QDateTime::currentDateTime().toUTC()),
                   Message(Msg) {
}


ReceiveJobMessage::ReceiveJobMessage(RJ_MSG_TYPE MsgType, const QDateTime dt, const QString &Msg)
                 : Type(MsgType),
                   Created(dt),
                   Message(Msg) {
}


// static
QString ReceiveJobMessage::asString(RJ_MSG_TYPE MsgType, const char * Format, ...) {
QDateTime dt(QDateTime::currentDateTime().toUTC());
const char * chrMsgType = "???";
QString      Message;
size_t       varlength = 0;
char       * str;

   switch(MsgType) {
   case RJ_MSG_INFO :
     chrMsgType = "RJ_MSG_INFO";
     break;
   case RJ_MSG_WARNING :
     chrMsgType = "RJ_MSG_WARNING";
     break;
   case RJ_MSG_ERROR :
     chrMsgType = "RJ_MSG_ERROR";
   }
   if (Format == nullptr || *Format == 0) {
     Message = "???";
   } else {
     va_list ap;
     va_start(ap, Format);
     varlength = size_t(vsnprintf(nullptr, 0, Format, ap));
     va_end(ap);
     str = new char[varlength + 4];
     va_start(ap, Format);
     vsnprintf(str, varlength + 1, Format, ap);
     va_end(ap);
     Message = str;
     delete [] str;
   }
   return QString(chrMsgType) + "€€" + dt.toString("yyyyMMddHHmmss") + "€€" + Message;
}


// static
ReceiveJobMessage ReceiveJobMessage::fromString(const QString & MsgStr) {
QStringList lst = MsgStr.split("€€");
RJ_MSG_TYPE MsgType = RJ_MSG_ERROR;

  if (lst.count() < 3)
    return ReceiveJobMessage(RJ_MSG_ERROR, QString("Invalid argument: \"") + MsgStr + "\"");
  if (lst[0] == "RJ_MSG_INFO")
    MsgType = RJ_MSG_INFO;
  if (lst[0] == "RJ_MSG_WARNING")
    MsgType = RJ_MSG_WARNING;
  return ReceiveJobMessage(MsgType, QDateTime::fromString(lst[1], "yyyyMMddHHmmss"), lst[2]);
}


QString ReceiveJobMessage::TypeName() const {

  switch(Type) {
  case RJ_MSG_INFO :
    return QObject::tr("RJ_MSG_INFO");
  case RJ_MSG_WARNING :
    return QObject::tr("RJ_MSG_WARNING");
  case RJ_MSG_ERROR :
    return QObject::tr("RJ_MSG_ERROR");
  }
  return "";
}
