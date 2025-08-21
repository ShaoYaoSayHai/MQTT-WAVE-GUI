#ifndef FILEWORKER_H
#define FILEWORKER_H


#include <QObject>
#include <QDateTime>
#include <QMutex>
#include <QVector>

class FileWorker : public QObject
{
    Q_OBJECT
public:
    explicit FileWorker(QObject *parent = nullptr);

public slots:

    // 接收数据并保存
    void saveDataChunk( QVector<double> data );

    void saveDataChunkWithId( int id , QVector<double> data );

signals:

    void saveCompleted(const QString &fileName);
    void errorOccurred(const QString &error);

private:

    uint16_t fileCnt = 0 ;

    QString generateFileName() {
        return "data_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz") + ".txt";
    }

    QMutex m_mutex; // 互斥锁

};

#endif // FILEWORKER_H
