/**
 * @file NetworkManager.cpp
 * @brief RANOnline EP7 AI系统 - 网络管理器实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "NetworkManager.h"
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QJsonDocument>
#include <QtNetwork/QHostAddress>

/**
 * @brief 构造函数
 */
NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_connectionType(ConnectionType::TCP_SOCKET)
    , m_serverPort(0)
    , m_connected(false)
    , m_reconnecting(false)
    , m_reconnectAttempts(0)
    , m_maxReconnectAttempts(MAX_RECONNECT_ATTEMPTS)
    , m_heartbeatTimer(new QTimer(this))
    , m_reconnectTimer(new QTimer(this))
    , m_queueTimer(new QTimer(this))
{
    // 设置定时器
    m_heartbeatTimer->setInterval(HEARTBEAT_INTERVAL);
    m_heartbeatTimer->setSingleShot(false);
    
    m_reconnectTimer->setInterval(RECONNECT_INTERVAL);
    m_reconnectTimer->setSingleShot(true);
    
    m_queueTimer->setInterval(MESSAGE_QUEUE_INTERVAL);
    m_queueTimer->setSingleShot(false);
    
    // 连接信号
    connect(m_heartbeatTimer, &QTimer::timeout, this, &NetworkManager::sendHeartbeat);
    connect(m_reconnectTimer, &QTimer::timeout, this, &NetworkManager::onReconnectTimer);
    connect(m_queueTimer, &QTimer::timeout, this, &NetworkManager::processMessageQueue);
}

/**
 * @brief 析构函数
 */
NetworkManager::~NetworkManager()
{
    stop();
}

/**
 * @brief 初始化网络管理器
 */
bool NetworkManager::initialize(ConnectionType type, const QString& address, quint16 port)
{
    m_connectionType = type;
    m_serverAddress = address;
    m_serverPort = port;
    
    if (type == ConnectionType::NAMED_PIPE || type == ConnectionType::LOCAL_SOCKET) {
        m_pipeName = address;
    }
    
    return createConnection();
}

/**
 * @brief 连接到后端
 */
bool NetworkManager::connectToBackend()
{
    if (m_connected) {
        return true;
    }
    
    qDebug() << "NetworkManager: 正在连接到后端...";
    
    switch (m_connectionType) {
        case ConnectionType::TCP_SOCKET:
            if (m_tcpSocket) {
                m_tcpSocket->connectToHost(QHostAddress(m_serverAddress), m_serverPort);
                return true;
            }
            break;
            
        case ConnectionType::LOCAL_SOCKET:
        case ConnectionType::NAMED_PIPE:
            if (m_localSocket) {
                m_localSocket->connectToServer(m_pipeName);
                return true;
            }
            break;
    }
    
    return false;
}

/**
 * @brief 断开连接
 */
void NetworkManager::disconnectFromBackend()
{
    qDebug() << "NetworkManager: 断开连接";
    
    m_heartbeatTimer->stop();
    m_reconnectTimer->stop();
    
    if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->disconnectFromHost();
    }
    
    if (m_localSocket && m_localSocket->state() == QLocalSocket::ConnectedState) {
        m_localSocket->disconnectFromServer();
    }
    
    m_connected = false;
    emit connectionStateChanged(false);
}

/**
 * @brief 发送消息
 */
bool NetworkManager::sendMessage(const NetworkMessage& message)
{
    if (!m_connected) {
        qDebug() << "NetworkManager: 未连接，将消息加入队列";
        QMutexLocker locker(&m_queueMutex);
        m_messageQueue.enqueue(message);
        return false;
    }
    
    QByteArray data = serializeMessage(message);
    qint64 bytesWritten = 0;
    
    switch (m_connectionType) {
        case ConnectionType::TCP_SOCKET:
            if (m_tcpSocket) {
                bytesWritten = m_tcpSocket->write(data);
                m_tcpSocket->flush();
            }
            break;
            
        case ConnectionType::LOCAL_SOCKET:
        case ConnectionType::NAMED_PIPE:
            if (m_localSocket) {
                bytesWritten = m_localSocket->write(data);
                m_localSocket->flush();
            }
            break;
    }
    
    if (bytesWritten > 0) {
        m_stats.totalBytesSent += bytesWritten;
        m_stats.messagesSent++;
        return true;
    }
    
    return false;
}

/**
 * @brief 发送异步请求
 */
QString NetworkManager::sendAsyncRequest(const NetworkMessage& message)
{
    QString requestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    NetworkMessage request = message;
    request.requestId = requestId;
    request.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    // 保存请求用于响应匹配
    m_pendingRequests[requestId] = request;
    
    if (sendMessage(request)) {
        return requestId;
    }
    
    m_pendingRequests.remove(requestId);
    return QString();
}

/**
 * @brief 是否已连接
 */
bool NetworkManager::isConnected() const
{
    return m_connected;
}

/**
 * @brief 获取连接统计信息
 */
