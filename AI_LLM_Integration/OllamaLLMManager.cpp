#include "OllamaLLMManager.h"
#include <QNetworkReply>
#include <QJsonParseError>
#include <QUrlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QMutexLocker>
#include <algorithm>

// ====================================================================
// LLMRequestTask Implementation
// ====================================================================

LLMRequestTask::LLMRequestTask(const LLMRequestConfig &config, 
                               const OllamaServerConfig &server,
                               QObject *parent)
    : QObject(parent)
    , QRunnable()
    , m_config(config)
    , m_server(server)
    , m_networkManager(std::make_unique<QNetworkAccessManager>())
{
    setAutoDelete(true);
}

void LLMRequestTask::run()
{
    QElapsedTimer timer;
    timer.start();
    
    // 構建請求URL
    QString url = QString("http://%1:%2/api/generate").arg(m_server.host).arg(m_server.port);
    
    // 構建請求JSON
    QJsonObject requestJson;
    requestJson["model"] = m_config.modelName;
    requestJson["prompt"] = m_config.prompt;
    requestJson["stream"] = m_config.stream;
    
    // 添加可選參數
    QJsonObject options;
    options["temperature"] = m_config.temperature;
    options["num_predict"] = m_config.maxTokens;
    options["top_k"] = m_config.topK;
    options["top_p"] = m_config.topP;
    options["repeat_penalty"] = m_config.repeatPenalty;
    requestJson["options"] = options;
      if (!m_config.systemPrompt.isEmpty()) {
        requestJson["system"] = m_config.systemPrompt;
    }
    
    if (!m_config.context.isEmpty()) {
        QJsonArray contextArray;
        for (const QString &item : m_config.context) {
            contextArray.append(item);
        }
        requestJson["context"] = contextArray;
    }
    
    QJsonDocument requestDoc(requestJson);
    QByteArray requestData = requestDoc.toJson();
    
    // 創建HTTP請求
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "RANOnline-AI-LLM-Client/2.0");
    
    // 發送POST請求
    QNetworkReply *reply = m_networkManager->post(request, requestData);
    
    // 設置超時
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(30000); // 30秒超時
    
    // 連接信號
    connect(reply, &QNetworkReply::finished, [this, reply, timer]() {
        LLMResponse response;
        response.requestId = m_config.requestId;
        response.modelName = m_config.modelName;
        response.academy = m_config.academy;
        response.timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
        response.responseTime = timer.elapsed();
        
        if (reply->error() == QNetworkReply::NoError) {
            if (m_config.stream) {
                // 流式回應已在readyRead中處理
                response.isDone = true;
                response.isError = false;
            } else {
                // 非流式回應
                QByteArray responseData = reply->readAll();
                QJsonParseError parseError;
                QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &parseError);
                
                if (parseError.error == QJsonParseError::NoError) {
                    QJsonObject responseObj = responseDoc.object();
                    response.content = responseObj["response"].toString();
                    response.isDone = responseObj["done"].toBool();
                    response.isError = false;
                } else {
                    response.isError = true;
                    response.errorMessage = QString("JSON解析錯誤: %1").arg(parseError.errorString());
                }
            }
        } else {
            response.isError = true;
            response.errorMessage = reply->errorString();
        }
        
        emit requestCompleted(response);
        reply->deleteLater();
    });
    
    // 處理流式數據
    if (m_config.stream) {
        connect(reply, &QNetworkReply::readyRead, [this, reply]() {
            processStreamResponse(reply);
        });
    }
      // 處理超時
    connect(&timeoutTimer, &QTimer::timeout, [reply]() {
        reply->abort();
    });
    
    // 處理網絡錯誤
    connect(reply, &QNetworkReply::errorOccurred,
            [this](QNetworkReply::NetworkError error) {
        handleNetworkError(error);
    });
    
    // 等待請求完成
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.exec();
}

