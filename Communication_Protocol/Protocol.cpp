/**
 * @file Protocol.cpp
 * @brief RANOnline EP7 AI系统 - 通信协议实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "Protocol.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

/**
 * @brief MessageFrame构造函数
 */
MessageFrame::MessageFrame()
    : header(PROTOCOL_HEADER)
    , version(PROTOCOL_VERSION)
    , messageType(0)
    , messageId(0)
    , timestamp(0)
    , payloadSize(0)
    , checksum(0)
    , flags(0)
{
}

/**
 * @brief MessageFrame带参数构造函数
 */
MessageFrame::MessageFrame(quint32 type, const QByteArray& data)
    : header(PROTOCOL_HEADER)
    , version(PROTOCOL_VERSION)
    , messageType(type)
    , messageId(QDateTime::currentMSecsSinceEpoch())
    , timestamp(messageId)
    , payloadSize(data.size())
    , checksum(calculateChecksum(data))
    , flags(0)
    , payload(data)
{
}

/**
 * @brief 计算校验和
 */
quint32 MessageFrame::calculateChecksum(const QByteArray& data)
{
    quint32 checksum = 0;
    for (int i = 0; i < data.size(); ++i) {
        checksum += static_cast<quint8>(data[i]);
    }
    return checksum;
}

/**
 * @brief 验证校验和
 */
bool MessageFrame::verifyChecksum() const
{
    return checksum == calculateChecksum(payload);
}

/**
 * @brief 序列化消息帧
 */
QByteArray MessageFrame::serialize() const
{
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // 写入头部
    stream << header;
    stream << version;
    stream << messageType;
    stream << messageId;
    stream << timestamp;
    stream << payloadSize;
    stream << checksum;
    stream << flags;
    
    // 写入负载数据
    if (!payload.isEmpty()) {
        stream.writeRawData(payload.constData(), payload.size());
    }
    
    return result;
}

/**
 * @brief 反序列化消息帧
 */
