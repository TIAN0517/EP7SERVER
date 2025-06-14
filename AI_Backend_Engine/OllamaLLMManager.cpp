#include "OllamaLLMManager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QDebug>
#include <algorithm>

Q_LOGGING_CATEGORY(llmManager, "ranonline.llm.manager")

namespace RANOnline {
namespace AI {

// ====================================================================
// OllamaLLMManager Implementation
// ====================================================================

OllamaLLMManager::OllamaLLMManager(QObject *parent)
    : QObject(parent)
    , m_ollamaUrl("http://localhost:11434")
    , m_networkManager(std::make_unique<QNetworkAccessManager>(this))
    , m_modelSelectionStrategy("balanced")
    , m_autoRetryEnabled(true)
    , m_maxConcurrentRequests(3)
    , m_currentConcurrentRequests(0)
    , m_settings(new QSettings(this))
    , m_queueTimer(new QTimer(this))
    , m_healthCheckTimer(new QTimer(this))
    , m_totalRequests(0)
    , m_successfulRequests(0)
    , m_failedRequests(0)
{
    setupNetworking();
    setupTimers();
    setupLogging();
    
    // 註冊自定義類型用於信號槽
    qRegisterMetaType<ModelInfo>("ModelInfo");
    qRegisterMetaType<LLMRequestConfig>("LLMRequestConfig");
    qRegisterMetaType<LLMResponse>("LLMResponse");
    qRegisterMetaType<ScenarioType>("ScenarioType");
    
    qCInfo(llmManager) << "Ollama LLM Manager created";
}

OllamaLLMManager::~OllamaLLMManager()
{
    // 取消所有活動請求
    for (auto reply : m_activeRequests) {
        if (reply && reply->isRunning()) {
            reply->abort();
        }
    }
    
    // 清理定時器
    for (auto timer : m_requestTimers) {
        timer->stop();
        timer->deleteLater();
    }
    
    qCInfo(llmManager) << "Ollama LLM Manager destroyed";
}

bool OllamaLLMManager::initialize(const QString &ollamaUrl)
{
    QMutexLocker locker(&m_mutex);
    
    m_ollamaUrl = ollamaUrl;
    qCInfo(llmManager) << "Initializing with Ollama URL:" << m_ollamaUrl;
    
    // 測試服務連接
    QNetworkRequest request(QUrl(m_ollamaUrl + "/api/version"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "RANOnline-LLM-Manager/2.0");
    
    auto reply = m_networkManager->get(request);
    
    // 同步等待響應（僅用於初始化）
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeoutTimer.start(5000); // 5秒超時
    
    loop.exec();
    
    bool success = false;
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) {
            m_isServiceAvailable = true;
            success = true;
            qCInfo(llmManager) << "Ollama service is available, version:" 
                             << doc.object().value("version").toString();
        }
    } else {
        qCWarning(llmManager) << "Failed to connect to Ollama service:" << reply->errorString();
    }
    
    reply->deleteLater();
    
    if (success) {
        // 刷新模型列表
        refreshModelList();
        
        // 載入配置
        loadConfiguration("ollama_config.json");
        
        // 啟動定時器
        m_queueTimer->start(100);  // 100ms處理隊列
        m_healthCheckTimer->start(30000); // 30秒健康檢查
        
        m_isInitialized = true;
        qCInfo(llmManager) << "Ollama LLM Manager initialized successfully";
    }
    
    return success;
}

bool OllamaLLMManager::isServiceAvailable() const
{
    return m_isServiceAvailable.load();
}

void OllamaLLMManager::loadConfiguration(const QString &configPath)
{
    QString fullPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + configPath;
    QFile file(fullPath);
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject()) {
            QJsonObject config = doc.object();
            
            // 載入全域選項
            if (config.contains("globalOptions")) {
                m_globalOptions = config["globalOptions"].toObject();
            }
            
            // 載入模型選擇策略
            if (config.contains("modelSelectionStrategy")) {
                m_modelSelectionStrategy = config["modelSelectionStrategy"].toString();
            }
            
            // 載入其他設置
            if (config.contains("autoRetryEnabled")) {
                m_autoRetryEnabled = config["autoRetryEnabled"].toBool();
            }
            
            if (config.contains("maxConcurrentRequests")) {
                m_maxConcurrentRequests = config["maxConcurrentRequests"].toInt();
            }
            
            qCInfo(llmManager) << "Configuration loaded from:" << fullPath;
        }
    } else {
        qCInfo(llmManager) << "No configuration file found, using defaults";
    }
}

