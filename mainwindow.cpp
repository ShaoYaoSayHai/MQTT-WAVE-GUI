#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

double Math_MaxValue( QVector<double> buffer );
double Math_MinValue( QVector<double> buffer );

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pxMqttClient = new QMQTT::Client();

    pxMqttClient->setHost(QHostAddress(this->mqttUrl)) ;
    pxMqttClient->setPort(this->mqttPort);
    pxMqttClient->setClientId(this->mqttClientID);
    pxMqttClient->setUsername(this->mqttUser);
    pxMqttClient->setPassword(this->mqttPawd.toUtf8());

    connect( pxMqttClient , SIGNAL(connected()) , this , SLOT(MQTT_ClientConnected()) );
    connect( pxMqttClient , SIGNAL(received(QMQTT::Message))   , this , SLOT(MQTT_RecvMsg(QMQTT::Message)));
    connect( pxMqttClient , SIGNAL(subscribed(QString,quint8)) , this , SLOT(MQTT_SubscribedSuccess(QString,quint8) ) );

    pxMqttClient->setCleanSession(true);
    pxMqttClient->connectToHost();

//    connect( this , &MainWindow::QCustomPlot_WaveUpdateSignal , this , &MainWindow::QCustomPlot_SetData );

    // 初始化绘图1与绘图2
    QCustomPlotWaveInit( ui->QCustomPlot_1 );
    QCustomPlotWaveInit( ui->QCustomPlot_2 );
    QCustomPlotWaveInit( ui->QCustomPlot_1 );

    QPen pen1;
    pen1.setColor(QColor(Qt::yellow));
    pen1.setWidth(3);
    pen1.setStyle(Qt::SolidLine);     // 设置线条样式为点划线
    pen1.setCapStyle(Qt::RoundCap);   // 设置线条末端为圆形
    pen1.setJoinStyle(Qt::BevelJoin); // 设置线条连接点为斜角连接
    ui->QCustomPlot_1->graph(1)->setPen(pen1);

    QPen pen2;
    pen2.setColor(QColor(Qt::yellow));
    pen2.setWidth(3);
    pen2.setStyle(Qt::SolidLine);     // 设置线条样式为点划线
    pen2.setCapStyle(Qt::RoundCap);   // 设置线条末端为圆形
    pen2.setJoinStyle(Qt::BevelJoin); // 设置线条连接点为斜角连接
    ui->QCustomPlot_1->graph(0)->setPen(pen2);

    // 假设你从某个地方获取了 JSON 字符串
    QString jsonData = "{\"press_1\" : 2132.99, \"press_2\" : 2236.111}";

    // 创建解析器实例
    JsonParser parser;
    if(parser.parseJsonString(jsonData)) {
        double press1 = parser.getPress1();
        double press2 = parser.getPress2();

        qDebug() << "Parsed values:";
        qDebug() << "Press 1:" << press1;
        qDebug() << "Press 2:" << press2;
    } else {
        qDebug() << "Failed to parse JSON data";
    }

    pxJsonParser = new JsonParser();

/* ================================================================================================= */
/* ========================================= 文件处理 BEGIN ========================================== */

    // 初始化组件
    m_processor = new DataProcesser(1,this);
    m_fileWorker = new FileWorker;
    m_workerThread = new QThread(this);

    m_processor_2 = new DataProcesser(2,this);

    // 将Worker移至子线程
    m_fileWorker->moveToThread(m_workerThread);
    m_workerThread->start();

    // 信号槽连接
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(ui->btnImport, &QPushButton::clicked, this, &MainWindow::onImportClicked);
//    connect(m_processor, &DataProcesser::chunkReady, m_fileWorker, &FileWorker::saveDataChunk);
    connect(m_fileWorker, &FileWorker::saveCompleted, this, &MainWindow::handleSaveCompleted);

//    connect(m_processor_2 , &DataProcesser::chunkReady , m_fileWorker , &FileWorker::saveDataChunk) ;// 第二个保存数据

    connect(m_processor, &DataProcesser::chunkReadyWidthId, m_fileWorker, &FileWorker::saveDataChunkWithId);
    connect(m_processor_2, &DataProcesser::chunkReadyWidthId, m_fileWorker, &FileWorker::saveDataChunkWithId);

    // 资源清理
    connect(m_workerThread, &QThread::finished, m_fileWorker, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);

/* ================================================================================================= */
/* ========================================= 文件处理 END  ========================================== */

}

MainWindow::~MainWindow()
{
    if( pxJsonParser ){
        pxJsonParser->deleteLater();
    }

    if( pxMqttClient ){
        pxMqttClient->deleteLater();
    }

    m_workerThread->quit();
    m_workerThread->wait();

    delete ui;
}