void LLMRequestTask::processStreamResponse(QNetworkReply *reply)
{
    static QByteArray buffer;
    buffer.append(reply->readAll());
    
    // 按行處理JSON
    while (buffer.contains('\n')) {
        int newlineIndex = buffer.indexOf('\n');
        QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);
        
        if (line.trimmed().isEmpty()) continue;
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
        
        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            QString content = obj["response"].toString();
            bool done = obj["done"].toBool();
            
            if (!content.isEmpty()) {
                emit streamUpdate(m_config.requestId, content);
            }
            
            if (done) {
                emit requestProgress(m_config.requestId, 100);
                break;
            }
        }
    }
}

void LLMRequestTask::handleNetworkError(QNetworkReply::NetworkError error)
{
    LLMResponse response;
    response.requestId = m_config.requestId;
    response.modelName = m_config.modelName;
    response.academy = m_config.academy;
    response.timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    response.isError = true;
    
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
        response.errorMessage = "連接被拒絕，請檢查Ollama服務是否運行";
        break;
    case QNetworkReply::HostNotFoundError:
        response.errorMessage = "找不到主機，請檢查服務器地址";
        break;
    case QNetworkReply::TimeoutError:
        response.errorMessage = "請求超時";
        break;
    default:
        response.errorMessage = QString("網絡錯誤: %1").arg(static_cast<int>(error));
        break;
    }
    
    emit requestCompleted(response);
}

// ====================================================================
// LoadBalancer Implementation
// ====================================================================

LoadBalancer::LoadBalancer(QObject *parent)
    : QObject(parent)
{
}

void LoadBalancer::addServer(const OllamaServerConfig &server)
{
    QMutexLocker locker(&m_mutex);
    m_servers[server.name] = server;
}

void LoadBalancer::removeServer(const QString &serverName)
{
    QMutexLocker locker(&m_mutex);
    m_servers.remove(serverName);
}

void LoadBalancer::updateServerLoad(const QString &serverName, int load)
{
    QMutexLocker locker(&m_mutex);
    if (m_servers.contains(serverName)) {
        m_servers[serverName].currentLoad = load;
        m_servers[serverName].lastResponseTime = QDateTime::currentMSecsSinceEpoch();
    }
}

OllamaServerConfig LoadBalancer::selectOptimalServer() const
{
    QMutexLocker locker(&m_mutex);
    
    if (m_servers.isEmpty()) {
        return OllamaServerConfig();
    }
    
    QList<OllamaServerConfig> activeServers;
    for (const auto &server : m_servers) {
        if (server.isActive && server.currentLoad < server.maxConcurrent) {
            activeServers.append(server);
        }
    }
    
    if (activeServers.isEmpty()) {
        return m_servers.first(); // 回退到第一個服務器
    }
    
    if (m_balanceStrategy == "round_robin") {
        // 輪詢策略
        OllamaServerConfig selected = activeServers[m_roundRobinIndex % activeServers.size()];
        m_roundRobinIndex++;
        return selected;
    } else if (m_balanceStrategy == "least_connections") {
        // 最少連接策略
        auto minLoadServer = std::min_element(activeServers.begin(), activeServers.end(),
            [](const OllamaServerConfig &a, const OllamaServerConfig &b) {
                return a.currentLoad < b.currentLoad;
            });
        return *minLoadServer;
    } else if (m_balanceStrategy == "weighted") {
        // 加權策略
        int totalWeight = 0;
        for (const auto &server : activeServers) {
            totalWeight += server.weight;
        }
        
        int randomValue = QRandomGenerator::global()->bounded(totalWeight);
        int currentWeight = 0;
        
        for (const auto &server : activeServers) {
            currentWeight += server.weight;
            if (randomValue < currentWeight) {
                return server;
            }
        }
    }
    
    return activeServers.first();
}

QList<OllamaServerConfig> LoadBalancer::getAllServers() const
{
    QMutexLocker locker(&m_mutex);
    return m_servers.values();
}

void LoadBalancer::setBalanceStrategy(const QString &strategy)
{
    QMutexLocker locker(&m_mutex);
    m_balanceStrategy = strategy;
}

