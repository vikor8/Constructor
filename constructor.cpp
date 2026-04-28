#include "constructor.h"
#include "ui_constructor.h"
#include <QToolBar>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QDebug>
#include <QRegularExpression>
#include <QDateTime>
#include <QInputDialog>
#include <QProgressDialog>
#include <QThread>
#include <QFileInfo>
#include <QDirIterator>
#include <QRegularExpression>

Constructor::Constructor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Constructor)
    , m_settings("Best-studio", "ConstructorApp")
    , m_orderLineEdit(nullptr)
    , m_productLineEdit(nullptr)
{
    ui->setupUi(this);
    // Загружаем сохраненные настройки
    loadSettings();

    //Создаем менюбар
    QMenuBar *menuBar = new QMenuBar(this);

    // Создаем меню "Файл"
    QMenu *fileMenu = menuBar->addMenu("Файл");
    QAction *toFolderAction = fileMenu->addAction("Папка ТО");
    QAction *drawingsFolderAction = fileMenu->addAction("Папка Чертежи");
    QAction *advertisingFolderAction = fileMenu->addAction("Реклама");
    QAction *glassFolderAction = fileMenu->addAction("Цех стекла");
    QAction *databaseFolderAction = fileMenu->addAction("База данных");
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Выход");

    // Подключаем сигналы к слотам
    connect(toFolderAction, &QAction::triggered, this, &Constructor::selectTOFolder);
    connect(drawingsFolderAction, &QAction::triggered, this, &Constructor::selectDrawingsFolder);
    connect(advertisingFolderAction, &QAction::triggered, this, &Constructor::selectAdvertisingFolder);
    connect(glassFolderAction, &QAction::triggered, this, &Constructor::selectGlassFolder);
    connect(databaseFolderAction, &QAction::triggered, this, &Constructor::selectDatabaseFolder);
    connect(exitAction, &QAction::triggered, this, &Constructor::exitApplication);

    // Устанавливаем менюбар для главного окна
    setMenuBar(menuBar);

    // Создаем первый тулбар
    QToolBar *tbar = new QToolBar("ToolBar", this);
    tbar->setAllowedAreas(Qt::LeftToolBarArea);
    tbar->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, tbar);

    // Создаем и добавляем QLineEdit
    m_orderLineEdit = new QLineEdit(tbar);
    m_orderLineEdit->setPlaceholderText("Заказ");
    m_orderLineEdit->setMaximumWidth(70);
    tbar->addWidget(m_orderLineEdit);

    // Добавляем кнопку заказ
    QAction *actionOrder = tbar->addAction("Заказ");
    QAction *actionSketch = tbar->addAction("Эскизы");
    QAction *actionDrawTO = tbar->addAction("Чертежи в ТО");
    QAction *actionDraw = tbar->addAction("Чертежи");
    QAction *actionCompare = tbar->addAction("Сравнить с ТО");

    // Подключаем кнопку "Заказ"
    connect(actionOrder, &QAction::triggered, this, &Constructor::openOrderFolder);
    connect(actionSketch, &QAction::triggered, this, &Constructor::openSketchesFolder);
    connect(actionDrawTO, &QAction::triggered, this, &Constructor::openDrawingsInTOFolder);
    connect(actionDraw, &QAction::triggered, this, &Constructor::openDrawingsFolder);

//     Для остальных кнопок пока заглушки (можно добавить позже)

