/**
 * @file AIDecisionEngine.h
 * @brief RAN Online四部門AI智能決策引擎
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "AIPlayerGenerator.h"

namespace RANOnline {
namespace AI {

/**
 * @brief LLM模型類型
 */
enum class LLMModel {
    LLAMA3,      // 隊長決策
    PHI3,        // 副隊決策  
    MISTRAL,     // 氣功決策
    GEMMA,       // 通用決策
    QWEN2,       // 中文優化
    DEEPSEEK     // 程式優化
};

/**
 * @brief AI決策日誌項目
 */
struct AIDecisionLog {
    QString timestamp;
    QString aiId;
    QString action;
    QString skill;
    QString target;
    QString result;
    int responseTime;
    QString errorMessage;
    LLMModel usedModel;
    bool isSuccess;
};

/**
 * @brief 測試報告統計
 */
struct TestReport {
    int totalRequests;
    int successfulRequests;
    int failedRequests;
    double averageResponseTime;
    double successRate;
    double testDuration;
    QMap<QString, int> errorTypes;
    QMap<LLMModel, int> modelUsage;
};

/**
 * @brief AI智能決策引擎
 */
class AIDecisionEngine : public QObject
{
    Q_OBJECT

public:
    explicit AIDecisionEngine(QObject *parent = nullptr);
    
    // 設定Ollama服務器
    void setOllamaServer(const QString &host, int port);
    
    // 單個AI決策
    void makeAIDecision(const AIDecisionRequest &request, LLMModel model = LLMModel::GEMMA);
    
    // 批量AI決策
    void makeBatchDecisions(const QJsonArray &requests);
    
    // 團隊分層決策（不同職業使用不同模型）
    void makeTeamDecisions(const QJsonObject &teamData);
    
    // 自動測試系統
    void runAutomatedTest(int testCount = 1000);
    
    // 生成測試報告
    TestReport generateTestReport() const;
    
    // 導出決策日誌
    bool exportDecisionLogs(const QString &filePath) const;
    
    // 清理日誌
    void clearLogs();
    
    // 設定自動修復
    void setAutoRepairEnabled(bool enabled);
    
    // 批量測試方法
    void runBatchTest(const QJsonArray &testData, const QString &modelType);

signals:
    void decisionCompleted(const QString &aiId, const AIDecisionResponse &response);
    void batchDecisionCompleted(const QJsonArray &results);
    void testProgressUpdated(int completed, int total);
    void testCompleted(const TestReport &report);
    void logGenerated(const QString &logEntry);
    void errorOccurred(const QString &error);
    void autoRepairTriggered(const QString &aiId, const QString &repairAction);
    void decisionMade(const AIDecisionResponse &response);

private slots:
    void onLLMResponseReceived();
    void onNetworkError(QNetworkReply::NetworkError error);
    void onTestTimerTimeout();

private:
    // 發送LLM請求
    void sendLLMRequest(const AIDecisionRequest &request, LLMModel model, const QString &requestId);
    
    // 構建提示詞
    QString buildPrompt(const AIDecisionRequest &request) const;
    
    // 解析LLM回應
    AIDecisionResponse parseLLMResponse(const QString &response, const AIDecisionRequest &request) const;
    
    // 選擇模型
    LLMModel selectModel(const QString &department, const QString &role) const;
    
    // 獲取模型名稱
    QString getModelName(LLMModel model) const;
    
    // 記錄決策日誌
    void logDecision(const AIDecisionLog &log);
    
    // 自動修復邏輯
    void attemptAutoRepair(const QString &aiId, const QString &error);
    
    // 驗證決策有效性
    bool validateDecision(const AIDecisionResponse &response, const AIDecisionRequest &request) const;
    
    // 統計分析
    void updateStatistics(const AIDecisionLog &log);
    
    // 初始化模型
    void initializeModels();
    
    // 初始化錯誤處理
    void initializeErrorHandling();

private:
    QNetworkAccessManager *m_networkManager;
    AIPlayerGenerator *m_playerGenerator;
    
    QString m_ollamaHost;
    int m_ollamaPort;
    
    // 決策日誌
    QList<AIDecisionLog> m_decisionLogs;
    
    // 請求追蹤
    QMap<QString, QPair<AIDecisionRequest, QElapsedTimer>> m_pendingRequests;
    
    // 測試相關
    QTimer *m_testTimer;
    int m_currentTestCount;
    int m_totalTestCount;
    QJsonArray m_testRequests;
    
    // 統計資料
    TestReport m_currentReport;
    
    // 設定
    bool m_autoRepairEnabled;
    int m_maxRetryCount;
    int m_requestTimeout;
    
    // 模型負載平衡
    QMap<LLMModel, int> m_modelLoad;
    
    // 錯誤處理
    QStringList m_knownErrors;
    QMap<QString, QString> m_repairActions;
};

} // namespace AI
} // namespace RANOnline
