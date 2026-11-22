#include "constructor.h"
#include "UserDialog.h"
#include "statsdatabase.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("Best-studio", "ConstructorApp");
    QString username = settings.value("current_user").toString();

    // Если имени нет (первый запуск) — показываем диалог выбора
    if (username.isEmpty()) {
        // Временная база, чтобы получить список пользователей
        StatsDatabase tempDb;
        QString dbPath = "\\\\Server\\Shared\\ConstructorDB\\shared_workstats.db"; // Ваш путь
        tempDb.init(dbPath);

        UserDialog dialog(&tempDb, "");
        if (dialog.exec() == QDialog::Accepted) {
            username = dialog.selectedUser();
            if (username.isEmpty()) return 0;
            settings.setValue("current_user", username);
        } else {
            return 0;
        }
    }

    Constructor w(username);
    w.show();
    return QApplication::exec();
}