//    connect(actionTZ, &QAction::triggered, [this]() {
//        QMessageBox::information(this, "Информация", "Функция в разработке");
//    });

    connect(actionCompare, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });

    // Создаем второй тулбар
    QToolBar *tbar2 = new QToolBar("ToolBar2", this);
    tbar2->setAllowedAreas(Qt::LeftToolBarArea);
    tbar2->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, tbar2);

    // Создаем и добавляем QLineEdit
    m_productLineEdit = new QLineEdit(tbar2);
    m_productLineEdit->setPlaceholderText("№ Изделия");
    m_productLineEdit->setMaximumWidth(70);
    tbar2->addWidget(m_productLineEdit);

    // Добавляем кнопку изделие
    QAction *actionProduction = tbar2->addAction("Изделие");
    QAction *actionTO = tbar2->addAction("ТО");
    QAction *actionTOMove = tbar2->addAction("->ТО");

    // Подключаем кнопки второго тулбара (заглушки)
    connect(actionProduction, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
    connect(actionTO, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
    connect(actionTOMove, &QAction::triggered, this, &Constructor::copyToTOFolder);
}

Constructor::~Constructor()
{
    saveSettings();
    delete ui;
}

// Валидация ввода номера заказа
bool Constructor::validateOrderInput(const QString& orderNumber)
{
    if (orderNumber.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите номер заказа!");
        return false;
    }

    if (m_toFolder.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите папку ТО в меню 'Файл'!");
        return false;
    }

    return true;
}

// Открытие папки заказа/
void Constructor::openOrderFolder()
{
    // Получаем номер заказа из поля ввода
    QString orderNumber = m_orderLineEdit->text().trimmed();

    // Валидация ввода
    if (!validateOrderInput(orderNumber)) {
        return;
    }

    // Проверяем существует ли директория ТО
    QDir toDir(m_toFolder);
    if (!toDir.exists()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Папка ТО не существует:\n%1\n\nПожалуйста, выберите правильную папку в меню 'Файл'.")
                            .arg(m_toFolder));
        return;
    }

    // Формируем шаблоны для поиска
    QString searchPattern1 = "№ " + orderNumber;           // "№ 123"
    QString searchPattern2 = "№" + orderNumber;            // "№123"
    QString searchPattern3 = orderNumber;                  // "123"

    // Получаем список всех папок
    QStringList allFolders = toDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Ищем папки, соответствующие любому из шаблонов
    QStringList foundFolders;

    for (const QString& folderName : allFolders) {
        // Проверяем точное соответствие
        if (folderName == searchPattern1 || folderName == searchPattern2) {
            foundFolders.append(folderName);
        }
        // Проверяем, начинается ли папка с "№ " и содержит номер заказа
        else if (folderName.startsWith("№ ") && folderName.contains(orderNumber)) {
            foundFolders.append(folderName);
        }
        // Проверяем, начинается ли папка с "№" и содержит номер заказа
        else if (folderName.startsWith("№") && folderName.contains(orderNumber)) {
            foundFolders.append(folderName);
        }
        // Проверяем, начинается ли папка просто с номера заказа
        else if (folderName.startsWith(orderNumber)) {
            foundFolders.append(folderName);
        }
    }

    // Если папки не найдены, пробуем поискать с другим форматом
    if (foundFolders.isEmpty()) {
        for (const QString& folderName : allFolders) {
            if (folderName.contains("№") && folderName.contains(orderNumber)) {
                foundFolders.append(folderName);
            }
        }
    }

    if (foundFolders.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                               QString("Папка с заказом №%1 не найдена в:\n%2\n\n"
                                       "Проверьте, что папка имеет формат:\n"
                                       "- '№ %1'\n"
                                       "- '№%1'\n"
                                       "- или содержит '%1'")
                               .arg(orderNumber)
                               .arg(m_toFolder));
        return;
    }

    // Если найдено несколько папок, показываем выбор
    QString selectedOrderFolder;
    if (foundFolders.size() > 1) {
        bool ok;
        QString folderName = QInputDialog::getItem(this,
                                                   "Выбор папки заказа",
                                                   "Найдено несколько папок. Выберите нужную:",
                                                   foundFolders,
                                                   0,
                                                   false,
                                                   &ok);
        if (ok && !folderName.isEmpty()) {
            selectedOrderFolder = folderName;
        } else {
            return;
        }
    } else {
        selectedOrderFolder = foundFolders.first();
    }

    // Формируем полный путь к папке заказа
    QString orderFolderPath = toDir.filePath(selectedOrderFolder);

    // Ищем подпапку "ТЗ" или "Техническое задание"
    QDir orderDir(orderFolderPath);
    QStringList tzFolders = orderDir.entryList(QStringList() << "ТЗ" << "ТЗ*" << "Техническое задание" << "Technical specification",
                                                QDir::Dirs | QDir::NoDotAndDotDot);

    if (tzFolders.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                           QString("В папке заказа не найдена папка 'ТЗ'.\nОткрываю корневую папку заказа:\n%1")
                           .arg(orderFolderPath));
        // Открываем корневую папку заказа
        QDesktopServices::openUrl(QUrl::fromLocalFile(orderFolderPath));
        return;
    }

    // Берем первую найденную папку ТЗ (обычно она одна)
    QString tzFolderName = tzFolders.first();
    QString tzFolderPath = orderDir.filePath(tzFolderName);

    // Ищем в папке ТЗ подпапки с датами
    QDir tzDir(tzFolderPath);
    QStringList dateFolders = tzDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (dateFolders.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                           QString("В папке ТЗ не найдено подпапок с датами.\nОткрываю папку ТЗ:\n%1")
                           .arg(tzFolderPath));
        QDesktopServices::openUrl(QUrl::fromLocalFile(tzFolderPath));
        return;
    }

    // Функция для парсинга даты из имени папки
    auto parseDateFromFolder = [&tzFolderPath](const QString& folderName) -> QDateTime {
        // Пробуем разные форматы дат
        QRegularExpression dateRegex1("(\\d{2})\\.(\\d{2})\\.(\\d{4})"); // DD.MM.YYYY
        QRegularExpression dateRegex2("(\\d{4})-(\\d{2})-(\\d{2})");     // YYYY-MM-DD
        QRegularExpression dateRegex3("(\\d{2})-(\\d{2})-(\\d{4})");     // DD-MM-YYYY
        QRegularExpression dateRegex4("(\\d{8})");                        // YYYYMMDD

        QRegularExpressionMatch match = dateRegex1.match(folderName);
        if (match.hasMatch()) {
            int day = match.captured(1).toInt();
            int month = match.captured(2).toInt();
            int year = match.captured(3).toInt();
            return QDateTime(QDate(year, month, day), QTime(0, 0));
        }

        match = dateRegex2.match(folderName);
        if (match.hasMatch()) {
            int year = match.captured(1).toInt();
            int month = match.captured(2).toInt();
            int day = match.captured(3).toInt();
            return QDateTime(QDate(year, month, day), QTime(0, 0));
        }

        match = dateRegex3.match(folderName);
        if (match.hasMatch()) {
            int day = match.captured(1).toInt();
            int month = match.captured(2).toInt();
            int year = match.captured(3).toInt();
            return QDateTime(QDate(year, month, day), QTime(0, 0));
        }

        match = dateRegex4.match(folderName);
        if (match.hasMatch()) {
            QString dateStr = match.captured(1);
            if (dateStr.length() == 8) {
                int year = dateStr.left(4).toInt();
                int month = dateStr.mid(4, 2).toInt();
                int day = dateStr.right(2).toInt();
                return QDateTime(QDate(year, month, day), QTime(0, 0));
            }
        }

        // Если дата не распознана, используем дату модификации папки
        QDir dir(tzFolderPath + "/" + folderName);
        QFileInfo info(dir.path());
        return info.lastModified();
    };

    // Находим папку с самой ранней датой
    QString earliestDateFolder;
    QDateTime earliestDate;

    for (const QString& folderName : dateFolders) {
        QDateTime folderDate = parseDateFromFolder(folderName);
        if (!earliestDate.isValid() || folderDate > earliestDate) {
            earliestDate = folderDate;
            earliestDateFolder = folderName;
        }
    }

    if (earliestDateFolder.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                           "Не удалось определить папку с самой ранней датой.\nОткрываю папку ТЗ.");
        QDesktopServices::openUrl(QUrl::fromLocalFile(tzFolderPath));
        return;
    }

    // Формируем полный путь к папке с самой ранней датой
    QString finalPath = tzDir.filePath(earliestDateFolder);

    // Открываем папку в проводнике (без сообщения об успехе)
    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(finalPath));

    if (!opened) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Не удалось открыть папку:\n%1")
                            .arg(finalPath));
    }
}