bool MessageFrame::deserialize(const QByteArray& data)
{
    if (data.size() < static_cast<int>(sizeof(MessageFrameHeader))) {
        return false;
    }
    
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // 读取头部
    stream >> header;
    stream >> version;
    stream >> messageType;
    stream >> messageId;
    stream >> timestamp;
    stream >> payloadSize;
    stream >> checksum;
    stream >> flags;
    
    // 验证头部
    if (header != PROTOCOL_HEADER || version != PROTOCOL_VERSION) {
        return false;
    }
    
    // 检查数据完整性
    if (data.size() < static_cast<int>(sizeof(MessageFrameHeader) + payloadSize)) {
        return false;
    }
    
    // 读取负载数据
    if (payloadSize > 0) {
        payload.resize(payloadSize);
        stream.readRawData(payload.data(), payloadSize);
        
        // 验证校验和
        if (!verifyChecksum()) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief BinaryProtocol构造函数
 */
BinaryProtocol::BinaryProtocol(QObject *parent)
    : QObject(parent)
    , m_sequenceNumber(0)
{
}

/**
 * @brief 编码消息
 */
QByteArray BinaryProtocol::encodeMessage(const Message& message)
{
    QJsonObject json;
    json["type"] = static_cast<int>(message.type);
    json["data"] = message.data;
    json["requestId"] = message.requestId;
    json["timestamp"] = message.timestamp;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    MessageFrame frame(static_cast<quint32>(message.type), jsonData);
    frame.messageId = ++m_sequenceNumber;
    
    return frame.serialize();
}

/**
 * @brief 解码消息
 */
Message BinaryProtocol::decodeMessage(const QByteArray& data)
{
    Message message;
    
    MessageFrame frame;
    if (!frame.deserialize(data)) {
        qWarning() << "BinaryProtocol: 消息帧解析失败";
        return message;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(frame.payload, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "BinaryProtocol: JSON解析失败:" << error.errorString();
        return message;
    }
    
    QJsonObject json = doc.object();
    message.type = static_cast<MessageType>(json["type"].toInt());
    message.data = json["data"].toObject();
    message.requestId = json["requestId"].toString();
    message.timestamp = json["timestamp"].toVariant().toLongLong();
    
    return message;
}

/**
 * @brief 创建心跳消息
 */
Message BinaryProtocol::createHeartbeatMessage()
{
    Message message;
    message.type = MessageType::HEARTBEAT;
    message.timestamp = QDateTime::currentMSecsSinceEpoch();
    message.requestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    return message;
}

/**
 * @brief 创建响应消息
 */
Message BinaryProtocol::createResponseMessage(const Message& request, const QJsonObject& responseData)
{
    Message response;
    response.type = static_cast<MessageType>(static_cast<int>(request.type) + 1); // 假设响应类型是请求类型+1
    response.data = responseData;
    response.requestId = request.requestId;
    response.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    return response;
}

/**
 * @brief SocketClient构造函数
 */
SocketClient::SocketClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_protocol(new BinaryProtocol(this))
    , m_connected(false)
    , m_reconnectTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_reconnectAttempts(0)
    , m_maxReconnectAttempts(5)
{
    setupTimers();
    connectSignals();
}

/**
 * @brief 连接到服务器
 */
bool SocketClient::connectToServer(const QString& host, quint16 port)
{
    m_host = host;
    m_port = port;
    
    qDebug() << "SocketClient: 连接到服务器" << host << ":" << port;
    
    m_socket->connectToHost(host, port);
    return true;
}

/**
 * @brief 断开连接
 */
void SocketClient::disconnectFromServer()
{
    qDebug() << "SocketClient: 断开连接";
    
    m_heartbeatTimer->stop();
    m_reconnectTimer->stop();
    
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

/**
 * @brief 发送消息
 */
bool SocketClient::sendMessage(const Message& message)
{
    if (!m_connected) {
        qWarning() << "SocketClient: 未连接，无法发送消息";
        return false;
    }
    
    QByteArray data = m_protocol->encodeMessage(message);
    
    qint64 bytesWritten = m_socket->write(data);
    m_socket->flush();
    
    if (bytesWritten == data.size()) {
        emit messageSent(message);
        return true;
    }
    
    qWarning() << "SocketClient: 消息发送不完整";
    return false;
}

/**
 * @brief 是否已连接
 */
bool SocketClient::isConnected() const
{
    return m_connected;
}

/**
 * @brief 设置定时器
 */
void SocketClient::setupTimers()
{
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(5000); // 5秒重连间隔
    
    m_heartbeatTimer->setInterval(30000); // 30秒心跳间隔
    
    connect(m_reconnectTimer, &QTimer::timeout, this, &SocketClient::attemptReconnect);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &SocketClient::sendHeartbeat);
}

/**
 * @brief 连接信号
 */
void SocketClient::connectSignals()
{
    connect(m_socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &SocketClient::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &SocketClient::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &SocketClient::onDataReceived);
}

/**
 * @brief 处理连接成功
 */
void SocketClient::onConnected()
{
    qDebug() << "SocketClient: 连接成功";
    
    m_connected = true;
    m_reconnectAttempts = 0;
    m_reconnectTimer->stop();
    m_heartbeatTimer->start();
    
    emit connected();
}

/**
 * @brief 处理连接断开
 */
void SocketClient::onDisconnected()
{
    qDebug() << "SocketClient: 连接断开";
    
    m_connected = false;
    m_heartbeatTimer->stop();
    
    emit disconnected();
    
    // 自动重连
    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        startReconnect();
    }
}

/**
 * @brief 处理连接错误
 */
void SocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorString = m_socket->errorString();
    qWarning() << "SocketClient: 连接错误:" << errorString;
    
    emit errorOccurred(errorString);
    
    // 尝试重连
    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        startReconnect();
    }
}

/**
 * @brief 处理数据接收
 */
