#include "mainwindow.h"
#include "demorunner.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("医院诊疗信息管理系统");

    const QStringList args = a.arguments();

    // 数据库连接失败截图模式：把数据库路径指向无效位置并自动截下错误提示
    if (args.contains("--dbfail")) {
        qputenv("QT3_BAD_DB", "1");
        const int pos = args.indexOf("--out");
        const QString outPath = (pos >= 0 && pos + 1 < args.size())
                                    ? args.at(pos + 1)
                                    : "C:/Windows/Temp/db_fail.png";
        QTimer::singleShot(1200, [outPath, &a]() {
            if (auto *dlg = qobject_cast<QMessageBox *>(QApplication::activeModalWidget())) {
                dlg->grab().save(outPath);
                dlg->accept();
            }
            QTimer::singleShot(300, &a, &QCoreApplication::quit);
        });
    }

    MainWindow w;
    w.show();

    // 自动演示模式：--demo [--dir 目录] [--interval 毫秒] [--shots]
    // 仅用于录制实验视频和导出截图，普通运行时不会触发。
    if (args.contains("--demo")) {
        QString dir = "C:/Windows/Temp/qt3_demo";
        int interval = 2500;
        const int dirPos = args.indexOf("--dir");
        if (dirPos >= 0 && dirPos + 1 < args.size()) {
            dir = args.at(dirPos + 1);
        }
        const int intervalPos = args.indexOf("--interval");
        if (intervalPos >= 0 && intervalPos + 1 < args.size()) {
            interval = args.at(intervalPos + 1).toInt();
        }
        QDir().mkpath(dir);
        auto *runner = new DemoRunner(&w, dir, interval,
                                      args.contains("--shots"),
                                      args.contains("--validation") ? 1 : 0, &w);
        runner->start();
    }

    return a.exec();
}
