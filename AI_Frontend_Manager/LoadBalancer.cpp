#include "LoadBalancer.h"
#include <QDebug>
#include <QMutexLocker>
#include <QUuid>
#include <QRandomGenerator>
#include <QtAlgorithms>
#include <algorithm>

LoadBalancer::LoadBalancer(QObject *parent)
    : QObject(parent)
    , m_queueProcessor(new QTimer(this))
    , m_healthCheckTimer(new QTimer(this))
    , m_autoScalingTimer(new QTimer(this))
{
    // 连接定时器
    connect(m_queueProcessor, &QTimer::timeout, this, &LoadBalancer::processRequestQueue);
    connect(m_healthCheckTimer, &QTimer::timeout, this, &LoadBalancer::performHealthCheck);
    connect(m_autoScalingTimer, &QTimer::timeout, this, &LoadBalancer::checkAutoScaling);
    
    // 启动请求队列处理器
    m_queueProcessor->start(100); // 每100ms处理一次队列
    
    // 启动自动扩缩容检查
    m_autoScalingTimer->start(60000); // 每分钟检查一次
    
    m_lastStatsUpdate = QDateTime::currentDateTime();
    
    qDebug() << "⚖️ LoadBalancer initialized with algorithm:" << static_cast<int>(m_algorithm);
}

LoadBalancer::~LoadBalancer()
{
    qDebug() << "⚖️ LoadBalancer destroyed";
}

void LoadBalancer::setAlgorithm(Algorithm algorithm)
{
    QMutexLocker locker(&m_mutex);
    if (m_algorithm != algorithm) {
        m_algorithm = algorithm;
        
        // 重置算法相关状态
        m_roundRobinIndex = 0;
        m_weightedRoundRobinCounters.clear();
        
        qDebug() << "⚖️ Load balancing algorithm changed to:" << static_cast<int>(algorithm);
        emit algorithmChanged(algorithm);
    }
}

LoadBalancer::Algorithm LoadBalancer::getCurrentAlgorithm() const
{
    QMutexLocker locker(&m_mutex);
    return m_algorithm;
}

bool LoadBalancer::addAIInstance(const AIInstance &instance)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_instances.contains(instance.id)) {
        qWarning() << "⚖️ AI instance already exists:" << instance.id;
        return false;
    }
    
    AIInstance newInstance = instance;
    newInstance.createdTime = QDateTime::currentDateTime();
    newInstance.lastHealthCheck = QDateTime::currentDateTime();
    
    m_instances.insert(instance.id, newInstance);
    
    qDebug() << "⚖️ AI instance added:" << instance.id << instance.name;
    emit instanceAdded(instance.id);
    
    updateStatistics();
    return true;
}

bool LoadBalancer::removeAIInstance(const QString &instanceId)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_instances.contains(instanceId)) {
        qWarning() << "⚖️ AI instance not found:" << instanceId;
        return false;
    }
    
    // 检查是否有活跃请求
    int activeRequests = 0;
    for (auto it = m_activeRequests.begin(); it != m_activeRequests.end(); ++it) {
        if (it.value().assignedInstanceId == instanceId) {
            activeRequests++;
        }
    }
    
    if (activeRequests > 0) {
        qWarning() << "⚖️ Cannot remove AI instance with active requests:" << instanceId << "(" << activeRequests << "requests)";
        return false;
    }
    
    m_instances.remove(instanceId);
    
    qDebug() << "⚖️ AI instance removed:" << instanceId;
    emit instanceRemoved(instanceId);
    
    updateStatistics();
    return true;
}

bool LoadBalancer::updateAIInstance(const QString &instanceId, const AIInstance &instance)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_instances.contains(instanceId)) {
        qWarning() << "⚖️ AI instance not found for update:" << instanceId;
        return false;
    }
    
    AIInstance updatedInstance = instance;
    updatedInstance.id = instanceId; // 确保ID不变
    updatedInstance.createdTime = m_instances[instanceId].createdTime; // 保持创建时间
    
    m_instances[instanceId] = updatedInstance;
    
    updateStatistics();
    return true;
}

