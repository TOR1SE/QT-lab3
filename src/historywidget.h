#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QSqlQueryModel>
#include <QWidget>

class QTableView;
class QLabel;

class HistoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    void reload();

signals:
    void backToWelcome(int page);

private:
    QSqlQueryModel *m_model;
    QTableView *m_view;
    QLabel *m_count;
};

#endif // HISTORYWIDGET_H
