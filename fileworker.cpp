#include "fileworker.h"
#include <QFile>
#include <QDebug>
#include <QDir>

// 生成安全的目录名（移除非法字符）
QString safeDirectoryName(const int &id) ;

FileWorker::FileWorker(QObject *parent) : QObject(parent)
{

}

void FileWorker::saveDataChunk(QVector<double> data)
{
    QMutexLocker locker(&m_mutex);
    qDebug()<<"保存数据开始" ;
    QString fileName = generateFileName();
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("文件创建失败: " + file.errorString());
        qDebug()<<"保存数据失败"+file.errorString() ;
        return;
    }

    QTextStream out(&file);
    for (double value : data) {
        out << value << "\n";
    }
    file.close();

    emit saveCompleted(fileName);
}

void FileWorker::saveDataChunkWithId(int id, QVector<double> data)
{
    QMutexLocker locker(&m_mutex);
    qDebug()<<"保存数据开始" ;

    // 确保ID是安全的目录名
    QString safeId = safeDirectoryName(id);

    // 创建目录路径
    QString dirPath = "./data" + safeId;
    QDir dir;

    // 如果目录不存在，则创建它
    if (!dir.exists(dirPath)) {
        if (!dir.mkpath(dirPath)) {
            qDebug() << "创建目录失败:" << dirPath;
        }
    }

//    QString fileName = "./data"+QString(id)+"/"+generateFileName();
    // 生成完整的文件路径
    QString fileName = dirPath + "/" + generateFileName();
    qDebug()<<"Path:"<<fileName ;
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("文件创建失败: " + file.errorString());
        qDebug()<<"保存数据失败"+file.errorString() ;
        return;
    }

    QTextStream out(&file);
    for (double value : data) {
        out << value << "\n";
    }
    file.close();

    emit saveCompleted(fileName);
}

// 生成安全的目录名（移除非法字符）
QString safeDirectoryName(const int &id) {
    // 移除路径中的非法字符（Windows和Unix系统略有不同）
    QString safeId = QString::number(id);
    QVector<QChar> illegalChars = {'\\', '/', ':', '*', '?', '"', '<', '>', '|', '\0'};

    for (QChar &c : safeId) {
        if (illegalChars.contains(c) || c.unicode() < 32) {
            c = '_'; // 用下划线替换非法字符
        }
    }

    return safeId;
}
