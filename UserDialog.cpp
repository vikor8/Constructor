#include "UserDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDialogButtonBox>

UserDialog::UserDialog(StatsDatabase *db, const QString &currentUser, QWidget *parent)
    : QDialog(parent), m_db(db), m_currentUser(currentUser)
{
    setWindowTitle("Выбор пользователя");
    setMinimumWidth(300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Выберите пользователя:"));

    m_combo = new QComboBox(this);
    layout->addWidget(m_combo);

    m_showHiddenCheck = new QCheckBox("Показывать всех", this);
    layout->addWidget(m_showHiddenCheck);

    m_hideButton = new QPushButton("Скрыть выбранного пользователя", this);
    layout->addWidget(m_hideButton);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(bb);

    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_showHiddenCheck, &QCheckBox::toggled, this, &UserDialog::reloadList);
    connect(m_hideButton, &QPushButton::clicked, this, &UserDialog::onHideUser);
    connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int /*index*/) {
        m_hideButton->setEnabled(m_combo->currentText() != m_currentUser);
    });
    reloadList();
}

void UserDialog::reloadList()
{
    m_combo->clear();
    QList<QPair<int, QString>> users = m_db->getUsers(m_showHiddenCheck->isChecked());

    // ИСПРАВЛЕНИЕ: используем std::as_const(users) вместо просто users
    for (const auto &u : std::as_const(users)) {
        m_combo->addItem(u.second, u.first);
    }

    // Выделяем текущего пользователя
    int idx = m_combo->findText(m_currentUser);
    if (idx >= 0) {
        m_combo->setCurrentIndex(idx);
    }
    m_hideButton->setEnabled(m_combo->currentText() != m_currentUser);
}

void UserDialog::onHideUser()
{
    int idx = m_combo->currentIndex();
    if (idx < 0) return;

    QString name = m_combo->currentText();
    int id = m_combo->itemData(idx).toInt();

    if (name == m_currentUser) {
        QMessageBox::warning(this, "Ошибка", "Вы не можете скрыть самого себя.");
        return;
    }

    auto reply = QMessageBox::question(this, "Подтверждение",
                                       QString("Вы уверены, что хотите скрыть пользователя '%1'?\n"
                                               "Он исчезнет из списка, но его статистика останется в базе.").arg(name),
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_db->setUserActive(id, false);
        reloadList();
    }
}

QString UserDialog::selectedUser() const
{
    return m_combo->currentText();
}