#ifndef GENDERDELEGATE_H
#define GENDERDELEGATE_H

#include <QStyledItemDelegate>

// 自定义代理：性别列用下拉框编辑，数据库里仍存整数 1/2
class GenderDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit GenderDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // GENDERDELEGATE_H
