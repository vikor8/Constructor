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

private:
    QSqlDatabase m_db;
    bool createTables();
    int getActiveSessionId(const QString &itemNumber, bool isSketch);

};

#endif // STATSDATABASE_H