void SocketClient::onDataReceived()
{
    QByteArray data = m_socket->readAll();
    m_receiveBuffer.append(data);
    
    // 处理完整的消息帧
    while (m_receiveBuffer.size() >= static_cast<int>(sizeof(MessageFrameHeader))) {
        // 读取消息长度
        MessageFrameHeader header;
        QDataStream headerStream(m_receiveBuffer.left(sizeof(MessageFrameHeader)));
        headerStream.setByteOrder(QDataStream::LittleEndian);
        headerStream.readRawData(reinterpret_cast<char*>(&header), sizeof(MessageFrameHeader));
        
        quint32 totalSize = sizeof(MessageFrameHeader) + header.payloadSize;
        
        // 检查是否有完整消息
        if (m_receiveBuffer.size() < static_cast<int>(totalSize)) {
            break;
        }
        
        // 提取消息数据
        QByteArray messageData = m_receiveBuffer.left(totalSize);
        m_receiveBuffer.remove(0, totalSize);
        
        // 解码消息
        Message message = m_protocol->decodeMessage(messageData);
        
        if (message.type != MessageType::HEARTBEAT) {
            emit messageReceived(message);
        }
    }
}

/**
 * @brief 开始重连
 */
void SocketClient::startReconnect()
{
    if (!m_reconnectTimer->isActive()) {
        qDebug() << "SocketClient: 开始重连，尝试次数:" << m_reconnectAttempts + 1;
        m_reconnectTimer->start();
    }
}

/**
 * @brief 尝试重连
 */
void SocketClient::attemptReconnect()
{
    if (m_reconnectAttempts >= m_maxReconnectAttempts) {
        qWarning() << "SocketClient: 达到最大重连次数，停止重连";
        return;
    }
    
    m_reconnectAttempts++;
    qDebug() << "SocketClient: 重连尝试" << m_reconnectAttempts << "/" << m_maxReconnectAttempts;
    
    m_socket->connectToHost(m_host, m_port);
}

/**
 * @brief 发送心跳
 */
void SocketClient::sendHeartbeat()
{
    Message heartbeat = m_protocol->createHeartbeatMessage();
    sendMessage(heartbeat);
}

/**
 * @brief NamedPipeClient构造函数
 */
NamedPipeClient::NamedPipeClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
    , m_protocol(new BinaryProtocol(this))
    , m_connected(false)
    , m_reconnectTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_reconnectAttempts(0)
    , m_maxReconnectAttempts(5)
{
    setupTimers();
    connectSignals();
}

/**
 * @brief 连接到命名管道
 */
bool NamedPipeClient::connectToPipe(const QString& pipeName)
{
    m_pipeName = pipeName;
    
    qDebug() << "NamedPipeClient: 连接到命名管道" << pipeName;
    
    m_socket->connectToServer(pipeName);
    return true;
}

/**
 * @brief 断开连接
 */
void NamedPipeClient::disconnectFromPipe()
{
    qDebug() << "NamedPipeClient: 断开连接";
    
    m_heartbeatTimer->stop();
    m_reconnectTimer->stop();
    
    if (m_socket->state() == QLocalSocket::ConnectedState) {
        m_socket->disconnectFromServer();
    }
}

/**
 * @brief 发送消息
 */
bool NamedPipeClient::sendMessage(const Message& message)
{
    if (!m_connected) {
        qWarning() << "NamedPipeClient: 未连接，无法发送消息";
        return false;
    }
    
    QByteArray data = m_protocol->encodeMessage(message);
    
    qint64 bytesWritten = m_socket->write(data);
    m_socket->flush();
    
    if (bytesWritten == data.size()) {
        emit messageSent(message);
        return true;
    }
    
    qWarning() << "NamedPipeClient: 消息发送不完整";
    return false;
}

/**
 * @brief 是否已连接
 */
bool NamedPipeClient::isConnected() const
{
    return m_connected;
}

/**
 * @brief 设置定时器
 */
