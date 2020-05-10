#ifndef TABLEWIDGETRS_H
#define TABLEWIDGETRS_H

#include <QTableWidget>

class TableWidgetRS : public QTableWidget {
  Q_OBJECT

public:
  explicit TableWidgetRS(QWidget *parent = 0);
  
protected :

  virtual void resizeEvent(QResizeEvent * event);
//  virtual void showEvent(QShowEvent * event);

};

#endif // TABLEWIDGETRS_H
