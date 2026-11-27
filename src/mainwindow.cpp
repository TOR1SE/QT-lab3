#include "mainwindow.h"

#include "dbmanager.h"
#include "loginwidget.h"
#include "welcomewidget.h"
#include "patientwidget.h"
#include "historywidget.h"
#include "simpletablewidget.h"

#include <QAction>
#include <QMessageBox>
#include <QStackedWidget>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("医院诊疗信息管理系统  -  2024414300214 林苗苗");
    resize(960, 640);

    QString dbError;
    if (!DbManager::instance()->open(&dbError)) {
        QMessageBox::critical(this, "数据库连接失败", dbError);
    }

    m_stack = new QStackedWidget(this);
    m_stack->setObjectName("stack");
    setCentralWidget(m_stack);

    m_login = new LoginWidget(this);
    m_welcome = new WelcomeWidget(this);
    m_patients = new PatientWidget(this);
    m_history = new HistoryWidget(this);
    m_departments = new SimpleTableWidget("Department", "科室管理", this);
    m_doctors = new SimpleTableWidget("Doctor", "医生管理", this);

    m_stack->addWidget(m_login);    // 0
    m_stack->addWidget(m_welcome);  // 1
    m_stack->addWidget(m_patients); // 2
    m_stack->addWidget(m_history);  // 3
    m_stack->addWidget(m_departments); // 4
    m_stack->addWidget(m_doctors);     // 5

    connect(m_login, &LoginWidget::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(m_welcome, &WelcomeWidget::navigate, this, &MainWindow::onNavigate);
    connect(m_history, &HistoryWidget::backToWelcome, this, &MainWindow::onNavigate);
    connect(m_departments, &SimpleTableWidget::backToWelcome, this, &MainWindow::onNavigate);
    connect(m_doctors, &SimpleTableWidget::backToWelcome, this, &MainWindow::onNavigate);

    buildToolBar();
    m_stack->setCurrentWidget(m_login);
}

void MainWindow::buildToolBar()
{
    m_toolBar = addToolBar("导航");
    m_toolBar->setObjectName("mainToolBar");
    m_toolBar->setMovable(false);

    QAction *homeAct = m_toolBar->addAction("主页");
    QAction *patientAct = m_toolBar->addAction("患者管理");
    QAction *historyAct = m_toolBar->addAction("操作记录");
    QAction *logoutAct = m_toolBar->addAction("退出登录");

    connect(homeAct, &QAction::triggered, this, [this]() {
        if (!m_currentUser.isEmpty()) m_stack->setCurrentWidget(m_welcome);
    });
    connect(patientAct, &QAction::triggered, this, [this]() {
        if (!m_currentUser.isEmpty()) {
            m_patients->setCurrentUser(m_currentUser);
            m_patients->reload();
            m_stack->setCurrentWidget(m_patients);
        }
    });
    connect(historyAct, &QAction::triggered, this, [this]() {
        if (!m_currentUser.isEmpty()) {
            m_history->reload();
            m_stack->setCurrentWidget(m_history);
        }
    });
    connect(logoutAct, &QAction::triggered, this, &MainWindow::onLogout);
}

void MainWindow::onLoginSuccess(const QString &user)
{
    m_currentUser = user;
    DbManager::instance()->logEvent(user, "登录系统");
    m_stack->setCurrentWidget(m_welcome);
}

void MainWindow::onNavigate(int page)
{
    switch (page) {
    case 1: m_stack->setCurrentWidget(m_welcome); break;
    case 2:
        m_patients->setCurrentUser(m_currentUser);
        m_patients->reload();
        m_stack->setCurrentWidget(m_patients);
        break;
    case 3:
        m_history->reload();
        m_stack->setCurrentWidget(m_history);
        break;
    case 4:
        m_departments->reload();
        m_stack->setCurrentWidget(m_departments);
        break;
    case 5:
        m_doctors->reload();
        m_stack->setCurrentWidget(m_doctors);
        break;
    default: break;
    }
}

void MainWindow::onLogout()
{
    if (m_currentUser.isEmpty()) return;
    DbManager::instance()->logEvent(m_currentUser, "退出登录");
    m_currentUser.clear();
    m_stack->setCurrentWidget(m_login);
}
