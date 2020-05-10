#ifndef DECODERTHREAD_H
#define DECODERTHREAD_H

#include <QThread>
#include "ReceiveJob.h"

class DecoderThread : public QThread {
  Q_OBJECT

public:

  DecoderThread(QString & cmd, int JobId);

  int jobId() { return JobId; }
  bool    success;

protected :

  QString Cmd;
  // ReceiveJob * pJob;
  int JobId;

  virtual void run();

signals :

  void TextInfo(QString, int);
  void Progress(int, int);
  
};

#endif // DECODERTHREAD_H
