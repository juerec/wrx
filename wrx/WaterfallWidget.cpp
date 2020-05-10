#include "WaterfallWidget.h"

#include "Logger.h"

WaterfallWidget::WaterfallWidget(QWidget *parent)
               : QFrame(parent),
                 Pixmap(size()),
                 CurrLineImage(width(), 1,QImage::Format_RGB32) {
  // logger.debug("WaterfallWidget::WaterfallWidget()", __LINE__, "size() = %i;%i", size().width(), size().height());
  Pixmap.fill(QColor(32, 64, 128));
  currLine = 0;
}


void WaterfallWidget::paintEvent(QPaintEvent * ) {
QPainter p(this);

  p.drawPixmap(1, 0, Pixmap, 0, currLine, -1, -1);
  if (currLine)
    p.drawPixmap(1, Pixmap.height() - currLine, Pixmap, 0,  0, -1, currLine);
}


void WaterfallWidget::resizeEvent(QResizeEvent * event) {
  Pixmap = QPixmap(event->size());
  Pixmap.fill(QColor(32, 64, 128));
  currLine = 0;
  CurrLineImage = QImage(event->size().width(), 1, QImage::Format_RGB32);
}


void WaterfallWidget::newRGBLine(RGBLine * pLine, int /* iLine */ ) {

  // logger.debug("WaterfallWidget::newRGBLine()", __LINE__, "iLine = %i    currLine = %i", iLine, currLine);
  currLine--;
  if (currLine < 0)
    currLine = Pixmap.height() - 1;

  int w = CurrLineImage.width();
  if (w > pLine->nPixel)
    w = pLine->nPixel;
  for (int x = 0; x < w; x++)
    CurrLineImage.setPixel(x, 0, pLine->Pixels[x]);
  QPainter painter(&Pixmap);
  painter.drawImage(1, currLine, CurrLineImage, 0, 0, w, 1);
  delete pLine;
  update();
}
