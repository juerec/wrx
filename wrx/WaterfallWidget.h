#ifndef WATERFALLWIDGET_H
#define WATERFALLWIDGET_H

#include <QtGui>
#include <QFrame>

#include "RGBLine.h"

class WaterfallWidget : public QFrame {

  Q_OBJECT

public:
  explicit WaterfallWidget(QWidget *parent = 0);

protected :

  virtual void paintEvent(QPaintEvent * e);
  virtual void resizeEvent(QResizeEvent * event);

  QPixmap Pixmap;
  QImage  CurrLineImage;
  int     currLine;

signals:
  
public slots:
  void newRGBLine(RGBLine *, int iLine);
};

#endif // WATERFALLWIDGET_H
