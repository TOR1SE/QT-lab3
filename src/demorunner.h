#ifndef DEMORUNNER_H
#define DEMORUNNER_H

#include <QObject>
#include <QString>

class MainWindow;

// 自动演示：驱动界面完成登录/增删改查/异常场景，并记录时间戳、导出截图
class DemoRunner : public QObject
{
    Q_OBJECT
public:
    DemoRunner(MainWindow *window, const QString &logDir, int intervalMs,
               bool saveShots, int mode = 0, QObject *parent = nullptr);
    void start();

private:
    void step(int n);
    void log(const QString &text);
    void shot(const QString &name);
    QWidget *findWidget(const QString &objectName);
    void click(const QString &objectName);       // 可能打开模态窗口，排队执行
    void clickNow(const QString &objectName);    // 立即执行
    void setText(const QString &objectName, const QString &text);
    void selectRow(int row);

    MainWindow *m_window;
    QString m_dir;
    int m_interval;
    bool m_shots;
    int m_mode;
    int m_step = 0;
};

#endif // DEMORUNNER_H