void OllamaLLMManager::saveConfiguration(const QString &configPath)
{
    QString fullPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + configPath;
    QDir().mkpath(QFileInfo(fullPath).absolutePath());
    
    QJsonObject config;
    config["globalOptions"] = m_globalOptions;
    config["modelSelectionStrategy"] = m_modelSelectionStrategy;
    config["autoRetryEnabled"] = m_autoRetryEnabled;
    config["maxConcurrentRequests"] = m_maxConcurrentRequests;
    
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(config);
        file.write(doc.toJson());
        qCInfo(llmManager) << "Configuration saved to:" << fullPath;
    } else {
        qCWarning(llmManager) << "Failed to save configuration to:" << fullPath;
    }
}

bool OllamaLLMManager::refreshModelList()
{
    if (!m_isServiceAvailable) {
        return false;
    }
    
    qCInfo(llmManager) << "Refreshing model list...";
    
    QNetworkRequest request(QUrl(m_ollamaUrl + "/api/tags"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    auto reply = m_networkManager->get(request);
    
    // 同步等待響應
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeoutTimer.start(10000); // 10秒超時
    
    loop.exec();
    
    bool success = false;
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) {
            QJsonArray models = doc.object()["models"].toArray();
            
            QMutexLocker locker(&m_mutex);
            m_availableModels.clear();
            
            for (const QJsonValue &modelValue : models) {
                QJsonObject modelObj = modelValue.toObject();
                ModelInfo info;
                
                info.name = modelObj["name"].toString();
                info.size = modelObj["size"].toVariant().toLongLong();
                info.digest = modelObj["digest"].toString();
                info.modifiedAt = QDateTime::fromString(
                    modelObj["modified_at"].toString(), Qt::ISODate);
                
                QJsonObject details = modelObj["details"].toObject();
                info.family = details["family"].toString();
                info.parameterSize = details["parameter_size"].toString();
                info.quantizationLevel = details["quantization_level"].toString();
                info.isAvailable = true;
                
                m_availableModels[info.name] = info;
                
                qCInfo(llmManager) << "Found model:" << info.name 
                                 << "Family:" << info.family 
                                 << "Size:" << QString::number(info.size / 1024.0 / 1024.0, 'f', 1) << "MB";
            }
            
            success = true;
            qCInfo(llmManager) << "Model list refreshed, found" << m_availableModels.size() << "models";
        }
    } else {
        qCWarning(llmManager) << "Failed to refresh model list:" << reply->errorString();
    }
    
    reply->deleteLater();
    return success;
}

QList<ModelInfo> OllamaLLMManager::getAvailableModels() const
{
    QMutexLocker locker(&m_mutex);
    return m_availableModels.values();
}