// ====================================================================
// OllamaLLMManager Implementation
// ====================================================================

OllamaLLMManager::OllamaLLMManager(QObject *parent)
    : QObject(parent)
    , m_loadBalancer(std::make_unique<LoadBalancer>(this))
    , m_threadPool(std::make_unique<QThreadPool>(this))
    , m_networkManager(std::make_unique<QNetworkAccessManager>(this))
    , m_healthCheckTimer(new QTimer(this))
    , m_cleanupTimer(new QTimer(this))
    , m_statsTimer(new QTimer(this))
{
    // 設置線程池
    m_threadPool->setMaxThreadCount(m_maxConcurrentRequests);
    
    // 設置定時器
    m_healthCheckTimer->setInterval(30000); // 30秒健康檢查
    m_cleanupTimer->setInterval(60000);     // 60秒清理
    m_statsTimer->setInterval(5000);        // 5秒統計更新
    
    // 連接信號
    connect(m_healthCheckTimer, &QTimer::timeout, this, &OllamaLLMManager::checkServerHealth);
    connect(m_cleanupTimer, &QTimer::timeout, this, &OllamaLLMManager::cleanupCompletedRequests);
    connect(m_statsTimer, &QTimer::timeout, this, &OllamaLLMManager::updateStatistics);
}

OllamaLLMManager::~OllamaLLMManager()
{
    shutdown();
}

bool OllamaLLMManager::initialize()
{
    qDebug() << "初始化Ollama LLM管理器...";
    
    // 添加默認服務器
    OllamaServerConfig defaultServer;
    defaultServer.name = "本地Ollama";
    defaultServer.host = "localhost";
    defaultServer.port = 11434;
    defaultServer.baseUrl = QString("http://%1:%2").arg(defaultServer.host).arg(defaultServer.port);
    defaultServer.isActive = true;
    defaultServer.weight = 1;
    defaultServer.maxConcurrent = 5;
    
    m_loadBalancer->addServer(defaultServer);
    
    // 刷新可用模型
    refreshModels();
    
    // 啟動定時器
    m_healthCheckTimer->start();
    m_cleanupTimer->start();
    m_statsTimer->start();
    
    qDebug() << "Ollama LLM管理器初始化完成";
    return true;
}

void OllamaLLMManager::shutdown()
{
    qDebug() << "關閉Ollama LLM管理器...";
    
    // 停止定時器
    m_healthCheckTimer->stop();
    m_cleanupTimer->stop();
    m_statsTimer->stop();
    
    // 取消所有請求
    cancelAllRequests();
    
    // 等待線程池完成
    m_threadPool->waitForDone(5000);
    
    qDebug() << "Ollama LLM管理器已關閉";
}

bool OllamaLLMManager::isAvailable() const
{
    QMutexLocker locker(&m_mutex);
    auto servers = m_loadBalancer->getAllServers();
    return std::any_of(servers.begin(), servers.end(), 
                      [](const OllamaServerConfig &server) { return server.isActive; });
}

void OllamaLLMManager::addServer(const OllamaServerConfig &server)
{
    m_loadBalancer->addServer(server);
    qDebug() << "添加服務器:" << server.name << server.host << server.port;
}

void OllamaLLMManager::removeServer(const QString &serverName)
{
    m_loadBalancer->removeServer(serverName);
    qDebug() << "移除服務器:" << serverName;
}

QList<OllamaServerConfig> OllamaLLMManager::getServers() const
{
    return m_loadBalancer->getAllServers();
}

