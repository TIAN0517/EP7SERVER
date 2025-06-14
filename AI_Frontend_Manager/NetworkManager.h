/**
 * @file NetworkManager.h
 * @brief RANOnline EP7 AI系统 - 网络管理器头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QLocalSocket>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <memory>

/**
 * @enum ConnectionType
 * @brief 连接类型枚举
 */
enum class ConnectionType
{
    TCP_SOCKET,     // TCP Socket连接
    NAMED_PIPE,     // Named Pipe连接
    LOCAL_SOCKET    // Local Socket连接
};

/**
 * @enum MessageType
 * @brief 消息类型枚举
 */
enum class MessageType
{
    // AI管理消息
    AI_GENERATE_REQUEST = 1000,
    AI_GENERATE_RESPONSE = 1001,
    AI_START_REQUEST = 1002,
    AI_START_RESPONSE = 1003,
    AI_STOP_REQUEST = 1004,
    AI_STOP_RESPONSE = 1005,
    AI_DELETE_REQUEST = 1006,
    AI_DELETE_RESPONSE = 1007,
    AI_LIST_REQUEST = 1008,
    AI_LIST_RESPONSE = 1009,
    
    // 状态监控消息
    STATUS_REQUEST = 2000,
    STATUS_RESPONSE = 2001,
    PERFORMANCE_DATA = 2002,
    SYSTEM_STATS = 2003,
    
    // 配置消息
    CONFIG_GET_REQUEST = 3000,
    CONFIG_GET_RESPONSE = 3001,
    CONFIG_SET_REQUEST = 3002,
    CONFIG_SET_RESPONSE = 3003,
    
    // 错误和通知
    ERROR_MESSAGE = 9000,
    WARNING_MESSAGE = 9001,
    INFO_MESSAGE = 9002,
    
    // 心跳和连接
    HEARTBEAT = 9999
};

/**
 * @struct NetworkMessage
 * @brief 网络消息结构
 */
struct NetworkMessage
{
    MessageType type;
    qint64 timestamp;
    QString requestId;
    QJsonObject data;
    
    NetworkMessage() 
        : type(MessageType::INFO_MESSAGE)
        , timestamp(0) 
    {}
    
    NetworkMessage(MessageType t, const QJsonObject& d = QJsonObject())
        : type(t)
        , timestamp(QDateTime::currentMSecsSinceEpoch())
        , requestId(QString::number(timestamp))
        , data(d)
    {}
};

/**
 * @class NetworkManager
 * @brief 网络管理器类
 * 
 * 功能包括:
 * 1. 前端与后端通信
 * 2. 多种连接方式支持
 * 3. 消息队列管理
 * 4. 自动重连机制
 * 5. 心跳检测
 * 6. 异步消息处理
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit NetworkManager(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~NetworkManager();

    /**
     * @brief 初始化网络管理器
     * @param type 连接类型
     * @param address 连接地址
     * @param port 端口（TCP连接时使用）
     * @return 是否成功
     */
    bool initialize(ConnectionType type, const QString& address, quint16 port = 0);
    
    /**
     * @brief 连接到后端
     * @return 是否成功
     */
    bool connectToBackend();
    
    /**
     * @brief 断开连接
     */
    void disconnectFromBackend();
    
    /**
     * @brief 发送消息
     * @param message 消息内容
     * @return 是否成功
     */
    bool sendMessage(const NetworkMessage& message);
    
    /**
     * @brief 发送异步请求
     * @param message 请求消息
     * @return 请求ID
     */
    QString sendAsyncRequest(const NetworkMessage& message);
    
    /**
     * @brief 是否已连接
     * @return 连接状态
     */
    bool isConnected() const;
    
    /**
     * @brief 获取连接统计信息
     * @return 统计信息JSON对象
     */
    QJsonObject getConnectionStats() const;

signals:
    /**
     * @brief 连接状态改变信号
     * @param connected 是否已连接
     */
    void connectionStateChanged(bool connected);
    
    /**
     * @brief 消息接收信号
     * @param message 接收到的消息
     */
    void messageReceived(const NetworkMessage& message);
    
    /**
     * @brief 异步响应信号
     * @param requestId 请求ID
     * @param response 响应消息
     */
    void asyncResponseReceived(const QString& requestId, const NetworkMessage& response);
    
    /**
     * @brief 错误信号
     * @param error 错误信息
     */
    void errorOccurred(const QString& error);
    
    /**
     * @brief 性能数据信号
     * @param data 性能数据
     */
    void performanceDataReceived(const QJsonObject& data);

public slots:
    /**
     * @brief 启动网络管理器
     */
    void start();
    
    /**
     * @brief 停止网络管理器
     */
    void stop();
    
    /**
     * @brief 重新连接
     */
    void reconnect();

private slots:
    /**
     * @brief 处理连接成功
     */
    void onConnected();
    
    /**
     * @brief 处理连接断开
     */
    void onDisconnected();
    
    /**
     * @brief 处理连接错误
     */
    void onError();
    
    /**
     * @brief 处理数据接收
     */
    void onDataReceived();
    
    /**
     * @brief 发送心跳
     */
    void sendHeartbeat();
    
    /**
     * @brief 处理重连定时器
     */
    void onReconnectTimer();
    
    /**
     * @brief 处理消息队列
     */
    void processMessageQueue();

private:
    /**
     * @brief 创建连接对象
     * @return 是否成功
     */
    bool createConnection();
    
    /**
     * @brief 序列化消息
     * @param message 消息对象
     * @return 序列化后的数据
     */
    QByteArray serializeMessage(const NetworkMessage& message);
    
    /**
     * @brief 反序列化消息
     * @param data 数据
     * @return 消息对象
     */
    NetworkMessage deserializeMessage(const QByteArray& data);
    
    /**
     * @brief 处理接收到的消息
     * @param message 消息
     */
    void handleReceivedMessage(const NetworkMessage& message);
    
    /**
     * @brief 更新连接统计
     */
    void updateConnectionStats();

private:
    // 连接配置
    ConnectionType m_connectionType;
    QString m_serverAddress;
    quint16 m_serverPort;
    QString m_pipeName;
    
    // 连接对象
    std::unique_ptr<QTcpSocket> m_tcpSocket;
    std::unique_ptr<QLocalSocket> m_localSocket;
    
    // 状态管理
    bool m_connected;
    bool m_reconnecting;
    int m_reconnectAttempts;
    int m_maxReconnectAttempts;
    
    // 消息处理
    QQueue<NetworkMessage> m_messageQueue;
    QMap<QString, NetworkMessage> m_pendingRequests;
    QMutex m_queueMutex;
    QByteArray m_receiveBuffer;
    
    // 定时器
    QTimer* m_heartbeatTimer;
    QTimer* m_reconnectTimer;
    QTimer* m_queueTimer;
    
    // 统计信息
    struct ConnectionStats {
        qint64 connectTime = 0;
        qint64 totalBytesSent = 0;
        qint64 totalBytesReceived = 0;
        int messagesSent = 0;
        int messagesReceived = 0;
        int reconnectCount = 0;
        double averageLatency = 0.0;
    } m_stats;
    
    // 常量
    static constexpr int HEARTBEAT_INTERVAL = 30000;     // 30秒
    static constexpr int RECONNECT_INTERVAL = 5000;      // 5秒
    static constexpr int MAX_RECONNECT_ATTEMPTS = 10;     // 最大重连次数
    static constexpr int MESSAGE_QUEUE_INTERVAL = 100;   // 消息队列处理间隔
};
