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
    static QString extractNumberKey(const QString &folderName);
    static QString extractOrderNumber(const QString &folderName);

    // Разбор имени папки изделия вида "220.04(1-5B)_M-2.11 Хранение в нише"
    static QString parseItemArticle(const QString &folderName); // "220.04(1-5B)"
    static QString parseItemName(const QString &folderName);    // "M-2.11 Хранение в нише"
    static int     parseItemPoints(const QString &folderName);  // 5 (из скобок)


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