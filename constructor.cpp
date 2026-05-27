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
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QApplication>
#include <QProcess>
#include <QVBoxLayout>

Constructor::Constructor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Constructor)
    , m_settings("Best-studio", "ConstructorApp")
    , m_fileSystemModel(nullptr)
    , m_orderLineEdit(nullptr)
    , m_productLineEdit(nullptr)
{
    ui->setupUi(this);

    // Устанавливаем layout для центрального виджета
    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(ui->m_toFolderView);

    // Настраиваем модель файловой системы
    setupFileSystemModel();

    // Загружаем сохраненные настройки
    loadSettings();

    // Устанавливаем начальную папку, если она есть
    if (!m_toFolder.isEmpty()) {
        openFolderInView(m_toFolder);
    }

    // Создаем менюбар
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
    QToolBar *tbar = new QToolBar("Панель заказов", this);
    tbar->setAllowedAreas(Qt::LeftToolBarArea);
    tbar->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, tbar);

    // Создаем и добавляем QLineEdit
    m_orderLineEdit = new QLineEdit(tbar);
    m_orderLineEdit->setPlaceholderText("Заказ");
    m_orderLineEdit->setMaximumWidth(70);
    tbar->addWidget(m_orderLineEdit);

    // Добавляем кнопки
    QAction *actionOrder = tbar->addAction("Заказ");
    QAction *actionSketch = tbar->addAction("Эскизы");
    QAction *actionDrawTO = tbar->addAction("Чертежи в ТО");
    QAction *actionDraw = tbar->addAction("Чертежи");
    QAction *actionCompare = tbar->addAction("Сравнить с ТО");

    // Подключаем кнопки
    connect(actionOrder, &QAction::triggered, this, &Constructor::openOrderFolder);
    connect(actionSketch, &QAction::triggered, this, &Constructor::openSketchesFolder);
    connect(actionDrawTO, &QAction::triggered, this, &Constructor::openDrawingsInTOFolder);
    connect(actionDraw, &QAction::triggered, this, &Constructor::openDrawingsFolder);

    connect(actionCompare, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });

    // Создаем второй тулбар
    QToolBar *tbar2 = new QToolBar("Панель изделий", this);
    tbar2->setAllowedAreas(Qt::LeftToolBarArea);
    tbar2->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, tbar2);

    // Создаем и добавляем QLineEdit
    m_productLineEdit = new QLineEdit(tbar2);
    m_productLineEdit->setPlaceholderText("№ Изделия");
    m_productLineEdit->setMaximumWidth(70);
    tbar2->addWidget(m_productLineEdit);

    // Добавляем кнопки
    QAction *actionProduction = tbar2->addAction("Изделие");
    QAction *actionTO = tbar2->addAction("ТО");
    QAction *actionTOMove = tbar2->addAction("->ТО");

    // Подключаем кнопки второго тулбара
    connect(actionProduction, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
    connect(actionTO, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Информация", "Функция в разработке");
    });
    connect(actionTOMove, &QAction::triggered, this, &Constructor::copyToTOFolder);

    // Устанавливаем заголовок окна
    setWindowTitle("Constructor - Управление заказами");

    // Добавляем подсказку в статусбар
    statusBar()->showMessage("Ctrl+Клик - открыть в проводнике | Двойной клик - открыть файл | Можно растягивать колонки");
}

Constructor::~Constructor()
{
    saveSettings();
    delete ui;
}

