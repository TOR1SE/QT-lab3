#include "patientwidget.h"

#include "dbmanager.h"
#include "genderdelegate.h"
#include "patientdialog.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>

Qt::ItemFlags PatientQueryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QSqlQueryModel::flags(index);
    if (index.isValid() && index.column() == 3) {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

bool PatientQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != 3 || role != Qt::EditRole) {
        return false;
    }
    const QString id = index.siblingAtColumn(0).data().toString();
    QSqlQuery q(QSqlDatabase::database("hospital_connection"));
    q.prepare("UPDATE Patient SET SEX = ? WHERE ID = ?");
    q.addBindValue(value);
    q.addBindValue(id);
    if (!q.exec()) {
        return false;
    }
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    DbManager::instance()->logEvent("table", QString("在表格中修改 %1 的性别").arg(id));
    return true;
}

PatientWidget::PatientWidget(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:#f5f7fa;");

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setObjectName("searchEdit");
    m_searchEdit->setPlaceholderText("输入编号 / 姓名 / 身份证 / 手机号");
    m_searchEdit->setMinimumWidth(260);
    auto *searchBtn = new QPushButton("查找", this);
    searchBtn->setObjectName("searchBtn");
    auto *allBtn = new QPushButton("显示全部", this);
    allBtn->setObjectName("allBtn");
    auto *addBtn = new QPushButton("添加", this);
    addBtn->setObjectName("addBtn");
    auto *editBtn = new QPushButton("修改", this);
    editBtn->setObjectName("editBtn");
    auto *deleteBtn = new QPushButton("删除", this);
    deleteBtn->setObjectName("deleteBtn");
    deleteBtn->setStyleSheet("color:#c0392b;");

    auto *bar = new QHBoxLayout();
    bar->addWidget(m_searchEdit);
    bar->addWidget(searchBtn);
    bar->addWidget(allBtn);
    bar->addStretch();
    bar->addWidget(addBtn);
    bar->addWidget(editBtn);
    bar->addWidget(deleteBtn);

    m_model = new PatientQueryModel(this);
    m_view = new QTableView(this);
    m_view->setObjectName("patientView");
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->verticalHeader()->setVisible(false);
    m_view->setAlternatingRowColors(true);
    m_view->setItemDelegateForColumn(3, new GenderDelegate(this));

    m_status = new QLabel(this);
    m_status->setMinimumHeight(22);
    m_countLabel = new QLabel(this);
    m_countLabel->setAlignment(Qt::AlignRight);

    auto *statusRow = new QHBoxLayout();
    statusRow->addWidget(m_status);
    statusRow->addStretch();
    statusRow->addWidget(m_countLabel);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(bar);
    layout->addLayout(statusRow);
    layout->addWidget(m_view);
    setLayout(layout);

    connect(searchBtn, &QPushButton::clicked, this, &PatientWidget::search);
    connect(allBtn, &QPushButton::clicked, this, &PatientWidget::showAll);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &PatientWidget::search);
    connect(addBtn, &QPushButton::clicked, this, &PatientWidget::addPatient);
    connect(editBtn, &QPushButton::clicked, this, &PatientWidget::editPatient);
    connect(deleteBtn, &QPushButton::clicked, this, &PatientWidget::deletePatient);
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PatientWidget::onSelectionChanged);

    reload();
}

void PatientWidget::applyQuery(const QString &where)
{
    const QString sql =
        "SELECT ID, ID_CARD, NAME, SEX, DOB, HEIGHT, WEIGHT, MOBILEPHONE, AGE, CREATEDTIMESTAMP "
        "FROM Patient " + where + " ORDER BY ID";
    m_model->setQuery(sql, QSqlDatabase::database("hospital_connection"));
    if (m_model->lastError().isValid()) {
        m_status->setText("查询失败：" + m_model->lastError().text());
    } else if (m_model->rowCount() == 0) {
        m_status->setText("未找到相关记录，请检查查询条件");
    } else {
        m_status->clear();
    }

    const QStringList headers = {"编号", "身份证号", "姓名", "性别", "出生日期",
                                 "身高(cm)", "体重(kg)", "手机号", "年龄", "创建时间"};
    for (int i = 0; i < headers.size(); ++i) {
        m_model->setHeaderData(i, Qt::Horizontal, headers.at(i));
    }
    m_countLabel->setText(QString("共 %1 条记录").arg(m_model->rowCount()));
    onSelectionChanged();
}

void PatientWidget::reload()
{
    applyQuery("");
}

void PatientWidget::search()
{
    const QString key = m_searchEdit->text().trimmed().replace("'", "''");
    if (key.isEmpty()) {
        reload();
        return;
    }
    const QString where = QString("WHERE ID LIKE '%%1%' OR NAME LIKE '%%1%' "
                                  "OR ID_CARD LIKE '%%1%' OR MOBILEPHONE LIKE '%%1%'").arg(key);
    applyQuery(where);
    DbManager::instance()->logEvent(m_currentUser, QString("查询患者：%1").arg(key));
}

void PatientWidget::showAll()
{
    m_searchEdit->clear();
    reload();
}

void PatientWidget::addPatient()
{
    PatientDialog dlg(false, this);
    if (dlg.exec() != QDialog::Accepted) return;
    DbManager::instance()->logEvent(m_currentUser, "新增患者 " + dlg.values().value(0));
    reload();
    m_status->setText("添加成功，表格已刷新");
}

void PatientWidget::editPatient()
{
    const QStringList v = selectedValues();
    if (v.isEmpty()) {
        QMessageBox::information(this, "修改患者", "请先选中一行记录");
        return;
    }
    PatientDialog dlg(true, this);
    dlg.load(v);
    if (dlg.exec() != QDialog::Accepted) return;
    DbManager::instance()->logEvent(m_currentUser, "修改患者 " + v.value(0));
    reload();
    m_status->setText("修改成功，表格已刷新");
}

void PatientWidget::deletePatient()
{
    const QStringList v = selectedValues();
    if (v.isEmpty()) {
        QMessageBox::information(this, "删除患者", "请先选中一行记录");
        return;
    }
    const int answer = QMessageBox::question(
        this, "确认删除",
        QString("确定要删除患者【%1】（编号 %2）吗？此操作不可撤销。")
            .arg(v.value(2), v.value(0)),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;

    QString error;
    if (!DbManager::instance()->deletePatient(v.value(0), &error)) {
        QMessageBox::critical(this, "删除失败", error);
        return;
    }
    DbManager::instance()->logEvent(m_currentUser, "删除患者 " + v.value(0));
    reload();
    m_status->setText("已删除，表格已刷新");
}

void PatientWidget::onSelectionChanged()
{
    const bool hasRow = m_view->selectionModel()->hasSelection();
    for (QPushButton *btn : findChildren<QPushButton *>()) {
        if (btn->text() == "修改" || btn->text() == "删除") {
            btn->setEnabled(hasRow);
        }
    }
}

QStringList PatientWidget::selectedValues() const
{
    const QModelIndexList rows = m_view->selectionModel()->selectedRows();
    if (rows.isEmpty()) return {};
    QStringList v;
    for (int c = 0; c < 10; ++c) {
        v << m_model->index(rows.first().row(), c).data().toString();
    }
    return v;
}
