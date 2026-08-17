#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include <QMainWindow>
#include <QSettings>
#include <QComboBox>
#include <QLineEdit>
#include <QFileSystemModel>
#include <QTreeView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>

#include "sortfilterproxymodel.h"
#include "statsdatabase.h"
#include "flipbutton.h"
#include "modebutton.h"

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
    void onPlayStopToggled(bool playIconShown);
    void onModeChanged(bool sketch);
    void updateTimerLabel();
    void onProductLineEditChanged(const QString &text);
    void toggleStatsVisibility(bool visible);
    void toggleToolbar1Visibility(bool visible);
    void toggleToolbar2Visibility(bool visible);

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

    QStringList m_orderHistory;
    int m_maxHistoryDisplay = 3;

    // Статистика и таймер
    StatsDatabase *m_statsDb;
    QTimer *m_displayTimer;
    QElapsedTimer m_elapsed;
    bool m_timerRunning = false;
    bool m_isSketchMode = true;
    QLabel *m_timeLabel = nullptr;
    FlipButton *m_playStopBtn = nullptr;
    ModeButton *m_modeBtn = nullptr;
    QString m_currentItem;
    int m_sketchAccumulatedSec = 0;   // накопленное время эскиза (сек)
    int m_drawingAccumulatedSec = 0;  // накопленное время чертежей (сек)
    // Указатели на кнопки первого тулбара
    QAction *m_actionOrder = nullptr;
    QAction *m_actionSketch = nullptr;
    QAction *m_actionDrawTO = nullptr;
    QAction *m_actionDraw = nullptr;
    QAction *m_actionPZ = nullptr;

    // Указатели на кнопки второго тулбара
    QAction *m_actionTOMove = nullptr;
    QAction *m_actionCopyName = nullptr;
    QAction *m_actionCopyArticul = nullptr;


    QToolBar *m_toolbar1 = nullptr;
    QToolBar *m_toolbar2 = nullptr;
    QToolBar *m_toolbar3 = nullptr;
    QWidget *m_statsContainer = nullptr;
};

#endif // CONSTRUCTOR_H