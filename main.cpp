#include "constructor.h"
#include <QApplication>
#include <QInputDialog>
#include <QSettings>
#include <QLineEdit>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Открываем настройки (те же, что и в Constructor)
    QSettings settings("Best-studio", "ConstructorApp");

    // Пытаемся получить сохранённое имя пользователя
    QString username = settings.value("current_user").toString();

    // Если имя пустое (первый запуск или его стерли) — спрашиваем
    if (username.isEmpty()) {
        bool ok;
        username = QInputDialog::getText(nullptr,
                                         "Первый запуск",
                                         "Введите ваше имя (логин):",
                                         QLineEdit::Normal,
                                         "",
                                         &ok).trimmed();

        // Если нажали Отмену или ввели пустоту — закрываем программу
        if (!ok || username.isEmpty()) {
            return 0;
        }

        // Сохраняем имя для будущих запусков
        settings.setValue("current_user", username);
    }

    // Запускаем главное окно с нужным именем
    Constructor w(username);
    w.show();
    return QApplication::exec();
}