void OllamaLLMManager::refreshModels()
{
    qDebug() << "刷新可用模型列表...";
    
    auto servers = m_loadBalancer->getAllServers();
    for (const auto &server : servers) {
        if (!server.isActive) continue;
        
        QString url = QString("http://%1:%2/api/tags").arg(server.host).arg(server.port);
        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", "RANOnline-AI-LLM-Client/2.0");
        
        QNetworkReply *reply = m_networkManager->get(request);
        
        connect(reply, &QNetworkReply::finished, [this, reply, server]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray responseData = reply->readAll();
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
                
                if (parseError.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    QJsonArray models = obj["models"].toArray();
                    
                    QMutexLocker locker(&m_mutex);
                    for (const auto &modelValue : models) {
                        QJsonObject modelObj = modelValue.toObject();
                          LLMModelInfo info;
                        info.name = modelObj["name"].toString();
                        info.size = QString::number(static_cast<qint64>(modelObj["size"].toDouble()));
                        info.digest = modelObj["digest"].toString();
                        info.modifiedAt = modelObj["modified_at"].toString();
                        
                        QJsonObject details = modelObj["details"].toObject();
                        info.family = details["family"].toString();
                        info.parameterSize = details["parameter_size"].toString();
                        info.quantization = details["quantization_level"].toString();
                        info.isAvailable = true;
                        
                        m_availableModels[info.name] = info;
                    }
                    
                    updateServerHealth(server.name, true);
                    emit modelsRefreshed(m_availableModels.values());
                } else {
                    qWarning() << "解析模型列表失敗:" << parseError.errorString();
                    updateServerHealth(server.name, false);
                }
            } else {
                qWarning() << "獲取模型列表失敗:" << reply->errorString();
                updateServerHealth(server.name, false);
            }
            
            reply->deleteLater();
        });
    }
}

QList<LLMModelInfo> OllamaLLMManager::getAvailableModels() const
{
    QMutexLocker locker(&m_mutex);
    return m_availableModels.values();
}

LLMModelInfo OllamaLLMManager::getModelInfo(const QString &modelName) const
{
    QMutexLocker locker(&m_mutex);
    return m_availableModels.value(modelName);
}

bool OllamaLLMManager::isModelAvailable(const QString &modelName) const
{
    QMutexLocker locker(&m_mutex);
    return m_availableModels.contains(modelName) && m_availableModels[modelName].isAvailable;
}

QString OllamaLLMManager::submitRequest(const LLMRequestConfig &config)
{
    QString requestId = config.requestId.isEmpty() ? generateRequestId() : config.requestId;
    
    // 檢查模型是否可用
    if (!isModelAvailable(config.modelName)) {
        qWarning() << "模型不可用:" << config.modelName;
        emit requestFailed(requestId, QString("模型 %1 不可用").arg(config.modelName));
        return requestId;
    }
    
    // 選擇最佳服務器
    OllamaServerConfig server = m_loadBalancer->selectOptimalServer();
    if (server.name.isEmpty()) {
        qWarning() << "沒有可用的服務器";
        emit requestFailed(requestId, "沒有可用的服務器");
        return requestId;
    }
    
    // 創建請求配置副本並設置ID
    LLMRequestConfig requestConfig = config;
    requestConfig.requestId = requestId;
    
    // 記錄活動請求
    {
        QMutexLocker locker(&m_mutex);
        m_activeRequests[requestId] = requestConfig;
    }
    
    // 創建請求任務
    auto *task = new LLMRequestTask(requestConfig, server, this);
    
    // 連接信號
    connect(task, &LLMRequestTask::requestCompleted, this, &OllamaLLMManager::onRequestCompleted);
    connect(task, &LLMRequestTask::requestProgress, this, &OllamaLLMManager::onRequestProgress);
    connect(task, &LLMRequestTask::streamUpdate, this, &OllamaLLMManager::onStreamUpdate);
    
    // 提交到線程池
    m_threadPool->start(task);
    
    // 更新服務器負載
    m_loadBalancer->updateServerLoad(server.name, server.currentLoad + 1);
    
    // 更新統計
    m_totalRequests++;
    
    logRequest(requestConfig);
    
    qDebug() << "提交請求:" << requestId << "模型:" << config.modelName << "服務器:" << server.name;
    
    return requestId;
}

