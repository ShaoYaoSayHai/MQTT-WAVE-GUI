#include "dataprocesser.h"
#include <QDebug>

DataProcesser::DataProcesser( int id , QObject *parent) : QObject(parent) , process_id(id)
{

}


void DataProcesser::appendData(double value) {
    m_buffer.append(value);
    if (m_buffer.size() >= CHUNK_SIZE) {
        emit chunkReady(m_buffer);
        emit chunkReadyWidthId( this->process_id , m_buffer );
        m_buffer.clear();
        qDebug()<<"触发文件保存" ;
    }
}
