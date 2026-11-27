#include "demorunner.h"

#include "mainwindow.h"

#include <QAbstractButton>
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QTextStream>
#include <QTimer>

DemoRunner::DemoRunner(MainWindow *window, const QString &logDir, int intervalMs,
                       bool saveShots, int mode, QObject *parent)
    : QObject(parent), m_window(window), m_dir(logDir),
      m_interval(intervalMs), m_shots(saveShots), m_mode(mode)
{
}

void DemoRunner::log(const QString &text)
{
    QFile f(m_dir + "/demo.log");
    if (f.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&f);
        out << QDateTime::currentDateTime().toString("HH:mm:ss.zzz") << "  " << text << "\n";
    }
}

void DemoRunner::shot(const QString &name)
{
    if (!m_shots) return;
    QWidget *w = QApplication::activeModalWidget();
    if (!w) w = m_window;
    w->grab().save(m_dir + "/" + name + ".png");
}

QWidget *DemoRunner::findWidget(const QString &objectName)
{
    QWidget *modal = QApplication::activeModalWidget();
    if (modal) {
        if (auto *w = modal->findChild<QWidget *>(objectName, Qt::FindChildrenRecursively)) {
            return w;
        }
    }
    return m_window->findChild<QWidget *>(objectName, Qt::FindChildrenRecursively);
}

void DemoRunner::click(const QString &objectName)
{
    if (auto *btn = qobject_cast<QAbstractButton *>(findWidget(objectName))) {
        QMetaObject::invokeMethod(btn, "click", Qt::QueuedConnection);
    }
}

void DemoRunner::clickNow(const QString &objectName)
{
    if (auto *btn = qobject_cast<QAbstractButton *>(findWidget(objectName))) {
        btn->click();
    }
}

void DemoRunner::setText(const QString &objectName, const QString &text)
{
    if (auto *edit = qobject_cast<QLineEdit *>(findWidget(objectName))) {
        edit->setText(text);
    }
}

void DemoRunner::selectRow(int row)
{
    if (auto *view = qobject_cast<QTableView *>(findWidget("patientView"))) {
        view->selectRow(row);
    }
}

void DemoRunner::start()
{
    log(m_mode == 1 ? "数据校验演示开始" : "演示开始");
    QTimer::singleShot(1200, this, [this]() { step(m_mode == 1 ? 100 : 0); });
}

void DemoRunner::step(int n)
{
    m_step = n;
    switch (n) {
    // 登录：先演示错误凭据，再演示正确凭据
    case 0:
        setText("userEdit", "admin");
        setText("passEdit", "wrong123");
        log("输入错误密码 wrong123");
        click("loginBtn");
        break;
    case 1: break; // 等待按钮动作完成
    case 2:
        shot("01_login_wrong");
        setText("passEdit", "123456");
        log("输入正确密码 123456 并登录");
        click("loginBtn");
        break;
    case 3: break;
    case 4:
        shot("02_welcome");
        log("进入患者管理，显示全部记录");
        click("patientBtn");
        break;
    case 5: break;
    // 查询
    case 6:
        shot("03_patient_list");
        setText("searchEdit", "王");
        click("searchBtn");
        log("查询姓名含 王 的患者");
        break;
    case 7: break;
    case 8:
        shot("04_search_hit");
        setText("searchEdit", "不存在的人");
        click("searchBtn");
        log("查询无结果场景");
        break;
    case 9: break;
    // 新增
    case 10:
        shot("05_search_empty");
        click("allBtn");
        click("addBtn");
        log("打开新增对话框");
        break;
    case 11: break;
    case 12:
        setText("dlgId", "P099");
        setText("dlgName", "测试患者");
        setText("dlgIdCard", "441900200001010099");
        setText("dlgMobile", "13900139099");
        shot("06_add_dialog");
        log("填写必填字段并保存新增记录 P099");
        clickNow("dlgSave");
        break;
    case 13: break;
    // 修改
    case 14:
        shot("07_after_add");
        selectRow(3);
        click("editBtn");
        log("选中第 4 行，打开修改对话框");
        break;
    case 15: break;
    case 16:
        setText("dlgName", "测试患者已修改");
        shot("08_edit_dialog");
        log("修改姓名并保存");
        clickNow("dlgSave");
        break;
    case 17: break;
    // 删除确认
    case 18:
        shot("09_after_edit");
        selectRow(3);
        click("deleteBtn");
        log("点击删除，弹出确认框");
        break;
    case 19: break;
    case 20:
        shot("10_delete_confirm");
        if (auto *dlg = qobject_cast<QMessageBox *>(QApplication::activeModalWidget())) {
            if (auto *yes = dlg->button(QMessageBox::Yes)) yes->click();
        }
        log("确认删除 P099");
        break;
    case 21: break;
    // 操作记录与选做页面
    case 22:
        shot("11_after_delete");
        QMetaObject::invokeMethod(m_window, "onNavigate", Qt::QueuedConnection, Q_ARG(int, 3));
        log("查看 History 操作记录");
        break;
    case 23: break;
    case 24:
        shot("12_history");
        QMetaObject::invokeMethod(m_window, "onNavigate", Qt::QueuedConnection, Q_ARG(int, 4));
        log("查看科室管理（选做）");
        break;
    case 25: break;
    case 26:
        shot("13_department");
        QMetaObject::invokeMethod(m_window, "onNavigate", Qt::QueuedConnection, Q_ARG(int, 5));
        log("查看医生管理（选做）");
        break;
    case 27: break;
    case 28:
        shot("14_doctor");
        QMetaObject::invokeMethod(m_window, "onNavigate", Qt::QueuedConnection, Q_ARG(int, 1));
        log("返回欢迎页");
        break;
    case 29: break;
    case 30:
        shot("15_back_home");
        log("演示结束");
        return;

    // 数据校验演示：重复身份证 + 必填字段缺失
    case 100:
        setText("userEdit", "admin");
        setText("passEdit", "123456");
        click("loginBtn");
        break;
    case 101: break;
    case 102:
        click("patientBtn");
        break;
    case 103: break;
    case 104:
        click("addBtn");
        break;
    case 105: break;
    case 106:
        setText("dlgId", "P100");
        setText("dlgName", "重复身份证测试");
        setText("dlgIdCard", "441900199001010011"); // 与 P001 重复
        setText("dlgMobile", "13800138100");
        shot("17_dup_before");
        log("提交与 P001 重复的身份证号");
        clickNow("dlgSave");
        break;
    case 107: break;
    case 108:
        shot("18_dup_error");
        log("界面提示身份证号已存在");
        setText("dlgName", "");
        setText("dlgIdCard", "");
        break;
    case 109:
        shot("19_before_missing");
        log("清空姓名和身份证后再次提交");
        clickNow("dlgSave");
        break;
    case 110: break;
    case 111:
        shot("20_missing_error");
        log("界面提示必填字段不能为空");
        clickNow("dlgCancel");
        break;
    case 112:
        log("数据校验演示结束");
        return;
    default:
        return;
    }
    QTimer::singleShot(m_interval, this, [this]() { step(m_step + 1); });
}
