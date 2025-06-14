#include "PerformanceMonitor.h"
#include <QDebug>
#include <QMutexLocker>
#include <QDateTime>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "psapi.lib")
#endif

PerformanceMonitor::PerformanceMonitor(QObject *parent)
    : QObject(parent)
    , m_updateTimer(new QTimer(this))
{
    // 连接定时器
    connect(m_updateTimer, &QTimer::timeout, this, &PerformanceMonitor::updateMetrics);
    
    // 初始化系统句柄
#ifdef Q_OS_WIN
    m_processHandle = GetCurrentProcess();
#endif
    
    qDebug() << "🔍 PerformanceMonitor initialized";
}

PerformanceMonitor::~PerformanceMonitor()
{
    stopMonitoring();
    qDebug() << "🔍 PerformanceMonitor destroyed";
}

void PerformanceMonitor::startMonitoring(int intervalMs)
{
    if (m_updateTimer->isActive()) {
        return;
    }
    
    qDebug() << "🔍 Starting performance monitoring, interval:" << intervalMs << "ms";
    
    m_updateTimer->start(intervalMs);
    
    // 立即更新一次
    updateMetrics();
}

void PerformanceMonitor::stopMonitoring()
{
    if (m_updateTimer->isActive()) {
        qDebug() << "🔍 Stopping performance monitoring";
        m_updateTimer->stop();
    }
}

bool PerformanceMonitor::isMonitoring() const
{
    return m_updateTimer->isActive();
}

PerformanceMonitor::SystemMetrics PerformanceMonitor::getSystemMetrics() const
{
    QMutexLocker locker(&m_mutex);
    return m_currentSystemMetrics;
}

PerformanceMonitor::AIMetrics PerformanceMonitor::getAIMetrics() const
{
    QMutexLocker locker(&m_mutex);
    return m_currentAIMetrics;
}

QList<PerformanceMonitor::SystemMetrics> PerformanceMonitor::getSystemHistory(int maxPoints) const
{
    QMutexLocker locker(&m_mutex);
    if (m_systemHistory.size() <= maxPoints) {
        return m_systemHistory;
    }
    return m_systemHistory.mid(m_systemHistory.size() - maxPoints);
}

QList<PerformanceMonitor::AIMetrics> PerformanceMonitor::getAIHistory(int maxPoints) const
{
    QMutexLocker locker(&m_mutex);
    if (m_aiHistory.size() <= maxPoints) {
        return m_aiHistory;
    }
    return m_aiHistory.mid(m_aiHistory.size() - maxPoints);
}

void PerformanceMonitor::setWarningThresholds(double cpuThreshold, double memoryThreshold)
{
    m_cpuWarningThreshold = cpuThreshold;
    m_memoryWarningThreshold = memoryThreshold;
    qDebug() << "🔍 Warning thresholds set - CPU:" << cpuThreshold << "%, Memory:" << memoryThreshold << "%";
}

void PerformanceMonitor::setCriticalThresholds(double cpuThreshold, double memoryThreshold)
{
    m_cpuCriticalThreshold = cpuThreshold;
    m_memoryCriticalThreshold = memoryThreshold;
    qDebug() << "🔍 Critical thresholds set - CPU:" << cpuThreshold << "%, Memory:" << memoryThreshold << "%";
}

void PerformanceMonitor::updateAIMetrics(int activeAIs, double responseTime, int requests)
{
    QMutexLocker locker(&m_mutex);
    
    m_currentAIMetrics.activeAIs = activeAIs;
    m_currentAIMetrics.averageResponseTime = responseTime;
    m_totalRequests += requests;
    m_currentAIMetrics.totalRequests = m_totalRequests;
    
    // 计算请求成功率
    if (m_totalRequests > 0) {
        m_currentAIMetrics.requestsPerSecond = static_cast<double>(requests);
    }
    
    emit aiMetricsUpdated(m_currentAIMetrics);
}

void PerformanceMonitor::updateNetworkMetrics(quint64 bytesReceived, quint64 bytesSent)
{
    QMutexLocker locker(&m_mutex);
    
    m_currentSystemMetrics.networkBytesReceived += bytesReceived;
    m_currentSystemMetrics.networkBytesSent += bytesSent;
}

