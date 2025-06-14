/**
 * @file AISystemIntegration.h
 * @brief AI系統集成介面 - 連接新AI決策核心與現有RANOnline AI系統
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 * 
 * 🔗 集成介面特性:
 * ✅ 無縫集成現有AI系統
 * ✅ 保持向後相容性
 * ✅ 提供統一的API介面
 * ✅ 支援熱切換AI策略
 * ✅ 完整的事件處理機制
 * ✅ 性能監控和統計
 */

#pragma once

#include "AIDecisionCore.h"
#include "AIPlayerBrain.h"
#include "AIPlayerGenerator.h"
#include "AIDecisionEngine.h"
#include "AIManagementWidget.h"
#include "GameAIProtocol.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QHash>
#include <memory>

// 前向聲明
namespace RANOnline { namespace AI {
    class AIPlayerData;
    class AIDecisionEngine;
    class AIPlayerGenerator;
    class AIManagementWidget;
}}

namespace JyAI {

// ========================================================================
// AI系統集成管理器
// ========================================================================

/**
 * @brief AI系統集成管理器 - 統一管理新舊AI系統
 */
class AISystemIntegrationManager : public QObject
{
    Q_OBJECT

public:
    explicit AISystemIntegrationManager(QObject *parent = nullptr);
    virtual ~AISystemIntegrationManager();

    // ===== 系統初始化 =====
    
    /**
     * @brief 初始化集成系統
     * @param existingEngine 現有的AI決策引擎
     * @param existingGenerator 現有的AI玩家生成器
     * @param existingWidget 現有的AI管理界面
     * @return 是否初始化成功
     */
    bool initialize(RANOnline::AI::AIDecisionEngine *existingEngine = nullptr,
                   RANOnline::AI::AIPlayerGenerator *existingGenerator = nullptr,
                   RANOnline::AI::AIManagementWidget *existingWidget = nullptr);

    /**
     * @brief 啟動集成系統
     */
    void start();

    /**
     * @brief 停止集成系統
     */
    void stop();

    /**
     * @brief 檢查系統是否已初始化
     */
    bool isInitialized() const { return m_isInitialized; }

    /**
     * @brief 檢查系統是否正在運行
     */
    bool isRunning() const { return m_isRunning; }

    // ===== AI玩家管理 =====

    /**
     * @brief 創建新的AI玩家 (使用新AI決策核心)
     * @param playerId 玩家ID
     * @param playerData 玩家數據
     * @param useNewAI 是否使用新AI決策核心
     * @return 是否創建成功
     */
    bool createAIPlayer(const QString &playerId, 
                       const RANOnline::AI::AIPlayerData &playerData,
                       bool useNewAI = true);

    /**
     * @brief 移除AI玩家
     * @param playerId 玩家ID
     * @return 是否移除成功
     */
    bool removeAIPlayer(const QString &playerId);

    /**
     * @brief 切換AI玩家的決策系統
     * @param playerId 玩家ID
     * @param useNewAI 是否使用新AI決策核心
     * @return 是否切換成功
     */
    bool switchAISystem(const QString &playerId, bool useNewAI);

    /**
     * @brief 設置AI玩家的決策策略 (僅對新AI系統有效)
     * @param playerId 玩家ID
     * @param strategy 決策策略
     * @return 是否設置成功
     */
    bool setPlayerStrategy(const QString &playerId, DecisionStrategyType strategy);

    /**
     * @brief 獲取AI玩家列表
     */
    QStringList getAIPlayerList() const;

    /**
     * @brief 獲取使用新AI系統的玩家列表
     */
    QStringList getNewAIPlayerList() const;

    /**
     * @brief 獲取使用舊AI系統的玩家列表
     */
    QStringList getLegacyAIPlayerList() const;

    // ===== 系統配置 =====

    /**
     * @brief 設置全局AI配置
     * @param config 配置參數
     */
    void setGlobalAIConfig(const QVariantMap &config);

    /**
     * @brief 獲取全局AI配置
     */
    QVariantMap getGlobalAIConfig() const;

    /**
     * @brief 重載AI配置
     */
    bool reloadConfiguration();

    /**
     * @brief 啟用/禁用調試模式
     * @param enabled 是否啟用調試
     */
    void enableDebugMode(bool enabled);

    // ===== 統計和監控 =====

    /**
     * @brief 獲取系統統計資訊
     */
    struct SystemStats {
        int totalAIPlayers;
        int newAIPlayers;
        int legacyAIPlayers;
        double averageDecisionTime;
        double averageUpdateFrequency;
        int totalDecisions;
        int totalErrors;
        QDateTime lastUpdateTime;
    };

    SystemStats getSystemStats() const;

    /**
     * @brief 獲取詳細的系統報告
     */
    QString getDetailedReport() const;

    /**
     * @brief 導出系統日誌
     * @param filePath 日誌文件路徑
     * @return 是否導出成功
     */
    bool exportSystemLog(const QString &filePath) const;

signals:
    // ===== 系統狀態信號 =====
    
    /**
     * @brief 系統初始化完成
     */
    void systemInitialized();

    /**
     * @brief 系統啟動
     */
    void systemStarted();

    /**
     * @brief 系統停止
     */
    void systemStopped();

    /**
     * @brief 系統錯誤
     */
    void systemError(const QString &error);

    // ===== AI玩家信號 =====

    /**
     * @brief AI玩家創建
     */
    void aiPlayerCreated(const QString &playerId, bool useNewAI);

    /**
     * @brief AI玩家移除
     */
    void aiPlayerRemoved(const QString &playerId);

