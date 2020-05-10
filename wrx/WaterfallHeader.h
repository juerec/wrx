#ifndef WATERFALLHEADER_H
#define WATERFALLHEADER_H

#include <QWidget>
#include "OpMode.h"

class WaterfallHeader : public QWidget
{
  Q_OBJECT
public:
  explicit WaterfallHeader(QWidget *parent = 0);

  void setParams(OpMode::OPMODE_ID mode, int left, int center, int right);

protected :
  virtual void paintEvent(QPaintEvent * e);
  virtual void resizeEvent(QResizeEvent * event);

  QPixmap Pixmap;
  QColor  BgColor;
  QColor  LightColor;
  QColor  NormalColor;

signals:
  
public slots:
  
};

#endif // WATERFALLHEADER_H