void MainWindow::MQTT_ClientConnected()
{
    qDebug()<<"连接成功";
}

void MainWindow::MQTT_ClientDisconnected()
{
    qDebug()<<"连接断开";
}


void MainWindow::on_pushButton_clicked()
{
    QMQTT::Message msg ;

    msg.setTopic(this->mqttTopic);
    msg.setPayload("demo-qt-publish");
    pxMqttClient->publish(msg);

    qDebug()<<"订阅";
}

void MainWindow::on_pushButton_3_clicked()
{
    pxMqttClient->unsubscribe( this->mqttSubTopic) ;
}

void MainWindow::on_pushButton_2_clicked()
{
    pxMqttClient->subscribe( this->mqttSubTopic , 1 );
}

void MainWindow::MQTT_SubscribedSuccess(QString topic, quint8 Qos)
{
    Q_UNUSED(Qos);
    QString msg = "订阅主题 ";
    msg += topic;
    msg += " 成功";
    qDebug()<<msg ;
}

void MainWindow::MQTT_RecvMsg(QMQTT::Message msg)
{
    QString recv_msg = "Topic:";
    QString data = msg.payload();
    recv_msg += msg.topic();
    recv_msg += "    Payload:";
    recv_msg += data;
//    qDebug()<<recv_msg ;

    if(pxJsonParser->parseJsonString(data)) {
        double press1 = pxJsonParser->getPress1();
        double press2 = pxJsonParser->getPress2();
//        qDebug() << "Parsed values:";
//        qDebug() << "Press 1:" << press1;
//        qDebug() << "Press 2:" << press2;

        MQTT_RX_BufferUpdate( doubleRxSensorPressBuffer_1 , press1 - 96500 );
        MQTT_RX_BufferUpdate( doubleRxSensorPressBuffer_2 , press2 - 96500 );

        QCustomPlot_UI_Update( ui->QCustomPlot_1 , doubleRxSensorPressBuffer_1 , ui->spinBox_press_1->value() );
        QCustomPlot_UI_Update( ui->QCustomPlot_2 , doubleRxSensorPressBuffer_2 , ui->spinBox_press_2->value() );
//        int showSize = ui->spinBox_press_1->value() ;
//        QCustomPlot_SetData( doubleRxSensorPressBuffer_1 , doubleRxSensorPressBuffer_2, showSize );

        m_processor->appendData( press1 );
        m_processor_2->appendData(press2) ;

    } else {
        qDebug() << "Failed to parse JSON data";
    }



}

void MainWindow::QCustomPlotWaveInit(QCustomPlot *wave)
{
        wave->addGraph();             // 一条直线？
        wave->xAxis->setLabel("X轴"); // 设定X轴
        wave->yAxis->setLabel("Y轴"); // 设定Y轴
        // 设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
        wave->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                      QCP::iSelectLegend | QCP::iSelectPlottables);
        QPen pen;
        pen.setColor(QColor(Qt::yellow));
        pen.setWidth(3);
        pen.setStyle(Qt::SolidLine);     // 设置线条样式为点划线
        pen.setCapStyle(Qt::RoundCap);   // 设置线条末端为圆形
        pen.setJoinStyle(Qt::BevelJoin); // 设置线条连接点为斜角连接
        wave->graph(0)->setPen(pen);

        wave->setBackground(QBrush("black"));
        //===================================

        wave->xAxis->setBasePen(QPen(QColor(Qt::gray)));
        wave->xAxis->setTickLabelColor(QColor(Qt::gray));
        wave->xAxis->setLabelColor(QColor(Qt::gray));
        wave->yAxis->setBasePen(QPen(QColor(Qt::gray)));
        wave->yAxis->setTickLabelColor(QColor(Qt::gray));
        wave->yAxis->setLabelColor(QColor(Qt::gray));
}

/**
 * @brief userQCustomPlot::userQCustomPlotWave_Update 绘图更新
 * @param wave 绘图区域指针
 * @param yAxis 键值对
 */
void MainWindow::QCustomPlotWaveUpdate(QCustomPlot *wave, QVector<double> yAxis)
{
    QVector<QCPGraphData> *mData;
    mData = wave->graph(0)->data()->coreData();
    mData->clear();

    QCPGraphData newPoint;
    qint64 buf_size = yAxis.size();
    for (int i = 0; i < buf_size; i++)
    {
        newPoint.key = i;
        newPoint.value = yAxis[i];
        mData->append(newPoint);
    }
    wave->replot();
}


