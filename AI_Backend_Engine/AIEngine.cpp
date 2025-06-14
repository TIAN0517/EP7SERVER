/**
 * @file AIEngine.cpp
 * @brief RANOnline EP7 AI系统 - AI推理引擎实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "AIEngine.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>
#include <QtCore/QRandomGenerator>
#include <QtConcurrent/QtConcurrent>
#include <random>
#include <algorithm>

/**
 * @brief 构造函数
 */
AIEngine::AIEngine(QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_threadPool(new QThreadPool(this))
    , m_commandQueue(new PriorityQueue<AICommand>())
    , m_performanceMonitor(new PerformanceMonitor(this))
    , m_loadBalancer(new LoadBalancer(this))
    , m_processTimer(new QTimer(this))
    , m_statusTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_totalAiCount(0)
    , m_activeAiCount(0)
    , m_currentServerId(1)
{
    // 配置线程池
    int idealThreadCount = QThread::idealThreadCount();
    m_threadPool->setMaxThreadCount(qMax(4, idealThreadCount * 2));
    
    // 配置定时器
    m_processTimer->setInterval(100);  // 100ms处理一次命令队列
    m_statusTimer->setInterval(5000);  // 5秒更新一次状态
    m_heartbeatTimer->setInterval(30000); // 30秒发送一次心跳
    
    // 连接信号槽
    connectSignalsAndSlots();
    
    qDebug() << "AIEngine: 初始化完成, 线程池大小:" << m_threadPool->maxThreadCount();
}

/**
 * @brief 析构函数
 */
AIEngine::~AIEngine()
{
    shutdown();
}

/**
 * @brief 初始化AI引擎
 */
bool AIEngine::initialize(const QJsonObject& config)
{
    qDebug() << "AIEngine: 开始初始化...";
    
    m_config = config;
    
    // 解析配置
    parseConfiguration();
    
    // 初始化性能监控
    if (!m_performanceMonitor->initialize()) {
        qCritical() << "AIEngine: 性能监控初始化失败";
        return false;
    }
    
    // 初始化负载均衡器
    if (!m_loadBalancer->initialize(m_serverConfigs)) {
        qCritical() << "AIEngine: 负载均衡器初始化失败";
        return false;
    }
    
    qDebug() << "AIEngine: 初始化成功";
    return true;
}

/**
 * @brief 启动AI引擎
 */
bool AIEngine::start()
{
    if (m_running) {
        qWarning() << "AIEngine: 已经在运行中";
        return true;
    }
    
    qDebug() << "AIEngine: 启动AI引擎...";
    
    m_running = true;
    
    // 启动定时器
    m_processTimer->start();
    m_statusTimer->start();
    m_heartbeatTimer->start();
    
    // 启动性能监控
    m_performanceMonitor->start();
    
    // 启动负载均衡器
    m_loadBalancer->start();
    
    emit engineStarted();
    qDebug() << "AIEngine: 启动完成";
    
    return true;
}

/**
 * @brief 停止AI引擎
 */
void AIEngine::stop()
{
    if (!m_running) {
        return;
    }
    
    qDebug() << "AIEngine: 停止AI引擎...";
    
    m_running = false;
    
    // 停止定时器
    m_processTimer->stop();
    m_statusTimer->stop();
    m_heartbeatTimer->stop();
    
    // 停止所有AI实例
    stopAllAiInstances();
    
    // 停止组件
    m_performanceMonitor->stop();
    m_loadBalancer->stop();
    
    // 等待线程池完成
    m_threadPool->waitForDone(10000); // 等待最多10秒
    
    emit engineStopped();
    qDebug() << "AIEngine: 停止完成";
}

/**
 * @brief 关闭AI引擎
 */
void AIEngine::shutdown()
{
    qDebug() << "AIEngine: 开始关闭...";
    
    stop();
    
    // 清理资源
    {
        QMutexLocker locker(&m_aiInstancesMutex);
        m_aiInstances.clear();
    }
    
    delete m_commandQueue;
    m_commandQueue = nullptr;
    
    qDebug() << "AIEngine: 关闭完成";
}

/**
 * @brief 创建AI实例
 */
QString AIEngine::createAiInstance(const AIPlayerData& playerData)
{
    QString aiId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // 创建AI实例
    auto aiInstance = std::make_shared<AIPlayerInstance>();
    aiInstance->id = aiId;
    aiInstance->playerData = playerData;
    aiInstance->status = AIStatus::OFFLINE;
    aiInstance->createdTime = QDateTime::currentDateTime();
    aiInstance->lastUpdateTime = aiInstance->createdTime;
    
    // 分配服务器
    int serverId = m_loadBalancer->assignServer(playerData.school);
    aiInstance->playerData.serverId = serverId;
    
    // 添加到集合
    {
        QMutexLocker locker(&m_aiInstancesMutex);
        m_aiInstances[aiId] = aiInstance;
        m_totalAiCount++;
    }
    
    // 发送创建命令
    AICommand command;
    command.type = AICommandType::CREATE;
    command.aiId = aiId;
    command.priority = CommandPriority::NORMAL;
    command.timestamp = QDateTime::currentMSecsSinceEpoch();
    command.data = aiInstanceToJson(aiInstance);
    
    addCommand(command);
    
    emit aiInstanceCreated(aiId, playerData);
    
    qDebug() << "AIEngine: 创建AI实例" << aiId << "学校:" << static_cast<int>(playerData.school)
             << "服务器:" << serverId;
    
    return aiId;
}

// 继续添加其他方法的实现...
// [为了简洁，这里显示主要的初始化和创建方法]