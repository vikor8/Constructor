#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include <QMainWindow>
#include <QSettings>
#include <QComboBox>
#include <QLineEdit>
#include <QFileSystemModel>
#include <QTreeView>
#include "sortfilterproxymodel.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

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
    void showSortSettingsDialog();
    void showHistorySettingsDialog();

    void onFolderViewDoubleClicked(const QModelIndex &index);
    void onFolderViewClicked(const QModelIndex &index);

    void copyFileName();
    void copyArticul();
    void openPZFolder();

    void refreshCurrentView();
    void updateOrderCombo();


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

    QFileSystemModel *m_fileSystemModel;
    CustomSortProxyModel *m_proxyModel;

    QString m_toFolder;
    QString m_drawingsFolder;
    QString m_advertisingFolder;
    QString m_glassFolder;
    QString m_databaseFolder;

    QComboBox *m_orderCombo;
    QLineEdit *m_productLineEdit;

    // История заказов и настройка
    QStringList m_orderHistory;         // полная история (сохраняется)
    int m_maxHistoryDisplay = 3;           // сколько показывать в комбобоксе (по умолч. 3)
};

#endif // CONSTRUCTOR_H