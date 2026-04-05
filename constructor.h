#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H


#include <QMainWindow>
#include <QSettings>

class QLineEdit;

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
    void selectTOFolder();           // Слот для выбора папки ТО
    void selectDrawingsFolder();      // Слот для выбора папки Чертежи
    void selectAdvertisingFolder();      // Слот для выбора папки Цех рекламы
    void selectGlassFolder();      // Слот для выбора папки Цех стекла
    void selectDatabaseFolder();      // Слот для выбора папки База данных
    void exitApplication();           // Слот для выхода из приложения

private:
    Ui::Constructor *ui;
    QLineEdit *order_line_edit, *product_line_edit;
    QSettings m_settings;

        QString m_toFolder;
        QString m_drawingsFolder;
        QString m_advertisingFolder;
        QString m_glassFolder;
        QString m_databaseFolder;

        void loadSettings();
        void saveSettings();
};
#endif // CONSTRUCTOR_H
