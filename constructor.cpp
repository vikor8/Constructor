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

Constructor::Constructor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Constructor)
    , m_settings("Best-studio", "ConstructorApp")
{
    ui->setupUi(this);
    // Загружаем сохраненные настройки
    loadSettings();

    //Создаем менюбар
    QMenuBar *menuBar=new QMenuBar(this);

    // Создаем меню "Файл"
    QMenu *fileMenu = menuBar->addMenu("Файл");
    QAction *toFolderAction = fileMenu ->addAction("Папка ТО");
    QAction *drawingsFolderAction = fileMenu ->addAction("Папка Чертежи");
    QAction *advertisingFolderAction =fileMenu ->addAction("Реклама");
    QAction *glassFolderAction = fileMenu -> addAction("Цех стекла");
    QAction *databaseFolderAction = fileMenu -> addAction("База данных");
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Выход");

    // Подключаем сигналы к слотам
    connect(toFolderAction, &QAction::triggered, this, &Constructor::selectTOFolder);
    connect(drawingsFolderAction, &QAction::triggered, this, &Constructor::selectDrawingsFolder);
    connect(advertisingFolderAction, &QAction::triggered, this, &::Constructor::selectAdvertisingFolder);
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
    QLineEdit *orderLineEdit = new QLineEdit(this);
    orderLineEdit->setPlaceholderText("Заказ"); // Устанавливаем подсказку
    orderLineEdit->setMaximumWidth(70); // Устанавливаем максимальную ширину
    tbar->addWidget(orderLineEdit); // Добавляем виджет в тулбар

    // Добавляем кнопку заказ
    QAction *actionlOrder = tbar->addAction("Заказ");
    QAction *actionTZ = tbar->addAction("ТЗ");
    QAction *actionDraw = tbar->addAction("Чертежи");
    QAction *actionSketch = tbar->addAction("Эскизы");
    QAction *actionCompare = tbar->addAction("Сравнить с ТО");

//    // Расширяемый виджет
//    QWidget *spacer = new QWidget(this);
//    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    tbar->addWidget(spacer);



   //   Создаем второй тулбар
    QToolBar *tbar2 = new QToolBar("ToolBar2", this);
    tbar2->setAllowedAreas(Qt::LeftToolBarArea);
    tbar2->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, tbar2);

//   Создаем и добавляем QLineEdit
    QLineEdit *productLineEdit = new QLineEdit(this);
    productLineEdit->setPlaceholderText("№ Изделия");
    productLineEdit->setMaximumWidth(70);
    tbar2->addWidget(productLineEdit);

    // Добавляем кнопку изделие
    QAction *acttonProduction = tbar2->addAction("Изделие");
    QAction *actionTO = tbar2->addAction("ТО");
    QAction *actionTOMove = tbar2->addAction("->ТО");
//    QAction *actionTO = tbar2->addAction("ТО");

}

Constructor::~Constructor()
{
    saveSettings();
    delete ui;
}
    //Выбор папки ТО
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
    }
}
    //Выбор папки Чертежи
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
    }
}

//Выбор папки Цех рекламы
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
    }
}

//Выбор папки Цех стекла
void Constructor::selectGlassFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку Цех стекла",  // Исправлен заголовок
        m_glassFolder.isEmpty() ? QDir::homePath() : m_glassFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_glassFolder = folder;
        saveSettings();
    }
}

//Выбор папки База данных
void Constructor::selectDatabaseFolder()
{
    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку База данных",  // Исправлен заголовок
        m_databaseFolder.isEmpty() ? QDir::homePath() : m_databaseFolder,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!folder.isEmpty()) {
        m_databaseFolder = folder;
        saveSettings();
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
}

void Constructor::saveSettings()
{
    // Сохраняем пути к папкам
    m_settings.setValue("to_folder", m_toFolder);
    m_settings.setValue("drawings_folder", m_drawingsFolder);
}
