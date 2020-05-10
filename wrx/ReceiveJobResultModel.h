#ifndef RECEIVEJOBRESULTMODEL_H
#define RECEIVEJOBRESULTMODEL_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QFileInfo>
#include <QPixmap>

#define COL_RESULT          0
#define COL_STARTVIEW       1


class ReceiveJobResultModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ReceiveJobResultModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    /*
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    */

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;


    QList<QFileInfo> lstResults;

protected :

  QPixmap  pixmapTxt;
  QPixmap  pixmapWav;
  QPixmap  pixmapBmp;
  // QPixmap  pixmapDelete;
  // QPixmap  pixmapSpectrum;

private:
};

#endif // RECEIVEJOBRESULTMODEL_H
