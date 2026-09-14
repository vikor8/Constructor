#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QCollator>
#include <QSettings>
#include <QSet>
#include <QColor>
#include <QDir>

class CustomSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit CustomSortProxyModel(QObject *parent = nullptr);

    void setFilePriority(const QStringList &priorityList);
    QStringList getFilePriority() const;
    void loadSettings();
    void saveSettings();

    // === Подсветка папок, отсутствующих в эталонной папке ===
    void enableMissingHighlight(bool enable,
                                const QString &rootPath,
                                const QString &refFolder);
    void disableHighlight();
    void rebuildReferenceKeys();
    bool highlightMissingEnabled() const { return m_highlightMissing; }

    // Утилита: получить "числовой ключ" из имени папки.
    // "№ 220"              -> "220"
    // "220 ADY Москва"     -> "220"
    // "220.04(1-5B)_M-2"   -> "220.04"
    // ⬇⬇⬇ ЭТУ СТРОКУ НУЖНО ПЕРЕНЕСТИ ИЗ private В public ⬇⬇⬇
    static QString extractNumberKey(const QString &folderName);

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    int getFilePriority(const QString &suffix) const;
    void notifyViewRefresh();

    QStringList m_filePriorityList;

    // Подсветка
    bool m_highlightMissing = false;
    QString m_highlightRootPath;      // папка Чертежи (её прямые дети проверяются)
    QString m_referenceFolder;        // папка ТО
    QSet<QString> m_referenceKeys;    // номера заказов из ТО
};

#endif // SORTFILTERPROXYMODEL_H