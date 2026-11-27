#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

class QLineEdit;
class QLabel;

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);

signals:
    void loginSuccess(const QString &username);

private slots:
    void doLogin();
    void doRegister();

private:
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
    QLabel *m_status;
};

#endif // LOGINWIDGET_H
