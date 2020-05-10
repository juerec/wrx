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
