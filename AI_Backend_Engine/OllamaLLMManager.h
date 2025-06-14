#ifndef OLLAMALLMMANAGER_H
#define OLLAMALLMMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMutex>
#include <QQueue>
#include <QSettings>
#include <QThread>
#include <QEventLoop>
#include <QLoggingCategory>
#include <memory>
#include <atomic>

Q_DECLARE_LOGGING_CATEGORY(llmManager)

/**
 * @brief Ollama本地LLM統一管理器
 * 
 * 功能特性:
 * - 支援多種LLM模型自動切換 (llama3, phi3, mistral, gemma, qwen2, deepseek-coder)
 * - 智能請求分流和負載均衡
 * - 異步流式API調用
 * - 自動重試機制和錯誤恢復
 * - 詳細異常日誌記錄
 * - 場景化模型選擇策略
 * - 參數化prompt模板管理
 * - 批次請求處理
 * - 連接池管理
 * 
 * @author Jy技術團隊
 * @date 2025-06-14
 * @version 2.0.0
 */

namespace RANOnline {
namespace AI {

/**
 * @brief LLM模型信息結構
 */
struct ModelInfo {
    QString name;               // 模型名稱
    QString family;             // 模型系列
    QString parameterSize;      // 參數規模
    QString quantizationLevel;  // 量化等級
    qint64 size;               // 模型大小(字節)
    QDateTime modifiedAt;       // 修改時間
    QString digest;             // 模型摘要
    bool isAvailable = false;   // 是否可用
    double avgResponseTime = 0.0; // 平均響應時間(ms)
    int successCount = 0;       // 成功請求計數
    int errorCount = 0;         // 錯誤請求計數
};

/**
 * @brief 請求場景類型
 */
enum class ScenarioType {
    GeneralChat,        // 一般對話
    CodeGeneration,     // 代碼生成
    TechnicalSupport,   // 技術支援
    GameNarrative,      // 遊戲劇情
    DataAnalysis,       // 數據分析
    Translation,        // 翻譯
    Summarization,      // 摘要
    QuestionAnswering,  // 問答
    CreativeWriting,    // 創意寫作
    Debugging          // 調試協助
};

/**
 * @brief LLM請求配置
 */
struct LLMRequestConfig {
    QString model;              // 指定模型名稱（空則自動選擇）
    ScenarioType scenario = ScenarioType::GeneralChat; // 場景類型
    QString prompt;             // 用戶輸入
    QString systemPrompt;       // 系統提示詞
    QJsonObject options;        // 模型參數選項
    bool stream = true;         // 是否使用流式輸出
    int maxRetries = 3;         // 最大重試次數
    int timeoutMs = 30000;      // 超時時間(毫秒)
    QVariantMap metadata;       // 附加元數據
};

/**
 * @brief LLM響應結果
 */
struct LLMResponse {
    bool success = false;       // 請求是否成功
    QString content;            // 響應內容
    QString model;              // 使用的模型
    QString errorMessage;       // 錯誤信息
    QJsonObject rawResponse;    // 原始JSON響應
    qint64 responseTimeMs = 0;  // 響應時間
    int totalTokens = 0;        // 總token數
    QDateTime timestamp;        // 響應時間戳
    QVariantMap metadata;       // 響應元數據
};

/**
 * @brief 批次請求項目
 */
struct BatchRequestItem {
    QString id;                 // 請求ID
    LLMRequestConfig config;    // 請求配置
    QDateTime submitTime;       // 提交時間
    int priority = 0;           // 優先級（越高越優先）
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

    // ============================================================
    // 🔧 初始化和配置
    // ============================================================
    
    /**
     * @brief 初始化LLM管理器
     * @param ollamaUrl Ollama服務器URL（默認: http://localhost:11434）
     * @return 是否初始化成功
     */
    bool initialize(const QString &ollamaUrl = "http://localhost:11434");
    
    /**
     * @brief 檢查Ollama服務是否可用
     * @return 服務狀態
     */
    bool isServiceAvailable() const;
    
    /**
     * @brief 載入配置文件
     * @param configPath 配置文件路徑
     */
    void loadConfiguration(const QString &configPath);
    