void NamedPipeClient::setupTimers()
{
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(5000); // 5秒重连间隔
    
    m_heartbeatTimer->setInterval(30000); // 30秒心跳间隔
    
    connect(m_reconnectTimer, &QTimer::timeout, this, &NamedPipeClient::attemptReconnect);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &NamedPipeClient::sendHeartbeat);
}

/**
 * @brief 连接信号
 */
void NamedPipeClient::connectSignals()
{
    connect(m_socket, &QLocalSocket::connected, this, &NamedPipeClient::onConnected);
    connect(m_socket, &QLocalSocket::disconnected, this, &NamedPipeClient::onDisconnected);
    connect(m_socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
            this, &NamedPipeClient::onError);
    connect(m_socket, &QLocalSocket::readyRead, this, &NamedPipeClient::onDataReceived);
}

/**
 * @brief 处理连接成功
 */
void NamedPipeClient::onConnected()
{
    qDebug() << "NamedPipeClient: 连接成功";
    
    m_connected = true;
    m_reconnectAttempts = 0;
    m_reconnectTimer->stop();
    m_heartbeatTimer->start();
    
    emit connected();
}

/**
 * @brief 处理连接断开
 */
void NamedPipeClient::onDisconnected()
{
    qDebug() << "NamedPipeClient: 连接断开";
    
    m_connected = false;
    m_heartbeatTimer->stop();
    
    emit disconnected();
    
    // 自动重连
    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        startReconnect();
    }
}

/**
 * @brief 处理连接错误
 */
void NamedPipeClient::onError(QLocalSocket::LocalSocketError error)
{
    QString errorString = m_socket->errorString();
    qWarning() << "NamedPipeClient: 连接错误:" << errorString;
    
    emit errorOccurred(errorString);
    
    // 尝试重连
    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        startReconnect();
    }
}

/**
 * @brief 处理数据接收
 */
void NamedPipeClient::onDataReceived()
{
    QByteArray data = m_socket->readAll();
    m_receiveBuffer.append(data);
    
    // 处理完整的消息帧
    while (m_receiveBuffer.size() >= static_cast<int>(sizeof(MessageFrameHeader))) {
        // 读取消息长度
        MessageFrameHeader header;
        QDataStream headerStream(m_receiveBuffer.left(sizeof(MessageFrameHeader)));
        headerStream.setByteOrder(QDataStream::LittleEndian);
        headerStream.readRawData(reinterpret_cast<char*>(&header), sizeof(MessageFrameHeader));
        
        quint32 totalSize = sizeof(MessageFrameHeader) + header.payloadSize;
        
        // 检查是否有完整消息
        if (m_receiveBuffer.size() < static_cast<int>(totalSize)) {
            break;
        }
        
        // 提取消息数据
        QByteArray messageData = m_receiveBuffer.left(totalSize);
        m_receiveBuffer.remove(0, totalSize);
        
        // 解码消息
        Message message = m_protocol->decodeMessage(messageData);
        
        if (message.type != MessageType::HEARTBEAT) {
            emit messageReceived(message);
        }
    }
}

/**
 * @brief 开始重连
 */
void NamedPipeClient::startReconnect()
{
    if (!m_reconnectTimer->isActive()) {
        qDebug() << "NamedPipeClient: 开始重连，尝试次数:" << m_reconnectAttempts + 1;
        m_reconnectTimer->start();
    }
}

/**
 * @brief 尝试重连
 */
void NamedPipeClient::attemptReconnect()
{
    if (m_reconnectAttempts >= m_maxReconnectAttempts) {
        qWarning() << "NamedPipeClient: 达到最大重连次数，停止重连";
        return;
    }
    
    m_reconnectAttempts++;
    qDebug() << "NamedPipeClient: 重连尝试" << m_reconnectAttempts << "/" << m_maxReconnectAttempts;
    
    m_socket->connectToServer(m_pipeName);
}

/**
 * @brief 发送心跳
 */
void NamedPipeClient::sendHeartbeat()
{
    Message heartbeat = m_protocol->createHeartbeatMessage();
    sendMessage(heartbeat);
}
