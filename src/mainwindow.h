#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class LoginWidget;
class WelcomeWidget;
class PatientWidget;
class HistoryWidget;
class SimpleTableWidget;
class QToolBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoginSuccess(const QString &user);
    void onNavigate(int page);
    void onLogout();

private:
    void buildToolBar();

    QStackedWidget *m_stack;
    LoginWidget *m_login;
    WelcomeWidget *m_welcome;
    PatientWidget *m_patients;
    HistoryWidget *m_history;
    SimpleTableWidget *m_departments;
    SimpleTableWidget *m_doctors;
    QToolBar *m_toolBar;
    QString m_currentUser;
};

#endif // MAINWINDOW_H