    /**
     * @brief 保存配置文件
     * @param configPath 配置文件路徑
     */
    void saveConfiguration(const QString &configPath);

    // ============================================================
    // 📋 模型管理
    // ============================================================
    
    /**
     * @brief 刷新可用模型列表
     * @return 是否成功獲取模型列表
     */
    bool refreshModelList();
    
    /**
     * @brief 獲取所有可用模型
     * @return 模型信息列表
     */
    QList<ModelInfo> getAvailableModels() const;
    
    /**
     * @brief 根據場景選擇最佳模型
     * @param scenario 使用場景
     * @return 推薦的模型名稱
     */
    QString selectBestModel(ScenarioType scenario) const;
    
    /**
     * @brief 獲取模型統計信息
     * @param modelName 模型名稱
     * @return 模型信息
     */
    ModelInfo getModelInfo(const QString &modelName) const;

    // ============================================================
    // 🚀 核心請求方法
    // ============================================================
    
    /**
     * @brief 同步LLM請求
     * @param config 請求配置
     * @return 響應結果
     */
    LLMResponse generateSync(const LLMRequestConfig &config);
    
    /**
     * @brief 異步LLM請求
     * @param config 請求配置
     * @param requestId 請求ID（用於追蹤）
     */
    void generateAsync(const LLMRequestConfig &config, const QString &requestId = QString());
    
    /**
     * @brief 流式LLM請求
     * @param config 請求配置
     * @param requestId 請求ID
     */
    void generateStream(const LLMRequestConfig &config, const QString &requestId = QString());

    // ============================================================
    // 📦 批次和隊列管理
    // ============================================================
    
    /**
     * @brief 添加批次請求
     * @param items 批次請求項目列表
     * @return 批次ID
     */
    QString addBatchRequest(const QList<BatchRequestItem> &items);
    
    /**
     * @brief 取消請求
     * @param requestId 請求ID
     * @return 是否成功取消
     */
    bool cancelRequest(const QString &requestId);
    
    /**
     * @brief 獲取隊列狀態
     * @return 隊列信息
     */
    QJsonObject getQueueStatus() const;

    // ============================================================
    // 🎯 便捷方法
    // ============================================================
    
    /**
     * @brief 快速對話請求
     * @param message 用戶消息
     * @param systemPrompt 系統提示詞
     * @param modelHint 模型提示（空則自動選擇）
     * @return 響應內容
     */
    QString quickChat(const QString &message, 
                     const QString &systemPrompt = QString(),
                     const QString &modelHint = QString());
    
    /**
     * @brief 代碼生成請求
     * @param requirement 需求描述
     * @param language 編程語言
     * @return 生成的代碼
     */
    QString generateCode(const QString &requirement, const QString &language = "cpp");
    
    /**
     * @brief 技術問題解答
     * @param question 技術問題
     * @param context 上下文信息
     * @return 解答內容
     */
    QString answerTechnicalQuestion(const QString &question, const QString &context = QString());

    // ============================================================
    // ⚙️ 設置和控制
    // ============================================================
    
    /**
     * @brief 設置全域參數
     * @param options 參數選項
     */
    void setGlobalOptions(const QJsonObject &options);
    
    /**
     * @brief 啟用/禁用自動重試
     * @param enabled 是否啟用
     */
    void setAutoRetryEnabled(bool enabled);
    
    /**
     * @brief 設置最大並發請求數
     * @param maxConcurrent 最大並發數
     */
    void setMaxConcurrentRequests(int maxConcurrent);
    
    /**
     * @brief 設置模型選擇策略
     * @param strategy 選擇策略（"performance", "accuracy", "balanced"）
     */
    void setModelSelectionStrategy(const QString &strategy);

signals:
    // ============================================================
    // 📡 信號
    // ============================================================
    
    /**
     * @brief 異步請求完成信號
     * @param requestId 請求ID
     * @param response 響應結果
     */
    void requestCompleted(const QString &requestId, const LLMResponse &response);
    
    /**
     * @brief 流式響應數據信號
     * @param requestId 請求ID
     * @param chunk 數據塊
     * @param isComplete 是否完成
     */
    void streamDataReceived(const QString &requestId, const QString &chunk, bool isComplete);
    
