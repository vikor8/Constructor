#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include <QMainWindow>
#include <QSettings>
#include <QLineEdit>

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

private:
    void loadSettings();
    void saveSettings();
    bool validateOrderInput(const QString& orderNumber);

    Ui::Constructor *ui;
    QSettings m_settings;

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
