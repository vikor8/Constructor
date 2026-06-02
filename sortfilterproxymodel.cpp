#include "sortfilterproxymodel.h"

CustomSortProxyModel::CustomSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // Приоритет по умолчанию
    m_filePriorityList = QStringList()
                         << "pdf"
                         << "bln"
                         << "b3d"
                         << "xls"
                         << "xlsx"
                         << "dxf"
                         << "igs"
                         << "sldprt"
                         << "sldasm"
                         << "slddrw"
                         << "xbir";

    loadSettings();
}

void CustomSortProxyModel::setFilePriority(const QStringList &priorityList)
{
    m_filePriorityList = priorityList;
    saveSettings();
    invalidate(); // Пересортировать
}

QStringList CustomSortProxyModel::getFilePriority() const
{
    return m_filePriorityList;
}

void CustomSortProxyModel::loadSettings()
{
    QSettings settings("Best-studio", "ConstructorApp");
    QStringList saved = settings.value("file_priority", m_filePriorityList).toStringList();
    if (!saved.isEmpty()) {
        m_filePriorityList = saved;
    }
}

void CustomSortProxyModel::saveSettings()
{
    QSettings settings("Best-studio", "ConstructorApp");
    settings.setValue("file_priority", m_filePriorityList);
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
        QString leftSuffix = leftInfo.suffix().toLower();
        QString rightSuffix = rightInfo.suffix().toLower();

        int leftPriority = getFilePriority(leftSuffix);
        int rightPriority = getFilePriority(rightSuffix);

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
    // Проверяем, есть ли в списке приоритетов
    int index = m_filePriorityList.indexOf(suffix);
    if (index >= 0) {
        return index;
    }

    // Файлы со знаком $ в имени - самые последние
    if (suffix.contains('$')) {
        return 9999;
    }

    // Группируем остальные файлы по типу
    // Возвращаем хеш суффикса + большое смещение
    if (!suffix.isEmpty()) {
        return 1000 + qHash(suffix) % 1000;
    }

    // Файлы без расширения
    return 2000;
}