    /**
     * @brief 請求錯誤信號
     * @param requestId 請求ID
     * @param errorMessage 錯誤信息
     * @param retryCount 重試次數
     */
    void requestError(const QString &requestId, const QString &errorMessage, int retryCount);
    
    /**
     * @brief 模型切換信號
     * @param fromModel 原模型
     * @param toModel 新模型
     * @param reason 切換原因
     */
    void modelSwitched(const QString &fromModel, const QString &toModel, const QString &reason);
    
    /**
     * @brief 批次處理進度信號
     * @param batchId 批次ID
     * @param completed 完成數量
     * @param total 總數量
     */
    void batchProgress(const QString &batchId, int completed, int total);

private slots:
    void onNetworkReplyFinished();
    void onStreamDataReady();
    void onRequestTimeout();
    void onProcessQueue();
    void onHealthCheck();

private:
    // ============================================================
    // 🔧 內部方法
    // ============================================================
    
    void setupNetworking();
    void setupTimers();
    void setupLogging();
    QString buildPrompt(const LLMRequestConfig &config);
    QJsonObject buildRequestPayload(const LLMRequestConfig &config);
    LLMResponse parseResponse(QNetworkReply *reply, const LLMRequestConfig &config);
    void updateModelStatistics(const QString &model, bool success, qint64 responseTime);
    void logRequest(const LLMRequestConfig &config, const LLMResponse &response);
    void retryRequest(const QString &requestId, const LLMRequestConfig &config, int retryCount);
    QString generateRequestId();
    void processRequestQueue();
    bool isModelHealthy(const QString &modelName) const;
    void switchToBackupModel(const QString &failedModel, const LLMRequestConfig &config);

    // ============================================================
    // 📊 成員變量
    // ============================================================
    
    // 網絡和連接
    QString m_ollamaUrl;
    std::unique_ptr<QNetworkAccessManager> m_networkManager;
    QMap<QString, QNetworkReply*> m_activeRequests;
    
    // 模型管理
    QMap<QString, ModelInfo> m_availableModels;
    QString m_modelSelectionStrategy;
    QJsonObject m_globalOptions;
    
    // 請求隊列和配置
    QQueue<BatchRequestItem> m_requestQueue;
    QMap<QString, LLMRequestConfig> m_pendingRequests;
    QMap<QString, int> m_retryCounters;
    
    // 設置和控制
    bool m_autoRetryEnabled;
    int m_maxConcurrentRequests;
    int m_currentConcurrentRequests;
    QSettings *m_settings;
    
    // 定時器和監控
    QTimer *m_queueTimer;
    QTimer *m_healthCheckTimer;
    QMap<QString, QTimer*> m_requestTimers;
    
    // 線程安全
    mutable QMutex m_mutex;
    mutable QMutex m_queueMutex;
    mutable QMutex m_statsMutex;
    
    // 狀態管理
    std::atomic<bool> m_isInitialized{false};
    std::atomic<bool> m_isServiceAvailable{false};
    
    // 統計信息
    QMap<QString, QVariantMap> m_modelStats;
    qint64 m_totalRequests;
    qint64 m_successfulRequests;
    qint64 m_failedRequests;
};

/**
 * @brief LLM請求工作線程
 */
class LLMRequestWorker : public QObject
{
    Q_OBJECT

public:
    explicit LLMRequestWorker(OllamaLLMManager *manager, QObject *parent = nullptr);

public slots:
    void processRequest(const QString &requestId, const LLMRequestConfig &config);

signals:
    void requestFinished(const QString &requestId, const LLMResponse &response);
    void streamData(const QString &requestId, const QString &chunk, bool isComplete);
    void requestFailed(const QString &requestId, const QString &error);

private:
    OllamaLLMManager *m_manager;
};

} // namespace AI
} // namespace RANOnline

Q_DECLARE_METATYPE(RANOnline::AI::ModelInfo)
Q_DECLARE_METATYPE(RANOnline::AI::LLMRequestConfig)
Q_DECLARE_METATYPE(RANOnline::AI::LLMResponse)
Q_DECLARE_METATYPE(RANOnline::AI::ScenarioType)

#endif // OLLAMALLMMANAGER_H