LoadBalancer::AIInstance LoadBalancer::getAIInstance(const QString &instanceId) const
{
    QMutexLocker locker(&m_mutex);
    return m_instances.value(instanceId);
}

QList<LoadBalancer::AIInstance> LoadBalancer::getAllAIInstances() const
{
    QMutexLocker locker(&m_mutex);
    return m_instances.values();
}

QList<LoadBalancer::AIInstance> LoadBalancer::getHealthyAIInstances() const
{
    QMutexLocker locker(&m_mutex);
    QList<AIInstance> healthy;
    for (const auto &instance : m_instances.values()) {
        if (instance.isHealthy && instance.isActive) {
            healthy.append(instance);
        }
    }
    return healthy;
}

QString LoadBalancer::submitRequest(const Request &request)
{
    QMutexLocker locker(&m_mutex);
    
    Request newRequest = request;
    if (newRequest.id.isEmpty()) {
        newRequest.id = generateRequestId();
    }
    newRequest.timestamp = QDateTime::currentDateTime();
    
    // 添加到队列
    m_requestQueue.enqueue(newRequest);
    
    qDebug() << "⚖️ Request submitted:" << newRequest.id << "Type:" << newRequest.type;
    
    emit queueSizeChanged(m_requestQueue.size());
    updateStatistics();
    
    return newRequest.id;
}

bool LoadBalancer::cancelRequest(const QString &requestId)
{
    QMutexLocker locker(&m_mutex);
    
    // 从队列中移除
    QQueue<Request> newQueue;
    bool found = false;
    while (!m_requestQueue.isEmpty()) {
        Request req = m_requestQueue.dequeue();
        if (req.id == requestId) {
            found = true;
            qDebug() << "⚖️ Request cancelled from queue:" << requestId;
        } else {
            newQueue.enqueue(req);
        }
    }
    m_requestQueue = newQueue;
    
    // 从活跃请求中移除
    if (m_activeRequests.contains(requestId)) {
        m_activeRequests.remove(requestId);
        found = true;
        qDebug() << "⚖️ Active request cancelled:" << requestId;
    }
    
    if (found) {
        emit queueSizeChanged(m_requestQueue.size());
        updateStatistics();
    }
    
    return found;
}

LoadBalancer::Request LoadBalancer::getRequest(const QString &requestId) const
{
    QMutexLocker locker(&m_mutex);
    
    // 检查活跃请求
    if (m_activeRequests.contains(requestId)) {
        return m_activeRequests[requestId];
    }
    
    // 检查队列中的请求
    for (const auto &request : m_requestQueue) {
        if (request.id == requestId) {
            return request;
        }
    }
    
    return Request(); // 返回空请求
}

QList<LoadBalancer::Request> LoadBalancer::getPendingRequests() const
{
    QMutexLocker locker(&m_mutex);
    QList<Request> pending;
    for (const auto &request : m_requestQueue) {
        pending.append(request);
    }
    return pending;
}

int LoadBalancer::getQueueSize() const
{
    QMutexLocker locker(&m_mutex);
    return m_requestQueue.size();
}

void LoadBalancer::startHealthCheck(int intervalMs)
{
    if (m_healthCheckTimer->isActive()) {
        m_healthCheckTimer->stop();
    }
    
    qDebug() << "⚖️ Starting health check, interval:" << intervalMs << "ms";
    m_healthCheckTimer->start(intervalMs);
    
    // 立即执行一次健康检查
    performHealthCheck();
}

void LoadBalancer::stopHealthCheck()
{
    if (m_healthCheckTimer->isActive()) {
        qDebug() << "⚖️ Stopping health check";
        m_healthCheckTimer->stop();
    }
}