void PerformanceMonitor::updateMetrics()
{
    SystemMetrics newMetrics;
    
    // 收集系统指标
    newMetrics.cpuUsage = getCpuUsage();
    getMemoryUsage(newMetrics.memoryUsed, newMetrics.memoryTotal);
    if (newMetrics.memoryTotal > 0) {
        newMetrics.memoryUsagePercent = (static_cast<double>(newMetrics.memoryUsed) / newMetrics.memoryTotal) * 100.0;
    }
    newMetrics.diskUsage = getDiskUsage();
    getNetworkStats(newMetrics.networkBytesReceived, newMetrics.networkBytesSent);
    
    {
        QMutexLocker locker(&m_mutex);
        
        // 更新当前指标
        m_currentSystemMetrics = newMetrics;
        
        // 添加到历史记录
        m_systemHistory.append(newMetrics);
        if (m_systemHistory.size() > 1000) { // 保持最近1000个数据点
            m_systemHistory.removeFirst();
        }
        
        // 更新AI指标历史
        m_aiHistory.append(m_currentAIMetrics);
        if (m_aiHistory.size() > 1000) {
            m_aiHistory.removeFirst();
        }
    }
    
    // 检查阈值警告
    if (newMetrics.cpuUsage > m_cpuCriticalThreshold) {
        emit performanceCritical(QString("CPU使用率过高: %1%").arg(newMetrics.cpuUsage, 0, 'f', 1));
    } else if (newMetrics.cpuUsage > m_cpuWarningThreshold) {
        emit performanceWarning(QString("CPU使用率警告: %1%").arg(newMetrics.cpuUsage, 0, 'f', 1));
    }
    
    if (newMetrics.memoryUsagePercent > m_memoryCriticalThreshold) {
        emit performanceCritical(QString("内存使用率过高: %1%").arg(newMetrics.memoryUsagePercent, 0, 'f', 1));
    } else if (newMetrics.memoryUsagePercent > m_memoryWarningThreshold) {
        emit performanceWarning(QString("内存使用率警告: %1%").arg(newMetrics.memoryUsagePercent, 0, 'f', 1));
    }
    
    emit systemMetricsUpdated(newMetrics);
}

double PerformanceMonitor::getCpuUsage()
{
#ifdef Q_OS_WIN
    static PDH_HCOUNTER cpuTotal;
    static PDH_HQUERY cpuQuery;
    static bool initialized = false;
    
    if (!initialized) {
        PdhOpenQuery(NULL, NULL, &cpuQuery);
        PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        initialized = true;
        return 0.0; // 第一次调用返回0
    }
    
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    
    return counterVal.doubleValue;
#else
    // Linux/Mac实现
    static QFile statFile("/proc/stat");
    static quint64 lastIdle = 0, lastTotal = 0;
    
    if (!statFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 0.0;
    }
    
    QString line = statFile.readLine();
    statFile.close();
    
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 8) return 0.0;
    
    quint64 user = parts[1].toULongLong();
    quint64 nice = parts[2].toULongLong();
    quint64 system = parts[3].toULongLong();
    quint64 idle = parts[4].toULongLong();
    
    quint64 total = user + nice + system + idle;
    quint64 totalDiff = total - lastTotal;
    quint64 idleDiff = idle - lastIdle;
    
    double cpuUsage = 0.0;
    if (totalDiff > 0) {
        cpuUsage = ((totalDiff - idleDiff) * 100.0) / totalDiff;
    }
    
    lastTotal = total;
    lastIdle = idle;
    
    return cpuUsage;
#endif
}

void PerformanceMonitor::getMemoryUsage(quint64 &used, quint64 &total)
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    total = memInfo.ullTotalPhys;
    used = total - memInfo.ullAvailPhys;
#else
    // Linux实现
    QFile meminfoFile("/proc/meminfo");
    if (!meminfoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        used = total = 0;
        return;
    }
    
    quint64 memTotal = 0, memAvailable = 0;
    while (!meminfoFile.atEnd()) {
        QString line = meminfoFile.readLine();
        if (line.startsWith("MemTotal:")) {
            memTotal = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong() * 1024;
        } else if (line.startsWith("MemAvailable:")) {
            memAvailable = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong() * 1024;
        }
    }
    
    total = memTotal;
    used = memTotal - memAvailable;
#endif
}

double PerformanceMonitor::getDiskUsage()
{
#ifdef Q_OS_WIN
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes;
    if (GetDiskFreeSpaceEx(L"C:\\", &freeBytesAvailable, &totalNumberOfBytes, NULL)) {
        double usedBytes = totalNumberOfBytes.QuadPart - freeBytesAvailable.QuadPart;
        return (usedBytes / totalNumberOfBytes.QuadPart) * 100.0;
    }
#endif
    return 0.0;
}

void PerformanceMonitor::getNetworkStats(quint64 &received, quint64 &sent)
{
    // 这里可以实现网络统计收集
    // 暂时使用累积值
    received = m_currentSystemMetrics.networkBytesReceived;
    sent = m_currentSystemMetrics.networkBytesSent;
}

#include "PerformanceMonitor.moc"
