#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QCollator>

class CustomSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit CustomSortProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    int getFilePriority(const QString &suffix) const;
};

#endif // SORTFILTERPROXYMODEL_H