void OllamaLLMManager::submitBatchRequests(const QList<LLMRequestConfig> &configs)
{
    qDebug() << "提交批量請求，數量:" << configs.size();
    
    for (const auto &config : configs) {
        submitRequest(config);
    }
}

void OllamaLLMManager::cancelRequest(const QString &requestId)
{
    QMutexLocker locker(&m_mutex);
    if (m_activeRequests.contains(requestId)) {
        m_activeRequests.remove(requestId);
        qDebug() << "取消請求:" << requestId;
    }
}

void OllamaLLMManager::cancelAllRequests()
{
    QMutexLocker locker(&m_mutex);
    int count = m_activeRequests.size();
    m_activeRequests.clear();
    qDebug() << "取消所有請求，數量:" << count;
}

QString OllamaLLMManager::generateRequestId() const
{
    return QCryptographicHash::hash(
        QString("%1_%2").arg(QDateTime::currentMSecsSinceEpoch())
                       .arg(QRandomGenerator::global()->generate()).toUtf8(),
        QCryptographicHash::Md5).toHex().left(16);
}

void OllamaLLMManager::onRequestCompleted(const LLMResponse &response)
{
    {
        QMutexLocker locker(&m_mutex);
        
        // 移除活動請求
        m_activeRequests.remove(response.requestId);
        
        // 記錄完成的響應
        m_completedResponses[response.requestId] = response;
        
        // 更新統計
        if (response.isError) {
            m_failedRequests++;
            
            // 檢查是否需要重試
            if (m_autoRetryEnabled && m_activeRequests.contains(response.requestId)) {
                LLMRequestConfig config = m_activeRequests[response.requestId];
                if (config.retryCount > 0) {
                    config.retryCount--;
                    qDebug() << "重試請求:" << response.requestId << "剩餘重試次數:" << config.retryCount;
                    
                    QTimer::singleShot(m_retryDelay, [this, config]() {
                        submitRequest(config);
                    });
                    return;
                }
            }
        } else {
            m_successfulRequests++;
            m_responseTimes.append(response.responseTime);
            
            // 更新模型使用統計
            m_modelUsage[response.modelName]++;
            
            // 限制響應時間記錄數量
            if (m_responseTimes.size() > 1000) {
                m_responseTimes.removeFirst();
            }
        }
    }
    
    logResponse(response);
    emit requestCompleted(response);
    
    if (response.isError) {
        emit requestFailed(response.requestId, response.errorMessage);
    }
}

void OllamaLLMManager::onRequestProgress(const QString &requestId, int progress)
{
    emit requestProgress(requestId, progress);
}

void OllamaLLMManager::onStreamUpdate(const QString &requestId, const QString &content)
{
    emit streamUpdate(requestId, content);
}

void OllamaLLMManager::checkServerHealth()
{
    auto servers = m_loadBalancer->getAllServers();
    for (const auto &server : servers) {
        QString url = QString("http://%1:%2/api/tags").arg(server.host).arg(server.port);
        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", "RANOnline-AI-LLM-Client/2.0");
        
        QNetworkReply *reply = m_networkManager->get(request);
        
        // 設置較短的超時時間用於健康檢查
        QTimer *timeoutTimer = new QTimer();
        timeoutTimer->setSingleShot(true);
        timeoutTimer->start(5000); // 5秒超時
        
        connect(timeoutTimer, &QTimer::timeout, [reply]() {
            reply->abort();
        });
        
        connect(reply, &QNetworkReply::finished, [this, reply, server, timeoutTimer]() {
            bool isHealthy = (reply->error() == QNetworkReply::NoError);
            updateServerHealth(server.name, isHealthy);
            
            timeoutTimer->deleteLater();
            reply->deleteLater();
        });
    }
}

void OllamaLLMManager::cleanupCompletedRequests()
{
    QMutexLocker locker(&m_mutex);
      // 清理超過1小時的完成響應
    qint64 cutoffTime = QDateTime::currentDateTime().addSecs(-3600).toMSecsSinceEpoch();
    auto it = m_completedResponses.begin();
    while (it != m_completedResponses.end()) {
        if (it.value().timestamp < cutoffTime) {
            it = m_completedResponses.erase(it);
        } else {
            ++it;
        }
    }
}

