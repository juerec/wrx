#ifndef SCHEDTABLEMODEL_H
#define SCHEDTABLEMODEL_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>

#include <qpixmap.h>

#define COL_ENABLE          0
#define COL_PROGRESS        1
#define COL_START           2
#define COL_END             3
#define COL_NAME            4
#define COL_CALLSIGN_RADIO  5
#define COL_RESULTS         6
#define COL_ERRORS          7


class SchedTableModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit SchedTableModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  virtual Qt::ItemFlags	flags ( const QModelIndex & index ) const;

protected :
  QPixmap ProgressPixmap(int Percent) const;

  QList<QPixmap> lstPixmaps;

  int   StartBluePi;
  int   StartOk;
  int   StartResult;
  int   StartMessage;

signals:
  void radioChanged();
  
public slots:
  
};

#endif // SCHEDTABLEMODEL_H
