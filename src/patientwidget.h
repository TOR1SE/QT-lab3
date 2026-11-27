#ifndef PATIENTWIDGET_H
#define PATIENTWIDGET_H

#include <QSqlQueryModel>
#include <QWidget>

class QTableView;
class QLineEdit;
class QLabel;

// 只读 QSqlQueryModel 默认不可编辑，这里只开放性别列用于演示自定义代理
class PatientQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit PatientQueryModel(QObject *parent = nullptr) : QSqlQueryModel(parent) {}

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};

class PatientWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PatientWidget(QWidget *parent = nullptr);
    void reload();
    void setCurrentUser(const QString &user) { m_currentUser = user; }

private slots:
    void search();
    void showAll();
    void addPatient();
    void editPatient();
    void deletePatient();
    void onSelectionChanged();

private:
    void applyQuery(const QString &where);
    QStringList selectedValues() const;

    PatientQueryModel *m_model;
    QTableView *m_view;
    QLineEdit *m_searchEdit;
    QLabel *m_status;
    QLabel *m_countLabel;
    QString m_currentUser;
};

#endif // PATIENTWIDGET_H
