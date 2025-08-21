#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qmqtt.h"
#include "qcustomplot.h"

// JSON解析
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

// JSON 解析
#include "jsonparser.h"

// 子线程
#include "QThread"

#include "dataprocesser.h"
#include "fileworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QMQTT::Client *pxMqttClient = nullptr ;

    QString mqttUrl = "43.136.183.228";
    int     mqttPort = 1883 ;

    QString mqttClientID = "QT-MQTT-CLIENT-16623032521";
    QString mqttUser     = "EricZ0603";
    QString mqttPawd     = "EricZ0603..";

    QString mqttTopic    = "IGSV_C_CLIENT_TO_SERVER";
    QString mqttSubTopic = "IGSV_C_SERVER_TO_CLIENT"; // 订阅Topic

    QVector<double> doubleRxSensorPressBuffer_1; // 传感器压力数据 1
    QVector<double> doubleRxSensorPressBuffer_2; // 传感器压力数据 2

    // JSON解析类
    JsonParser *pxJsonParser = nullptr;

    QThread *pxMqttThread = nullptr ;

    DataProcesser *m_processor; // 文件导入类
    DataProcesser *m_processor_2 ;
    FileWorker *m_fileWorker;   // 文件保存类
    QThread *m_workerThread;

private slots:

    void MQTT_ClientConnected();

    void MQTT_ClientDisconnected();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void MQTT_SubscribedSuccess( QString topic , quint8 Qos );
    void MQTT_RecvMsg(QMQTT::Message msg); //接收消息处理

    // 绘图窗口与曲线初始化
    void QCustomPlotWaveInit(QCustomPlot *wave);
    // 绘图曲线绘制
    void QCustomPlotWaveUpdate(QCustomPlot *wave, QVector<double> yAxis);

    // DataUpdate 数据集合更新
    void MQTT_RX_BufferUpdate( QVector<double> &buffer , double value );

    void QCustomPlot_UI_Update( QCustomPlot *wave , QVector<double> buffer , int size );

    void QCustomPlot_SetData( QVector<double> buffer_1 , QVector<double> buffer_2 , int size );

    void QCustomPlot_SIGN_Update();
    void on_pushButton_3_clicked();

    void onStartClicked(); // 开始测试按钮
    void onImportClicked(); // 导入文件按钮
    void handleSaveCompleted(const QString &fileName); // 保存完成标记

signals:

    void QCustomPlot_WaveUpdateSignal();

};
#endif // MAINWINDOW_H
