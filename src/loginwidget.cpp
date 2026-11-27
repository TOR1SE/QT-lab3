#include "loginwidget.h"

#include "dbmanager.h"

#include <QFormLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:#f5f7fa;");

    auto *title = new QLabel("欢迎使用诊疗测试系统", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:26px;font-weight:bold;color:#2c3e50;margin:30px;");

    auto *sub = new QLabel("医院诊疗信息管理系统  ·  开发者：2024414300214 林苗苗", this);
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#7f8c8d;font-size:12px;");

    m_userEdit = new QLineEdit(this);
    m_userEdit->setObjectName("userEdit");
    m_userEdit->setPlaceholderText("请输入用户名");
    m_passEdit = new QLineEdit(this);
    m_passEdit->setObjectName("passEdit");
    m_passEdit->setPlaceholderText("请输入密码");
    m_passEdit->setEchoMode(QLineEdit::Password);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(14);
    form->addRow("用户名：", m_userEdit);
    form->addRow("密码：", m_passEdit);

    auto *loginBtn = new QPushButton("登录", this);
    loginBtn->setObjectName("loginBtn");
    auto *registerBtn = new QPushButton("注册", this);
    loginBtn->setMinimumHeight(34);
    registerBtn->setMinimumHeight(34);
    loginBtn->setStyleSheet("QPushButton{background:#2d8cf0;color:white;border-radius:4px;font-weight:bold;}"
                            "QPushButton:hover{background:#2478d2;}");
    registerBtn->setStyleSheet("QPushButton{background:white;border:1px solid #c8ccd4;border-radius:4px;}"
                               "QPushButton:hover{background:#eef3f8;}");

    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(registerBtn);
    btnRow->addWidget(loginBtn);
    btnRow->addStretch();

    m_status = new QLabel(this);
    m_status->setAlignment(Qt::AlignCenter);
    m_status->setMinimumHeight(22);
    m_status->setStyleSheet("color:#c0392b;");

    auto *hint = new QLabel("演示账号：admin / 123456", this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("color:#95a5a6;font-size:12px;");

    auto *layout = new QVBoxLayout(this);
    layout->addStretch(2);
    layout->addWidget(title);
    layout->addWidget(sub);
    layout->addStretch(1);
    layout->addLayout(form);
    layout->addSpacing(12);
    layout->addLayout(btnRow);
    layout->addSpacing(8);
    layout->addWidget(m_status);
    layout->addWidget(hint);
    layout->addStretch(2);
    setLayout(layout);

    if (!DbManager::instance()->isOpen()) {
        m_status->setText("数据库连接失败，请检查 hospital.db 是否存在");
        loginBtn->setEnabled(false);
        registerBtn->setEnabled(false);
    }

    connect(loginBtn, &QPushButton::clicked, this, &LoginWidget::doLogin);
    connect(registerBtn, &QPushButton::clicked, this, &LoginWidget::doRegister);
    connect(m_passEdit, &QLineEdit::returnPressed, this, &LoginWidget::doLogin);
}

void LoginWidget::doLogin()
{
    const QString user = m_userEdit->text().trimmed();
    const QString pass = m_passEdit->text();

    if (user.isEmpty() || pass.isEmpty()) {
        m_status->setText("用户名和密码不能为空");
        return;
    }

    QString msg;
    QString name;
    if (DbManager::instance()->authenticate(user, pass, &msg, &name)) {
        m_status->clear();
        emit loginSuccess(user);
    } else {
        m_status->setText(msg);
    }
}

void LoginWidget::doRegister()
{
    bool ok = false;
    const QString user = QInputDialog::getText(this, "注册用户", "用户名：", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || user.isEmpty()) return;

    const QString name = QInputDialog::getText(this, "注册用户", "姓名：", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || name.isEmpty()) return;

    const QString pass = QInputDialog::getText(this, "注册用户", "密码：", QLineEdit::Password, "", &ok);
    if (!ok || pass.isEmpty()) return;

    QString error;
    if (!DbManager::instance()->addUser(user, pass, name, &error)) {
        QMessageBox::warning(this, "注册失败", error);
        return;
    }
    DbManager::instance()->logEvent(user, "注册新用户");
    m_status->setText("注册成功，请登录");
}