    /**
     * @brief AI系統切換
     */
    void aiSystemSwitched(const QString &playerId, bool useNewAI);

    /**
     * @brief AI行為執行
     */
    void aiActionExecuted(const QString &playerId, const QString &action, const QVariantMap &parameters);

    // ===== 性能監控信號 =====

    /**
     * @brief 性能統計更新
     */
    void performanceStatsUpdated(const SystemStats &stats);

    /**
     * @brief 性能警告
     */
    void performanceWarning(const QString &playerId, const QString &warning);

private slots:
    void onUpdateTimer();
    void onNewAIPlayerAction(const QString &playerId, const AIAction &action);
    void onLegacyAIPlayerAction(const QString &playerId, const QVariantMap &action);
    void onAIPlayerError(const QString &playerId, const QString &error);

private:
    // ===== 私有方法 =====
    void setupConnections();
    void updateStatistics();
    void handleAIPlayerAction(const QString &playerId, const AIAction &action);
    void forwardActionToGame(const QString &playerId, const AIAction &action);
    void migratePlayerData(const QString &playerId, bool toNewAI);
    void logSystemEvent(const QString &event, const QString &details = QString());

    // ===== 成員變量 =====
    
    // 系統狀態
    bool m_isInitialized;
    bool m_isRunning;
    bool m_debugMode;
    QTimer *m_updateTimer;
    QDateTime m_startTime;

    // AI系統組件
    std::unique_ptr<AIPlayerManager> m_newAIManager;
    RANOnline::AI::AIDecisionEngine *m_legacyEngine;
    RANOnline::AI::AIPlayerGenerator *m_legacyGenerator;
    RANOnline::AI::AIManagementWidget *m_legacyWidget;

    // 玩家管理
    QHash<QString, bool> m_playerAIType; // true = 新AI，false = 舊AI
    QHash<QString, RANOnline::AI::AIPlayerData> m_playerData;

    // 統計資料
    mutable SystemStats m_stats;
    QHash<QString, QDateTime> m_playerLastUpdate;
    QHash<QString, int> m_playerDecisionCount;
    QHash<QString, double> m_playerDecisionTime;
    QList<QString> m_systemLog;

    // 配置
    QVariantMap m_globalConfig;
    QString m_configFilePath;
};

// ========================================================================
// AI行為轉換器
// ========================================================================

/**
 * @brief AI行為轉換器 - 在新舊AI系統之間轉換行為數據
 */
class AIActionConverter : public QObject
{
    Q_OBJECT

public:
    explicit AIActionConverter(QObject *parent = nullptr);

    /**
     * @brief 將新AI行為轉換為舊AI行為格式
     * @param newAction 新AI行為
     * @return 舊AI行為格式
     */
    static QVariantMap convertNewToLegacy(const AIAction &newAction);

    /**
     * @brief 將舊AI行為轉換為新AI行為格式
     * @param legacyAction 舊AI行為
     * @return 新AI行為格式
     */
    static AIAction convertLegacyToNew(const QVariantMap &legacyAction);

    /**
     * @brief 將感知數據轉換為舊AI格式
     * @param perception 新AI感知數據
     * @return 舊AI感知數據格式
     */
    static QVariantMap convertPerceptionToLegacy(const PerceptionData &perception);

    /**
     * @brief 將舊AI感知數據轉換為新AI格式
     * @param legacyPerception 舊AI感知數據
     * @return 新AI感知數據格式
     */
    static PerceptionData convertPerceptionFromLegacy(const QVariantMap &legacyPerception);

private:
    // 靜態轉換映射表
    static QHash<ActionType, QString> s_actionTypeMap;
    static QHash<QString, ActionType> s_reverseActionTypeMap;
    
    static void initializeMappings();
    static bool s_mappingsInitialized;
};

// ========================================================================
// AI系統橋接器
// ========================================================================

/**
 * @brief AI系統橋接器 - 提供統一的API介面
 */
class AISystemBridge : public QObject
{
    Q_OBJECT

public:
    explicit AISystemBridge(AISystemIntegrationManager *manager, QObject *parent = nullptr);

    // ===== 統一API介面 =====

    /**
     * @brief 執行AI決策 (統一介面)
     * @param playerId 玩家ID
     * @param gameState 遊戲狀態
     * @return AI決策結果
     */
    QVariantMap makeAIDecision(const QString &playerId, const QVariantMap &gameState);

    /**
     * @brief 更新AI狀態 (統一介面)
     * @param playerId 玩家ID
     * @param stateUpdate 狀態更新
     */
    void updateAIState(const QString &playerId, const QVariantMap &stateUpdate);

    /**
     * @brief 提供AI學習回饋 (統一介面)
     * @param playerId 玩家ID
     * @param action 執行的行為
     * @param reward 獎勵值
     */
    void provideAIFeedback(const QString &playerId, const QVariantMap &action, double reward);

    /**
     * @brief 獲取AI狀態 (統一介面)
     * @param playerId 玩家ID
     * @return AI狀態資訊
     */
    QVariantMap getAIState(const QString &playerId) const;

    /**
     * @brief 設置AI參數 (統一介面)
     * @param playerId 玩家ID
     * @param parameters 參數設置
     */
    void setAIParameters(const QString &playerId, const QVariantMap &parameters);

signals:
    /**
     * @brief AI決策完成
     */
    void aiDecisionMade(const QString &playerId, const QVariantMap &decision);

    /**
     * @brief AI狀態更新
     */
    void aiStateUpdated(const QString &playerId, const QVariantMap &state);

private:
    AISystemIntegrationManager *m_manager;
};

} // namespace JyAI