void OllamaLLMManager::updateStatistics()
{
    emit statisticsUpdated();
}

void OllamaLLMManager::updateServerHealth(const QString &serverName, bool isHealthy)
{
    auto servers = m_loadBalancer->getAllServers();
    for (auto &server : servers) {
        if (server.name == serverName) {
            bool wasHealthy = server.isActive;
            server.isActive = isHealthy;
            
            if (wasHealthy != isHealthy) {
                emit serverStatusChanged(serverName, isHealthy);
                qDebug() << "服務器狀態變更:" << serverName << (isHealthy ? "在線" : "離線");
            }
            break;
        }
    }
}

void OllamaLLMManager::logRequest(const LLMRequestConfig &config)
{
    qDebug() << QString("[REQUEST] ID:%1 Model:%2 Academy:%3 Prompt:%4")
                .arg(config.requestId)
                .arg(config.modelName)
                .arg(config.academy)
                .arg(config.prompt.left(100));
}

void OllamaLLMManager::logResponse(const LLMResponse &response)
{
    QString status = response.isError ? "ERROR" : "SUCCESS";
    qDebug() << QString("[RESPONSE] ID:%1 Status:%2 Time:%3ms Content:%4")
                .arg(response.requestId)
                .arg(status)
                .arg(response.responseTime)
                .arg(response.content.left(100));
}

int OllamaLLMManager::getActiveRequestCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_activeRequests.size();
}

int OllamaLLMManager::getTotalRequestCount() const
{
    return m_totalRequests.load();
}

double OllamaLLMManager::getAverageResponseTime() const
{
    QMutexLocker locker(&m_statsMutex);
    if (m_responseTimes.isEmpty()) return 0.0;
    
    qint64 total = 0;
    for (qint64 time : m_responseTimes) {
        total += time;
    }
    return static_cast<double>(total) / m_responseTimes.size();
}

QMap<QString, int> OllamaLLMManager::getModelUsageStats() const
{
    QMutexLocker locker(&m_statsMutex);
    return m_modelUsage;
}

void OllamaLLMManager::setRetryPolicy(int maxRetries, int retryDelay)
{
    m_maxRetries = maxRetries;
    m_retryDelay = retryDelay;
}

void OllamaLLMManager::enableAutoRetry(bool enabled)
{
    m_autoRetryEnabled = enabled;
}

void OllamaLLMManager::loadConfiguration(const QString &configFile)
{
    QString fileName = configFile.isEmpty() ? 
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/llm_config.ini" : 
        configFile;
    
    QSettings settings(fileName, QSettings::IniFormat);
    
    // 加載基本配置
    m_defaultModel = settings.value("general/default_model", "llama3:latest").toString();
    m_maxRetries = settings.value("general/max_retries", 3).toInt();
    m_retryDelay = settings.value("general/retry_delay", 1000).toInt();
    m_autoRetryEnabled = settings.value("general/auto_retry", true).toBool();
    m_maxConcurrentRequests = settings.value("general/max_concurrent", 10).toInt();
    
    // 加載服務器配置
    int serverCount = settings.beginReadArray("servers");
    for (int i = 0; i < serverCount; ++i) {
        settings.setArrayIndex(i);
        
        OllamaServerConfig server;
        server.name = settings.value("name").toString();
        server.host = settings.value("host").toString();
        server.port = settings.value("port").toInt();
        server.baseUrl = QString("http://%1:%2").arg(server.host).arg(server.port);
        server.isActive = settings.value("active", true).toBool();
        server.weight = settings.value("weight", 1).toInt();
        server.maxConcurrent = settings.value("max_concurrent", 5).toInt();
        
        if (!server.name.isEmpty() && !server.host.isEmpty()) {
            addServer(server);
        }
    }
    settings.endArray();
    
    // 更新線程池設置
    m_threadPool->setMaxThreadCount(m_maxConcurrentRequests);
    
    qDebug() << "配置已加載:" << fileName;
}

