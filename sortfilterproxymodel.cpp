#include "sortfilterproxymodel.h"

CustomSortProxyModel::CustomSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool CustomSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(sourceModel());
    if (!fsModel) {
        return QSortFilterProxyModel::lessThan(left, right);
    }

    QFileInfo leftInfo = fsModel->fileInfo(left);
    QFileInfo rightInfo = fsModel->fileInfo(right);

    // Папки всегда сверху
    if (leftInfo.isDir() && !rightInfo.isDir()) {
        return sortOrder() == Qt::AscendingOrder;
    }
    if (!leftInfo.isDir() && rightInfo.isDir()) {
        return sortOrder() != Qt::AscendingOrder;
    }

    // Если оба файла - сортируем по типу
    if (!leftInfo.isDir() && !rightInfo.isDir()) {
        int leftPriority = getFilePriority(leftInfo.suffix().toLower());
        int rightPriority = getFilePriority(rightInfo.suffix().toLower());

        if (leftPriority != rightPriority) {
            if (sortOrder() == Qt::AscendingOrder) {
                return leftPriority < rightPriority;
            } else {
                return leftPriority > rightPriority;
            }
        }

        // Если одинаковый приоритет - сортируем по имени
        QCollator collator;
        collator.setNumericMode(true);
        return collator.compare(leftInfo.fileName(), rightInfo.fileName()) < 0;
    }

    // Если оба папки - сортируем по имени
    QCollator collator;
    collator.setNumericMode(true);
    return collator.compare(leftInfo.fileName(), rightInfo.fileName()) < 0;
}

int CustomSortProxyModel::getFilePriority(const QString &suffix) const
{
    // Приоритеты: меньше число = выше в списке
    if (suffix == "pdf") return 0;
    if (suffix == "bln") return 1;
    if (suffix == "b3d") return 2;
    if (suffix == "xls" || suffix == "xlsx") return 3;
    if (suffix == "dxf") return 4;
    if (suffix == "igs") return 5;
    if (suffix == "sldprt" || suffix == "sldasm" || suffix == "slddrw") return 6;
    if (suffix == "xbir") return 7;

    return 100; // Остальные файлы в конце
}