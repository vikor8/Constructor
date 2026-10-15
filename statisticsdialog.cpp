#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include "statsdatabase.h"
#include <QTimer>
#include <QPushButton>
#include <QDate>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QMap>
#include <QBrush>
#include <QColor>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QInputDialog>
#include <cmath>

StatisticsDialog::StatisticsDialog(StatsDatabase *db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , m_db(db)
{
    ui->setupUi(this);

    // Итоговая строка под таблицей
    m_totalLabel = new QLabel(this);
    m_totalLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_totalLabel->setStyleSheet(
        "QLabel { padding: 4px 12px; color: #333; font-weight: bold; "
        "border-top: 1px solid #d0d0d0; }");

    // Вставляем метку в основной вертикальный layout диалога —
    // между таблицей и строкой кнопок
    if (auto *vlay = qobject_cast<QVBoxLayout*>(layout())) {
        vlay->insertWidget(vlay->count() - 1, m_totalLabel);
    }
    // Модель для таблицы
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({
        QStringLiteral("Изделие"),
        QStringLiteral("Наименование"),
        QStringLiteral("Баллы"),
        QStringLiteral("→ ТО")
    });
    ui->statsTree->setModel(m_model);
    connect(ui->statsTree, &QTreeView::doubleClicked,
            this, &StatisticsDialog::onTreeDoubleClicked);
    ui->statsTree->setRootIsDecorated(true);

    // Период
    ui->periodCombo->addItems({
        QStringLiteral("Сегодня"),
        QStringLiteral("Текущая неделя"),
        QStringLiteral("Текущий месяц"),
        QStringLiteral("Прошлый месяц"),
        QStringLiteral("Всё время"),
        QStringLiteral("Свой диапазон…")
    });
    ui->periodCombo->setCurrentIndex(2);   // Текущий месяц

    // Кнопки
    connect(ui->closeButton,      &QPushButton::clicked, this, &QDialog::close);
    connect(ui->refreshDataButton, &QPushButton::clicked, this, &StatisticsDialog::onRefreshClicked);
    connect(ui->periodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatisticsDialog::onPeriodChanged);
    connect(ui->onlyScoredCheck, &QCheckBox::checkStateChanged,
            this, &StatisticsDialog::onRefreshClicked);
    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(300);
    connect(m_searchTimer, &QTimer::timeout,
            this, &StatisticsDialog::onRefreshClicked);

    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, [this]() {
                m_searchTimer->start();
            });
    connect(ui->radioByOrder, &QRadioButton::toggled, this, [this](bool on) {
        if (on) onRefreshClicked();
    });
    connect(ui->radioByDate, &QRadioButton::toggled, this, [this](bool on) {
        if (on) onRefreshClicked();
    });
    connect(ui->radioByNone, &QRadioButton::toggled, this, [this](bool on) {
        if (on) onRefreshClicked();
    });

    connect(ui->dateFromEdit, &QDateEdit::dateChanged,
            this, &StatisticsDialog::onRefreshClicked);
    connect(ui->dateToEdit, &QDateEdit::dateChanged,
            this, &StatisticsDialog::onRefreshClicked);
    connect(ui->exportCsvButton, &QPushButton::clicked,
            this, &StatisticsDialog::onExportCsvClicked);
    connect(ui->copyButton, &QPushButton::clicked,
            this, &StatisticsDialog::onCopyClicked);

    // Первичная загрузка
    reloadData();
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

QDate StatisticsDialog::resolveFromDate() const
{
    const int idx = ui->periodCombo->currentIndex();
    const QDate today = QDate::currentDate();

    switch (idx) {
    case 0: return today;                                       // Сегодня
    case 1: return today.addDays(-(today.dayOfWeek() - 1));     // Понедельник текущей недели
    case 2: return QDate(today.year(), today.month(), 1);       // 1-е число текущего месяца
    case 3: {                                                   // Прошлый месяц
        QDate firstOfPrev = QDate(today.year(), today.month(), 1).addMonths(-1);
        return firstOfPrev;
    }
    case 4: return QDate();                                     // Всё время — без ограничения
    case 5: return ui->dateFromEdit->date();                    // Свой диапазон
    default: return QDate();
    }
}

QDate StatisticsDialog::resolveToDate() const
{
    const int idx = ui->periodCombo->currentIndex();
    const QDate today = QDate::currentDate();

    switch (idx) {
    case 0: return today;
    case 1: return today;
    case 2: return today;                                       // до сегодняшнего дня
    case 3: {                                                   // Прошлый месяц — до его последнего дня
        QDate firstOfThis = QDate(today.year(), today.month(), 1);
        return firstOfThis.addDays(-1);
    }
    case 4: return QDate();
    case 5: return ui->dateToEdit->date();
    default: return QDate();
    }
}

void StatisticsDialog::onPeriodChanged(int /*index*/)
{
    const int idx = ui->periodCombo->currentIndex();
    const bool custom = (idx == 5);

    if (custom) {
        // Первый заход в свой диапазон — проставим разумные значения
        if (!ui->dateFromEdit->date().isValid()
            || ui->dateFromEdit->date() == QDate(2000, 1, 1))
        {
            ui->dateFromEdit->setDate(QDate::currentDate().addMonths(-1));
            ui->dateToEdit->setDate(QDate::currentDate());
        }
    }

    ui->dateFromEdit->setEnabled(custom);
    ui->dateToEdit->setEnabled(custom);
    reloadData();
}

void StatisticsDialog::onRefreshClicked()
{
    reloadData();
}

void StatisticsDialog::reloadData()
{
    m_updating = true;

    m_model->removeRows(0, m_model->rowCount());

    if (!m_db) {
        m_updating = false;
        return;
    }

    const QDate   from = resolveFromDate();
    const QDate   to   = resolveToDate();
    const QString like = ui->searchEdit->text().trimmed();

    const QList<SentRecord> records = m_db->findSentRecords(from, to, like);

    // Какой режим группировки выбран
    enum GroupMode { ByOrder, ByDate, None };
    GroupMode mode = GroupMode::ByOrder;
    if (ui->radioByDate->isChecked())       mode = GroupMode::ByDate;
    else if (ui->radioByNone->isChecked())  mode = GroupMode::None;

    const bool onlyScored = ui->onlyScoredCheck->isChecked();

    // Фильтр "только с баллами"
    QList<SentRecord> filtered;
    for (const SentRecord &r : records) {
        if (onlyScored && r.points <= 0.0) continue;
        filtered.append(r);
    }

    // ========== Заголовки колонок под режим ==========
    if (mode == GroupMode::ByOrder) {
        m_model->setHorizontalHeaderLabels({
            QStringLiteral("Изделие"),
            QStringLiteral("Наименование"),
            QStringLiteral("Баллы"),
            QStringLiteral("Дата")
        });
    } else if (mode == GroupMode::ByDate) {
        m_model->setHorizontalHeaderLabels({
            QStringLiteral("Дата"),
            QStringLiteral("Изделия"),
            QStringLiteral("Баллы"),
            QStringLiteral("Время")
        });
    } else { // None
        m_model->setHorizontalHeaderLabels({
            QStringLiteral("Заказ"),
            QStringLiteral("Артикул / Наименование"),
            QStringLiteral("Баллы"),
            QStringLiteral("Дата")
        });
    }

    // ========== Заглушка при пустой выборке ==========
    if (filtered.isEmpty()) {
        m_model->setHorizontalHeaderLabels({
            QStringLiteral("Результат"),
            QString(), QString(), QString()
        });

        QStandardItem *empty = new QStandardItem(
            QStringLiteral("Нет данных за выбранный период"));
        empty->setEditable(false);
        empty->setTextAlignment(Qt::AlignCenter);

        QFont italic = empty->font();
        italic.setItalic(true);
        italic.setPointSize(italic.pointSize() + 2);
        empty->setFont(italic);
        empty->setForeground(QBrush(QColor(150, 150, 150)));

        m_model->appendRow({
            empty,
            new QStandardItem(),
            new QStandardItem(),
            new QStandardItem()
        });
        ui->statsTree->setFirstColumnSpanned(0, QModelIndex(), true);

        ui->labelOrdersValue->setText(QStringLiteral("0"));
        ui->labelItemsValue->setText(QStringLiteral("0"));
        ui->labelPointsValue->setText(QStringLiteral("0"));

        QString periodStr;
        if (from.isValid() && to.isValid())
            periodStr = QString("%1 — %2").arg(from.toString("dd.MM.yyyy"),
                                               to.toString("dd.MM.yyyy"));
        else
            periodStr = QStringLiteral("всё время");
        ui->labelPeriodValue->setText(periodStr);

        m_totalLabel->setText(
            QStringLiteral("Итого:  0 заказов  ·  0 изделий  ·  0 баллов"));

        m_updating = false;
        return;
    }

    // ================= ВЕТКА 1: группировка по заказу =================
    if (mode == GroupMode::ByOrder) {

        QMap<QString, QList<SentRecord>> byOrder;
        for (const SentRecord &r : filtered)
            byOrder[r.orderNumber].append(r);

        for (auto it = byOrder.constBegin(); it != byOrder.constEnd(); ++it) {
            const QString &orderNum = it.key();
            const QList<SentRecord> &list = it.value();

            double orderPoints = 0.0;
            for (const SentRecord &r : list) orderPoints += r.points;

            QStandardItem *orderItem = new QStandardItem(
                QStringLiteral("Заказ № %1").arg(orderNum));
            orderItem->setEditable(false);

            QStandardItem *orderCount = new QStandardItem(
                pluralizeItems(list.size()));
            orderCount->setEditable(false);
            orderCount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QStandardItem *orderPointsItem = new QStandardItem(
                formatPoints(orderPoints));
            orderPointsItem->setEditable(false);
            orderPointsItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QStandardItem *orderEmpty = new QStandardItem();
            orderEmpty->setEditable(false);

            QFont bold = orderItem->font();
            bold.setBold(true);
            orderItem->setFont(bold);
            orderCount->setFont(bold);
            orderPointsItem->setFont(bold);

            for (const SentRecord &r : list) {
                QStandardItem *art = new QStandardItem(r.article);
                art->setEditable(false);

                QStandardItem *nm = new QStandardItem(r.name);
                nm->setEditable(false);

                QStandardItem *pt = new QStandardItem(formatPoints(r.points));
                pt->setEditable(false);
                pt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                pt->setData(r.id,     Qt::UserRole);      // id записи в БД
                pt->setData(r.points, Qt::UserRole + 1);  // старое значение (double)
                pt->setToolTip(QStringLiteral("Двойной клик — изменить баллы"));

                QStandardItem *dt = new QStandardItem(
                    r.sentAt.toString("dd.MM.yyyy"));
                dt->setEditable(false);

                orderItem->appendRow({ art, nm, pt, dt });
            }
            m_model->appendRow({ orderItem, orderCount, orderPointsItem, orderEmpty });
        }
    }
    // ================= ВЕТКА 2: группировка по дате =================
    else if (mode == GroupMode::ByDate) {

        QMap<QDate, QList<SentRecord>> byDate;
        for (const SentRecord &r : filtered)
            byDate[r.sentAt.date()].append(r);

        QList<QDate> dates = byDate.keys();
        std::sort(dates.begin(), dates.end(), std::greater<QDate>());

        for (const QDate &d : dates) {
            const QList<SentRecord> &list = byDate.value(d);

            double dayPoints = 0.0;
            for (const SentRecord &r : list) dayPoints += r.points;

            QStandardItem *dayItem = new QStandardItem(d.toString("dd.MM.yyyy"));
            dayItem->setEditable(false);

            QStandardItem *dayCount = new QStandardItem(
                pluralizeItems(list.size()));
            dayCount->setEditable(false);
            dayCount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QStandardItem *dayPointsItem = new QStandardItem(
                formatPoints(dayPoints));
            dayPointsItem->setEditable(false);
            dayPointsItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QStandardItem *dayEmpty = new QStandardItem();
            dayEmpty->setEditable(false);

            QFont bold = dayItem->font();
            bold.setBold(true);
            dayItem->setFont(bold);
            dayCount->setFont(bold);
            dayPointsItem->setFont(bold);

            for (const SentRecord &r : list) {
                QStandardItem *art = new QStandardItem(r.article);
                art->setEditable(false);

                QStandardItem *nm = new QStandardItem(r.name);
                nm->setEditable(false);

                QStandardItem *pt = new QStandardItem(formatPoints(r.points));
                pt->setEditable(false);
                pt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                pt->setData(r.id,     Qt::UserRole);
                pt->setData(r.points, Qt::UserRole + 1);
                pt->setToolTip(QStringLiteral("Двойной клик — изменить баллы"));

                QStandardItem *dt = new QStandardItem(
                    r.sentAt.toString("HH:mm"));
                dt->setEditable(false);

                dayItem->appendRow({ art, nm, pt, dt });
            }
            m_model->appendRow({ dayItem, dayCount, dayPointsItem, dayEmpty });
        }
    }
    // ================= ВЕТКА 3: без группировки =================
    else {
        for (const SentRecord &r : filtered) {
            QStandardItem *order = new QStandardItem(r.orderNumber);
            order->setEditable(false);

            QStandardItem *nameAndArticle = new QStandardItem(
                r.article + "  " + r.name);
            nameAndArticle->setEditable(false);

            QStandardItem *pt = new QStandardItem(formatPoints(r.points));
            pt->setEditable(false);
            pt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            pt->setData(r.id,     Qt::UserRole);
            pt->setData(r.points, Qt::UserRole + 1);
            pt->setToolTip(QStringLiteral("Двойной клик — изменить баллы"));

            QStandardItem *dt = new QStandardItem(
                r.sentAt.toString("dd.MM.yyyy"));
            dt->setEditable(false);

            m_model->appendRow({ order, nameAndArticle, pt, dt });
        }
    }

    // Раскрыть все группы
    ui->statsTree->expandAll();

    // Настройка колонок
    QHeaderView *hdr = ui->statsTree->header();
    hdr->setSectionResizeMode(0, QHeaderView::Interactive);
    hdr->setSectionResizeMode(1, QHeaderView::Stretch);
    hdr->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    hdr->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    ui->statsTree->setColumnWidth(0, 220);
    ui->statsTree->setColumnWidth(2, 70);
    ui->statsTree->setColumnWidth(3, 150);

    // Период
    QString periodStr;
    if (from.isValid() && to.isValid())
        periodStr = QString("%1 — %2")
                        .arg(from.toString("dd.MM.yyyy"),
                             to.toString("dd.MM.yyyy"));
    else if (from.isValid())
        periodStr = QString("с %1").arg(from.toString("dd.MM.yyyy"));
    else if (to.isValid())
        periodStr = QString("до %1").arg(to.toString("dd.MM.yyyy"));
    else
        periodStr = QStringLiteral("всё время");
    ui->labelPeriodValue->setText(periodStr);

    // Итоги: заказы, изделия, баллы + строка "Итого: ..."
    m_updating = false;
    recalcTotals();
}

QString StatisticsDialog::pluralizeItems(int n)
{
    const int lastDigit   = n % 10;
    const int lastTwoDigs = n % 100;
    QString word;
    if (lastTwoDigs >= 11 && lastTwoDigs <= 14)
        word = QStringLiteral("изделий");
    else if (lastDigit == 1)
        word = QStringLiteral("изделие");
    else if (lastDigit >= 2 && lastDigit <= 4)
        word = QStringLiteral("изделия");
    else
        word = QStringLiteral("изделий");
    return QString("%1 %2").arg(n).arg(word);
}

void StatisticsDialog::onExportCsvClicked()
{
    const QString path = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Сохранить как CSV"),
        QStringLiteral("statistics.csv"),
        QStringLiteral("CSV (*.csv)"));

    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"),
                             QStringLiteral("Не удалось открыть файл:\n%1").arg(path));
        return;
    }

    QTextStream out(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    // BOM — чтобы Excel корректно распознал UTF-8
    out << QChar(0xFEFF);

    // Заголовок
    out << "Заказ;Артикул;Наименование;Баллы;Отправлено\n";

    // Пробегаем по модели
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QStandardItem *orderRow = m_model->item(i);
        const QString orderLabel = orderRow->text();   // "Заказ № 220"
        for (int j = 0; j < orderRow->rowCount(); ++j) {
            QStandardItem *child = orderRow->child(j);
            out << orderLabel                      << ';'
                << child->text()                   << ';'   // артикул
                << orderRow->child(j, 1)->text()   << ';'   // наименование
                << orderRow->child(j, 2)->text()   << ';'   // баллы
                << orderRow->child(j, 3)->text()   << '\n'; // дата
        }
    }
    f.close();

    QMessageBox::information(this, QStringLiteral("Готово"),
                             QStringLiteral("Файл сохранён:\n%1").arg(path));
}