// Открытие папки чертежей для заказа
void Constructor::openDrawingsFolder()
{
    // Получаем номер заказа из поля ввода
    QString orderNumber = m_orderLineEdit->text().trimmed();

    // Валидация ввода
    if (orderNumber.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите номер заказа!");
        return;
    }

    // Проверяем, выбрана ли папка Чертежи
    if (m_drawingsFolder.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите папку Чертежи в меню 'Файл'!");
        return;
    }

    // Проверяем существует ли директория Чертежи
    QDir drawingsDir(m_drawingsFolder);
    if (!drawingsDir.exists()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Папка Чертежи не существует:\n%1\n\nПожалуйста, выберите правильную папку в меню 'Файл'.")
                            .arg(m_drawingsFolder));
        return;
    }

    // Получаем список всех папок
    QStringList allFolders = drawingsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Ищем папки, которые начинаются с номера заказа
    QStringList foundFolders;

    for (const QString& folderName : allFolders) {
        // Проверяем, начинается ли папка с номера заказа
        if (folderName.startsWith(orderNumber)) {
            foundFolders.append(folderName);
        }
    }

    if (foundFolders.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                               QString("Папка с чертежами для заказа №%1 не найдена в:\n%2\n\n"
                                       "Папка должна начинаться с номера заказа '%3'")
                               .arg(orderNumber)
                               .arg(m_drawingsFolder)
                               .arg(orderNumber));
        return;
    }

    // Если найдено несколько папок, показываем выбор
    QString selectedFolder;
    if (foundFolders.size() > 1) {
        bool ok;
        QString folderName = QInputDialog::getItem(this,
                                                   "Выбор папки чертежей",
                                                   "Найдено несколько папок. Выберите нужную:",
                                                   foundFolders,
                                                   0,
                                                   false,
                                                   &ok);
        if (ok && !folderName.isEmpty()) {
            selectedFolder = folderName;
        } else {
            return;
        }
    } else {
        selectedFolder = foundFolders.first();
    }

    // Формируем полный путь к папке чертежей
    QString drawingsFolderPath = drawingsDir.filePath(selectedFolder);

    // Открываем папку в проводнике (без сообщения об успехе)
    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(drawingsFolderPath));

    if (!opened) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Не удалось открыть папку:\n%1")
                            .arg(drawingsFolderPath));
    }
}