// Настройка модели файловой системы
void Constructor::setupFileSystemModel()
{
    m_fileSystemModel = new QFileSystemModel(this);
    m_fileSystemModel->setRootPath(QDir::rootPath());
    m_fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    // Настраиваем QTreeView из UI
    ui->m_toFolderView->setModel(m_fileSystemModel);
    ui->m_toFolderView->setSortingEnabled(true);
    ui->m_toFolderView->setAnimated(true);
    ui->m_toFolderView->setIndentation(20);
    ui->m_toFolderView->setRootIndex(m_fileSystemModel->index(QDir::homePath()));

    // Скрываем заголовок
    ui->m_toFolderView->setHeaderHidden(true);

    // НАСТРОЙКА КОЛОНОК ДЛЯ РЕГУЛИРОВКИ РАЗМЕРА
    // Разрешаем растягивать последнюю секцию
    ui->m_toFolderView->header()->setStretchLastSection(true);

    // Устанавливаем режим изменения размера - можно растягивать
    ui->m_toFolderView->header()->setSectionResizeMode(QHeaderView::Interactive);

    // Устанавливаем минимальную ширину для колонки с именем
    ui->m_toFolderView->header()->setMinimumSectionSize(200);

    // Начальная ширина колонки с именем
    ui->m_toFolderView->setColumnWidth(0, 300);

    // Разрешаем пользователю менять ширину колонок
    ui->m_toFolderView->header()->setSectionsMovable(false);
    ui->m_toFolderView->header()->setSectionsClickable(true);

    // Подключаем обработчики кликов
    connect(ui->m_toFolderView, &QTreeView::doubleClicked,
            this, &Constructor::onFolderViewDoubleClicked);
    connect(ui->m_toFolderView, &QTreeView::clicked,
            this, &Constructor::onFolderViewClicked);
}

// Открытие папки в представлении
void Constructor::openFolderInView(const QString &path)
{
    QModelIndex targetIndex = m_fileSystemModel->index(path);
    ui->m_toFolderView->setRootIndex(targetIndex);

    // Получаем имя папки
    QDir dir(path);
    QString folderName = dir.dirName();
    if (folderName.isEmpty()) {
        folderName = path; // Для корневых папок типа "C:\"
    }

    // Устанавливаем заголовок окна с именем папки
    setWindowTitle(QString("Constructor - %1").arg(folderName));

    // Обновляем статусбар
    statusBar()->showMessage(QString("Текущая папка: %1").arg(path), 5000);
}

// Открытие в проводнике
void Constructor::openInExplorer(const QString &path)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

// Обработка клика с проверкой на Ctrl
void Constructor::onFolderViewClicked(const QModelIndex &index)
{
    // Проверяем, нажат ли Ctrl
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        QFileInfo fileInfo = m_fileSystemModel->fileInfo(index);
        QString path = fileInfo.absoluteFilePath();

        // Открываем в проводнике с выделением файла/папки
        if (fileInfo.isDir()) {
            openInExplorer(path);
        } else {
            // Для файлов открываем папку и выделяем файл
            QString explorerCmd = QString("explorer.exe /select,\"%1\"").arg(QDir::toNativeSeparators(path));
            QProcess::startDetached(explorerCmd);
        }

        statusBar()->showMessage(QString("Открыто в проводнике: %1").arg(path), 3000);
    }
}

// Обработка двойного клика в дереве файлов
void Constructor::onFolderViewDoubleClicked(const QModelIndex &index)
{
    QFileInfo fileInfo = m_fileSystemModel->fileInfo(index);
    if (fileInfo.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        statusBar()->showMessage(QString("Открыт файл: %1").arg(fileInfo.fileName()), 3000);
    }
}

