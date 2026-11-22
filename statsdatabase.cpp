#include "statsdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QDate>
#include <QDebug>

StatsDatabase::StatsDatabase(QObject *parent) : QObject(parent) {}

StatsDatabase::~StatsDatabase() {
    if (m_db.isOpen()) m_db.close();
}

bool StatsDatabase::init(const QString &dbPath)
{
    // 1. Проверяем, существует ли уже соединение с таким именем.
    // Это нужно, чтобы при смене базы (через настройки) не создавать дубликаты.
    if (QSqlDatabase::contains("stats_connection")) {
        m_db = QSqlDatabase::database("stats_connection");
        if (m_db.isOpen()) {
            m_db.close();
        }
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "stats_connection");
    }

    // 2. Устанавливаем путь к базе данных (локальный или сетевой)
    m_db.setDatabaseName(dbPath);

    // 3. Пытаемся открыть
    if (!m_db.open()) {
        qWarning() << "Не удалось открыть базу данных:" << m_db.lastError().text()
        << "Путь:" << dbPath;
        return false;
    }

    // 4. Настройка PRAGMA для работы в сети и целостности данных
    QSqlQuery q(m_db);

    // Включаем режим WAL для поддержки одновременного чтения и записи
    if (!q.exec("PRAGMA journal_mode=WAL;")) {
        qWarning() << "Не удалось включить WAL режим:" << q.lastError().text();
    }

    // Включаем проверку внешних ключей (нужно для связи user_id -> users.id)
    q.exec("PRAGMA foreign_keys = ON;");

    // Если база заблокирована другим пользователем, ждем до 5 секунд, прежде чем выдать ошибку
    q.exec("PRAGMA busy_timeout = 5000;");

    // 5. Создаем таблицы, если база пустая (например, новый пользователь)
    return createTables();
}

bool StatsDatabase::createTables()
{
    QSqlQuery q(m_db);

    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS work_sessions ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "item_number TEXT NOT NULL,"
            "sketch_time_sec INTEGER DEFAULT 0,"
            "sketch_start DATETIME,"
            "sketch_end DATETIME,"
            "drawing_time_sec INTEGER DEFAULT 0,"
            "drawing_start DATETIME,"
            "drawing_end DATETIME,"
            "total_time_sec INTEGER DEFAULT 0"
            ")")) {
        return false;
    }

    if (!q.exec(
            "CREATE TABLE IF NOT EXISTS sent_to_to ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "article TEXT NOT NULL,"       // артикул с баллами, "220.04(1-5B)"
            "name TEXT,"                   // наименование
            "order_number TEXT,"           // "220"
            "points INTEGER DEFAULT 0,"    // 5
            "sent_at DATETIME NOT NULL"
            ")")) {
        return false;
    }

    // Индекс по дате — ускорит выборку по периоду
    q.exec("CREATE INDEX IF NOT EXISTS idx_sent_at ON sent_to_to(sent_at)");

    return true;
}

int StatsDatabase::getActiveSessionId(const QString &itemNumber, bool isSketch) {
    QSqlQuery q(m_db);
    QString field = isSketch ? "sketch_start" : "drawing_start";
    QString endField = isSketch ? "sketch_end" : "drawing_end";
    q.prepare(QString("SELECT id FROM work_sessions WHERE item_number = ? AND %1 IS NOT NULL AND %2 IS NULL").arg(field, endField));
    q.addBindValue(itemNumber);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return -1;
}

bool StatsDatabase::startSession(const QString &itemNumber, bool isSketch) {
    if (itemNumber.isEmpty()) return false;
    // Проверяем, нет ли уже активной сессии (не остановленной)
    if (getActiveSessionId(itemNumber, isSketch) != -1) {
        qWarning() << "Session already active for" << itemNumber << (isSketch ? "sketch" : "drawing");
        return false;
    }
    QSqlQuery q(m_db);
    QString startField = isSketch ? "sketch_start" : "drawing_start";
    q.prepare(QString("INSERT INTO work_sessions (item_number, %1) VALUES (?, datetime('now','localtime'))").arg(startField));
    q.addBindValue(itemNumber);
    return q.exec();
}

bool StatsDatabase::stopSession(const QString &itemNumber, bool isSketch, int elapsedSec) {
    int sessionId = getActiveSessionId(itemNumber, isSketch);
    if (sessionId == -1) {
        qWarning() << "No active session to stop for" << itemNumber;
        return false;
    }
    QSqlQuery q(m_db);
    QString timeField = isSketch ? "sketch_time_sec" : "drawing_time_sec";
    QString endField = isSketch ? "sketch_end" : "drawing_end";
    q.prepare(QString("UPDATE work_sessions SET %1 = %1 + ?, %2 = datetime('now','localtime'), total_time_sec = total_time_sec + ? WHERE id = ?")
                  .arg(timeField, endField));
    q.addBindValue(elapsedSec);
    q.addBindValue(elapsedSec);
    q.addBindValue(sessionId);
    return q.exec();
}

WorkSession StatsDatabase::getSession(const QString &itemNumber) {
    WorkSession ws;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM work_sessions WHERE item_number = ? ORDER BY id DESC LIMIT 1");
    q.addBindValue(itemNumber);
    if (q.exec() && q.next()) {
        ws.id = q.value(0).toInt();
        ws.itemNumber = q.value(1).toString();
        ws.sketchTimeSec = q.value(2).toInt();
        ws.sketchStart = q.value(3).toDateTime();
        ws.sketchEnd = q.value(4).toDateTime();
        ws.drawingTimeSec = q.value(5).toInt();
        ws.drawingStart = q.value(6).toDateTime();
        ws.drawingEnd = q.value(7).toDateTime();
        ws.totalTimeSec = q.value(8).toInt();
    }
    return ws;
}