// Открытие папки эскизов для заказа
void Constructor::openSketchesFolder()
{
    // Получаем номер заказа из поля ввода
    QString orderNumber = m_orderLineEdit->text().trimmed();

    // Валидация ввода
    if (!validateOrderInput(orderNumber)) {
        return;
    }

    // Проверяем существует ли директория ТО
    QDir toDir(m_toFolder);
    if (!toDir.exists()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Папка ТО не существует:\n%1\n\nПожалуйста, выберите правильную папку в меню 'Файл'.")
                            .arg(m_toFolder));
        return;
    }

    // Формируем шаблоны для поиска папки заказа
    QString searchPattern1 = "№ " + orderNumber;
    QString searchPattern2 = "№" + orderNumber;
    QString searchPattern3 = orderNumber;

    // Получаем список всех папок
    QStringList allFolders = toDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Ищем папку заказа
    QStringList foundOrderFolders;

    for (const QString& folderName : allFolders) {
        if (folderName == searchPattern1 || folderName == searchPattern2) {
            foundOrderFolders.append(folderName);
        }
        else if (folderName.startsWith("№ ") && folderName.contains(orderNumber)) {
            foundOrderFolders.append(folderName);
        }
        else if (folderName.startsWith("№") && folderName.contains(orderNumber)) {
            foundOrderFolders.append(folderName);
        }
        else if (folderName.startsWith(orderNumber)) {
            foundOrderFolders.append(folderName);
        }
    }

    if (foundOrderFolders.isEmpty()) {
        for (const QString& folderName : allFolders) {
            if (folderName.contains("№") && folderName.contains(orderNumber)) {
                foundOrderFolders.append(folderName);
            }
        }
    }

    if (foundOrderFolders.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                               QString("Папка с заказом №%1 не найдена в:\n%2")
                               .arg(orderNumber)
                               .arg(m_toFolder));
        return;
    }

    // Выбираем папку заказа
    QString selectedOrderFolder;
    if (foundOrderFolders.size() > 1) {
        bool ok;
        QString folderName = QInputDialog::getItem(this,
                                                   "Выбор папки заказа",
                                                   "Найдено несколько папок. Выберите нужную:",
                                                   foundOrderFolders,
                                                   0,
                                                   false,
                                                   &ok);
        if (ok && !folderName.isEmpty()) {
            selectedOrderFolder = folderName;
        } else {
            return;
        }
    } else {
        selectedOrderFolder = foundOrderFolders.first();
    }

    // Формируем полный путь к папке заказа
    QString orderFolderPath = toDir.filePath(selectedOrderFolder);

    // Ищем подпапку "Эскизы" или "Sketch"
    QDir orderDir(orderFolderPath);
    QStringList sketchesFolders = orderDir.entryList(QStringList() << "Эскизы" << "Эскиз*" << "Sketch" << "Sketches",
                                                      QDir::Dirs | QDir::NoDotAndDotDot);

    if (sketchesFolders.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                           QString("В папке заказа не найдена папка 'Эскизы'.\nОткрываю корневую папку заказа:\n%1")
                           .arg(orderFolderPath));
        QDesktopServices::openUrl(QUrl::fromLocalFile(orderFolderPath));
        return;
    }

    // Берем первую найденную папку Эскизы (обычно она одна)
    QString sketchesFolderName = sketchesFolders.first();
    QString sketchesFolderPath = orderDir.filePath(sketchesFolderName);

    // Открываем папку Эскизы в проводнике (без сообщения об успехе)
    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(sketchesFolderPath));

    if (!opened) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Не удалось открыть папку:\n%1")
                            .arg(sketchesFolderPath));
    }
}

