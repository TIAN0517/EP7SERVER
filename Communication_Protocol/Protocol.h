/**
 * @file Protocol.h
 * @brief RANOnline EP7 AI系统 - 通信协议头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 * 
 * 功能特色:
 * - 高效二进制协议设计
 * - Protobuf序列化支持
 * - 本地Socket通信
 * - NamedPipe跨进程通信
 * - 消息队列管理
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

/**
 * @enum MessageType
 * @brief 消息类型枚举
 */
enum class MessageType : uint16_t {
    // 系统控制消息 (0x0000 - 0x00FF)
    SYSTEM_PING = 0x0001,           ///< 心跳包
    SYSTEM_PONG = 0x0002,           ///< 心跳响应
    SYSTEM_SHUTDOWN = 0x0003,       ///< 系统关闭
    SYSTEM_STATUS = 0x0004,         ///< 系统状态
    
    // AI管理消息 (0x0100 - 0x01FF)
    AI_CREATE = 0x0101,             ///< 创建AI
    AI_CREATE_BATCH = 0x0102,       ///< 批量创建AI
    AI_DELETE = 0x0103,             ///< 删除AI
    AI_UPDATE = 0x0104,             ///< 更新AI
    AI_QUERY = 0x0105,              ///< 查询AI
    AI_LIST = 0x0106,               ///< AI列表
    
    // AI指令消息 (0x0200 - 0x02FF)
    AI_COMMAND_MOVE = 0x0201,       ///< 移动指令
    AI_COMMAND_ATTACK = 0x0202,     ///< 攻击指令
    AI_COMMAND_SKILL = 0x0203,      ///< 技能指令
    AI_COMMAND_CHAT = 0x0204,       ///< 聊天指令
    AI_COMMAND_QUEST = 0x0205,      ///< 任务指令
    AI_COMMAND_LEVELUP = 0x0206,    ///< 升级指令
    
    // 状态报告消息 (0x0300 - 0x03FF)
    STATUS_AI_UPDATE = 0x0301,      ///< AI状态更新
    STATUS_SERVER_LOAD = 0x0302,    ///< 服务器负载
    STATUS_PERFORMANCE = 0x0303,    ///< 性能统计
    STATUS_ERROR = 0x0304,          ///< 错误报告
    
    // 数据库同步消息 (0x0400 - 0x04FF)
    DB_SYNC_AI = 0x0401,           ///< 同步AI数据
    DB_SYNC_STATS = 0x0402,        ///< 同步统计数据
    DB_QUERY = 0x0403,             ///< 数据库查询
    DB_RESULT = 0x0404,            ///< 查询结果
    
    // 响应消息 (0xFF00 - 0xFFFF)
    RESPONSE_SUCCESS = 0xFF01,      ///< 成功响应
    RESPONSE_ERROR = 0xFF02,        ///< 错误响应
    RESPONSE_TIMEOUT = 0xFF03,      ///< 超时响应
    RESPONSE_INVALID = 0xFF04       ///< 无效请求响应
};

/**
 * @struct MessageHeader
 * @brief 消息头结构
 */
struct MessageHeader {
    uint32_t magic;                 ///< 魔数标识 (0x52414E4F = "RANO")
    uint16_t version;               ///< 协议版本
    MessageType type;               ///< 消息类型
    uint32_t length;                ///< 消息体长度
    uint32_t sequence;              ///< 序列号
    uint64_t timestamp;             ///< 时间戳
    uint32_t checksum;              ///< 校验和
    uint32_t reserved;              ///< 保留字段
    
    MessageHeader() : magic(0x52414E4F), version(1), type(MessageType::SYSTEM_PING), 
                     length(0), sequence(0), timestamp(0), checksum(0), reserved(0) {}
    
    static constexpr size_t SIZE = 32; ///< 消息头固定大小
};

/**
 * @class Message
 * @brief 通用消息类
 */
class Message {
public:
    /**
     * @brief 构造函数
     * @param type 消息类型
     */
    explicit Message(MessageType type);
    
    /**
     * @brief 析构函数
     */
    virtual ~Message() = default;
    
    /**
     * @brief 获取消息类型
     * @return 消息类型
     */
    MessageType getType() const { return m_header.type; }
    
    /**
     * @brief 获取序列号
     * @return 序列号
     */
    uint32_t getSequence() const { return m_header.sequence; }
    
    /**
     * @brief 设置序列号
     * @param sequence 序列号
     */
    void setSequence(uint32_t sequence) { m_header.sequence = sequence; }
    
    /**
     * @brief 获取时间戳
     * @return 时间戳
     */
    uint64_t getTimestamp() const { return m_header.timestamp; }
    
    /**
     * @brief 设置消息体数据
     * @param data 数据
     */
    void setData(const std::vector<uint8_t>& data);
    