QString OllamaLLMManager::selectBestModel(ScenarioType scenario) const
{
    QMutexLocker locker(&m_mutex);
    
    if (m_availableModels.isEmpty()) {
        return QString();
    }
    
    // 基於場景的模型選擇策略
    QStringList preferredModels;
    
    switch (scenario) {
    case ScenarioType::CodeGeneration:
        preferredModels << "deepseek-coder:latest" << "llama3:latest" << "mistral:latest";
        break;
    case ScenarioType::TechnicalSupport:
        preferredModels << "llama3:latest" << "mistral:latest" << "qwen2:latest";
        break;
    case ScenarioType::GameNarrative:
        preferredModels << "gemma:latest" << "llama3:latest" << "mistral:latest";
        break;
    case ScenarioType::DataAnalysis:
        preferredModels << "qwen2:latest" << "llama3:latest" << "mistral:latest";
        break;
    case ScenarioType::Translation:
        preferredModels << "qwen2:latest" << "gemma:latest" << "llama3:latest";
        break;
    case ScenarioType::CreativeWriting:
        preferredModels << "gemma:latest" << "llama3:latest" << "mistral:latest";
        break;
    case ScenarioType::Debugging:
        preferredModels << "deepseek-coder:latest" << "llama3:latest" << "phi3:latest";
        break;
    case ScenarioType::QuestionAnswering:
        preferredModels << "llama3:latest" << "qwen2:latest" << "mistral:latest";
        break;
    case ScenarioType::Summarization:
        preferredModels << "phi3:latest" << "qwen2:latest" << "gemma:latest";
        break;
    case ScenarioType::GeneralChat:
    default:
        preferredModels << "llama3:latest" << "mistral:latest" << "phi3:latest" << "gemma:latest" << "qwen2:latest";
        break;
    }
    
    // 查找可用的首選模型
    for (const QString &modelName : preferredModels) {
        if (m_availableModels.contains(modelName) && 
            m_availableModels[modelName].isAvailable &&
            isModelHealthy(modelName)) {
            return modelName;
        }
    }
    
    // 如果沒有首選模型，使用性能最好的可用模型
    QString bestModel;
    double bestScore = -1.0;
    
    for (auto it = m_availableModels.begin(); it != m_availableModels.end(); ++it) {
        const ModelInfo &info = it.value();
        if (!info.isAvailable || !isModelHealthy(it.key())) {
            continue;
        }
        
        // 計算模型評分（基於成功率和響應時間）
        double successRate = info.successCount > 0 ? 
            (double)info.successCount / (info.successCount + info.errorCount) : 0.5;
        double responseScore = info.avgResponseTime > 0 ? 
            1000.0 / info.avgResponseTime : 1.0; // 響應時間越短越好
        
        double score = successRate * 0.7 + responseScore * 0.3;
        
        if (score > bestScore) {
            bestScore = score;
            bestModel = it.key();
        }
    }
    
    if (bestModel.isEmpty() && !m_availableModels.isEmpty()) {
        // 最後備選：返回第一個可用模型
        bestModel = m_availableModels.firstKey();
    }
    
    qCInfo(llmManager) << "Selected model for scenario" << (int)scenario << ":" << bestModel;
    return bestModel;
}

ModelInfo OllamaLLMManager::getModelInfo(const QString &modelName) const
{
    QMutexLocker locker(&m_mutex);
    return m_availableModels.value(modelName);
}

