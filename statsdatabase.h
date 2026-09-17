#ifndef STATSDATABASE_H
#define STATSDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QList>

struct WorkSession {
    int id = -1;
    QString itemNumber;
    int sketchTimeSec = 0;      // общее время эскиза
    QDateTime sketchStart;
    QDateTime sketchEnd;
    int drawingTimeSec = 0;     // общее время чертежей
    QDateTime drawingStart;
    QDateTime drawingEnd;
    int totalTimeSec = 0;
};

struct SentRecord {
    int id = -1;
    QString article;       // "220.04(1-5B)"
    QString name;          // "M-2.11 Хранение в нише"
    QString orderNumber;   // "220"
    int points = 0;        // 5
    QDateTime sentAt;      // дата и время отправки
};

class StatsDatabase : public QObject
{
    Q_OBJECT
public:
    explicit StatsDatabase(QObject *parent = nullptr);
    ~StatsDatabase();

    bool init(const QString &dbPath);
    bool startSession(const QString &itemNumber, bool isSketch); // isSketch = true (эскиз), false (чертежи)
    bool stopSession(const QString &itemNumber, bool isSketch, int elapsedSec);
    WorkSession getSession(const QString &itemNumber);
    QList<WorkSession> getAllSessions();
     int getTotalTime(const QString &itemNumber, bool isSketch);

    // Запись факта отправки изделия в ТО
    bool recordSentToTO(const QString &article,
                        const QString &name,
                        const QString &orderNumber,
                        int points);

    // Получение отправок за период (from — включительно, to — включительно)
    QList<SentRecord> findSentRecords(const QDate &from,
                                      const QDate &to,
                                      const QString &articleLike = QString());


private:
    QSqlDatabase m_db;
    bool createTables();
    int getActiveSessionId(const QString &itemNumber, bool isSketch);

};

#endif // STATSDATABASE_H