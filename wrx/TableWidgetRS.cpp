#include "TableWidgetRS.h"
#include "Logger.h"

#include <QResizeEvent>

TableWidgetRS::TableWidgetRS(QWidget *parent)
             : QTableWidget(parent) {
}


// virtual
void TableWidgetRS::resizeEvent(QResizeEvent * event) {

  QTableWidget::resizeEvent(event);

QSize size = event->size();
int   width = size.width() - 8;
int   col2  = 80;
int   col1  = ((width - col2) * 60) / 100;
int   col0  = width - col1 - col2;

  // logger.debug("TabWidgetRS::resizeEvent()", __LINE__, "size = %i - %i", size.width(), size.height());

  setColumnWidth(2, col2);
  setColumnWidth(1, col1);
  setColumnWidth(0, col0);
}
