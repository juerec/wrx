#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QCheckBox>

class QModelIndex;
class QWidget;
class QVariant;

class ComboBoxDelegate : public QStyledItemDelegate { //QItemDelegate {

Q_OBJECT

public:
  ComboBoxDelegate(QObject *parent = 0);
  ~ComboBoxDelegate();

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  // void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected :

};

#endif // COMBOBOXDELEGATE_H
