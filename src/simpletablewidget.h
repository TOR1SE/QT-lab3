#ifndef SIMPLETABLEWIDGET_H
#define SIMPLETABLEWIDGET_H

#include <QSqlQueryModel>
#include <QWidget>

class QTableView;
class QLabel;

// 选做功能：科室 / 医生信息的简单维护
class SimpleTableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleTableWidget(const QString &table, const QString &title,
                               QWidget *parent = nullptr);
    void reload();

signals:
    void backToWelcome(int page);

private slots:
    void addRecord();
    void deleteRecord();

private:
    QString m_table;
    QSqlQueryModel *m_model;
    QTableView *m_view;
    QLabel *m_count;
};

#endif // SIMPLETABLEWIDGET_H