LLMResponse OllamaLLMManager::generateSync(const LLMRequestConfig &config)
{
    if (!m_isInitialized || !m_isServiceAvailable) {
        LLMResponse response;
        response.success = false;
        response.errorMessage = "LLM service not available";
        return response;
    }
    
    QElapsedTimer timer;
    timer.start();
    
    LLMResponse response;
    response.timestamp = QDateTime::currentDateTime();
    
    // 選擇模型
    QString modelName = config.model.isEmpty() ? selectBestModel(config.scenario) : config.model;
    if (modelName.isEmpty()) {
        response.errorMessage = "No suitable model available";
        return response;
    }
    
    response.model = modelName;
    
    // 構建請求
    QJsonObject payload = buildRequestPayload(config);
    payload["model"] = modelName;
    
    QNetworkRequest request(QUrl(m_ollamaUrl + "/api/generate"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "RANOnline-LLM-Manager/2.0");
    
    QJsonDocument doc(payload);
    auto reply = m_networkManager->post(request, doc.toJson());
    
    // 同步等待響應
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeoutTimer.start(config.timeoutMs);
    
    loop.exec();
    
    response = parseResponse(reply, config);
    response.responseTimeMs = timer.elapsed();
    
    // 更新統計
    updateModelStatistics(modelName, response.success, response.responseTimeMs);
    logRequest(config, response);
    
    reply->deleteLater();
    
    // 統計計數
    m_totalRequests++;
    if (response.success) {
        m_successfulRequests++;
    } else {
        m_failedRequests++;
    }
    
    return response;
}

void OllamaLLMManager::generateAsync(const LLMRequestConfig &config, const QString &requestId)
{
    QString id = requestId.isEmpty() ? generateRequestId() : requestId;
    
    // 添加到請求隊列
    BatchRequestItem item;
    item.id = id;
    item.config = config;
    item.submitTime = QDateTime::currentDateTime();
    item.priority = 0; // 默認優先級
    
    QMutexLocker locker(&m_queueMutex);
    m_requestQueue.enqueue(item);
    
    qCInfo(llmManager) << "Async request queued:" << id;
}

void OllamaLLMManager::generateStream(const LLMRequestConfig &config, const QString &requestId)
{
    QString id = requestId.isEmpty() ? generateRequestId() : requestId;
    
    // 流式請求配置
    LLMRequestConfig streamConfig = config;
    streamConfig.stream = true;
    
    generateAsync(streamConfig, id);
}

QString OllamaLLMManager::addBatchRequest(const QList<BatchRequestItem> &items)
{
    QString batchId = generateRequestId();
    
    QMutexLocker locker(&m_queueMutex);
    for (const BatchRequestItem &item : items) {
        BatchRequestItem batchItem = item;
        batchItem.config.metadata["batchId"] = batchId;
        m_requestQueue.enqueue(batchItem);
    }
    
    qCInfo(llmManager) << "Batch request added:" << batchId << "Items:" << items.size();
    return batchId;
}

bool OllamaLLMManager::cancelRequest(const QString &requestId)
{
    // 取消網絡請求
    if (m_activeRequests.contains(requestId)) {
        auto reply = m_activeRequests[requestId];
        if (reply && reply->isRunning()) {
            reply->abort();
            m_activeRequests.remove(requestId);
            qCInfo(llmManager) << "Request cancelled:" << requestId;
            return true;
        }
    }
    
    // 從隊列中移除
    QMutexLocker locker(&m_queueMutex);
    QQueue<BatchRequestItem> newQueue;
    bool found = false;
    
    while (!m_requestQueue.isEmpty()) {
        BatchRequestItem item = m_requestQueue.dequeue();
        if (item.id != requestId) {
            newQueue.enqueue(item);
        } else {
            found = true;
        }
    }
    
    m_requestQueue = newQueue;
    
    if (found) {
        qCInfo(llmManager) << "Request removed from queue:" << requestId;
    }
    
    return found;
}

QJsonObject OllamaLLMManager::getQueueStatus() const
{
    QMutexLocker locker(&m_queueMutex);
    
    QJsonObject status;
    status["queueSize"] = m_requestQueue.size();
    status["activeRequests"] = m_activeRequests.size();
    status["maxConcurrent"] = m_maxConcurrentRequests;
    status["totalRequests"] = (qint64)m_totalRequests;
    status["successfulRequests"] = (qint64)m_successfulRequests;
    status["failedRequests"] = (qint64)m_failedRequests;
    
    // 模型統計
    QJsonObject modelStats;
    for (auto it = m_availableModels.begin(); it != m_availableModels.end(); ++it) {
        const ModelInfo &info = it.value();
        QJsonObject stat;
        stat["successCount"] = info.successCount;
        stat["errorCount"] = info.errorCount;
        stat["avgResponseTime"] = info.avgResponseTime;
        stat["isAvailable"] = info.isAvailable;
        modelStats[it.key()] = stat;
    }
    status["modelStatistics"] = modelStats;
    
    return status;
}

QString OllamaLLMManager::quickChat(const QString &message, 
                                   const QString &systemPrompt,
                                   const QString &modelHint)
{
    LLMRequestConfig config;
    config.prompt = message;
    config.systemPrompt = systemPrompt.isEmpty() ? 
        "You are a helpful AI assistant in the RANOnline game system. Please provide helpful and accurate responses." : 
        systemPrompt;
    config.model = modelHint;
    config.scenario = ScenarioType::GeneralChat;
    config.stream = false;
    
    LLMResponse response = generateSync(config);
    return response.success ? response.content : QString("Error: %1").arg(response.errorMessage);
}

QString OllamaLLMManager::generateCode(const QString &requirement, const QString &language)
{
    LLMRequestConfig config;
    config.prompt = QString("Generate %1 code for the following requirement:\n%2")
                   .arg(language, requirement);
    config.systemPrompt = QString(
        "You are an expert %1 programmer. Generate clean, well-commented, and efficient code. "
        "Include necessary headers and follow best practices for %1 development."
    ).arg(language);
    config.scenario = ScenarioType::CodeGeneration;
    config.stream = false;
    
    LLMResponse response = generateSync(config);
    return response.success ? response.content : QString("Error: %1").arg(response.errorMessage);
}

QString OllamaLLMManager::answerTechnicalQuestion(const QString &question, const QString &context)
{
    LLMRequestConfig config;
    config.prompt = context.isEmpty() ? question : 
                   QString("Context: %1\n\nQuestion: %2").arg(context, question);
    config.systemPrompt = 
        "You are a technical expert assistant. Provide detailed, accurate, and practical answers "
        "to technical questions. Include examples and explanations when appropriate.";
    config.scenario = ScenarioType::TechnicalSupport;
    config.stream = false;
    
    LLMResponse response = generateSync(config);
    return response.success ? response.content : QString("Error: %1").arg(response.errorMessage);
}

void OllamaLLMManager::setGlobalOptions(const QJsonObject &options)
{
    QMutexLocker locker(&m_mutex);
    m_globalOptions = options;
    qCInfo(llmManager) << "Global options updated";
}

void OllamaLLMManager::setAutoRetryEnabled(bool enabled)
{
    m_autoRetryEnabled = enabled;
    qCInfo(llmManager) << "Auto retry" << (enabled ? "enabled" : "disabled");
}

void OllamaLLMManager::setMaxConcurrentRequests(int maxConcurrent)
{
    m_maxConcurrentRequests = qMax(1, maxConcurrent);
    qCInfo(llmManager) << "Max concurrent requests set to:" << m_maxConcurrentRequests;
}

void OllamaLLMManager::setModelSelectionStrategy(const QString &strategy)
{
    QMutexLocker locker(&m_mutex);
    m_modelSelectionStrategy = strategy;
    qCInfo(llmManager) << "Model selection strategy set to:" << strategy;
}

// ============================================================
// Private Slots
// ============================================================

void OllamaLLMManager::onNetworkReplyFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    // 查找對應的請求ID
    QString requestId;
    for (auto it = m_activeRequests.begin(); it != m_activeRequests.end(); ++it) {
        if (it.value() == reply) {
            requestId = it.key();
            break;
        }
    }
    
    if (!requestId.isEmpty()) {
        // 獲取請求配置
        LLMRequestConfig config = m_pendingRequests.value(requestId);
        
        // 解析響應
        LLMResponse response = parseResponse(reply, config);
        
        // 更新統計
        updateModelStatistics(response.model, response.success, response.responseTimeMs);
        logRequest(config, response);
        
        // 清理
        m_activeRequests.remove(requestId);
        m_pendingRequests.remove(requestId);
        m_currentConcurrentRequests--;
        
        // 停止請求定時器
        if (m_requestTimers.contains(requestId)) {
            m_requestTimers[requestId]->stop();
            m_requestTimers[requestId]->deleteLater();
            m_requestTimers.remove(requestId);
        }
        
        // 發送信號
        if (response.success) {
            emit requestCompleted(requestId, response);
            m_successfulRequests++;
        } else {
            emit requestError(requestId, response.errorMessage, 0);
            m_failedRequests++;
            
            // 自動重試邏輯
            if (m_autoRetryEnabled && config.maxRetries > 0) {
                int retryCount = m_retryCounters.value(requestId, 0);
                if (retryCount < config.maxRetries) {
                    retryRequest(requestId, config, retryCount + 1);
                }
            }
        }
        
        m_totalRequests++;
    }
    
    reply->deleteLater();
}