QList<WorkSession> StatsDatabase::getAllSessions() {
    QList<WorkSession> list;
    QSqlQuery q(m_db);
    if (q.exec("SELECT * FROM work_sessions ORDER BY id")) {
        while (q.next()) {
            WorkSession ws;
            ws.id = q.value(0).toInt();
            ws.itemNumber = q.value(1).toString();
            ws.sketchTimeSec = q.value(2).toInt();
            ws.sketchStart = q.value(3).toDateTime();
            ws.sketchEnd = q.value(4).toDateTime();
            ws.drawingTimeSec = q.value(5).toInt();
            ws.drawingStart = q.value(6).toDateTime();
            ws.drawingEnd = q.value(7).toDateTime();
            ws.totalTimeSec = q.value(8).toInt();
            list.append(ws);
        }
    }
    return list;
}

int StatsDatabase::getTotalTime(const QString &itemNumber, bool isSketch)
{
    QString field = isSketch ? "sketch_time_sec" : "drawing_time_sec";
    QSqlQuery q(m_db);
    q.prepare(QString("SELECT SUM(%1) FROM work_sessions WHERE item_number = ?").arg(field));
    q.addBindValue(itemNumber);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}

bool StatsDatabase::recordSentToTO(int userId, const QString &article, const QString &name,
                                   const QString &orderNumber, double points)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO sent_to_to (user_id, article, name, order_number, points, sent_at) "
              "VALUES (?, ?, ?, ?, ?, datetime('now','localtime'))");
    q.addBindValue(userId);
    q.addBindValue(article);
    q.addBindValue(name);
    q.addBindValue(orderNumber);
    q.addBindValue(points);
    return q.exec();
}

QList<SentRecord> StatsDatabase::findSentRecords(int userId,
                                                 const QDate &from,
                                                 const QDate &to,
                                                 const QString &articleLike)
{
    QList<SentRecord> list;

    if (userId <= 0) {
        qWarning() << "findSentRecords: некорректный userId" << userId;
        return list;
    }

    // 1. Базовый запрос.
    // Обязательно фильтруем по user_id.
    // Подзапрос (SELECT MAX(id) ... GROUP BY article) берет только последнюю отправку для каждого артикула
    // (чтобы не было дублей, если изделие отправляли несколько раз).
    QString sql =
        "SELECT s.id, s.article, s.name, s.order_number, s.points, s.sent_at "
        "FROM sent_to_to s "
        "WHERE s.user_id = ? "
        "AND s.id IN (SELECT MAX(id) FROM sent_to_to WHERE user_id = ? GROUP BY article)";

    // 2. Добавляем фильтры по дате, если они заданы
    if (from.isValid())
        sql += " AND date(s.sent_at) >= date(?)";
    if (to.isValid())
        sql += " AND date(s.sent_at) <= date(?)";

    // 3. Добавляем фильтр по поиску (артикул или наименование)
    if (!articleLike.isEmpty())
        sql += " AND (s.article LIKE ? OR s.name LIKE ?)";

    // 4. Сортировка: сначала новые, потом по заказу, потом по артикулу
    sql += " ORDER BY s.sent_at DESC, s.order_number ASC, s.article ASC";

    QSqlQuery q(m_db);
    q.prepare(sql);

    // 5. Привязываем параметры в том же порядке, в котором они идут в SQL-запросе
    q.addBindValue(userId); // для s.user_id = ?
    q.addBindValue(userId); // для подзапроса WHERE user_id = ?

    if (from.isValid())
        q.addBindValue(from.toString("yyyy-MM-dd"));
    if (to.isValid())
        q.addBindValue(to.toString("yyyy-MM-dd"));

    if (!articleLike.isEmpty()) {
        const QString like = "%" + articleLike + "%";
        q.addBindValue(like);
        q.addBindValue(like);
    }

    // 6. Выполняем и заполняем список
    if (q.exec()) {
        while (q.next()) {
            SentRecord r;
            r.id          = q.value(0).toInt();
            r.article     = q.value(1).toString();
            r.name        = q.value(2).toString();
            r.orderNumber = q.value(3).toString();
            r.points      = q.value(4).toDouble(); // toDouble для поддержки 3.5
            r.sentAt      = q.value(5).toDateTime();
            list.append(r);
        }
    } else {
        qWarning() << "findSentRecords failed:" << q.lastError().text();
    }

    return list;
}
bool StatsDatabase::updateSentPoints(int id, double newPoints)
{
    if (id <= 0) return false;

    QSqlQuery q(m_db);
    q.prepare("UPDATE sent_to_to SET points = ? WHERE id = ?");
    q.addBindValue(newPoints);
    q.addBindValue(id);
    if (!q.exec()) {
        qWarning() << "updateSentPoints failed:" << q.lastError().text();
        return false;
    }
    return true;
}
int StatsDatabase::getUserId(const QString &username) {
    QSqlQuery q(m_db);
    q.prepare("SELECT id FROM users WHERE username = ?");
    q.addBindValue(username);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return -1; // Не найден
}

int StatsDatabase::getOrCreateUserId(const QString &username)
{
    if (username.isEmpty()) return -1;

    QSqlQuery q(m_db);

    // 1. Пытаемся найти пользователя
    q.prepare("SELECT id FROM users WHERE username = ?");
    q.addBindValue(username);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }

    // 2. Если не нашли — создаем нового
    q.prepare("INSERT INTO users (username) VALUES (?)");
    q.addBindValue(username);
    if (q.exec()) {
        return q.lastInsertId().toInt();
    }

    return -1;
}