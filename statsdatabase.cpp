#include "statsdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>

StatsDatabase::StatsDatabase(QObject *parent) : QObject(parent) {}

StatsDatabase::~StatsDatabase() {
    if (m_db.isOpen()) m_db.close();
}

bool StatsDatabase::init(const QString &dbPath) {
    m_db = QSqlDatabase::addDatabase("QSQLITE", "stats_connection");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qWarning() << "Cannot open stats database:" << m_db.lastError().text();
        return false;
    }
    return createTables();
}

bool StatsDatabase::createTables() {
    QSqlQuery q(m_db);
    return q.exec(
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
        ")"
        );
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