void OllamaLLMManager::onStreamDataReady()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    // 查找對應的請求ID
    QString requestId;
    for (auto it = m_activeRequests.begin(); it != m_activeRequests.end(); ++it) {
        if (it.value() == reply) {
            requestId = it.key();
            break;
        }
    }
    
    if (!requestId.isEmpty()) {
        QByteArray data = reply->readAll();
        
        // 解析流式數據
        QStringList lines = QString::fromUtf8(data).split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            if (line.trimmed().isEmpty()) continue;
            
            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                QString chunk = obj.value("response").toString();
                bool done = obj.value("done").toBool();
                
                if (!chunk.isEmpty()) {
                    emit streamDataReceived(requestId, chunk, done);
                }
                
                if (done) {
                    // 流式響應完成
                    LLMResponse response;
                    response.success = true;
                    response.model = obj.value("model").toString();
                    response.timestamp = QDateTime::currentDateTime();
                    response.rawResponse = obj;
                    
                    emit requestCompleted(requestId, response);
                    
                    // 清理
                    m_activeRequests.remove(requestId);
                    m_pendingRequests.remove(requestId);
                    m_currentConcurrentRequests--;
                    break;
                }
            }
        }
    }
}

void OllamaLLMManager::onRequestTimeout()
{
    auto timer = qobject_cast<QTimer*>(sender());
    if (!timer) return;
    
    // 查找對應的請求ID
    QString requestId;
    for (auto it = m_requestTimers.begin(); it != m_requestTimers.end(); ++it) {
        if (it.value() == timer) {
            requestId = it.key();
            break;
        }
    }
    
    if (!requestId.isEmpty()) {
        qCWarning(llmManager) << "Request timeout:" << requestId;
        
        // 取消請求
        if (m_activeRequests.contains(requestId)) {
            m_activeRequests[requestId]->abort();
        }
        
        emit requestError(requestId, "Request timeout", 0);
        
        // 清理
        m_activeRequests.remove(requestId);
        m_pendingRequests.remove(requestId);
        m_currentConcurrentRequests--;
        timer->deleteLater();
        m_requestTimers.remove(requestId);
    }
}