QJsonObject NetworkManager::getConnectionStats() const
{
    QJsonObject stats;
    stats["connected"] = m_connected;
    stats["connectionType"] = static_cast<int>(m_connectionType);
    stats["serverAddress"] = m_serverAddress;
    stats["serverPort"] = m_serverPort;
    stats["connectTime"] = m_stats.connectTime;
    stats["totalBytesSent"] = static_cast<qint64>(m_stats.totalBytesSent);
    stats["totalBytesReceived"] = static_cast<qint64>(m_stats.totalBytesReceived);
    stats["messagesSent"] = m_stats.messagesSent;
    stats["messagesReceived"] = m_stats.messagesReceived;
    stats["reconnectCount"] = m_stats.reconnectCount;
    stats["averageLatency"] = m_stats.averageLatency;
    stats["pendingRequests"] = m_pendingRequests.size();
    stats["queuedMessages"] = m_messageQueue.size();
    
    return stats;
}

/**
 * @brief 启动网络管理器
 */
void NetworkManager::start()
{
    qDebug() << "NetworkManager: 启动网络管理器";
    
    m_queueTimer->start();
    
    if (!m_connected) {
        connectToBackend();
    }
}

/**
 * @brief 停止网络管理器
 */
void NetworkManager::stop()
{
    qDebug() << "NetworkManager: 停止网络管理器";
    
    m_queueTimer->stop();
    disconnectFromBackend();
}

/**
 * @brief 重新连接
 */
void NetworkManager::reconnect()
{
    if (m_reconnecting) {
        return;
    }
    
    qDebug() << "NetworkManager: 开始重连";
    
    m_reconnecting = true;
    m_reconnectAttempts = 0;
    
    disconnectFromBackend();
    m_reconnectTimer->start();
}

/**
 * @brief 处理连接成功
 */
void NetworkManager::onConnected()
{
    qDebug() << "NetworkManager: 连接成功";
    
    m_connected = true;
    m_reconnecting = false;
    m_reconnectAttempts = 0;
    m_stats.connectTime = QDateTime::currentMSecsSinceEpoch();
    
    // 启动心跳
    m_heartbeatTimer->start();
    
    // 处理队列中的消息
    processMessageQueue();
    
    emit connectionStateChanged(true);
}

/**
 * @brief 处理连接断开
 */
void NetworkManager::onDisconnected()
{
    qDebug() << "NetworkManager: 连接断开";
    
    bool wasConnected = m_connected;
    m_connected = false;
    m_heartbeatTimer->stop();
    
    if (wasConnected) {
        emit connectionStateChanged(false);
        
        // 自动重连
        if (!m_reconnecting && m_reconnectAttempts < m_maxReconnectAttempts) {
            reconnect();
        }
    }
}

/**
 * @brief 处理连接错误
 */
void NetworkManager::onError()
{
    QString errorString;
    
    switch (m_connectionType) {
        case ConnectionType::TCP_SOCKET:
            if (m_tcpSocket) {
                errorString = m_tcpSocket->errorString();
            }
            break;
            
        case ConnectionType::LOCAL_SOCKET:
        case ConnectionType::NAMED_PIPE:
            if (m_localSocket) {
                errorString = m_localSocket->errorString();
            }
            break;
    }
    
    qDebug() << "NetworkManager: 连接错误:" << errorString;
    emit errorOccurred(errorString);
    
    // 尝试重连
    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        reconnect();
    }
}

/**
 * @brief 处理数据接收
 */
void NetworkManager::onDataReceived()
{
    QByteArray newData;
    
    switch (m_connectionType) {
        case ConnectionType::TCP_SOCKET:
            if (m_tcpSocket) {
                newData = m_tcpSocket->readAll();
            }
            break;
            
        case ConnectionType::LOCAL_SOCKET:
        case ConnectionType::NAMED_PIPE:
            if (m_localSocket) {
                newData = m_localSocket->readAll();
            }
            break;
    }
    
    if (newData.isEmpty()) {
        return;
    }
    
    m_receiveBuffer.append(newData);
    m_stats.totalBytesReceived += newData.size();
    
    // 处理完整的消息
    while (m_receiveBuffer.size() >= sizeof(qint32)) {
        // 读取消息长度
        qint32 messageLength;
        QDataStream lengthStream(m_receiveBuffer.left(sizeof(qint32)));
        lengthStream.setByteOrder(QDataStream::LittleEndian);
        lengthStream >> messageLength;
        
        // 检查是否有完整消息
        if (m_receiveBuffer.size() < sizeof(qint32) + messageLength) {
            break;
        }
        
        // 提取消息数据
        QByteArray messageData = m_receiveBuffer.mid(sizeof(qint32), messageLength);
        m_receiveBuffer.remove(0, sizeof(qint32) + messageLength);
        
        // 反序列化消息
        NetworkMessage message = deserializeMessage(messageData);
        
        m_stats.messagesReceived++;
        handleReceivedMessage(message);
    }
}

/**
 * @brief 发送心跳
 */
