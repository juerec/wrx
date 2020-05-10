/****************************************************************************
**
**  This file is a part of the program "WRX".
**
**  Copyright © 2020 Jürgen Reche
**
** WRX is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** WRX is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "ComboBoxDelegate.h"

#include <QtGui>
#include <QComboBox>
#include <QCheckBox>

#include "SchedTableModel.h"
#include "Logger.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
                : QStyledItemDelegate(parent) {
}

ComboBoxDelegate::~ComboBoxDelegate() {
}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const {

  switch (index.column()) {
  case COL_ENABLE :
    return new QCheckBox(parent);
  case COL_CALLSIGN_RADIO :
    {
    QComboBox* editor = new QComboBox(parent);
    SchedTableModel * model = (SchedTableModel *)index.model();
    QStringList data = model->data(index, Qt::UserRole).toStringList();
    editor->addItems(data);
    int i = editor->findText(model->data(index, Qt::DisplayRole).toString());
    editor->setCurrentIndex(i);
    return editor;
    }

  default :
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

  switch (index.column()) {
  case COL_ENABLE :
    {
    QCheckBox * cb = static_cast<QCheckBox*>(editor);
    cb->setChecked(index.model()->data(index).toBool());
    break;
    }
  case COL_START :
  case COL_END :
  case COL_NAME :
    break;
  case COL_CALLSIGN_RADIO :
    QStyledItemDelegate::setEditorData(editor, index);
    break;

  }
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  switch (index.column()) {
  case COL_ENABLE :
    {
    QCheckBox * cb = static_cast<QCheckBox*>(editor);
    cb->setChecked(index.model()->data(index).toBool());
    model->setData(index, cb->isChecked(), Qt::EditRole);
    logger.debug("ComboBoxDelegate::setModelData()", __LINE__, "#####################  COL_ENABLE");
    break;
    }
  case COL_START :
  case COL_END :
  case COL_NAME :
    QStyledItemDelegate::setModelData(editor, model, index);
    break;
  case COL_CALLSIGN_RADIO :
    {
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
    logger.debug("ComboBoxDelegate::setModelData()", __LINE__, "#####################  COL_CALLSIGN_RADIO");
    }
  }
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
  editor->setGeometry(option.rect);
}