void StatisticsDialog::onCopyClicked()
{
    QStringList lines;
    lines << "Заказ\tАртикул\tНаименование\tБаллы\tОтправлено";

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QStandardItem *orderRow = m_model->item(i);
        const QString orderLabel = orderRow->text();
        for (int j = 0; j < orderRow->rowCount(); ++j) {
            lines << QString("%1\t%2\t%3\t%4\t%5")
            .arg(orderLabel)
                .arg(orderRow->child(j, 0)->text())
                .arg(orderRow->child(j, 1)->text())
                .arg(orderRow->child(j, 2)->text())
                .arg(orderRow->child(j, 3)->text());
        }
    }

    QApplication::clipboard()->setText(lines.join('\n'));
    QMessageBox::information(this,
                             QStringLiteral("Готово"),
                             QStringLiteral("Данные скопированы в буфер обмена"));
}
QString StatisticsDialog::pluralizeOrders(int n)
{
    const int d  = n % 10;
    const int dd = n % 100;
    if (dd >= 11 && dd <= 14) return QStringLiteral("заказов");
    if (d == 1)               return QStringLiteral("заказ");
    if (d >= 2 && d <= 4)     return QStringLiteral("заказа");
    return QStringLiteral("заказов");
}

QString StatisticsDialog::pluralizeDays(int n)
{
    const int d  = n % 10;
    const int dd = n % 100;
    if (dd >= 11 && dd <= 14) return QStringLiteral("дней");
    if (d == 1)               return QStringLiteral("день");
    if (d >= 2 && d <= 4)     return QStringLiteral("дня");
    return QStringLiteral("дней");
}