// Открытие папки Чертежи в ТО (в папке заказа)
void Constructor::openDrawingsInTOFolder()
{
    // Получаем номер заказа из поля ввода
    QString orderNumber = m_orderLineEdit->text().trimmed();

    // Валидация ввода
    if (!validateOrderInput(orderNumber)) {
        return;
    }

    // Проверяем существует ли директория ТО
    QDir toDir(m_toFolder);
    if (!toDir.exists()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Папка ТО не существует:\n%1\n\nПожалуйста, выберите правильную папку в меню 'Файл'.")
                            .arg(m_toFolder));
        return;
    }

    // Формируем шаблоны для поиска папки заказа
    QString searchPattern1 = "№ " + orderNumber;
    QString searchPattern2 = "№" + orderNumber;
    QString searchPattern3 = orderNumber;

    // Получаем список всех папок
    QStringList allFolders = toDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Ищем папку заказа
    QStringList foundOrderFolders;

    for (const QString& folderName : allFolders) {
        if (folderName == searchPattern1 || folderName == searchPattern2) {
            foundOrderFolders.append(folderName);
        }
        else if (folderName.startsWith("№ ") && folderName.contains(orderNumber)) {
            foundOrderFolders.append(folderName);
        }
        else if (folderName.startsWith("№") && folderName.contains(orderNumber)) {
            foundOrderFolders.append(folderName);
        }
        else if (folderName.startsWith(orderNumber)) {
            foundOrderFolders.append(folderName);
        }
    }

    if (foundOrderFolders.isEmpty()) {
        for (const QString& folderName : allFolders) {
            if (folderName.contains("№") && folderName.contains(orderNumber)) {
                foundOrderFolders.append(folderName);
            }
        }
    }

    if (foundOrderFolders.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                               QString("Папка с заказом №%1 не найдена в:\n%2")
                               .arg(orderNumber)
                               .arg(m_toFolder));
        return;
    }

    // Выбираем папку заказа
    QString selectedOrderFolder;
    if (foundOrderFolders.size() > 1) {
        bool ok;
        QString folderName = QInputDialog::getItem(this,
                                                   "Выбор папки заказа",
                                                   "Найдено несколько папок. Выберите нужную:",
                                                   foundOrderFolders,
                                                   0,
                                                   false,
                                                   &ok);
        if (ok && !folderName.isEmpty()) {
            selectedOrderFolder = folderName;
        } else {
            return;
        }
    } else {
        selectedOrderFolder = foundOrderFolders.first();
    }

    // Формируем полный путь к папке заказа
    QString orderFolderPath = toDir.filePath(selectedOrderFolder);

    // Ищем подпапку "Чертежи" или "Drawings"
    QDir orderDir(orderFolderPath);
    QStringList drawingsFolders = orderDir.entryList(QStringList() << "Чертежи" << "Чертеж*" << "Drawings" << "Drawing",
                                                      QDir::Dirs | QDir::NoDotAndDotDot);

    if (drawingsFolders.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                           QString("В папке заказа не найдена папка 'Чертежи'.\nОткрываю корневую папку заказа:\n%1")
                           .arg(orderFolderPath));
        QDesktopServices::openUrl(QUrl::fromLocalFile(orderFolderPath));
        return;
    }

    // Берем первую найденную папку Чертежи (обычно она одна)
    QString drawingsFolderName = drawingsFolders.first();
    QString drawingsFolderPath = orderDir.filePath(drawingsFolderName);

    // Открываем папку Чертежи в проводнике (без сообщения об успехе)
    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(drawingsFolderPath));

    if (!opened) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Не удалось открыть папку:\n%1")
                            .arg(drawingsFolderPath));
    }
}