void LoadBalancer::checkInstanceHealth(const QString &instanceId)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_instances.contains(instanceId)) {
        return;
    }
    
    AIInstance &instance = m_instances[instanceId];
    
    // 简单的健康检查逻辑
    bool wasHealthy = instance.isHealthy;
    
    // 检查响应时间
    if (instance.averageResponseTime > 5000.0) { // 超过5秒认为不健康
        instance.isHealthy = false;
    }
    
    // 检查资源使用率
    if (instance.cpuUsage > 95.0 || instance.memoryUsage > 95.0) {
        instance.isHealthy = false;
    }
    
    // 检查连接数
    if (instance.currentConnections >= instance.maxConnections) {
        instance.isHealthy = false;
    }
    
    // 如果长时间没有更新，认为不健康
    QDateTime now = QDateTime::currentDateTime();
    if (instance.lastHealthCheck.secsTo(now) > 300) { // 5分钟没有更新
        instance.isHealthy = false;
    }
    
    instance.lastHealthCheck = now;
    
    if (wasHealthy != instance.isHealthy) {
        qDebug() << "⚖️ Instance health changed:" << instanceId << "Healthy:" << instance.isHealthy;
        emit instanceHealthChanged(instanceId, instance.isHealthy);
    }
}

void LoadBalancer::enableAutoScaling(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_autoScalingEnabled = enabled;
    qDebug() << "⚖️ Auto-scaling" << (enabled ? "enabled" : "disabled");
}

void LoadBalancer::setScalingThresholds(double scaleUpThreshold, double scaleDownThreshold)
{
    QMutexLocker locker(&m_mutex);
    m_scaleUpThreshold = scaleUpThreshold;
    m_scaleDownThreshold = scaleDownThreshold;
    qDebug() << "⚖️ Scaling thresholds set - Up:" << scaleUpThreshold << "%, Down:" << scaleDownThreshold << "%";
}

void LoadBalancer::setInstanceLimits(int minInstances, int maxInstances)
{
    QMutexLocker locker(&m_mutex);
    m_minInstances = minInstances;
    m_maxInstances = maxInstances;
    qDebug() << "⚖️ Instance limits set - Min:" << minInstances << ", Max:" << maxInstances;
}

LoadBalancer::Statistics LoadBalancer::getStatistics() const
{
    QMutexLocker locker(&m_mutex);
    return m_statistics;
}

void LoadBalancer::handleRequestSuccess(const QString &requestId, const QString &instanceId, 
                                      double responseTime, const QByteArray &result)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_activeRequests.contains(requestId)) {
        qWarning() << "⚖️ Success callback for unknown request:" << requestId;
        return;
    }
    
    // 更新实例统计
    if (m_instances.contains(instanceId)) {
        AIInstance &instance = m_instances[instanceId];
        instance.successfulRequests++;
        instance.totalRequests++;
        instance.totalResponseTime += responseTime;
        instance.averageResponseTime = instance.totalResponseTime / instance.totalRequests;
        instance.currentConnections = qMax(0, instance.currentConnections - 1);
    }
    
    // 移除活跃请求
    m_activeRequests.remove(requestId);
    m_successfulRequests++;
    
    qDebug() << "⚖️ Request completed successfully:" << requestId << "ResponseTime:" << responseTime << "ms";
    
    emit requestCompleted(requestId, result);
    updateStatistics();
}

