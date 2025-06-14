#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <atomic>
#include <memory>

/**
 * @brief 性能监控器 - 实时监控系统性能指标
 * 
 * 功能特性:
 * - CPU使用率监控
 * - 内存使用统计
 * - 网络流量监控
 * - AI引擎性能指标
 * - 实时数据更新
 * - 历史数据记录
 */
class PerformanceMonitor : public QObject
{
    Q_OBJECT

public:
    struct SystemMetrics {
        double cpuUsage = 0.0;          // CPU使用率 (0-100)
        quint64 memoryUsed = 0;         // 内存使用量 (字节)
        quint64 memoryTotal = 0;        // 总内存 (字节)
        double memoryUsagePercent = 0.0; // 内存使用率 (0-100)
        quint64 networkBytesReceived = 0; // 网络接收字节数
        quint64 networkBytesSent = 0;    // 网络发送字节数
        double diskUsage = 0.0;         // 磁盘使用率 (0-100)
        int activeConnections = 0;      // 活跃连接数
    };

    struct AIMetrics {
        int activeAIs = 0;              // 活跃AI数量
        int totalRequests = 0;          // 总请求数
        double averageResponseTime = 0.0; // 平均响应时间 (毫秒)
        int successfulRequests = 0;     // 成功请求数
        int failedRequests = 0;         // 失败请求数
        double requestsPerSecond = 0.0; // 每秒请求数
        double aiCpuUsage = 0.0;        // AI引擎CPU使用率
        quint64 aiMemoryUsage = 0;      // AI引擎内存使用量
    };

    explicit PerformanceMonitor(QObject *parent = nullptr);
    ~PerformanceMonitor();

    // 启动/停止监控
    void startMonitoring(int intervalMs = 1000);
    void stopMonitoring();
    bool isMonitoring() const;

    // 获取实时指标
    SystemMetrics getSystemMetrics() const;
    AIMetrics getAIMetrics() const;

    // 历史数据
    QList<SystemMetrics> getSystemHistory(int maxPoints = 100) const;
    QList<AIMetrics> getAIHistory(int maxPoints = 100) const;

    // 性能阈值设置
    void setWarningThresholds(double cpuThreshold, double memoryThreshold);
    void setCriticalThresholds(double cpuThreshold, double memoryThreshold);

public slots:
    // 更新AI指标
    void updateAIMetrics(int activeAIs, double responseTime, int requests);
    void updateNetworkMetrics(quint64 bytesReceived, quint64 bytesSent);

signals:
    // 指标更新信号
    void systemMetricsUpdated(const SystemMetrics &metrics);
    void aiMetricsUpdated(const AIMetrics &metrics);
    
    // 警告信号
    void performanceWarning(const QString &message);
    void performanceCritical(const QString &message);

private slots:
    void updateMetrics();

private:
    // 系统指标收集
    double getCpuUsage();
    void getMemoryUsage(quint64 &used, quint64 &total);
    double getDiskUsage();
    void getNetworkStats(quint64 &received, quint64 &sent);

    // 数据成员
    QTimer *m_updateTimer;
    mutable QMutex m_mutex;
    
    SystemMetrics m_currentSystemMetrics;
    AIMetrics m_currentAIMetrics;
    
    QList<SystemMetrics> m_systemHistory;
    QList<AIMetrics> m_aiHistory;
    
    // 阈值设置
    double m_cpuWarningThreshold = 80.0;
    double m_memoryWarningThreshold = 85.0;
    double m_cpuCriticalThreshold = 95.0;
    double m_memoryCriticalThreshold = 95.0;
    
    // 性能计数器
    std::atomic<int> m_totalRequests{0};
    std::atomic<int> m_successfulRequests{0};
    std::atomic<int> m_failedRequests{0};
    
    // 平台特定数据
#ifdef Q_OS_WIN
    void *m_processHandle = nullptr;
    quint64 m_lastCpuTime = 0;
    quint64 m_lastSystemTime = 0;
#endif
};

#endif // PERFORMANCEMONITOR_H
