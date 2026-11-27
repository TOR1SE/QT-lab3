#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariant>

// 数据层：只负责数据库连接、SQL 执行和记录操作，不依赖任何界面类
class DbManager : public QObject
{
    Q_OBJECT
public:
    static DbManager *instance();

    bool open(QString *error = nullptr);
    bool isOpen() const;
    QString databasePath() const;

    bool authenticate(const QString &username, const QString &password,
                      QString *message = nullptr, QString *fullName = nullptr);
    bool addUser(const QString &username, const QString &password,
                 const QString &fullName, QString *error = nullptr);

    bool recordExists(const QString &idCard, QString *error = nullptr);
    bool idCardExistsExcept(const QString &idCard, const QString &ownId,
                            QString *error = nullptr);
    bool idExists(const QString &id, QString *error = nullptr);
    bool userExists(const QString &username, QString *error = nullptr);
    bool insertPatient(const QVariantList &values, QString *error = nullptr);
    bool updatePatient(const QString &id, const QVariantList &values, QString *error = nullptr);
    bool deletePatient(const QString &id, QString *error = nullptr);

    bool logEvent(const QString &user, const QString &event);

private:
    explicit DbManager(QObject *parent = nullptr);
    bool exec(const QString &sql, const QVariantList &params, QString *error = nullptr);
    void createSchema();

    QSqlDatabase m_db;
    bool m_ready = false;
};

#endif // DBMANAGER_H
