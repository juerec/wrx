#ifndef TEXTDECODERWORKER_H
#define TEXTDECODERWORKER_H

#include <QtCore>
#include <QMutex>
#include <QWaitCondition>

#include "ReceiveJob.h"
#include "SamplesFiFo.h"


class TextDecoderWorker : public QThread {

  Q_OBJECT

public:

  TextDecoderWorker(SamplesFiFo * _pTextDecoderFiFo, const QString &rRadioName);
  void signalThread();
  void termThread();
  bool setupDecoder(RadioSetup *pRadioSetup);
  bool startRecord(QString FileName);
  void stopRecord();
  bool openDecoderFile(const QString & FileName);
  void closeDecoderFile();

  QMutex         mutexCond;
  QWaitCondition waitCondTDThread;

  SamplesFiFo * pTextDecoderFiFo;

signals :

  void decodedText(QString);

protected :

  QMutex         mutexJobInfo;

  virtual void run();
  int           ThisThreadCmd;

  char          DecoderType[16];
  int           SampleRate;
  int           FFTSize;
  int           iWindowFunc;
  int           WindowSize;
  int           CenterFreq;
  int           Shift;
  bool          xchg;

  QString       RadioName;
  QString       FileName;
  FILE        * fDecoderRecord;

};

#endif // TEXTDECODERWORKER_H
