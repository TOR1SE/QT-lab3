#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class QDateEdit;
class QSpinBox;
class QLabel;

// 新增 / 修改患者共用的编辑对话框
class PatientDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PatientDialog(bool editing, QWidget *parent = nullptr);

    void load(const QStringList &values);
    QStringList values() const;

private slots:
    void onDateChanged();
    void accept() override;

private:
    bool m_editing;
    QString m_originalId;
    QLineEdit *m_id;
    QLineEdit *m_name;
    QLineEdit *m_idCard;
    QComboBox *m_sex;
    QDateEdit *m_dob;
    QLineEdit *m_height;
    QLineEdit *m_weight;
    QLineEdit *m_mobile;
    QSpinBox *m_age;
    QLabel *m_error;
};

#endif // PATIENTDIALOG_H