void OllamaLLMManager::onProcessQueue()
{
    processRequestQueue();
}

void OllamaLLMManager::onHealthCheck()
{
    // 檢查服務可用性
    QNetworkRequest request(QUrl(m_ollamaUrl + "/api/version"));
    auto reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        bool wasAvailable = m_isServiceAvailable;
        m_isServiceAvailable = (reply->error() == QNetworkReply::NoError);
        
        if (wasAvailable != m_isServiceAvailable) {
            qCInfo(llmManager) << "Service availability changed:" 
                             << (m_isServiceAvailable ? "Available" : "Unavailable");
        }
        
        reply->deleteLater();
    });
    
    // 更新模型健康狀態
    QMutexLocker locker(&m_statsMutex);
    for (auto it = m_availableModels.begin(); it != m_availableModels.end(); ++it) {
        ModelInfo &info = it.value();
        // 基於成功率判斷模型健康狀態
        if (info.successCount + info.errorCount > 0) {
            double successRate = (double)info.successCount / (info.successCount + info.errorCount);
            info.isAvailable = successRate > 0.5; // 成功率低於50%認為不健康
        }
    }
}

// ============================================================
// Private Methods
// ============================================================

void OllamaLLMManager::setupNetworking()
{
    // 設置網絡超時
    m_networkManager->setTransferTimeout(30000); // 30秒
    
    // 連接信號
    connect(m_networkManager.get(), &QNetworkAccessManager::finished,
            this, &OllamaLLMManager::onNetworkReplyFinished);
}

void OllamaLLMManager::setupTimers()
{
    // 設置隊列處理定時器
    m_queueTimer->setSingleShot(false);
    connect(m_queueTimer, &QTimer::timeout, this, &OllamaLLMManager::onProcessQueue);
    
    // 設置健康檢查定時器
    m_healthCheckTimer->setSingleShot(false);
    connect(m_healthCheckTimer, &QTimer::timeout, this, &OllamaLLMManager::onHealthCheck);
}

void OllamaLLMManager::setupLogging()
{
    // 設置日誌輸出目錄
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(logDir);
    
    qCInfo(llmManager) << "Log directory:" << logDir;
}

QString OllamaLLMManager::buildPrompt(const LLMRequestConfig &config)
{
    QString fullPrompt;
    
    if (!config.systemPrompt.isEmpty()) {
        fullPrompt += "System: " + config.systemPrompt + "\n\n";
    }
    
    fullPrompt += "User: " + config.prompt;
    
    return fullPrompt;
}

QJsonObject OllamaLLMManager::buildRequestPayload(const LLMRequestConfig &config)
{
    QJsonObject payload;
    
    payload["prompt"] = buildPrompt(config);
    payload["stream"] = config.stream;
    
    // 合併全域選項和請求特定選項
    QJsonObject options = m_globalOptions;
    for (auto it = config.options.begin(); it != config.options.end(); ++it) {
        options[it.key()] = it.value();
    }
    
    if (!options.isEmpty()) {
        payload["options"] = options;
    }
    
    return payload;
}