void LoadBalancer::handleRequestFailure(const QString &requestId, const QString &instanceId, 
                                      const QString &error)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_activeRequests.contains(requestId)) {
        qWarning() << "⚖️ Failure callback for unknown request:" << requestId;
        return;
    }
    
    Request &request = m_activeRequests[requestId];
    
    // 更新实例统计
    if (m_instances.contains(instanceId)) {
        AIInstance &instance = m_instances[instanceId];
        instance.failedRequests++;
        instance.totalRequests++;
        instance.currentConnections = qMax(0, instance.currentConnections - 1);
    }
    
    // 检查是否需要重试
    if (request.retryCount < request.maxRetries) {
        request.retryCount++;
        request.assignedInstanceId.clear();
        
        qDebug() << "⚖️ Request failed, retrying:" << requestId << "Retry:" << request.retryCount << "/" << request.maxRetries;
        
        // 重新加入队列
        m_requestQueue.enqueue(request);
        m_activeRequests.remove(requestId);
        
        emit requestRetrying(requestId, request.retryCount);
    } else {
        // 超过最大重试次数，请求失败
        m_activeRequests.remove(requestId);
        m_failedRequests++;
        
        qDebug() << "⚖️ Request failed permanently:" << requestId << "Error:" << error;
        emit requestFailed(requestId, error);
    }
    
    updateStatistics();
}

void LoadBalancer::updateInstanceMetrics(const QString &instanceId, double cpuUsage, 
                                       double memoryUsage, int connections)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_instances.contains(instanceId)) {
        AIInstance &instance = m_instances[instanceId];
        instance.cpuUsage = cpuUsage;
        instance.memoryUsage = memoryUsage;
        instance.currentConnections = connections;
        instance.lastHealthCheck = QDateTime::currentDateTime();
    }
}

void LoadBalancer::updateInstanceHealth(const QString &instanceId, bool isHealthy)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_instances.contains(instanceId)) {
        AIInstance &instance = m_instances[instanceId];
        bool wasHealthy = instance.isHealthy;
        instance.isHealthy = isHealthy;
        instance.lastHealthCheck = QDateTime::currentDateTime();
        
        if (wasHealthy != isHealthy) {
            qDebug() << "⚖️ Instance health updated:" << instanceId << "Healthy:" << isHealthy;
            emit instanceHealthChanged(instanceId, isHealthy);
        }
    }
}

void LoadBalancer::processRequestQueue()
{
    QMutexLocker locker(&m_mutex);
    
    // 处理队列中的请求
    while (!m_requestQueue.isEmpty()) {
        QList<AIInstance> healthyInstances = getHealthyAIInstances();
        if (healthyInstances.isEmpty()) {
            qWarning() << "⚖️ No healthy AI instances available for request processing";
            break;
        }
        
        QString selectedInstanceId;
        switch (m_algorithm) {
        case Algorithm::RoundRobin:
            selectedInstanceId = selectInstanceRoundRobin();
            break;
        case Algorithm::WeightedRoundRobin:
            selectedInstanceId = selectInstanceWeightedRoundRobin();
            break;
        case Algorithm::LeastConnections:
            selectedInstanceId = selectInstanceLeastConnections();
            break;
        case Algorithm::ResponseTime:
            selectedInstanceId = selectInstanceResponseTime();
            break;
        case Algorithm::ResourceBased:
            selectedInstanceId = selectInstanceResourceBased();
            break;
        }
        
        if (selectedInstanceId.isEmpty()) {
            break; // 没有可用实例
        }
        
        // 分配请求
        Request request = m_requestQueue.dequeue();
        assignRequest(request.id, selectedInstanceId);
    }
    
    if (!m_requestQueue.isEmpty()) {
        emit queueSizeChanged(m_requestQueue.size());
    }
}

void LoadBalancer::performHealthCheck()
{
    QMutexLocker locker(&m_mutex);
    
    for (auto it = m_instances.begin(); it != m_instances.end(); ++it) {
        checkInstanceHealth(it.key());
    }
    
    updateStatistics();
}

