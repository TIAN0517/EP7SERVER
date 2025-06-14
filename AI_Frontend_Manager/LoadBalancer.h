#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QMap>
#include <QDateTime>
#include <atomic>
#include <memory>

/**
 * @brief AI引擎负载均衡器 - 智能分配AI请求和资源管理
 * 
 * 功能特性:
 * - 多种负载均衡算法 (轮询、加权轮询、最少连接、响应时间)
 * - 动态AI实例管理
 * - 健康检查和故障转移
 * - 请求队列管理
 * - 性能监控集成
 * - 自动扩缩容
 */
class LoadBalancer : public QObject
{
    Q_OBJECT

public:
    enum class Algorithm {
        RoundRobin,        // 轮询
        WeightedRoundRobin, // 加权轮询
        LeastConnections,  // 最少连接
        ResponseTime,      // 响应时间
        ResourceBased      // 基于资源使用
    };

    struct AIInstance {
        QString id;                     // AI实例ID
        QString name;                   // 显示名称
        bool isActive = false;          // 是否活跃
        bool isHealthy = true;          // 健康状态
        int currentConnections = 0;     // 当前连接数
        int maxConnections = 100;       // 最大连接数
        double averageResponseTime = 0.0; // 平均响应时间
        double cpuUsage = 0.0;          // CPU使用率
        double memoryUsage = 0.0;       // 内存使用率
        int weight = 1;                 // 权重 (用于加权算法)
        QDateTime lastHealthCheck;      // 最后健康检查时间
        QDateTime createdTime;          // 创建时间
        
        // 统计信息
        int totalRequests = 0;          // 总请求数
        int successfulRequests = 0;     // 成功请求数
        int failedRequests = 0;         // 失败请求数
        double totalResponseTime = 0.0; // 总响应时间
    };

    struct Request {
        QString id;                     // 请求ID
        QString type;                   // 请求类型
        QByteArray data;               // 请求数据
        QDateTime timestamp;           // 请求时间戳
        int priority = 0;              // 优先级 (数字越大优先级越高)
        int retryCount = 0;            // 重试次数
        int maxRetries = 3;            // 最大重试次数
        QString assignedInstanceId;    // 分配的实例ID
    };

    explicit LoadBalancer(QObject *parent = nullptr);
    ~LoadBalancer();

    // 算法设置
    void setAlgorithm(Algorithm algorithm);
    Algorithm getCurrentAlgorithm() const;

    // AI实例管理
    bool addAIInstance(const AIInstance &instance);
    bool removeAIInstance(const QString &instanceId);
    bool updateAIInstance(const QString &instanceId, const AIInstance &instance);
    AIInstance getAIInstance(const QString &instanceId) const;
    QList<AIInstance> getAllAIInstances() const;
    QList<AIInstance> getHealthyAIInstances() const;

    // 请求处理
    QString submitRequest(const Request &request);
    bool cancelRequest(const QString &requestId);
    Request getRequest(const QString &requestId) const;
    QList<Request> getPendingRequests() const;
    int getQueueSize() const;

    // 健康检查
    void startHealthCheck(int intervalMs = 30000);
    void stopHealthCheck();
    void checkInstanceHealth(const QString &instanceId);

    // 自动扩缩容
    void enableAutoScaling(bool enabled);
    void setScalingThresholds(double scaleUpThreshold, double scaleDownThreshold);
    void setInstanceLimits(int minInstances, int maxInstances);

    // 统计信息
    struct Statistics {
        int totalInstances = 0;
        int healthyInstances = 0;
        int activeConnections = 0;
        int queuedRequests = 0;
        double averageResponseTime = 0.0;
        int totalRequests = 0;
        int successfulRequests = 0;
        int failedRequests = 0;
        double requestsPerSecond = 0.0;
    };
    Statistics getStatistics() const;

public slots:
    // 请求结果处理
    void handleRequestSuccess(const QString &requestId, const QString &instanceId, 
                            double responseTime, const QByteArray &result);
    void handleRequestFailure(const QString &requestId, const QString &instanceId, 
                            const QString &error);
    
    // AI实例状态更新
    void updateInstanceMetrics(const QString &instanceId, double cpuUsage, 
                             double memoryUsage, int connections);
    void updateInstanceHealth(const QString &instanceId, bool isHealthy);

signals:
    // 请求处理信号
    void requestAssigned(const QString &requestId, const QString &instanceId);
    void requestCompleted(const QString &requestId, const QByteArray &result);
    void requestFailed(const QString &requestId, const QString &error);
    void requestRetrying(const QString &requestId, int retryCount);

    // 实例状态信号
    void instanceAdded(const QString &instanceId);
    void instanceRemoved(const QString &instanceId);
    void instanceHealthChanged(const QString &instanceId, bool isHealthy);
    void instanceScaled(int newInstanceCount);

    // 负载均衡状态信号
    void algorithmChanged(Algorithm newAlgorithm);
    void statisticsUpdated(const Statistics &stats);
    void queueSizeChanged(int queueSize);

private slots:
    void processRequestQueue();
    void performHealthCheck();
    void checkAutoScaling();

private:
    // 负载均衡算法实现
    QString selectInstanceRoundRobin();
    QString selectInstanceWeightedRoundRobin();
    QString selectInstanceLeastConnections();
    QString selectInstanceResponseTime();
    QString selectInstanceResourceBased();

    // 内部辅助方法
    QString generateRequestId() const;
    QString generateInstanceId() const;
    void assignRequest(const QString &requestId, const QString &instanceId);
    void retryRequest(const QString &requestId);
    void updateStatistics();
    bool canScaleUp() const;
    bool canScaleDown() const;
    void scaleUp();
    void scaleDown();

    // 数据成员
    mutable QMutex m_mutex;
    Algorithm m_algorithm = Algorithm::RoundRobin;
    
    QMap<QString, AIInstance> m_instances;
    QQueue<Request> m_requestQueue;
    QMap<QString, Request> m_activeRequests;
    
    QTimer *m_queueProcessor = nullptr;
    QTimer *m_healthCheckTimer = nullptr;
    QTimer *m_autoScalingTimer = nullptr;
    
    // 轮询状态
    int m_roundRobinIndex = 0;
    QMap<QString, int> m_weightedRoundRobinCounters;
    
    // 自动扩缩容设置
    bool m_autoScalingEnabled = false;
    double m_scaleUpThreshold = 80.0;      // CPU/内存使用率阈值
    double m_scaleDownThreshold = 30.0;
    int m_minInstances = 1;
    int m_maxInstances = 10;
    
    // 统计数据
    Statistics m_statistics;
    std::atomic<int> m_totalRequests{0};
    std::atomic<int> m_successfulRequests{0};
    std::atomic<int> m_failedRequests{0};
    QDateTime m_lastStatsUpdate;
};

Q_DECLARE_METATYPE(LoadBalancer::Algorithm)
Q_DECLARE_METATYPE(LoadBalancer::AIInstance)
Q_DECLARE_METATYPE(LoadBalancer::Request)
Q_DECLARE_METATYPE(LoadBalancer::Statistics)

#endif // LOADBALANCER_H
