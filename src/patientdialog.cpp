#include "patientdialog.h"

#include "dbmanager.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QFormLayout>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QVBoxLayout>

PatientDialog::PatientDialog(bool editing, QWidget *parent)
    : QDialog(parent), m_editing(editing)
{
    setWindowTitle(editing ? "编辑患者信息" : "添加患者信息");
    setMinimumWidth(430);

    m_id = new QLineEdit(this);
    m_id->setObjectName("dlgId");
    m_id->setPlaceholderText("如 P004");
    m_name = new QLineEdit(this);
    m_name->setObjectName("dlgName");
    m_idCard = new QLineEdit(this);
    m_idCard->setObjectName("dlgIdCard");
    m_idCard->setMaxLength(20);
    m_sex = new QComboBox(this);
    m_sex->addItem("男", 1);
    m_sex->addItem("女", 2);
    m_dob = new QDateEdit(this);
    m_dob->setCalendarPopup(true);
    m_dob->setDisplayFormat("yyyy-MM-dd");
    m_dob->setDate(QDate(2000, 1, 1));
    m_height = new QLineEdit("170", this);
    m_height->setValidator(new QDoubleValidator(0, 300, 1, this));
    m_weight = new QLineEdit("60", this);
    m_weight->setValidator(new QDoubleValidator(0, 500, 1, this));
    m_mobile = new QLineEdit(this);
    m_mobile->setObjectName("dlgMobile");
    m_mobile->setMaxLength(11);
    m_mobile->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^1\\d{10}$"), this));
    m_age = new QSpinBox(this);
    m_age->setRange(0, 150);
    m_age->setReadOnly(true);
    m_age->setToolTip("根据出生日期自动计算");

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setSpacing(10);
    form->addRow("ID：", m_id);
    form->addRow("姓名：", m_name);
    form->addRow("身份证：", m_idCard);
    form->addRow("性别：", m_sex);
    form->addRow("出生日期：", m_dob);
    form->addRow("身高(cm)：", m_height);
    form->addRow("体重(kg)：", m_weight);
    form->addRow("手机号：", m_mobile);
    form->addRow("年龄：", m_age);

    m_error = new QLabel(this);
    m_error->setStyleSheet("color:#c0392b;");
    m_error->setMinimumHeight(20);

    auto *save = new QPushButton("保存", this);
    save->setObjectName("dlgSave");
    auto *cancel = new QPushButton("取消", this);
    cancel->setObjectName("dlgCancel");
    save->setStyleSheet("background:#2d8cf0;color:white;border-radius:4px;min-height:30px;");
    save->setDefault(true);
    auto *buttons = new QHBoxLayout();
    buttons->addStretch();
    buttons->addWidget(save);
    buttons->addWidget(cancel);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(m_error);
    layout->addLayout(buttons);
    setLayout(layout);

    connect(m_dob, &QDateEdit::dateChanged, this, &PatientDialog::onDateChanged);
    connect(save, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    onDateChanged();
}

void PatientDialog::onDateChanged()
{
    const QDate today = QDate::currentDate();
    int age = today.year() - m_dob->date().year();
    if (today < QDate(m_dob->date().year() + age, m_dob->date().month(), m_dob->date().day())) {
        --age;
    }
    m_age->setValue(qMax(0, age));
}

void PatientDialog::load(const QStringList &v)
{
    m_originalId = v.value(0);
    m_id->setText(v.value(0));
    m_name->setText(v.value(2));
    m_idCard->setText(v.value(1));
    const int sex = v.value(3, "1").toInt();
    m_sex->setCurrentIndex(m_sex->findData(sex == 2 ? 2 : 1));
    m_dob->setDate(QDate::fromString(v.value(4), "yyyy-MM-dd"));
    m_height->setText(v.value(5));
    m_weight->setText(v.value(6));
    m_mobile->setText(v.value(7));
    onDateChanged();
    if (v.size() > 8) {
        m_age->setValue(v.value(8).toInt());
    }
}

QStringList PatientDialog::values() const
{
    const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    return {
        m_id->text().trimmed(),
        m_idCard->text().trimmed(),
        m_name->text().trimmed(),
        QString::number(m_sex->currentData().toInt()),
        m_dob->date().toString("yyyy-MM-dd"),
        QString::number(m_height->text().toDouble(), 'f', 1),
        QString::number(m_weight->text().toDouble(), 'f', 1),
        m_mobile->text().trimmed(),
        QString::number(m_age->value()),
        now
    };
}

void PatientDialog::accept()
{
    const QStringList v = values();
    if (v.at(0).isEmpty() || v.at(1).isEmpty() || v.at(2).isEmpty()) {
        m_error->setText("ID、姓名和身份证号都不能为空");
        return;
    }
    if (v.at(1).length() < 15) {
        m_error->setText("身份证号位数不足，请检查");
        return;
    }
    if (!m_mobile->hasAcceptableInput()) {
        m_error->setText("手机号应为 11 位数字且以 1 开头");
        return;
    }
    if (m_dob->date() > QDate::currentDate()) {
        m_error->setText("出生日期不能晚于今天");
        return;
    }

    QString error;
    if (DbManager::instance()->idCardExistsExcept(v.at(1), m_originalId, &error)) {
        m_error->setText("该身份证号已存在，不能重复添加");
        return;
    }

    QVariantList params;
    for (const QString &s : v.mid(0, 10)) {
        params << s;
    }

    if (m_editing) {
        if (!DbManager::instance()->updatePatient(m_originalId, params, &error)) {
            m_error->setText("修改失败：" + error);
            return;
        }
    } else {
        // 新增时检查主键是否重复
        if (DbManager::instance()->idExists(v.at(0), &error)) {
            m_error->setText("该患者 ID 已存在，请换一个");
            return;
        }
        if (!DbManager::instance()->insertPatient(params, &error)) {
            m_error->setText("添加失败：" + error);
            return;
        }
    }
    QDialog::accept();
}