QString StatisticsDialog::pluralizePoints(int n)
{
    const int d  = n % 10;
    const int dd = n % 100;
    if (dd >= 11 && dd <= 14) return QStringLiteral("баллов");
    if (d == 1)               return QStringLiteral("балл");
    if (d >= 2 && d <= 4)     return QStringLiteral("балла");
    return QStringLiteral("баллов");
}

void StatisticsDialog::onItemChanged(QStandardItem *item)
{
    if (!item || m_updating) return;
    if (item->column() != 2) return;

    QStandardItem *parent = item->parent();

    const int recordId = item->data(Qt::UserRole).toInt();
    if (recordId <= 0) return;

    bool ok = false;
    const int newPoints = item->text().trimmed().toInt(&ok);
    const int oldPoints = item->data(Qt::UserRole + 1).toInt();

    if (!ok || newPoints < 0 || newPoints > 9999) {
        QMessageBox::warning(this, QStringLiteral("Некорректное значение"),
                             QStringLiteral("Введите целое число от 0 до 9999."));
        m_updating = true;
        item->setText(QString::number(oldPoints));
        m_updating = false;
        return;
    }

    if (newPoints == oldPoints) return;

    if (!m_db->updateSentPoints(recordId, newPoints)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"),
                             QStringLiteral("Не удалось сохранить баллы в БД."));
        m_updating = true;
        item->setText(QString::number(oldPoints));
        m_updating = false;
        return;
    }

    m_updating = true;
    item->setData(newPoints, Qt::UserRole + 1);

    if (parent) {
        int groupPoints = 0;
        for (int j = 0; j < parent->rowCount(); ++j) {
            if (QStandardItem *ptCell = parent->child(j, 2))
                groupPoints += ptCell->text().toInt();
        }

        const QModelIndex parentIdx       = parent->index();
        const QModelIndex parentPointsIdx = parentIdx.siblingAtColumn(2);

        if (QStandardItem *parentPointsCell = m_model->itemFromIndex(parentPointsIdx)) {
            parentPointsCell->setText(QString::number(groupPoints));
        }
    }
    m_updating = false;

    recalcTotals();
}

