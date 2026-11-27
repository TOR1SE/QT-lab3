#include "genderdelegate.h"

#include <QComboBox>

GenderDelegate::GenderDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QWidget *GenderDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                                      const QModelIndex &) const
{
    auto *box = new QComboBox(parent);
    box->addItem("男", 1);
    box->addItem("女", 2);
    return box;
}

void GenderDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto *box = qobject_cast<QComboBox *>(editor);
    if (!box) return;
    const int value = index.data(Qt::EditRole).toInt();
    const int pos = box->findData(value);
    box->setCurrentIndex(pos < 0 ? 0 : pos);
}

void GenderDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    auto *box = qobject_cast<QComboBox *>(editor);
    if (!box) return;
    model->setData(index, box->currentData(), Qt::EditRole);
}

QString GenderDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    const int sex = value.toInt();
    if (sex == 1) return "男";
    if (sex == 2) return "女";
    return QStyledItemDelegate::displayText(value, locale);
}