void OllamaLLMManager::saveConfiguration(const QString &configFile)
{
    QString fileName = configFile.isEmpty() ? 
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/llm_config.ini" : 
        configFile;
    
    // 確保目錄存在
    QDir dir = QFileInfo(fileName).absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QSettings settings(fileName, QSettings::IniFormat);
    
    // 保存基本配置
    settings.setValue("general/default_model", m_defaultModel);
    settings.setValue("general/max_retries", m_maxRetries);
    settings.setValue("general/retry_delay", m_retryDelay);
    settings.setValue("general/auto_retry", m_autoRetryEnabled);
    settings.setValue("general/max_concurrent", m_maxConcurrentRequests);
    
    // 保存服務器配置
    auto servers = getServers();
    settings.beginWriteArray("servers");
    for (int i = 0; i < servers.size(); ++i) {
        settings.setArrayIndex(i);
        const auto &server = servers[i];
        
        settings.setValue("name", server.name);
        settings.setValue("host", server.host);
        settings.setValue("port", server.port);
        settings.setValue("active", server.isActive);
        settings.setValue("weight", server.weight);
        settings.setValue("max_concurrent", server.maxConcurrent);
    }
    settings.endArray();
    
    qDebug() << "配置已保存:" << fileName;
}

void OllamaLLMManager::setDefaultModel(const QString &modelName)
{
    m_defaultModel = modelName;
}

QString OllamaLLMManager::getDefaultModel() const
{
    return m_defaultModel;
}

// ====================================================================
// PromptTemplateManager Implementation
// ====================================================================

PromptTemplateManager::PromptTemplateManager(QObject *parent)
    : QObject(parent)
{
    m_templateDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
}

void PromptTemplateManager::loadTemplates(const QString &templateDir)
{
    QString dir = templateDir.isEmpty() ? m_templateDirectory : templateDir;
    QDir templateDirectory(dir);
    
    if (!templateDirectory.exists()) {
        templateDirectory.mkpath(".");
        
        // 創建默認模板
        PromptTemplate gameTemplate;
        gameTemplate.id = "game_character";
        gameTemplate.name = "遊戲角色對話";
        gameTemplate.category = "遊戲";
        gameTemplate.templateText = "你是一個來自{{academy}}學院的{{character_type}}。你的性格是{{personality}}。請以這個角色的身份回應玩家的話：{{user_message}}";
        gameTemplate.variables["academy"] = "戰士";
        gameTemplate.variables["character_type"] = "學生";
        gameTemplate.variables["personality"] = "友善但略顯緊張";
        gameTemplate.variables["user_message"] = "";
        gameTemplate.description = "用於生成遊戲角色的對話回應";
        gameTemplate.tags << "遊戲" << "角色" << "對話";
        addTemplate(gameTemplate);
        
        PromptTemplate skillTemplate;
        skillTemplate.id = "skill_description";
        skillTemplate.name = "技能描述生成";
        skillTemplate.category = "遊戲";
        skillTemplate.templateText = "生成一個{{skill_type}}技能的描述：\n技能名稱：{{skill_name}}\n效果等級：{{level}}\n學院：{{academy}}\n請包含技能效果、冷卻時間和消耗。";
        skillTemplate.variables["skill_type"] = "攻擊";
        skillTemplate.variables["skill_name"] = "";
        skillTemplate.variables["level"] = "1";
        skillTemplate.variables["academy"] = "戰士";
        skillTemplate.description = "自動生成遊戲技能描述";
        skillTemplate.tags << "遊戲" << "技能" << "描述";
        addTemplate(skillTemplate);
        
        saveTemplates();
    }
    
    // 加載現有模板
    QStringList templateFiles = templateDirectory.entryList(QStringList("*.json"), QDir::Files);
    for (const QString &fileName : templateFiles) {
        QFile file(templateDirectory.filePath(fileName));
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
            
            if (error.error == QJsonParseError::NoError) {
                QJsonObject obj = doc.object();
                
                PromptTemplate tmpl;
                tmpl.id = obj["id"].toString();
                tmpl.name = obj["name"].toString();
                tmpl.category = obj["category"].toString();
                tmpl.templateText = obj["template"].toString();
                tmpl.description = obj["description"].toString();
                tmpl.isActive = obj["active"].toBool();
                
                QJsonObject vars = obj["variables"].toObject();
                for (auto it = vars.begin(); it != vars.end(); ++it) {
                    tmpl.variables[it.key()] = it.value().toString();
                }
                
                QJsonArray tags = obj["tags"].toArray();
                for (const auto &tag : tags) {
                    tmpl.tags.append(tag.toString());
                }
                
                m_templates[tmpl.id] = tmpl;
            }
        }
    }
    
    qDebug() << "已加載" << m_templates.size() << "個提示詞模板";
}