void StatisticsDialog::recalcTotals()
{
    int    totalOrders = 0;
    int    totalItems  = 0;
    double totalPoints = 0.0;

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QStandardItem *firstCol = m_model->item(i, 0);
        if (!firstCol) continue;

        if (firstCol->rowCount() > 0) {
            totalOrders++;
            for (int j = 0; j < firstCol->rowCount(); ++j) {
                QStandardItem *ptCell = firstCol->child(j, 2);
                if (!ptCell) continue;
                totalItems++;
                totalPoints += ptCell->text().toDouble();
            }
        } else {
            QStandardItem *ptCell = m_model->item(i, 2);
            if (!ptCell) continue;
            if (ptCell->data(Qt::UserRole).toInt() <= 0) continue;
            totalItems++;
            totalPoints += ptCell->text().toDouble();
        }
    }

    ui->labelOrdersValue->setText(QString::number(totalOrders));
    ui->labelItemsValue->setText(QString::number(totalItems));
    ui->labelPointsValue->setText(formatPoints(totalPoints));

    QString ordersLabel;
    if (ui->radioByDate->isChecked())
        ordersLabel = QString("%1 %2").arg(totalOrders).arg(pluralizeDays(totalOrders));
    else
        ordersLabel = QString("%1 %2").arg(totalOrders).arg(pluralizeOrders(totalOrders));

    m_totalLabel->setText(
        QStringLiteral("Итого:  %1  ·  %2  ·  %3 %4")
            .arg(ordersLabel,
                 pluralizeItems(totalItems),
                 formatPoints(totalPoints),
                 pluralizePoints(static_cast<int>(std::round(totalPoints)))));
}

