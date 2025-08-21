#ifndef QTTIMEOPUTILS_H
#define QTTIMEOPUTILS_H

#include <QObject>

class QtTimeOpUtils : public QObject
{
    Q_OBJECT
public:
    explicit QtTimeOpUtils(QObject *parent = nullptr);

    // 生成带毫秒级时间戳的文件名 (格式：数据_yyyyMMdd_hhmmsszzz.txt)
    QString generateTimestampFileName() ;

signals:

};

#endif // QTTIMEOPUTILS_H
