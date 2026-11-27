#include "dbmanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDateTime>

DbManager::DbManager(QObject *parent) : QObject(parent)
{
}

DbManager *DbManager::instance()
{
    static DbManager self;
    return &self;
}

QString DbManager::databasePath() const
{
    // 测试数据库连接失败时，可用环境变量把路径指到不存在的盘符
    if (qEnvironmentVariableIsSet("QT3_BAD_DB")) {
        return "Z:/no_such_dir/hospital.db";
    }
    // 数据库放在 exe 旁边，方便交作业时直接看到数据文件
    return QCoreApplication::applicationDirPath() + "/hospital.db";
}

bool DbManager::open(QString *error)
{
    if (m_ready && m_db.isOpen()) {
        return true;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", "hospital_connection");
    m_db.setDatabaseName(databasePath());

    if (!m_db.open()) {
        if (error) {
            *error = QString("无法打开数据库：%1\n路径：%2")
                         .arg(m_db.lastError().text(), databasePath());
        }
        return false;
    }

    createSchema();
    m_ready = true;
    return true;
}

bool DbManager::isOpen() const
{
    return m_ready && m_db.isOpen();
}

void DbManager::createSchema()
{
    const QStringList ddl = {
        "CREATE TABLE IF NOT EXISTS User ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "FULLNAME TEXT NOT NULL,"
        "USERNAME TEXT NOT NULL UNIQUE,"
        "PASSWORD TEXT NOT NULL)",
        "CREATE TABLE IF NOT EXISTS Department ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "NAME TEXT NOT NULL UNIQUE)",
        "CREATE TABLE IF NOT EXISTS Doctor ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "EMPLOYEENO TEXT NOT NULL UNIQUE,"
        "NAME TEXT NOT NULL,"
        "DEPARTMENT_ID INTEGER)",
        "CREATE TABLE IF NOT EXISTS Patient ("
        "ID TEXT PRIMARY KEY,"
        "ID_CARD TEXT NOT NULL UNIQUE,"
        "NAME TEXT NOT NULL,"
        "SEX INTEGER,"
        "DOB TEXT,"
        "HEIGHT REAL,"
        "WEIGHT REAL,"
        "MOBILEPHONE TEXT,"
        "AGE INTEGER,"
        "CREATEDTIMESTAMP TEXT NOT NULL)",
        "CREATE TABLE IF NOT EXISTS History ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "USER_ID TEXT,"
        "EVENT TEXT NOT NULL,"
        "TIMESTAMP TEXT NOT NULL)"
    };

    for (const QString &sql : ddl) {
        QSqlQuery q(m_db);
        q.exec(sql);
    }

    // 首次运行时写入登录账号和少量基础数据，便于直接验证功能
    QSqlQuery count(m_db);
    count.exec("SELECT COUNT(*) FROM User");
    if (count.next() && count.value(0).toInt() == 0) {
        exec("INSERT INTO User(FULLNAME,USERNAME,PASSWORD) VALUES(?,?,?)",
             {"系统管理员", "admin", "123456"});
        exec("INSERT INTO User(FULLNAME,USERNAME,PASSWORD) VALUES(?,?,?)",
             {"张医生", "doctor", "123456"});
    }

    QSqlQuery deptCount(m_db);
    deptCount.exec("SELECT COUNT(*) FROM Department");
    if (deptCount.next() && deptCount.value(0).toInt() == 0) {
        exec("INSERT INTO Department(NAME) VALUES(?)", {"内科"});
        exec("INSERT INTO Department(NAME) VALUES(?)", {"外科"});
        exec("INSERT INTO Department(NAME) VALUES(?)", {"儿科"});
    }

    QSqlQuery patientCount(m_db);
    patientCount.exec("SELECT COUNT(*) FROM Patient");
    if (patientCount.next() && patientCount.value(0).toInt() == 0) {
        const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        exec("INSERT INTO Patient VALUES(?,?,?,?,?,?,?,?,?,?)",
             {"P001", "441900199001010011", "陈晓明", 1, "1990-01-01", 172.5, 68.0,
              "13800138001", 36, now});
        exec("INSERT INTO Patient VALUES(?,?,?,?,?,?,?,?,?,?)",
             {"P002", "441900198505120022", "李秀英", 2, "1985-05-12", 160.0, 55.5,
              "13800138002", 41, now});
        exec("INSERT INTO Patient VALUES(?,?,?,?,?,?,?,?,?,?)",
             {"P003", "441900199808200033", "王建国", 1, "1998-08-20", 175.0, 70.0,
              "13800138003", 28, now});
    }
}

