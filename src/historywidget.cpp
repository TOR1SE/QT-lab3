#include "historywidget.h"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:#f5f7fa;");
    m_model = new QSqlQueryModel(this);
    m_view = new QTableView(this);
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->verticalHeader()->setVisible(false);
    m_view->setAlternatingRowColors(true);

    m_count = new QLabel(this);
    m_count->setAlignment(Qt::AlignRight);
    auto *back = new QPushButton("返回主页", this);
    auto *top = new QHBoxLayout();
    top->addWidget(new QLabel("用户操作记录（数据库 History 表）", this));
    top->addStretch();
    top->addWidget(m_count);
    top->addWidget(back);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(top);
    layout->addWidget(m_view);
    setLayout(layout);

    connect(back, &QPushButton::clicked, this, [this]() { emit backToWelcome(1); });
    reload();
}

void HistoryWidget::reload()
{
    m_model->setQuery("SELECT USER_ID AS 操作用户, EVENT AS 操作内容, TIMESTAMP AS 时间 "
                      "FROM History ORDER BY ID DESC",
                      QSqlDatabase::database("hospital_connection"));
    m_count->setText(QString("共 %1 条").arg(m_model->rowCount()));
}