// Копирование файлов из чертежей в папку ТО
void Constructor::copyToTOFolder()
{
    // Получаем номер изделия из поля ввода
    QString itemNumber = m_productLineEdit->text().trimmed();

    // Валидация ввода
    if (itemNumber.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите номер изделия!");
        return;
    }

    // Проверяем, выбрана ли папка Чертежи
    if (m_drawingsFolder.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите папку Чертежи в меню 'Файл'!");
        return;
    }

    // Проверяем, выбрана ли папка ТО
    if (m_toFolder.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите папку ТО в меню 'Файл'!");
        return;
    }

    // Получаем номер заказа (первая часть до точки)
    QString orderNumber = itemNumber;
    if (itemNumber.contains('.')) {
        orderNumber = itemNumber.split('.').first();
    }

    QDir drawingsDir(m_drawingsFolder);
    QDir toDir(m_toFolder);

    // Проверяем существование директорий
    if (!drawingsDir.exists()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Папка Чертежи не существует:\n%1").arg(m_drawingsFolder));
        return;
    }

    if (!toDir.exists()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Папка ТО не существует:\n%1").arg(m_toFolder));
        return;
    }

    // === Шаг 1: Найти папки заказа локально ===
    QStringList localOrderCandidates;
    QStringList allLocalFolders = drawingsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& folderName : allLocalFolders) {
        if (folderName.startsWith(orderNumber)) {
            localOrderCandidates.append(folderName);
        }
    }

    if (localOrderCandidates.isEmpty()) {
        QMessageBox::critical(this, "Ошибка",
                            QString("Не найдена папка заказа '%1' в:\n%2")
                            .arg(orderNumber)
                            .arg(m_drawingsFolder));
        return;
    }

    // Функция для выбора папки из списка
    auto selectFolder = [this](const QString& title, const QStringList& folders,
                                std::function<void(const QString&)> callback) {
        if (folders.size() == 1) {
            callback(folders.first());
        } else {
            bool ok;
            QString selected = QInputDialog::getItem(this, title,
                                                     "Найдено несколько папок. Выберите нужную:",
                                                     folders, 0, false, &ok);
            if (ok && !selected.isEmpty()) {
                callback(selected);
            }
        }
    };

    // Выбираем локальную папку заказа
    selectFolder("Выберите папку заказа (локально)", localOrderCandidates,
                 [this, itemNumber, orderNumber, &toDir, &drawingsDir, selectFolder](const QString& localOrderName) {

        QString orderFolderLocal = drawingsDir.filePath(localOrderName);

        // === Шаг 2: Найти папку изделия ===
        QDir orderDir(orderFolderLocal);
        QStringList itemCandidates;
        QStringList allItemFolders = orderDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const QString& folderName : allItemFolders) {
            if (folderName.startsWith(itemNumber)) {
                itemCandidates.append(folderName);
            }
        }

        if (itemCandidates.isEmpty()) {
            QMessageBox::critical(this, "Ошибка",
                                QString("Не найдена папка изделия '%1' в:\n%2")
                                .arg(itemNumber)
                                .arg(orderFolderLocal));
            return;
        }

        // Выбираем папку изделия
        selectFolder("Выберите папку изделия", itemCandidates,
                     [this, orderFolderLocal, itemNumber, orderNumber, &toDir](const QString& itemFolderName) {

            QString itemFolderLocal = QDir(orderFolderLocal).filePath(itemFolderName);

            // === Шаг 3: Найти сетевую папку заказа ===
            QStringList networkOrderCandidates;
            QStringList allNetworkFolders = toDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (const QString& folderName : allNetworkFolders) {
                if (folderName.startsWith("№ " + orderNumber) || folderName.startsWith("№" + orderNumber)) {
                    networkOrderCandidates.append(folderName);
                }
            }

            if (networkOrderCandidates.isEmpty()) {
                QMessageBox::critical(this, "Ошибка",
                                    QString("Не найдена сетевая папка '№ %1' в:\n%2")
                                    .arg(orderNumber)
                                    .arg(m_toFolder));
                return;
            }

            // Выбираем сетевую папку заказа
            auto processNetworkFolder = [this, itemFolderLocal, itemNumber, itemFolderName](const QString& networkOrderName) {
                QString networkOrderPath = QDir(m_toFolder).filePath(networkOrderName);
                QString drawingsFolderNetwork = QDir(networkOrderPath).filePath("чертежи");

                // Создаем папку "чертежи" если её нет
                QDir drawingsNetworkDir(drawingsFolderNetwork);
                if (!drawingsNetworkDir.exists()) {
                    drawingsNetworkDir.mkpath(".");
                }

                QString targetItemFolder = QDir(drawingsFolderNetwork).filePath(itemFolderName);

                // Проверяем, существует ли уже папка
                if (QDir(targetItemFolder).exists()) {
                    QMessageBox::StandardButton reply = QMessageBox::question(
                        this,
                        "Папка уже существует",
                        QString("Папка уже существует:\n%1\n\nЧто сделать?")
                        .arg(targetItemFolder),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
                    );

                    if (reply == QMessageBox::No) {
                        QMessageBox::information(this, "Пропущено", "Копирование отменено: папка уже существует.");
                        return;
                    } else if (reply == QMessageBox::Cancel) {
                        return;
                    } else if (reply == QMessageBox::Yes) {
                        // Удаляем старую папку
                        QDir(targetItemFolder).removeRecursively();
                    }
                }

                // Создаем новую папку
                QDir().mkpath(targetItemFolder);

                // === Копирование файлов ===
                QStringList extensionsMain = {".pdf", ".xls", ".bln", ".xbir"};  // Добавили .xbir
                QStringList extensionsLaser = {".dxf", ".igs"};

                QStringList copiedFiles;
                QStringList laserFiles;
                QStringList missingFiles;  // Список отсутствующих файлов

                QDir itemDir(itemFolderLocal);
                QStringList allFiles = itemDir.entryList(QDir::Files);

                // Собираем информацию о найденных файлах
                for (const QString& fileName : allFiles) {
                    QFileInfo fileInfo(fileName);
                    QString ext = fileInfo.suffix().toLower();

                    if (extensionsLaser.contains("." + ext)) {
                        laserFiles.append(fileName);
                    } else if (extensionsMain.contains("." + ext)) {
                        copiedFiles.append(fileName);
                    }
                }

                // Проверяем наличие обязательных основных файлов
                QStringList requiredMainFiles = {".pdf", ".xls", ".bln", ".xbir"};
                for (const QString& ext : requiredMainFiles) {
                    bool found = false;
                    for (const QString& fileName : copiedFiles) {
                        if (fileName.endsWith(ext, Qt::CaseInsensitive)) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        missingFiles.append(ext);
                    }
                }

                // Копируем основные файлы
                for (const QString& fileName : copiedFiles) {
                    QString srcPath = itemDir.filePath(fileName);
                    QString dstPath = QDir(targetItemFolder).filePath(fileName);
                    if (!QFile::copy(srcPath, dstPath)) {
                        QMessageBox::warning(this, "Предупреждение",
                                           QString("Не удалось скопировать файл:\n%1").arg(fileName));
                    }
                }

                // Копируем лазерные файлы в подпапку "Лазер"
                if (!laserFiles.isEmpty()) {
                    QString laserFolder = QDir(targetItemFolder).filePath("Лазер");
                    QDir().mkpath(laserFolder);

                    for (const QString& fileName : laserFiles) {
                        QString srcPath = itemDir.filePath(fileName);
                        QString dstPath = QDir(laserFolder).filePath(fileName);
                        if (!QFile::copy(srcPath, dstPath)) {
                            QMessageBox::warning(this, "Предупреждение",
                                               QString("Не удалось скопировать файл:\n%1").arg(fileName));
                        }
                    }
                }

                // Формируем HTML отчет
                QString report = QString(
                    "<html>"
                    "<body>"
                    "<h3>✅ Успешно обработано изделие: %1</h3>"
                    "<hr>"
                    "<b>Исходная папка:</b><br>%2<br><br>"
                    "<b>Целевая папка:</b><br>%3<br><br>"
                    "<b>Скопировано основных файлов:</b> %4<br>"
                ).arg(itemNumber)
                 .arg(itemFolderLocal)
                 .arg(targetItemFolder)
                 .arg(copiedFiles.size());

                // Добавляем информацию об отсутствующих файлах
                if (!missingFiles.isEmpty()) {
                    report += "<br><b><font color='red'>⚠️ Отсутствуют обязательные файлы:</font></b><br>";
                    for (const QString& ext : missingFiles) {
                        report += QString("<font color='red'>  • Файл с расширением %1 не найден</font><br>").arg(ext);
                    }
                }

                // Добавляем информацию о лазерных файлах
                if (!laserFiles.isEmpty()) {
                    report += QString("<br><b>Файлы для лазера (.DXF/.IGS):</b> %1 → в папку 'Лазер'<br>")
                              .arg(laserFiles.size());
                } else {
                    report += "<br><b>Файлы для лазера не найдены.</b><br>";
                }

                // Список скопированных основных файлов
                if (!copiedFiles.isEmpty()) {
                    report += "<br><b>Скопированные основные файлы:</b><br>";
                    for (const QString& fileName : copiedFiles) {
                        report += QString("  • %1<br>").arg(fileName);
                    }
                }

                // Список лазерных файлов
                if (!laserFiles.isEmpty()) {
                    report += "<br><b>Скопированные лазерные файлы:</b><br>";
                    for (const QString& fileName : laserFiles) {
                        report += QString("  • %1 → Лазер/<br>").arg(fileName);
                    }
                }

                report += "</body></html>";

                // Показываем отчет в HTML формате
                QMessageBox msgBox;
                msgBox.setWindowTitle("Результат копирования");
                msgBox.setTextFormat(Qt::RichText);
                msgBox.setText(report);
                msgBox.setStandardButtons(QMessageBox::Ok);

                // Если есть отсутствующие файлы, добавляем иконку предупреждения
                if (!missingFiles.isEmpty()) {
                    msgBox.setIcon(QMessageBox::Warning);
                } else {
                    msgBox.setIcon(QMessageBox::Information);
                }

                msgBox.exec();
            };

            if (networkOrderCandidates.size() == 1) {
                processNetworkFolder(networkOrderCandidates.first());
            } else {
                bool ok;
                QString selected = QInputDialog::getItem(this,
                                                         "Выберите сетевую папку заказа",
                                                         "Найдено несколько папок. Выберите нужную:",
                                                         networkOrderCandidates,
                                                         0,
                                                         false,
                                                         &ok);
                if (ok && !selected.isEmpty()) {
                    processNetworkFolder(selected);
                }
            }
        });
    });
}

