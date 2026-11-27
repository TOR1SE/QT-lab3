#include "welcomewidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

WelcomeWidget::WelcomeWidget(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background:#f5f7fa;");

    auto *title = new QLabel("欢迎使用医院诊疗信息管理系统", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:24px;font-weight:bold;color:#2c3e50;");

    auto *info = new QLabel("学号：2024414300214    姓名：林苗苗", this);
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("color:#7f8c8d;font-size:12px;");

    const QString cardStyle = QString(
        "QPushButton{background:white;border:1px solid #dfe4ea;border-radius:8px;"
        "font-size:17px;color:#2c3e50;min-height:120px;}"
        "QPushButton:hover{background:#eaf3ff;border-color:#2d8cf0;color:#2d8cf0;}");

    auto *deptBtn = new QPushButton("科室管理", this);
    auto *doctorBtn = new QPushButton("医生管理", this);
    auto *patientBtn = new QPushButton("患者管理", this);
    patientBtn->setObjectName("patientBtn");
    auto *historyBtn = new QPushButton("操作记录", this);
    for (auto *btn : {deptBtn, doctorBtn, patientBtn, historyBtn}) {
        btn->setStyleSheet(cardStyle);
    }

    auto *grid = new QGridLayout();
    grid->setSpacing(24);
    grid->addWidget(deptBtn, 0, 0);
    grid->addWidget(doctorBtn, 0, 1);
    grid->addWidget(patientBtn, 1, 0);
    grid->addWidget(historyBtn, 1, 1);

    auto *layout = new QVBoxLayout(this);
    layout->addStretch(1);
    layout->addWidget(title);
    layout->addWidget(info);
    layout->addStretch(1);
    layout->addLayout(grid);
    layout->addStretch(1);
    setLayout(layout);

    connect(deptBtn, &QPushButton::clicked, this, [this]() { emit navigate(4); });
    connect(doctorBtn, &QPushButton::clicked, this, [this]() { emit navigate(5); });
    connect(patientBtn, &QPushButton::clicked, this, [this]() { emit navigate(2); });
    connect(historyBtn, &QPushButton::clicked, this, [this]() { emit navigate(3); });
}