bool DbManager::exec(const QString &sql, const QVariantList &params, QString *error)
{
    QSqlQuery q(m_db);
    if (!q.prepare(sql)) {
        if (error) *error = q.lastError().text();
        return false;
    }
    for (const QVariant &v : params) {
        q.addBindValue(v);
    }
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::authenticate(const QString &username, const QString &password,
                             QString *message, QString *fullName)
{
    if (!isOpen()) {
        if (message) *message = "数据库尚未连接，无法登录";
        return false;
    }
    QSqlQuery q(m_db);
    q.prepare("SELECT PASSWORD, FULLNAME FROM User WHERE USERNAME = ?");
    q.addBindValue(username);
    if (!q.exec()) {
        if (message) *message = q.lastError().text();
        return false;
    }
    if (!q.next()) {
        if (message) *message = "用户不存在，请检查用户名";
        return false;
    }
    if (q.value(0).toString() != password) {
        if (message) *message = "密码错误，请重新输入";
        return false;
    }
    if (fullName) *fullName = q.value(1).toString();
    return true;
}

bool DbManager::userExists(const QString &username, QString *error)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM User WHERE USERNAME = ?");
    q.addBindValue(username);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return q.next() && q.value(0).toInt() > 0;
}

bool DbManager::addUser(const QString &username, const QString &password,
                        const QString &fullName, QString *error)
{
    if (userExists(username, error)) {
        if (error) *error = "用户名已存在";
        return false;
    }
    return exec("INSERT INTO User(FULLNAME,USERNAME,PASSWORD) VALUES(?,?,?)",
                {fullName, username, password}, error);
}

bool DbManager::recordExists(const QString &idCard, QString *error)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM Patient WHERE ID_CARD = ?");
    q.addBindValue(idCard);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return q.next() && q.value(0).toInt() > 0;
}

bool DbManager::idExists(const QString &id, QString *error)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM Patient WHERE ID = ?");
    q.addBindValue(id);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return q.next() && q.value(0).toInt() > 0;
}

bool DbManager::idCardExistsExcept(const QString &idCard, const QString &ownId, QString *error)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT ID FROM Patient WHERE ID_CARD = ?");
    q.addBindValue(idCard);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    while (q.next()) {
        if (q.value(0).toString() != ownId) {
            return true;
        }
    }
    return false;
}

bool DbManager::insertPatient(const QVariantList &values, QString *error)
{
    return exec("INSERT INTO Patient VALUES(?,?,?,?,?,?,?,?,?,?)", values, error);
}

bool DbManager::updatePatient(const QString &id, const QVariantList &values, QString *error)
{
    QVariantList params = values;
    params << id;
    return exec("UPDATE Patient SET ID=?,ID_CARD=?,NAME=?,SEX=?,DOB=?,HEIGHT=?,WEIGHT=?,"
                "MOBILEPHONE=?,AGE=?,CREATEDTIMESTAMP=? WHERE ID=?", params, error);
}

bool DbManager::deletePatient(const QString &id, QString *error)
{
    return exec("DELETE FROM Patient WHERE ID = ?", {id}, error);
}

bool DbManager::logEvent(const QString &user, const QString &event)
{
    if (!isOpen()) return false;
    return exec("INSERT INTO History(USER_ID,EVENT,TIMESTAMP) VALUES(?,?,?)",
                {user, event, QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")});
}