void NetworkManager::sendHeartbeat()
{
    NetworkMessage heartbeat(MessageType::HEARTBEAT);
    sendMessage(heartbeat);
}

/**
 * @brief 处理重连定时器
 */
void NetworkManager::onReconnectTimer()
{
    if (m_reconnectAttempts >= m_maxReconnectAttempts) {
        qDebug() << "NetworkManager: 达到最大重连次数，停止重连";
        m_reconnecting = false;
        return;
    }
    
    m_reconnectAttempts++;
    m_stats.reconnectCount++;
    
    qDebug() << QString("NetworkManager: 重连尝试 %1/%2")
                .arg(m_reconnectAttempts)
                .arg(m_maxReconnectAttempts);
    
    if (connectToBackend()) {
        // 连接已启动，等待连接结果
    } else {
        // 连接失败，继续重连
        m_reconnectTimer->start();
    }
}

/**
 * @brief 处理消息队列
 */
void NetworkManager::processMessageQueue()
{
    if (!m_connected || m_messageQueue.isEmpty()) {
        return;
    }
    
    QMutexLocker locker(&m_queueMutex);
    
    while (!m_messageQueue.isEmpty() && m_connected) {
        NetworkMessage message = m_messageQueue.dequeue();
        sendMessage(message);
    }
}

/**
 * @brief 创建连接对象
 */
bool NetworkManager::createConnection()
{
    switch (m_connectionType) {
        case ConnectionType::TCP_SOCKET:
            m_tcpSocket = std::make_unique<QTcpSocket>(this);
            
            connect(m_tcpSocket.get(), &QTcpSocket::connected, 
                    this, &NetworkManager::onConnected);
            connect(m_tcpSocket.get(), &QTcpSocket::disconnected, 
                    this, &NetworkManager::onDisconnected);
            connect(m_tcpSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
                    this, &NetworkManager::onError);
            connect(m_tcpSocket.get(), &QTcpSocket::readyRead, 
                    this, &NetworkManager::onDataReceived);
            
            return true;
            
        case ConnectionType::LOCAL_SOCKET:
        case ConnectionType::NAMED_PIPE:
            m_localSocket = std::make_unique<QLocalSocket>(this);
            
            connect(m_localSocket.get(), &QLocalSocket::connected, 
                    this, &NetworkManager::onConnected);
            connect(m_localSocket.get(), &QLocalSocket::disconnected, 
                    this, &NetworkManager::onDisconnected);
            connect(m_localSocket.get(), QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
                    this, &NetworkManager::onError);
            connect(m_localSocket.get(), &QLocalSocket::readyRead, 
                    this, &NetworkManager::onDataReceived);
            
            return true;
    }
    
    return false;
}

/**
 * @brief 序列化消息
 */
QByteArray NetworkManager::serializeMessage(const NetworkMessage& message)
{
    QJsonObject json;
    json["type"] = static_cast<int>(message.type);
    json["timestamp"] = message.timestamp;
    json["requestId"] = message.requestId;
    json["data"] = message.data;
    
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    // 添加消息长度前缀
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << static_cast<qint32>(jsonData.size());
    result.append(jsonData);
    
    return result;
}

/**
 * @brief 反序列化消息
 */
NetworkMessage NetworkManager::deserializeMessage(const QByteArray& data)
{
    NetworkMessage message;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "NetworkManager: JSON解析错误:" << error.errorString();
        return message;
    }
    
    QJsonObject json = doc.object();
    message.type = static_cast<MessageType>(json["type"].toInt());
    message.timestamp = json["timestamp"].toVariant().toLongLong();
    message.requestId = json["requestId"].toString();
    message.data = json["data"].toObject();
    
    return message;
}

/**
 * @brief 处理接收到的消息
 */
void NetworkManager::handleReceivedMessage(const NetworkMessage& message)
{
    // 检查是否为异步响应
    if (!message.requestId.isEmpty() && m_pendingRequests.contains(message.requestId)) {
        NetworkMessage originalRequest = m_pendingRequests.take(message.requestId);
        
        // 计算延迟
        qint64 latency = message.timestamp - originalRequest.timestamp;
        m_stats.averageLatency = (m_stats.averageLatency + latency) / 2.0;
        
        emit asyncResponseReceived(message.requestId, message);
        return;
    }
    
    // 处理特殊消息类型
    switch (message.type) {
        case MessageType::HEARTBEAT:
            // 心跳响应，不需要特殊处理
            break;
            
        case MessageType::PERFORMANCE_DATA:
            emit performanceDataReceived(message.data);
            break;
            
        case MessageType::ERROR_MESSAGE:
            emit errorOccurred(message.data["message"].toString());
            break;
            
        default:
            // 普通消息
            emit messageReceived(message);
            break;
    }
}

/**
 * @brief 更新连接统计
 */
void NetworkManager::updateConnectionStats()
{
    // 这里可以添加更多统计信息的更新逻辑
    // 当前统计信息在其他方法中更新
}
