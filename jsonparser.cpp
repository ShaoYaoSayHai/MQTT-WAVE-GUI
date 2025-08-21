#include "jsonparser.h"
#include <QDebug>

JsonParser::JsonParser(QObject *parent) : QObject(parent) , m_press1(0.0), m_press2(0.0) , m_hasValidData(false)
{

}

bool JsonParser::parseJsonString(const QString &jsonString)
{
    // 重置状态
    m_hasValidData = false;
    m_press1 = 0.0;
    m_press2 = 0.0;

    // 解析 JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());

    if(jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Failed to parse JSON";
        emit dataParsed(false);
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 提取 press_1
    if(jsonObj.contains("press_1")) {
        QJsonValue value = jsonObj.value("press_1");
        if(value.isDouble()) {
            m_press1 = value.toDouble();
        } else {
            qDebug() << "press_1 is not a number";
            emit dataParsed(false);
            return false;
        }
    } else {
        qDebug() << "Missing press_1 field";
        emit dataParsed(false);
        return false;
    }

    // 提取 press_2
    if(jsonObj.contains("press_2")) {
        QJsonValue value = jsonObj.value("press_2");
        if(value.isDouble()) {
            m_press2 = value.toDouble();
        } else {
            qDebug() << "press_2 is not a number";
            emit dataParsed(false);
            return false;
        }
    } else {
        qDebug() << "Missing press_2 field";
        emit dataParsed(false);
        return false;
    }

    m_hasValidData = true;
    emit dataParsed(true);
    return true;
}

