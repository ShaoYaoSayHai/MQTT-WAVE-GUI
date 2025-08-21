#ifndef DATAPROCESSER_H
#define DATAPROCESSER_H

#include <QObject>

class DataProcesser : public QObject
{
    Q_OBJECT
public:
    explicit DataProcesser( int id ,  QObject *parent = nullptr);

    void appendData(double value);

signals:

    void chunkReady(const QVector<double> &chunk);

    void chunkReadyWidthId( int id , const QVector<double> &chunk );

private:

    QVector<double> m_buffer;
    const int CHUNK_SIZE = 100;

    int process_id ;

};

#endif // DATAPROCESSER_H
