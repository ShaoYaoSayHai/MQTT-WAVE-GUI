#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>



class JsonParser : public QObject
{
    Q_OBJECT
public:
    explicit JsonParser(QObject *parent = nullptr);

    // 解析 JSON 字符串
    bool parseJsonString(const QString &jsonString);

    // 获取解析后的值
    double getPress1() const { return m_press1; }
    double getPress2() const { return m_press2; }

    // 检查是否有有效数据
    bool hasValidData() const { return m_hasValidData; }

private:

    double m_press1;
    double m_press2;
    bool m_hasValidData;

signals:

    void dataParsed(bool success);

signals:

};

#endif // JSONPARSER_H
