#ifndef DECODER_H
#define DECODER_H

#include <QList>
#include <QMutex>
#include <QWidget>
#include "DecoderThread.h"

class Decoder  {

public:

  Decoder();
  ~Decoder();

  void startDecoder(QString cmd, int JobId);
  bool findTerminatedDecoder(int * pJobId, bool * success);
  void deleteTerminatedDecoders();
  void setMainWindow(QWidget * pMainWindow) { this->pMainWindow = pMainWindow; }

protected :

  QList<DecoderThread *> lstDecoder;
  QMutex  mutex;
  QWidget * pMainWindow;

};

extern Decoder * pDecoder;

#endif // DECODER_H
