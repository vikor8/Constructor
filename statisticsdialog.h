#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

class StatsDatabase;

namespace Ui {
class StatisticsDialog;
}
class QLabel;   // forward declaration
class QStandardItemModel;
class QTimer;
class StatsDatabase;
class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(StatsDatabase *db, QWidget *parent = nullptr);
    ~StatisticsDialog();

private slots:
    void onRefreshClicked();
    void onPeriodChanged(int index);
    void onExportCsvClicked();
    void onCopyClicked();

private:
    void reloadData();
    QDate resolveFromDate() const;   // вычисляет дату начала по выбранному пресету
    QDate resolveToDate() const;

    Ui::StatisticsDialog *ui;
    StatsDatabase        *m_db = nullptr;
    QStandardItemModel   *m_model = nullptr;
    static QString pluralizeItems(int n);
    static QString pluralizeOrders(int n);
    static QString pluralizeDays(int n);
    static QString pluralizePoints(int n);
    QTimer *m_searchTimer = nullptr;
    QLabel *m_totalLabel = nullptr;

};

#endif