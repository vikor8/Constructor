#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QCollator>
#include <QSettings>

class CustomSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit CustomSortProxyModel(QObject *parent = nullptr);

    void setFilePriority(const QStringList &priorityList);
    QStringList getFilePriority() const;
    void loadSettings();
    void saveSettings();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    int getFilePriority(const QString &suffix) const;
    QStringList m_filePriorityList;
};

#endif // SORTFILTERPROXYMODEL_H