LLMResponse OllamaLLMManager::parseResponse(QNetworkReply *reply, const LLMRequestConfig &config)
{
    LLMResponse response;
    response.timestamp = QDateTime::currentDateTime();
    
    if (reply->error() != QNetworkReply::NoError) {
        response.success = false;
        response.errorMessage = reply->errorString();
        return response;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        response.success = false;
        response.errorMessage = "Invalid JSON response";
        return response;
    }
    
    QJsonObject obj = doc.object();
    response.rawResponse = obj;
    
    if (config.stream) {
        // 流式響應處理（在onStreamDataReady中處理）
        response.success = true;
    } else {
        // 非流式響應
        response.content = obj.value("response").toString();
        response.model = obj.value("model").toString();
        response.success = !response.content.isEmpty();
        
        if (!response.success) {
            response.errorMessage = obj.value("error").toString();
            if (response.errorMessage.isEmpty()) {
                response.errorMessage = "Empty response";
            }
        }
    }
    
    return response;
}

void OllamaLLMManager::updateModelStatistics(const QString &model, bool success, qint64 responseTime)
{
    QMutexLocker locker(&m_statsMutex);
    
    if (m_availableModels.contains(model)) {
        ModelInfo &info = m_availableModels[model];
        
        if (success) {
            info.successCount++;
            
            // 更新平均響應時間
            if (info.avgResponseTime == 0.0) {
                info.avgResponseTime = responseTime;
            } else {
                info.avgResponseTime = (info.avgResponseTime + responseTime) / 2.0;
            }
        } else {
            info.errorCount++;
        }
    }
}

void OllamaLLMManager::logRequest(const LLMRequestConfig &config, const LLMResponse &response)
{
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QString logFile = logDir + QString("/llm_requests_%1.log")
                      .arg(QDate::currentDate().toString("yyyy-MM-dd"));
    
    QFile file(logFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        
        stream << "[" << response.timestamp.toString(Qt::ISODate) << "] "
               << "Model: " << response.model << " "
               << "Success: " << (response.success ? "YES" : "NO") << " "
               << "ResponseTime: " << response.responseTimeMs << "ms "
               << "Scenario: " << (int)config.scenario << " "
               << "PromptLength: " << config.prompt.length() << " ";
        
        if (!response.success) {
            stream << "Error: " << response.errorMessage << " ";
        }
        
        stream << "\n";
    }
}

void OllamaLLMManager::retryRequest(const QString &requestId, const LLMRequestConfig &config, int retryCount)
{
    qCInfo(llmManager) << "Retrying request:" << requestId << "Attempt:" << retryCount;
    
    m_retryCounters[requestId] = retryCount;
    
    // 延遲重試（指數退避）
    int delay = qMin(1000 * qPow(2, retryCount - 1), 10000); // 最大10秒
    
    QTimer::singleShot(delay, [this, requestId, config]() {
        // 添加回隊列
        BatchRequestItem item;
        item.id = requestId;
        item.config = config;
        item.submitTime = QDateTime::currentDateTime();
        item.priority = 10; // 重試請求提高優先級
        
        QMutexLocker locker(&m_queueMutex);
        m_requestQueue.enqueue(item);
    });
}

QString OllamaLLMManager::generateRequestId()
{
    static QAtomicInteger<qint64> counter{0};
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    qint64 id = counter.fetchAndAddOrdered(1);
    return QString("req_%1_%2").arg(timestamp).arg(id);
}

void OllamaLLMManager::processRequestQueue()
{
    if (m_currentConcurrentRequests >= m_maxConcurrentRequests) {
        return; // 達到最大並發限制
    }
    
    QMutexLocker locker(&m_queueMutex);
    if (m_requestQueue.isEmpty()) {
        return;
    }
    
    // 按優先級排序（高優先級先處理）
    QList<BatchRequestItem> items;
    while (!m_requestQueue.isEmpty()) {
        items.append(m_requestQueue.dequeue());
    }
    
    std::sort(items.begin(), items.end(), 
              [](const BatchRequestItem &a, const BatchRequestItem &b) {
                  return a.priority > b.priority;
              });
    
    // 處理優先級最高的請求
    if (!items.isEmpty()) {
        BatchRequestItem item = items.takeFirst();
        
        // 將剩餘項目放回隊列
        for (const BatchRequestItem &remainingItem : items) {
            m_requestQueue.enqueue(remainingItem);
        }
        
        locker.unlock();
        
        // 處理請求
        processRequest(item);
    }
}

