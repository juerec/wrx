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
