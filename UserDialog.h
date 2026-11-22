#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include "statsdatabase.h"

class UserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UserDialog(StatsDatabase *db, const QString &currentUser, QWidget *parent = nullptr);
    QString selectedUser() const;

private slots:
    void reloadList();
    void onHideUser();

private:
    StatsDatabase *m_db;
    QString m_currentUser;
    QComboBox *m_combo;
    QCheckBox *m_showHiddenCheck;
    QPushButton *m_hideButton;
};

#endif // USERDIALOG_H