void OllamaLLMManager::processRequest(const BatchRequestItem &item)
{
    QString requestId = item.id;
    LLMRequestConfig config = item.config;
    
    // 選擇模型
    QString modelName = config.model.isEmpty() ? selectBestModel(config.scenario) : config.model;
    if (modelName.isEmpty()) {
        emit requestError(requestId, "No suitable model available", 0);
        return;
    }
    
    // 檢查模型健康狀態
    if (!isModelHealthy(modelName)) {
        QString backupModel = selectBestModel(config.scenario);
        if (backupModel != modelName && !backupModel.isEmpty()) {
            qCInfo(llmManager) << "Switching from unhealthy model" << modelName << "to" << backupModel;
            modelName = backupModel;
            emit modelSwitched(config.model, modelName, "Model health issue");
        } else {
            emit requestError(requestId, "No healthy model available", 0);
            return;
        }
    }
    
    // 構建請求
    QJsonObject payload = buildRequestPayload(config);
    payload["model"] = modelName;
    
    QNetworkRequest request(QUrl(m_ollamaUrl + "/api/generate"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "RANOnline-LLM-Manager/2.0");
    
    QJsonDocument doc(payload);
    auto reply = m_networkManager->post(request, doc.toJson());
    
    // 存儲請求信息
    m_activeRequests[requestId] = reply;
    m_pendingRequests[requestId] = config;
    m_currentConcurrentRequests++;
    
    // 設置請求超時定時器
    auto timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->timeout.connect([this, requestId]() {
        onRequestTimeout();
    });
    timeoutTimer->start(config.timeoutMs);
    m_requestTimers[requestId] = timeoutTimer;
    
    // 為流式請求連接數據信號
    if (config.stream) {
        connect(reply, &QNetworkReply::readyRead, this, &OllamaLLMManager::onStreamDataReady);
    }
    
    qCInfo(llmManager) << "Processing request:" << requestId << "Model:" << modelName;
}

bool OllamaLLMManager::isModelHealthy(const QString &modelName) const
{
    QMutexLocker locker(&m_statsMutex);
    
    if (!m_availableModels.contains(modelName)) {
        return false;
    }
    
    const ModelInfo &info = m_availableModels[modelName];
    if (!info.isAvailable) {
        return false;
    }
    
    // 如果沒有統計數據，認為是健康的
    if (info.successCount + info.errorCount == 0) {
        return true;
    }
    
    // 成功率大於50%且響應時間合理
    double successRate = (double)info.successCount / (info.successCount + info.errorCount);
    bool responseTimeOk = info.avgResponseTime < 30000; // 30秒內響應
    
    return successRate > 0.5 && responseTimeOk;
}

void OllamaLLMManager::switchToBackupModel(const QString &failedModel, const LLMRequestConfig &config)
{
    QString backupModel = selectBestModel(config.scenario);
    if (backupModel != failedModel && !backupModel.isEmpty()) {
        qCInfo(llmManager) << "Switching from failed model" << failedModel << "to backup" << backupModel;
        emit modelSwitched(failedModel, backupModel, "Model failure");
    }
}

// ====================================================================
// LLMRequestWorker Implementation
// ====================================================================

LLMRequestWorker::LLMRequestWorker(OllamaLLMManager *manager, QObject *parent)
    : QObject(parent), m_manager(manager)
{
}

void LLMRequestWorker::processRequest(const QString &requestId, const LLMRequestConfig &config)
{
    if (!m_manager) {
        emit requestFailed(requestId, "Manager not available");
        return;
    }
    
    LLMResponse response = m_manager->generateSync(config);
    
    if (response.success) {
        emit requestFinished(requestId, response);
    } else {
        emit requestFailed(requestId, response.errorMessage);
    }
}

} // namespace AI
} // namespace RANOnline

#include "OllamaLLMManager.moc"