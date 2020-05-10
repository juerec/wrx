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

#include "DecoderThread.h"
#include "Logger.h"
#include "ReceiveJobMessage.h"

#include <QProcess>

DecoderThread::DecoderThread(QString & cmd, int jobId)
             : QThread(nullptr) {
  Cmd     = cmd;
  JobId    = jobId;
  success = false;
}

void DecoderThread::run() {
QStringList  CmdArgs;
QProcess     Process;
QByteArray   allStdOut;
QByteArray   allStdErr;
char       * S;

  logger.debug("DecoderThread::run()", __LINE__, "JobId = %i   Starting Cmd: %s", JobId, qPrintable(Cmd));
  CmdArgs.append("-c");
  CmdArgs.append(Cmd);
  Process.start("sh", CmdArgs);
  if (!Process.waitForStarted()) {
    logger.error("DecoderThread::run()", __LINE__, "Error starting Cmd: %s", qPrintable(Cmd));
    emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "Error starting Cmd: %s", qPrintable(Cmd)), JobId);
    return;
  }
  if (!Process.waitForFinished(-1)) {
    logger.error("DecoderThread::run()", __LINE__, "Error runing Cmd: %s", qPrintable(Cmd));
    emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "Error runing Cmd: %s", qPrintable(Cmd)), JobId);
    return;
  }
  allStdErr = Process.readAllStandardError();
  S = allStdErr.data();
  if (*S != 0) {
    logger.error("DecoderThread::run()", __LINE__, "Error: %s\n Cmd: %s", S, qPrintable(Cmd));
    emit TextInfo(ReceiveJobMessage::asString(ReceiveJobMessage::RJ_MSG_ERROR, "Error: %s Cmd: %s", S, qPrintable(Cmd)), JobId);
    return;
  }
  allStdOut = Process.readAllStandardOutput();
  logger.debug("DecoderThread::run()", __LINE__, "stdout = \"%s\"", qPrintable(allStdOut));
  success = true;
  emit Progress(101, JobId);
}