// Поиск папки заказа
QString Constructor::findOrderFolder(const QString &orderNumber, const QString &basePath)
{
    if (basePath.isEmpty() || !QDir(basePath).exists()) {
        return QString();
    }

    QDir dir(basePath);
    const QStringList allFolders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList foundFolders;

    // Различные форматы имени папки
    QStringList exactPatterns;
    exactPatterns << "№ " + orderNumber;
    exactPatterns << "№" + orderNumber;
    exactPatterns << orderNumber;

    // Поиск точных совпадений
    for (const QString& folderName : allFolders) {
        for (const QString& pattern : exactPatterns) {
            if (folderName == pattern) {
                foundFolders.append(folderName);
                break;
            }
        }
    }

    // Если точных совпадений нет, ищем частичные
    if (foundFolders.isEmpty()) {
        for (const QString& folderName : allFolders) {
            if (folderName.startsWith("№ ") && folderName.contains(orderNumber)) {
                foundFolders.append(folderName);
            }
            else if (folderName.startsWith("№") && folderName.contains(orderNumber)) {
                foundFolders.append(folderName);
            }
            else if (folderName.startsWith(orderNumber)) {
                foundFolders.append(folderName);
            }
        }
    }

    // Последняя попытка - поиск по содержимому
    if (foundFolders.isEmpty()) {
        for (const QString& folderName : allFolders) {
            if (folderName.contains(orderNumber, Qt::CaseInsensitive)) {
                foundFolders.append(folderName);
            }
        }
    }

    if (foundFolders.isEmpty()) {
        return QString();
    }

    // Если найдено несколько папок, показываем выбор
    QString selectedFolder;
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
            selectedFolder = folderName;
        } else {
            return QString();
        }
    } else {
        selectedFolder = foundFolders.first();
    }

    return dir.filePath(selectedFolder);
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

// Открытие папки заказа в программе
void Constructor::openOrderFolder()
{
    QString orderNumber = m_orderLineEdit->text().trimmed();

    if (!validateOrderInput(orderNumber)) {
        return;
    }

    QString orderFolderPath = findOrderFolder(orderNumber, m_toFolder);

    if (orderFolderPath.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                                 QString("Папка с заказом №%1 не найдена в:\n%2\n\n"
                                         "Проверьте правильность номера заказа и выбранной папки ТО.")
                                     .arg(orderNumber, m_toFolder));
        return;
    }

    // Проверяем, нажат ли Ctrl
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        openInExplorer(orderFolderPath);
        return;
    }

    // Ищем подпапку "ТЗ"
    QDir orderDir(orderFolderPath);
    const QStringList tzFolders = orderDir.entryList(QStringList() << "ТЗ" << "ТЗ*" << "Техническое задание" << "Technical specification",
                                                     QDir::Dirs | QDir::NoDotAndDotDot);

    QString targetPath = orderFolderPath;

    if (!tzFolders.isEmpty()) {
        QString tzPath = orderDir.filePath(tzFolders.first());
        QDir tzDir(tzPath);
        const QStringList dateFolders = tzDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        if (!dateFolders.isEmpty()) {
            // Находим папку с самой поздней датой
            QDateTime latestDate;
            QString latestFolder;

            for (const QString& folderName : dateFolders) {
                QRegularExpression dateRegex("(\\d{2})\\.(\\d{2})\\.(\\d{4})");
                QRegularExpressionMatch match = dateRegex.match(folderName);

                if (match.hasMatch()) {
                    int day = match.captured(1).toInt();
                    int month = match.captured(2).toInt();
                    int year = match.captured(3).toInt();
                    QDateTime folderDate(QDate(year, month, day), QTime(0, 0));

                    if (!latestDate.isValid() || folderDate > latestDate) {
                        latestDate = folderDate;
                        latestFolder = folderName;
                    }
                }
            }

            if (!latestFolder.isEmpty()) {
                targetPath = tzDir.filePath(latestFolder);
            } else {
                targetPath = tzPath;
            }
        } else {
            targetPath = tzPath;
        }
    }

    // Отображаем папку в дереве
    openFolderInView(targetPath);
}

// Открытие папки чертежей в программе
void Constructor::openDrawingsFolder()
{
    QString orderNumber = m_orderLineEdit->text().trimmed();

    if (orderNumber.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите номер заказа!");
        return;
    }

    if (m_drawingsFolder.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите папку Чертежи в меню 'Файл'!");
        return;
    }

    QString orderFolderPath = findOrderFolder(orderNumber, m_drawingsFolder);

    if (orderFolderPath.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                                 QString("Папка с чертежами для заказа №%1 не найдена в:\n%2")
                                     .arg(orderNumber, m_drawingsFolder));
        return;
    }

    // Проверяем, нажат ли Ctrl
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        openInExplorer(orderFolderPath);
        return;
    }

    // Отображаем папку в дереве
    openFolderInView(orderFolderPath);
}