    /**
     * @brief 获取消息体数据
     * @return 数据引用
     */
    const std::vector<uint8_t>& getData() const { return m_data; }
    
    /**
     * @brief 序列化消息
     * @return 序列化后的数据
     */
    std::vector<uint8_t> serialize() const;
    
    /**
     * @brief 反序列化消息
     * @param data 序列化数据
     * @return 是否成功反序列化
     */
    bool deserialize(const std::vector<uint8_t>& data);
    
    /**
     * @brief 计算校验和
     * @return 校验和
     */
    uint32_t calculateChecksum() const;
    
    /**
     * @brief 验证消息完整性
     * @return 是否有效
     */
    bool isValid() const;

protected:
    MessageHeader m_header;         ///< 消息头
    std::vector<uint8_t> m_data;   ///< 消息体数据
    
private:
    static uint32_t s_sequenceCounter; ///< 全局序列号计数器
};

/**
 * @class SocketCommunicator
 * @brief Socket通信器类
 */
class SocketCommunicator {
public:
    /**
     * @brief 构造函数
     */
    SocketCommunicator();
    
    /**
     * @brief 析构函数
     */
    ~SocketCommunicator();
    
    /**
     * @brief 启动服务器模式
     * @param port 监听端口
     * @return 是否成功启动
     */
    bool startServer(uint16_t port);
    
    /**
     * @brief 连接到服务器
     * @param host 服务器地址
     * @param port 服务器端口
     * @return 是否成功连接
     */
    bool connectToServer(const std::string& host, uint16_t port);
    
    /**
     * @brief 停止通信
     */
    void stop();
    
    /**
     * @brief 发送消息
     * @param message 消息对象
     * @return 是否成功发送
     */
    bool sendMessage(const Message& message);
    
    /**
     * @brief 异步发送消息
     * @param message 消息对象
     * @param callback 完成回调
     * @return 是否成功加入发送队列
     */
    bool sendMessageAsync(const Message& message, std::function<void(bool)> callback = nullptr);
    
    /**
     * @brief 接收消息
     * @param timeout 超时时间（毫秒）
     * @return 消息指针，失败返回nullptr
     */
    std::unique_ptr<Message> receiveMessage(int timeout = 5000);
    
    /**
     * @brief 设置消息接收回调
     * @param callback 回调函数
     */
    void setMessageCallback(std::function<void(std::unique_ptr<Message>)> callback);
    
    /**
     * @brief 检查连接状态
     * @return 是否已连接
     */
    bool isConnected() const { return m_isConnected; }
    
    /**
     * @brief 获取统计信息
     * @return 统计数据映射
     */
    std::unordered_map<std::string, uint64_t> getStatistics() const;

private:
    /**
     * @brief 接收线程函数
     */
    void receiveThreadFunction();
    
    /**
     * @brief 发送线程函数
     */
    void sendThreadFunction();
    
    /**
     * @brief 客户端处理线程
     * @param clientSocket 客户端套接字
     */
    void handleClient(SOCKET clientSocket);
    
    /**
     * @brief 发送原始数据
     * @param socket 套接字
     * @param data 数据
     * @return 是否成功发送
     */
    bool sendRawData(SOCKET socket, const std::vector<uint8_t>& data);
    
    /**
     * @brief 接收原始数据
     * @param socket 套接字
     * @param data 数据缓冲区
     * @param size 数据大小
     * @return 实际接收的字节数
     */
    int receiveRawData(SOCKET socket, std::vector<uint8_t>& data, size_t size);

private:
    SOCKET m_serverSocket;          ///< 服务器套接字
    SOCKET m_clientSocket;          ///< 客户端套接字
    bool m_isServer;                ///< 是否为服务器模式
    bool m_isConnected;             ///< 连接状态
    bool m_shouldStop;              ///< 停止标志
    
    std::thread m_receiveThread;    ///< 接收线程
    std::thread m_sendThread;       ///< 发送线程
    std::vector<std::thread> m_clientThreads; ///< 客户端处理线程组
    
    std::queue<std::pair<Message, std::function<void(bool)>>> m_sendQueue; ///< 发送队列
    std::mutex m_sendQueueMutex;    ///< 发送队列互斥锁
    std::condition_variable m_sendCondition; ///< 发送条件变量
    
    std::function<void(std::unique_ptr<Message>)> m_messageCallback; ///< 消息回调
    
    // 统计信息
    mutable std::mutex m_statsMutex; ///< 统计互斥锁
    uint64_t m_messagesSent;        ///< 发送消息数
    uint64_t m_messagesReceived;    ///< 接收消息数
    uint64_t m_bytesSent;           ///< 发送字节数
    uint64_t m_bytesReceived;       ///< 接收字节数
    uint64_t m_errors;              ///< 错误计数
};

/**
 * @class NamedPipeCommunicator
 * @brief 命名管道通信器类（Windows专用）
 */
