#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QQueue>
#include <QMap>
#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include <memory>
#include <atomic>
#include <functional>
#include "AcademyTheme.h"

/**
 * @brief LLM請求任務 (用於線程池)
 */
class LLMRequestTask : public QObject, public QRunnable
{
    Q_OBJECT
    
public:
    explicit LLMRequestTask(const LLMRequestConfig &config, 
                           const OllamaServerConfig &server,
                           QObject *parent = nullptr);
    
    void run() override;

signals:
    void requestCompleted(const LLMResponse &response);
    void requestProgress(const QString &requestId, int progress);
    void streamUpdate(const QString &requestId, const QString &content);

private:
    LLMRequestConfig m_config;
    OllamaServerConfig m_server;
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    
    void processStreamResponse(QNetworkReply *reply);
    void handleNetworkError(QNetworkReply::NetworkError error);
};

/**
 * @brief 負載均衡器
 */
class LoadBalancer : public QObject
{
    Q_OBJECT

public:
    explicit LoadBalancer(QObject *parent = nullptr);
    
    void addServer(const OllamaServerConfig &server);
    void removeServer(const QString &serverName);
    void updateServerLoad(const QString &serverName, int load);
    OllamaServerConfig selectOptimalServer() const;
    QList<OllamaServerConfig> getAllServers() const;
    
    void setBalanceStrategy(const QString &strategy); // "round_robin", "least_connections", "weighted"

private:
    QMap<QString, OllamaServerConfig> m_servers;
    QString m_balanceStrategy = "least_connections";
    mutable int m_roundRobinIndex = 0;
    mutable QMutex m_mutex;
};

/**
 * @brief Ollama LLM管理器主類
 */
class OllamaLLMManager : public QObject
{
    Q_OBJECT

public:
    explicit OllamaLLMManager(QObject *parent = nullptr);
    ~OllamaLLMManager();

    // 基本功能
    bool initialize();
    void shutdown();
    bool isAvailable() const;
    
    // 服務器管理
    void addServer(const OllamaServerConfig &server);
    void removeServer(const QString &serverName);
    QList<OllamaServerConfig> getServers() const;
    
    // 模型管理
    void refreshModels();
    QList<LLMModelInfo> getAvailableModels() const;
    LLMModelInfo getModelInfo(const QString &modelName) const;
    bool isModelAvailable(const QString &modelName) const;
    
    // 請求處理
    QString submitRequest(const LLMRequestConfig &config);
    void submitBatchRequests(const QList<LLMRequestConfig> &configs);
    void cancelRequest(const QString &requestId);
    void cancelAllRequests();
    
    // 配置管理
    void loadConfiguration(const QString &configFile = "");
    void saveConfiguration(const QString &configFile = "");
    void setDefaultModel(const QString &modelName);
    QString getDefaultModel() const;
    
    // 統計信息
    int getActiveRequestCount() const;
    int getTotalRequestCount() const;
    double getAverageResponseTime() const;
    QMap<QString, int> getModelUsageStats() const;
    
    // 自動重試機制
    void setRetryPolicy(int maxRetries, int retryDelay);
    void enableAutoRetry(bool enabled);

signals:
    void requestCompleted(const LLMResponse &response);
    void requestFailed(const QString &requestId, const QString &error);
    void streamUpdate(const QString &requestId, const QString &content);
    void requestProgress(const QString &requestId, int progress);
    void modelsRefreshed(const QList<LLMModelInfo> &models);
    void serverStatusChanged(const QString &serverName, bool isOnline);
    void statisticsUpdated();

public slots:
    void onRequestCompleted(const LLMResponse &response);
    void onRequestProgress(const QString &requestId, int progress);
    void onStreamUpdate(const QString &requestId, const QString &content);

private slots:
    void checkServerHealth();
    void cleanupCompletedRequests();
    void updateStatistics();

private:
    // 核心組件
    std::unique_ptr<LoadBalancer> m_loadBalancer;
    std::unique_ptr<QThreadPool> m_threadPool;
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    
    // 數據存儲
    QMap<QString, LLMModelInfo> m_availableModels;
    QMap<QString, LLMRequestConfig> m_activeRequests;
    QMap<QString, LLMResponse> m_completedResponses;
    QQueue<LLMRequestConfig> m_requestQueue;
    
    // 配置
    QString m_defaultModel = "llama3:latest";
    int m_maxRetries = 3;
    int m_retryDelay = 1000; // ms
    bool m_autoRetryEnabled = true;
    int m_maxConcurrentRequests = 10;
    
    // 統計
    std::atomic<int> m_totalRequests{0};
    std::atomic<int> m_successfulRequests{0};
    std::atomic<int> m_failedRequests{0};
    QList<qint64> m_responseTimes;
    QMap<QString, int> m_modelUsage;
    
    // 定時器
    QTimer *m_healthCheckTimer;
    QTimer *m_cleanupTimer;
    QTimer *m_statsTimer;
    
    // 線程安全
    mutable QMutex m_mutex;
    mutable QMutex m_statsMutex;
    
    // 私有方法
    QString generateRequestId() const;
    void processRequestQueue();
    void retryFailedRequest(const QString &requestId);
    void updateServerHealth(const QString &serverName, bool isHealthy);
    void logRequest(const LLMRequestConfig &config);
    void logResponse(const LLMResponse &response);
};

/**
 * @brief 提示詞模板管理器
 */
class PromptTemplateManager : public QObject
{
    Q_OBJECT

public:    struct PromptTemplate {
        QString id;
        QString name;
        QString category;
        QString templateText;  // 修复template关键字冲突
        QMap<QString, QString> variables;
        QString description;
        QStringList tags;
        bool isActive = true;
    };

    explicit PromptTemplateManager(QObject *parent = nullptr);
    
    void loadTemplates(const QString &templateDir = "");
    void saveTemplates(const QString &templateDir = "");
    
    void addTemplate(const PromptTemplate &tmpl);
    void removeTemplate(const QString &templateId);
    PromptTemplate getTemplate(const QString &templateId) const;
    QList<PromptTemplate> getTemplatesByCategory(const QString &category) const;
    QList<PromptTemplate> getAllTemplates() const;
    
    QString renderTemplate(const QString &templateId, const QMap<QString, QString> &variables) const;
    QStringList getCategories() const;

private:
    QMap<QString, PromptTemplate> m_templates;
    QString m_templateDirectory;
};