// Открытие папки эскизов в программе
void Constructor::openSketchesFolder()
{
    QString orderNumber = m_orderLineEdit->text().trimmed();

    if (!validateOrderInput(orderNumber)) {
        return;
    }

    QString orderFolderPath = findOrderFolder(orderNumber, m_toFolder);

    if (orderFolderPath.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                                 QString("Папка с заказом №%1 не найдена в:\n%2")
                                     .arg(orderNumber, m_toFolder));
        return;
    }

    // Проверяем, нажат ли Ctrl
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        openInExplorer(orderFolderPath);
        return;
    }

    // Ищем подпапку "Эскизы"
    QDir orderDir(orderFolderPath);
    const QStringList sketchesFolders = orderDir.entryList(QStringList() << "Эскизы" << "Эскиз*" << "Sketch" << "Sketches",
                                                           QDir::Dirs | QDir::NoDotAndDotDot);

    QString targetPath;

    if (!sketchesFolders.isEmpty()) {
        targetPath = orderDir.filePath(sketchesFolders.first());
    } else {
        targetPath = orderFolderPath;
    }

    // Отображаем папку в дереве
    openFolderInView(targetPath);
}

// Открытие папки Чертежи в ТО в программе
void Constructor::openDrawingsInTOFolder()
{
    QString orderNumber = m_orderLineEdit->text().trimmed();

    if (!validateOrderInput(orderNumber)) {
        return;
    }

    QString orderFolderPath = findOrderFolder(orderNumber, m_toFolder);

    if (orderFolderPath.isEmpty()) {
        QMessageBox::information(this, "Не найдено",
                                 QString("Папка с заказом №%1 не найдена в:\n%2")
                                     .arg(orderNumber, m_toFolder));
        return;
    }

    // Проверяем, нажат ли Ctrl
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        openInExplorer(orderFolderPath);
        return;
    }

    // Ищем подпапку "Чертежи"
    QDir orderDir(orderFolderPath);
    const QStringList drawingsFolders = orderDir.entryList(QStringList() << "Чертежи" << "Чертеж*" << "Drawings" << "Drawing",
                                                           QDir::Dirs | QDir::NoDotAndDotDot);

    QString targetPath;

    if (!drawingsFolders.isEmpty()) {
        targetPath = orderDir.filePath(drawingsFolders.first());
    } else {
        targetPath = orderFolderPath;
    }

    // Отображаем папку в дереве
    openFolderInView(targetPath);
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
    const QStringList allLocalFolders = drawingsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList localOrderCandidates;

    for (const QString& folderName : allLocalFolders) {
        if (folderName.startsWith(orderNumber)) {
            localOrderCandidates.append(folderName);
        }
    }

    if (localOrderCandidates.isEmpty()) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Не найдена папка заказа '%1' в:\n%2")
                                  .arg(orderNumber, m_drawingsFolder));
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
                     const QStringList allItemFolders = orderDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                     QStringList itemCandidates;

                     for (const QString& folderName : allItemFolders) {
                         if (folderName.startsWith(itemNumber)) {
                             itemCandidates.append(folderName);
                         }
                     }

                     if (itemCandidates.isEmpty()) {
                         QMessageBox::critical(this, "Ошибка",
                                               QString("Не найдена папка изделия '%1' в:\n%2")
                                                   .arg(itemNumber, orderFolderLocal));
                         return;
                     }

                     // Выбираем папку изделия
                     selectFolder("Выберите папку изделия", itemCandidates,
                                  [this, orderFolderLocal, itemNumber, orderNumber, &toDir](const QString& itemFolderName) {

                                      QString itemFolderLocal = QDir(orderFolderLocal).filePath(itemFolderName);

                                      // === Шаг 3: Найти сетевую папку заказа ===
                                      const QStringList allNetworkFolders = toDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                                      QStringList networkOrderCandidates;

                                      for (const QString& folderName : allNetworkFolders) {
                                          if (folderName.startsWith("№ " + orderNumber) || folderName.startsWith("№" + orderNumber)) {
                                              networkOrderCandidates.append(folderName);
                                          }
                                      }

                                      if (networkOrderCandidates.isEmpty()) {
                                          QMessageBox::critical(this, "Ошибка",
                                                                QString("Не найдена сетевая папка '№ %1' в:\n%2")
                                                                    .arg(orderNumber, m_toFolder));
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
                                          QStringList extensionsMain = {".pdf", ".xls", ".bln", ".xbir"};
                                          QStringList extensionsLaser = {".dxf", ".igs"};
                                          QStringList extensionsSolidWorks = {".sldprt", ".sldasm", ".slddrw"};

                                          QStringList copiedFiles;
                                          QStringList laserFiles;
                                          QStringList solidWorksFiles;
                                          QStringList missingFiles;

                                          QDir itemDir(itemFolderLocal);
                                          const QStringList allFiles = itemDir.entryList(QDir::Files);

                                          // Собираем информацию о найденных файлах
                                          for (const QString& fileName : allFiles) {
                                              QFileInfo fileInfo(fileName);
                                              QString ext = "." + fileInfo.suffix().toLower();

                                              if (extensionsSolidWorks.contains(ext)) {
                                                  solidWorksFiles.append(fileName);
                                              } else if (extensionsLaser.contains(ext)) {
                                                  laserFiles.append(fileName);
                                              } else if (extensionsMain.contains(ext)) {
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

                                          // Копируем SolidWorks файлы в подпапку "SolidWorks"
                                          if (!solidWorksFiles.isEmpty()) {
                                              QString solidWorksFolder = QDir(targetItemFolder).filePath("SolidWorks");
                                              QDir().mkpath(solidWorksFolder);

                                              for (const QString& fileName : solidWorksFiles) {
                                                  QString srcPath = itemDir.filePath(fileName);
                                                  QString dstPath = QDir(solidWorksFolder).filePath(fileName);
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
                                                               ).arg(itemNumber, itemFolderLocal, targetItemFolder)
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

                                          // Добавляем информацию о SolidWorks файлах
                                          if (!solidWorksFiles.isEmpty()) {
                                              report += QString("<br><b>Файлы SolidWorks (.SLDPRT/.SLDASM/.SLDDRW):</b> %1 → в папку 'SolidWorks'<br>")
                                                            .arg(solidWorksFiles.size());
                                          } else {
                                              report += "<br><b>Файлы SolidWorks не найдены.</b><br>";
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

                                          // Список SolidWorks файлов
                                          if (!solidWorksFiles.isEmpty()) {
                                              report += "<br><b>Скопированные файлы SolidWorks:</b><br>";
                                              for (const QString& fileName : solidWorksFiles) {
                                                  report += QString("  • %1 → SolidWorks/<br>").arg(fileName);
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
        openFolderInView(folder);
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
        openFolderInView(folder);
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
    m_toFolder = m_settings.value("to_folder", "").toString();
    m_drawingsFolder = m_settings.value("drawings_folder", "").toString();
    m_advertisingFolder = m_settings.value("advertising_folder", "").toString();
    m_glassFolder = m_settings.value("glass_folder", "").toString();
    m_databaseFolder = m_settings.value("database_folder", "").toString();
}

void Constructor::saveSettings()
{
    m_settings.setValue("to_folder", m_toFolder);
    m_settings.setValue("drawings_folder", m_drawingsFolder);
    m_settings.setValue("advertising_folder", m_advertisingFolder);
    m_settings.setValue("glass_folder", m_glassFolder);
    m_settings.setValue("database_folder", m_databaseFolder);
}