class NamedPipeCommunicator {
public:
    /**
     * @brief 构造函数
     */
    NamedPipeCommunicator();
    
    /**
     * @brief 析构函数
     */
    ~NamedPipeCommunicator();
    
    /**
     * @brief 创建命名管道服务器
     * @param pipeName 管道名称
     * @return 是否成功创建
     */
    bool createServer(const std::string& pipeName);
    
    /**
     * @brief 连接到命名管道
     * @param pipeName 管道名称
     * @return 是否成功连接
     */
    bool connectToServer(const std::string& pipeName);
    
    /**
     * @brief 停止通信
     */
    void stop();
    
    /**
     * @brief 发送消息
     * @param message 消息对象
     * @return 是否成功发送
     */
    bool sendMessage(const Message& message);
    
    /**
     * @brief 接收消息
     * @param timeout 超时时间（毫秒）
     * @return 消息指针，失败返回nullptr
     */
    std::unique_ptr<Message> receiveMessage(int timeout = 5000);
    
    /**
     * @brief 检查连接状态
     * @return 是否已连接
     */
    bool isConnected() const { return m_isConnected; }

private:
    HANDLE m_pipeHandle;            ///< 管道句柄
    bool m_isServer;                ///< 是否为服务器模式
    bool m_isConnected;             ///< 连接状态
    std::string m_pipeName;         ///< 管道名称
};

/**
 * @class MessageQueue
 * @brief 消息队列管理器
 */
class MessageQueue {
public:
    /**
     * @brief 构造函数
     * @param maxSize 最大队列大小
     */
    explicit MessageQueue(size_t maxSize = 10000);
    
    /**
     * @brief 析构函数
     */
    ~MessageQueue();
    
    /**
     * @brief 推送消息到队列
     * @param message 消息对象
     * @param priority 优先级 (1-10)
     * @return 是否成功推送
     */
    bool push(std::unique_ptr<Message> message, int priority = 5);
    
    /**
     * @brief 从队列弹出消息
     * @param timeout 超时时间（毫秒）
     * @return 消息指针，超时或队列空时返回nullptr
     */
    std::unique_ptr<Message> pop(int timeout = -1);
    
    /**
     * @brief 获取队列大小
     * @return 队列中的消息数量
     */
    size_t size() const;
    
    /**
     * @brief 检查队列是否为空
     * @return 是否为空
     */
    bool empty() const;
    
    /**
     * @brief 清空队列
     */
    void clear();
    
    /**
     * @brief 设置最大队列大小
     * @param maxSize 最大大小
     */
    void setMaxSize(size_t maxSize);

private:
    struct QueueItem {
        std::unique_ptr<Message> message;
        int priority;
        std::chrono::steady_clock::time_point timestamp;
        
        QueueItem(std::unique_ptr<Message> msg, int prio) 
            : message(std::move(msg)), priority(prio) {
            timestamp = std::chrono::steady_clock::now();
        }
    };
    
    struct QueueItemComparator {
        bool operator()(const QueueItem& a, const QueueItem& b) {
            if (a.priority != b.priority) {
                return a.priority < b.priority; // 高优先级在前
            }
            return a.timestamp > b.timestamp; // 早的在前
        }
    };
    
    std::priority_queue<QueueItem, std::vector<QueueItem>, QueueItemComparator> m_queue;
    mutable std::mutex m_mutex;         ///< 队列互斥锁
    std::condition_variable m_condition; ///< 条件变量
    size_t m_maxSize;                   ///< 最大队列大小
};

/**
 * @namespace ProtocolUtils
 * @brief 协议工具函数命名空间
 */
namespace ProtocolUtils {
    /**
     * @brief 创建心跳消息
     * @return 心跳消息
     */
    std::unique_ptr<Message> createPingMessage();
    
    /**
     * @brief 创建心跳响应消息
     * @param originalSequence 原始序列号
     * @return 心跳响应消息
     */
    std::unique_ptr<Message> createPongMessage(uint32_t originalSequence);
    
    /**
     * @brief 创建AI创建消息
     * @param school 学校类型
     * @param level 等级
     * @param serverId 服务器ID
     * @return AI创建消息
     */
    std::unique_ptr<Message> createAICreateMessage(const std::string& school, int level, int serverId);
    
    /**
     * @brief 创建成功响应消息
     * @param originalSequence 原始序列号
     * @param data 响应数据
     * @return 成功响应消息
     */
    std::unique_ptr<Message> createSuccessResponse(uint32_t originalSequence, const std::string& data = "");
    
    /**
     * @brief 创建错误响应消息
     * @param originalSequence 原始序列号
     * @param errorMessage 错误信息
     * @return 错误响应消息
     */
    std::unique_ptr<Message> createErrorResponse(uint32_t originalSequence, const std::string& errorMessage);
}

#endif // PROTOCOL_H