void StatisticsDialog::onTreeDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    if (index.column() != 2) return;

    QStandardItem *item = m_model->itemFromIndex(index);
    if (!item) return;

    const int recordId = item->data(Qt::UserRole).toInt();
    if (recordId <= 0) return;

    const double oldPoints = item->data(Qt::UserRole + 1).toDouble();

    bool ok = false;
    const double newPoints = QInputDialog::getDouble(
        this,
        QStringLiteral("Изменение баллов"),
        QStringLiteral("Введите новое количество баллов:"),
        oldPoints,
        0.0,         // минимум
        9999.0,      // максимум
        1,           // число знаков после точки
        &ok);

    if (!ok || qAbs(newPoints - oldPoints) < 0.0001) return;

    if (!m_db->updateSentPoints(recordId, newPoints)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"),
                             QStringLiteral("Не удалось сохранить баллы в БД."));
        return;
    }

    m_updating = true;
    item->setData(formatPoints(newPoints), Qt::DisplayRole);
    item->setData(newPoints, Qt::UserRole + 1);

    if (QStandardItem *parent = item->parent()) {
        double groupPoints = 0.0;
        for (int j = 0; j < parent->rowCount(); ++j) {
            if (QStandardItem *ptCell = parent->child(j, 2)) {
                // Формат "5.5" или "5" — toDouble() справится
                groupPoints += ptCell->text().toDouble();
            }
        }
        const QModelIndex parentIdx       = parent->index();
        const QModelIndex parentPointsIdx = parentIdx.siblingAtColumn(2);
        if (QStandardItem *parentPointsCell = m_model->itemFromIndex(parentPointsIdx)) {
            parentPointsCell->setData(formatPoints(groupPoints), Qt::DisplayRole);
        }
    }
    m_updating = false;

    recalcTotals();
}

QString StatisticsDialog::formatPoints(double v)
{
    // Целые показываем без .0, дробные — с одним знаком
    if (qAbs(v - std::round(v)) < 0.001)
        return QString::number(static_cast<qlonglong>(std::round(v)));
    return QString::number(v, 'f', 1);
}