// Выбор папки ТО
void Constructor::selectTOFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку ТО",
        m_toFolder.isEmpty() ? QDir::homePath() : m_toFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_toFolder = folder;
        saveSettings();
        QMessageBox::information(this, "Информация", "Папка ТО успешно выбрана!");
    }
}

// Выбор папки Чертежи
void Constructor::selectDrawingsFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку Чертежи",
        m_drawingsFolder.isEmpty() ? QDir::homePath() : m_drawingsFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_drawingsFolder = folder;
        saveSettings();
        QMessageBox::information(this, "Информация", "Папка Чертежи успешно выбрана!");
    }
}

// Выбор папки Цех рекламы
void Constructor::selectAdvertisingFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку Цех рекламы",
        m_advertisingFolder.isEmpty() ? QDir::homePath() : m_advertisingFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_advertisingFolder = folder;
        saveSettings();
        QMessageBox::information(this, "Информация", "Папка Цех рекламы успешно выбрана!");
    }
}

// Выбор папки Цех стекла
void Constructor::selectGlassFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку Цех стекла",
        m_glassFolder.isEmpty() ? QDir::homePath() : m_glassFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_glassFolder = folder;
        saveSettings();
        QMessageBox::information(this, "Информация", "Папка Цех стекла успешно выбрана!");
    }
}

// Выбор папки База данных
void Constructor::selectDatabaseFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку База данных",
        m_databaseFolder.isEmpty() ? QDir::homePath() : m_databaseFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_databaseFolder = folder;
        saveSettings();
        QMessageBox::information(this, "Информация", "Папка База данных успешно выбрана!");
    }
}

void Constructor::exitApplication()
{
    close();
}

void Constructor::loadSettings()
{
    // Загружаем сохраненные пути к папкам
    m_toFolder = m_settings.value("to_folder", "").toString();
    m_drawingsFolder = m_settings.value("drawings_folder", "").toString();
    m_advertisingFolder = m_settings.value("advertising_folder", "").toString();
    m_glassFolder = m_settings.value("glass_folder", "").toString();
    m_databaseFolder = m_settings.value("database_folder", "").toString();
}

void Constructor::saveSettings()
{
    // Сохраняем пути к папкам
    m_settings.setValue("to_folder", m_toFolder);
    m_settings.setValue("drawings_folder", m_drawingsFolder);
    m_settings.setValue("advertising_folder", m_advertisingFolder);
    m_settings.setValue("glass_folder", m_glassFolder);
    m_settings.setValue("database_folder", m_databaseFolder);
}