/**
 * @brief MainWindow::MQTT_RX_BufferUpdate 更新接收数据数组
 * @param buffer 数组
 * @param value  参数
 */
void MainWindow::MQTT_RX_BufferUpdate(QVector<double> &buffer, double value)
{
    buffer.append(value) ; // 追加数据到末尾
}

void MainWindow::QCustomPlot_UI_Update(QCustomPlot *wave, QVector<double> buffer , int size)
{
    int length = buffer.size();
    QVector<double> updateBuffer ;
    if( length > size )
    {
        updateBuffer = buffer.mid( ( length - size ) , length ); // 截取最后100个数据
    }
    else
    {
        updateBuffer = buffer ;
    }
    // 设置最大边界
    wave->yAxis->setRange( ( Math_MinValue( updateBuffer ) - 1 ) , ( Math_MaxValue(updateBuffer) + 1 ) );
//    wave->yAxis->setRange( ( 95900 ) , ( 97000 ) );
    wave->xAxis->setRange(-2, size + 2 );
    // 执行更新
    QCustomPlotWaveUpdate( wave , updateBuffer ) ;
}

void MainWindow::QCustomPlot_SIGN_Update()
{
    this->QCustomPlot_SetData(doubleRxSensorPressBuffer_1 , doubleRxSensorPressBuffer_2 , ui->spinBox_press_1->value());
}

void MainWindow::QCustomPlot_SetData(QVector<double> buffer_1, QVector<double> buffer_2, int size)
{
//    QCPGraph *graph_1 = ui->QCustomPlot_1->addGraph();
//    QCPGraph *graph_2 = ui->QCustomPlot_1->addGraph();
    qint64 buf_size = buffer_1.size();
    QVector<double> updateBuffer_1 ,updateBuffer_2 ;
    if( buf_size > size )
    {
        updateBuffer_1 = buffer_1.mid( ( buf_size - size ) , size ); // 截取最后100个数据
        updateBuffer_2 = buffer_2.mid( ( buf_size - size ) , size ); // 截取最后100个数据
    }
    else
    {
        updateBuffer_1 = buffer_1 ;
        updateBuffer_2 = buffer_2 ;
    }
    QVector<double> xAxis;

    double maxValue = ( Math_MaxValue( updateBuffer_1 ) > Math_MaxValue( updateBuffer_2 ) ) ? Math_MaxValue( updateBuffer_1 ) : Math_MaxValue( updateBuffer_2 ) ;
    double minValue = ( Math_MinValue( updateBuffer_1 ) < Math_MinValue( updateBuffer_2 ) ) ? Math_MinValue( updateBuffer_1 ) : Math_MinValue( updateBuffer_2 ) ;

    xAxis.resize(updateBuffer_1.size()) ;
    for (int i = 0; i < (updateBuffer_1.size()); i++)
    {
        xAxis[i] = i ;
    }
    ui->QCustomPlot_1->graph(0)->setData( xAxis , updateBuffer_1);
    ui->QCustomPlot_1->graph(1)->setData( xAxis , updateBuffer_2 ) ;

    ui->QCustomPlot_1->yAxis->setRange( minValue-1 , maxValue+1 );
    ui->QCustomPlot_1->xAxis->setRange( -2 , size+1 );
    qDebug()<<"maxValue : "<<maxValue<<" - minValue : "<<minValue ;

    ui->QCustomPlot_1->replot();
}

double Math_MaxValue( QVector<double> buffer )
{
    double max_value = buffer[0] ;
    for( double value : buffer )
    {
        if( max_value < value )
        {
            max_value = value ;
        }
    }
    return max_value ;
}

double Math_MinValue( QVector<double> buffer )
{
    double min_value = buffer[0] ;
    for( double value : buffer )
    {
        if( min_value > value )
            min_value = value ;
    }
    return min_value ;
}

void MainWindow::onStartClicked() {
    // 模拟数据输入
    for (int i = 0; i < 200; ++i) {
        m_processor->appendData(static_cast<double>(rand()) / RAND_MAX);
    }
}

void MainWindow::onImportClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择数据文件");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "错误", "文件打开失败");
        return;
    }

    QTextStream in(&file);
    QVector<double> importedData;
    while (!in.atEnd()) {
        importedData.append(in.readLine().toDouble());
    }
    // 使用导入数据...
}

void MainWindow::handleSaveCompleted(const QString &fileName) {
//    ui->statusBar->showMessage("已保存: " + fileName, 3000);
//    Q_UNUSED(fileName) ;
    qDebug()<<"保存数据成功 _ "+fileName ;
}