void LoadBalancer::checkAutoScaling()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_autoScalingEnabled) {
        return;
    }
    
    // 计算平均资源使用率
    double avgCpuUsage = 0.0;
    double avgMemoryUsage = 0.0;
    int healthyCount = 0;
    
    for (const auto &instance : m_instances.values()) {
        if (instance.isHealthy && instance.isActive) {
            avgCpuUsage += instance.cpuUsage;
            avgMemoryUsage += instance.memoryUsage;
            healthyCount++;
        }
    }
    
    if (healthyCount == 0) {
        return;
    }
    
    avgCpuUsage /= healthyCount;
    avgMemoryUsage /= healthyCount;
    
    // 检查是否需要扩容
    if ((avgCpuUsage > m_scaleUpThreshold || avgMemoryUsage > m_scaleUpThreshold) && canScaleUp()) {
        scaleUp();
    }
    // 检查是否需要缩容
    else if ((avgCpuUsage < m_scaleDownThreshold && avgMemoryUsage < m_scaleDownThreshold) && canScaleDown()) {
        scaleDown();
    }
}

QString LoadBalancer::selectInstanceRoundRobin()
{
    QList<AIInstance> healthy = getHealthyAIInstances();
    if (healthy.isEmpty()) {
        return QString();
    }
    
    m_roundRobinIndex = m_roundRobinIndex % healthy.size();
    QString selectedId = healthy[m_roundRobinIndex].id;
    m_roundRobinIndex = (m_roundRobinIndex + 1) % healthy.size();
    
    return selectedId;
}

QString LoadBalancer::selectInstanceWeightedRoundRobin()
{
    QList<AIInstance> healthy = getHealthyAIInstances();
    if (healthy.isEmpty()) {
        return QString();
    }
    
    // 实现加权轮询逻辑
    QString selectedId;
    int maxWeightCount = 0;
    
    for (const auto &instance : healthy) {
        int currentCount = m_weightedRoundRobinCounters.value(instance.id, 0);
        if (currentCount < instance.weight) {
            if (instance.weight - currentCount > maxWeightCount) {
                maxWeightCount = instance.weight - currentCount;
                selectedId = instance.id;
            }
        }
    }
    
    if (selectedId.isEmpty()) {
        // 重置所有计数器
        m_weightedRoundRobinCounters.clear();
        selectedId = healthy.first().id;
    }
    
    m_weightedRoundRobinCounters[selectedId]++;
    return selectedId;
}

QString LoadBalancer::selectInstanceLeastConnections()
{
    QList<AIInstance> healthy = getHealthyAIInstances();
    if (healthy.isEmpty()) {
        return QString();
    }
    
    QString selectedId;
    int minConnections = INT_MAX;
    
    for (const auto &instance : healthy) {
        if (instance.currentConnections < minConnections) {
            minConnections = instance.currentConnections;
            selectedId = instance.id;
        }
    }
    
    return selectedId;
}

QString LoadBalancer::selectInstanceResponseTime()
{
    QList<AIInstance> healthy = getHealthyAIInstances();
    if (healthy.isEmpty()) {
        return QString();
    }
    
    QString selectedId;
    double minResponseTime = DBL_MAX;
    
    for (const auto &instance : healthy) {
        if (instance.averageResponseTime < minResponseTime) {
            minResponseTime = instance.averageResponseTime;
            selectedId = instance.id;
        }
    }
    
    return selectedId;
}

QString LoadBalancer::selectInstanceResourceBased()
{
    QList<AIInstance> healthy = getHealthyAIInstances();
    if (healthy.isEmpty()) {
        return QString();
    }
    
    QString selectedId;
    double minResourceUsage = DBL_MAX;
    
    for (const auto &instance : healthy) {
        double resourceUsage = (instance.cpuUsage + instance.memoryUsage) / 2.0;
        if (resourceUsage < minResourceUsage) {
            minResourceUsage = resourceUsage;
            selectedId = instance.id;
        }
    }
    
    return selectedId;
}

QString LoadBalancer::generateRequestId() const
{
    return "REQ_" + QUuid::createUuid().toString(QUuid::WithoutBraces).toUpper();
}

QString LoadBalancer::generateInstanceId() const
{
    return "AI_" + QUuid::createUuid().toString(QUuid::WithoutBraces).toUpper();
}