void PromptTemplateManager::saveTemplates(const QString &templateDir)
{
    QString dir = templateDir.isEmpty() ? m_templateDirectory : templateDir;
    QDir templateDirectory(dir);
    
    if (!templateDirectory.exists()) {
        templateDirectory.mkpath(".");
    }
    
    for (const auto &tmpl : m_templates) {
        QJsonObject obj;
        obj["id"] = tmpl.id;
        obj["name"] = tmpl.name;
        obj["category"] = tmpl.category;
        obj["template"] = tmpl.templateText;
        obj["description"] = tmpl.description;
        obj["active"] = tmpl.isActive;
        
        QJsonObject vars;
        for (auto it = tmpl.variables.begin(); it != tmpl.variables.end(); ++it) {
            vars[it.key()] = it.value();
        }
        obj["variables"] = vars;
        
        QJsonArray tags;
        for (const QString &tag : tmpl.tags) {
            tags.append(tag);
        }
        obj["tags"] = tags;
        
        QJsonDocument doc(obj);
        
        QFile file(templateDirectory.filePath(tmpl.id + ".json"));
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
        }
    }
}

void PromptTemplateManager::addTemplate(const PromptTemplate &tmpl)
{
    m_templates[tmpl.id] = tmpl;
}

void PromptTemplateManager::removeTemplate(const QString &templateId)
{
    m_templates.remove(templateId);
    
    // 刪除文件
    QString fileName = m_templateDirectory + "/" + templateId + ".json";
    QFile::remove(fileName);
}

PromptTemplateManager::PromptTemplate PromptTemplateManager::getTemplate(const QString &templateId) const
{
    return m_templates.value(templateId);
}

QList<PromptTemplateManager::PromptTemplate> PromptTemplateManager::getTemplatesByCategory(const QString &category) const
{
    QList<PromptTemplate> result;
    for (const auto &tmpl : m_templates) {
        if (tmpl.category == category && tmpl.isActive) {
            result.append(tmpl);
        }
    }
    return result;
}

QList<PromptTemplateManager::PromptTemplate> PromptTemplateManager::getAllTemplates() const
{
    return m_templates.values();
}

QString PromptTemplateManager::renderTemplate(const QString &templateId, const QMap<QString, QString> &variables) const
{
    if (!m_templates.contains(templateId)) {
        return QString();
    }
    
    QString result = m_templates[templateId].templateText;
    
    // 替換變量
    for (auto it = variables.begin(); it != variables.end(); ++it) {
        QString placeholder = QString("{{%1}}").arg(it.key());
        result.replace(placeholder, it.value());
    }
    
    return result;
}

QStringList PromptTemplateManager::getCategories() const
{
    QSet<QString> categories;
    for (const auto &tmpl : m_templates) {
        if (tmpl.isActive) {
            categories.insert(tmpl.category);
        }
    }
    return QStringList(categories.begin(), categories.end());
}
