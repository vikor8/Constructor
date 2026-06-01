#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include <QMainWindow>
#include <QSettings>
#include <QLineEdit>
#include <QFileSystemModel>
#include <QTreeView>
#include "sortfilterproxymodel.h"  // Добавляем инклуд

QT_BEGIN_NAMESPACE
namespace Ui { class Constructor; }
QT_END_NAMESPACE

class Constructor : public QMainWindow
{
    Q_OBJECT

public:
    Constructor(QWidget *parent = nullptr);
    ~Constructor();

private slots:
    void selectTOFolder();
    void selectDrawingsFolder();
    void selectAdvertisingFolder();
    void selectGlassFolder();
    void selectDatabaseFolder();
    void exitApplication();
    void openOrderFolder();
    void openDrawingsFolder();
    void openSketchesFolder();
    void openDrawingsInTOFolder();
    void copyToTOFolder();

    // Слот для обработки двойного клика в дереве файлов
    void onFolderViewDoubleClicked(const QModelIndex &index);

    // Слот для обработки клика с клавишами-модификаторами
    void onFolderViewClicked(const QModelIndex &index);

private:
    void loadSettings();
    void saveSettings();
    bool validateOrderInput(const QString& orderNumber);
    void setupFileSystemModel();
    QString findOrderFolder(const QString &orderNumber, const QString &basePath);
    void openFolderInView(const QString &path);
    void openInExplorer(const QString &path);
    QString findTargetPath(const QString &orderFolderPath, const QString &buttonType);

    Ui::Constructor *ui;
    QSettings m_settings;

    // Модель файловой системы
    QFileSystemModel *m_fileSystemModel;

    // Прокси-модель для сортировки
    CustomSortProxyModel *m_proxyModel;

    // Пути к папкам
    QString m_toFolder;
    QString m_drawingsFolder;
    QString m_advertisingFolder;
    QString m_glassFolder;
    QString m_databaseFolder;

    // Указатели на виджеты
    QLineEdit *m_orderLineEdit;
    QLineEdit *m_productLineEdit;
};

#endif // CONSTRUCTOR_H