void LoadBalancer::assignRequest(const QString &requestId, const QString &instanceId)
{
    if (!m_activeRequests.contains(requestId)) {
        qWarning() << "⚖️ Trying to assign unknown request:" << requestId;
        return;
    }
    
    Request &request = m_activeRequests[requestId];
    request.assignedInstanceId = instanceId;
    
    // 更新实例连接数
    if (m_instances.contains(instanceId)) {
        m_instances[instanceId].currentConnections++;
    }
    
    qDebug() << "⚖️ Request assigned:" << requestId << "to instance:" << instanceId;
    emit requestAssigned(requestId, instanceId);
}

void LoadBalancer::retryRequest(const QString &requestId)
{
    if (m_activeRequests.contains(requestId)) {
        Request request = m_activeRequests[requestId];
        request.assignedInstanceId.clear();
        m_activeRequests.remove(requestId);
        m_requestQueue.enqueue(request);
    }
}

void LoadBalancer::updateStatistics()
{
    m_statistics.totalInstances = m_instances.size();
    m_statistics.healthyInstances = getHealthyAIInstances().size();
    m_statistics.queuedRequests = m_requestQueue.size();
    m_statistics.totalRequests = m_totalRequests;
    m_statistics.successfulRequests = m_successfulRequests;
    m_statistics.failedRequests = m_failedRequests;
    
    // 计算活跃连接数和平均响应时间
    int totalConnections = 0;
    double totalResponseTime = 0.0;
    int responseTimeCount = 0;
    
    for (const auto &instance : m_instances.values()) {
        totalConnections += instance.currentConnections;
        if (instance.totalRequests > 0) {
            totalResponseTime += instance.averageResponseTime;
            responseTimeCount++;
        }
    }
    
    m_statistics.activeConnections = totalConnections;
    if (responseTimeCount > 0) {
        m_statistics.averageResponseTime = totalResponseTime / responseTimeCount;
    }
    
    // 计算请求/秒
    QDateTime now = QDateTime::currentDateTime();
    qint64 secondsElapsed = m_lastStatsUpdate.secsTo(now);
    if (secondsElapsed > 0) {
        m_statistics.requestsPerSecond = static_cast<double>(m_totalRequests) / secondsElapsed;
    }
    
    emit statisticsUpdated(m_statistics);
}

bool LoadBalancer::canScaleUp() const
{
    return m_instances.size() < m_maxInstances;
}

bool LoadBalancer::canScaleDown() const
{
    return m_instances.size() > m_minInstances;
}

void LoadBalancer::scaleUp()
{
    // 这里应该实现实际的扩容逻辑
    // 例如启动新的AI实例
    qDebug() << "⚖️ Scaling up (placeholder implementation)";
    
    // 示例：创建新实例
    AIInstance newInstance;
    newInstance.id = generateInstanceId();
    newInstance.name = QString("Auto-scaled AI %1").arg(m_instances.size() + 1);
    newInstance.isActive = true;
    newInstance.isHealthy = true;
    newInstance.maxConnections = 100;
    newInstance.weight = 1;
    
    addAIInstance(newInstance);
    emit instanceScaled(m_instances.size());
}

void LoadBalancer::scaleDown()
{
    // 这里应该实现实际的缩容逻辑
    // 例如停止最少使用的AI实例
    qDebug() << "⚖️ Scaling down (placeholder implementation)";
    
    // 示例：移除连接数最少且健康的实例
    QString instanceToRemove;
    int minConnections = INT_MAX;
    
    for (const auto &instance : m_instances.values()) {
        if (instance.isHealthy && instance.currentConnections < minConnections) {
            minConnections = instance.currentConnections;
            instanceToRemove = instance.id;
        }
    }
    
    if (!instanceToRemove.isEmpty() && minConnections == 0) {
        removeAIInstance(instanceToRemove);
        emit instanceScaled(m_instances.size());
    }
}

#include "LoadBalancer.moc"
