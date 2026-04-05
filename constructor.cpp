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
    QAction *actionTZ = tbar->addAction("ТЗ");
    QAction *actionDraw = tbar->addAction("Чертежи");
    QAction *actionSketch = tbar->addAction("Эскизы");
    QAction *actionCompare = tbar->addAction("Сравнить с ТО");

    // Подключаем кнопку "Заказ"
    connect(actionOrder, &QAction::triggered, this, &Constructor::openOrderFolder);

    // Для остальных кнопок пока заглушки (можно добавить позже)
    connect(actionTZ, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
    connect(actionDraw, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
    connect(actionSketch, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
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
    connect(actionTOMove, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
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
        if (!earliestDate.isValid() || folderDate < earliestDate) {
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
