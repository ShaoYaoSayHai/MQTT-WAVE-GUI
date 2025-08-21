#include "qttimeoputils.h"
#include "QDateTime"
#include <QFile>

QtTimeOpUtils::QtTimeOpUtils(QObject *parent) : QObject(parent)
{

}

QString QtTimeOpUtils::generateTimestampFileName()
{
    QDateTime now = QDateTime::currentDateTime();
    // 精确到毫秒避免重名
    QString timestamp = now.toString("yyyyMMdd_hhmmsszzz");
    return QString("数据_%1.txt").arg(timestamp);
}
