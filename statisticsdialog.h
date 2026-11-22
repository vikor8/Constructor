#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QDate>

class QLabel;
class QTimer;
class QStandardItemModel;
class StatsDatabase;

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    // <-- Добавили int userId
    explicit StatisticsDialog(StatsDatabase *db, int userId, QWidget *parent = nullptr);
    ~StatisticsDialog();

private slots:
    void onRefreshClicked();
    void onPeriodChanged(int index);
    void onExportCsvClicked();
    void onCopyClicked();
    void onAddItemClicked();
    void onTreeDoubleClicked(const QModelIndex &index);

private:
    static QString formatPoints(double v);
    static QString pluralizeItems(int n);
    static QString pluralizeOrders(int n);
    static QString pluralizeDays(int n);
    static QString pluralizePoints(int n);

    void reloadData();
    void recalcTotals();
    QDate resolveFromDate() const;
    QDate resolveToDate() const;

    Ui::StatisticsDialog *ui;
    StatsDatabase        *m_db = nullptr;
    int                   m_userId = -1;   // <-- Храним ID пользователя
    QStandardItemModel   *m_model = nullptr;
    QTimer               *m_searchTimer = nullptr;
    QLabel               *m_totalLabel = nullptr;
    bool                  m_updating = false;
};

#endif // STATISTICSDIALOG_H