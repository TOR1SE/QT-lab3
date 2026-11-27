#include "simpletablewidget.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>

SimpleTableWidget::SimpleTableWidget(const QString &table, const QString &title, QWidget *parent)
    : QWidget(parent), m_table(table)
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
    auto *addBtn = new QPushButton("添加", this);
    auto *delBtn = new QPushButton("删除", this);
    auto *back = new QPushButton("返回主页", this);
    auto *top = new QHBoxLayout();
    top->addWidget(new QLabel(title, this));
    top->addStretch();
    top->addWidget(m_count);
    top->addWidget(addBtn);
    top->addWidget(delBtn);
    top->addWidget(back);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(top);
    layout->addWidget(m_view);
    setLayout(layout);

    connect(back, &QPushButton::clicked, this, [this]() { emit backToWelcome(1); });
    connect(addBtn, &QPushButton::clicked, this, &SimpleTableWidget::addRecord);
    connect(delBtn, &QPushButton::clicked, this, &SimpleTableWidget::deleteRecord);
    reload();
}

void SimpleTableWidget::reload()
{
    if (m_table == "Department") {
        m_model->setQuery("SELECT ID, NAME AS 科室名称 FROM Department ORDER BY ID",
                          QSqlDatabase::database("hospital_connection"));
    } else {
        m_model->setQuery("SELECT ID, EMPLOYEENO AS 工号, NAME AS 姓名, "
                          "DEPARTMENT_ID AS 科室ID FROM Doctor ORDER BY ID",
                          QSqlDatabase::database("hospital_connection"));
    }
    m_count->setText(QString("共 %1 条").arg(m_model->rowCount()));
}

void SimpleTableWidget::addRecord()
{
    bool ok = false;
    QSqlQuery q(QSqlDatabase::database("hospital_connection"));

    if (m_table == "Department") {
        const QString name = QInputDialog::getText(this, "添加科室", "科室名称：",
                                                   QLineEdit::Normal, "", &ok).trimmed();
        if (!ok || name.isEmpty()) return;
        q.prepare("INSERT INTO Department(NAME) VALUES(?)");
        q.addBindValue(name);
    } else {
        const QString no = QInputDialog::getText(this, "添加医生", "工号：",
                                                 QLineEdit::Normal, "", &ok).trimmed();
        if (!ok || no.isEmpty()) return;
        const QString name = QInputDialog::getText(this, "添加医生", "姓名：",
                                                   QLineEdit::Normal, "", &ok).trimmed();
        if (!ok || name.isEmpty()) return;
        const int dept = QInputDialog::getInt(this, "添加医生", "科室 ID：", 1, 1, 999);
        q.prepare("INSERT INTO Doctor(EMPLOYEENO,NAME,DEPARTMENT_ID) VALUES(?,?,?)");
        q.addBindValue(no);
        q.addBindValue(name);
        q.addBindValue(dept);
    }

    if (!q.exec()) {
        QMessageBox::warning(this, "添加失败", q.lastError().text());
        return;
    }
    reload();
}

void SimpleTableWidget::deleteRecord()
{
    const auto rows = m_view->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        QMessageBox::information(this, "删除", "请先选中一行");
        return;
    }
    if (QMessageBox::question(this, "确认删除", "确定删除这条记录吗？",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        != QMessageBox::Yes) {
        return;
    }
    const QString id = m_model->index(rows.first().row(), 0).data().toString();
    QSqlQuery q(QSqlDatabase::database("hospital_connection"));
    q.prepare(QString("DELETE FROM %1 WHERE ID = ?").arg(m_table));
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::warning(this, "删除失败", q.lastError().text());
        return;
    }
    reload();
}
