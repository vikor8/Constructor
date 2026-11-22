#ifndef STATSDATABASE_H
#define STATSDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QList>
#include <QDate>

struct WorkSession {
    int id = -1;
    QString itemNumber;
    int sketchTimeSec = 0;
    QDateTime sketchStart;
    QDateTime sketchEnd;
    int drawingTimeSec = 0;
    QDateTime drawingStart;
    QDateTime drawingEnd;
    int totalTimeSec = 0;
};

struct SentRecord {
    int id = -1;
    QString article;
    QString name;
    QString orderNumber;
    double points = 0.0;
    QDateTime sentAt;
};

class StatsDatabase : public QObject
{
    Q_OBJECT
public:
    explicit StatsDatabase(QObject *parent = nullptr);
    ~StatsDatabase();

    bool init(const QString &dbPath);

    // Возвращает указатель на текущую БД (для прямых запросов)
    QSqlDatabase database() const { return m_db; }

    // --- Отправки в ТО ---
    // Добавили userId первым параметром
    bool recordSentToTO(int userId,
                        const QString &article,
                        const QString &name,
                        const QString &orderNumber,
                        double points);

    bool updateSentPoints(int id, double newPoints);

    bool updateSentRecord(int id,
                          const QString &article,
                          const QString &name,
                          const QString &orderNumber);

    // Добавили userId первым параметром
    QList<SentRecord> findSentRecords(int userId,
                                      const QDate &from,
                                      const QDate &to,
                                      const QString &articleLike);

    // --- Рабочие сессии (таймер) ---
    bool startSession(const QString &itemNumber, bool isSketch);
    bool stopSession(const QString &itemNumber, bool isSketch, int elapsedSec);
    WorkSession getSession(const QString &itemNumber);
    QList<WorkSession> getAllSessions();
    int getTotalTime(const QString &itemNumber, bool isSketch);

    // --- Пользователи ---
    int getUserId(const QString &username);
    int getOrCreateUserId(const QString &username);
    QList<QPair<int, QString>> getUsers(bool includeHidden = false);
    bool setUserActive(int userId, bool active);

private:
    QSqlDatabase m_db;
    bool createTables();
    int getActiveSessionId(const QString &itemNumber, bool isSketch);
};

#endif // STATSDATABASE_H