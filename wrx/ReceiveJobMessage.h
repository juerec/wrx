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

#ifndef RECEIVEJOBMESSAGE_H
#define RECEIVEJOBMESSAGE_H

#include <QString>
#include <QDateTime>
#include <stdarg.h>


class ReceiveJobMessage {
public:

    enum RJ_MSG_TYPE {
      RJ_MSG_INFO,
      RJ_MSG_WARNING,
      RJ_MSG_ERROR
    };

//  ReceiveJobMessage();
  ReceiveJobMessage(RJ_MSG_TYPE MsgType, const QString &Msg);
  ReceiveJobMessage(RJ_MSG_TYPE MsgType, const QDateTime dt, const QString &Msg);
  static QString asString(RJ_MSG_TYPE MsgType, const char * Format, ...);
  static ReceiveJobMessage fromString(const QString & MsgStr);
  QString  TypeName() const;

  RJ_MSG_TYPE  Type;
  QDateTime    Created;
  QString      Message;
};

#endif // RECEIVEJOBMESSAGE_H
