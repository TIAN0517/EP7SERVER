/**
 * @file AIPlayerBrain.h
 * @brief AI玩家大腦 - 集成AI決策核心的完整AI玩家實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 * 
 * 🎯 AI玩家大腦特性:
 * ✅ 集成AIDecisionCore決策系統
 * ✅ 完整的AI玩家行為模擬
 * ✅ 支援多種決策策略切換
 * ✅ 學習能力和經驗累積
 * ✅ 可擴展的行為模組
 * ✅ 性能監控和調試支援
 */

#pragma once

#include "AIDecisionCore.h"
#include "GameAIProtocol.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <memory>

namespace JyAI {

// ========================================================================
// AI玩家大腦類
// ========================================================================

/**
 * @brief AI玩家大腦 - 每個AI玩家的智能核心
 */
class AIPlayerBrain : public QObject
{
    Q_OBJECT

public:
    explicit AIPlayerBrain(const QString &playerId, QObject *parent = nullptr);
    virtual ~AIPlayerBrain();

    // ===== 基本設置 =====
    
    /**
     * @brief 設置AI玩家資料
     */
    void setPlayerData(const RANOnline::AI::AIPlayerData &playerData);
    
    /**
     * @brief 獲取AI玩家資料
     */
    RANOnline::AI::AIPlayerData getPlayerData() const;
    
    /**
     * @brief 設置決策策略
     */
    void setDecisionStrategy(DecisionStrategy strategy);
    
    /**
     * @brief 註冊自定義策略
     */
    void registerCustomStrategy(const QString &name, 
                               std::shared_ptr<IDecisionStrategy> strategy);

    // ===== 主要更新循環 =====
    
    /**
     * @brief 主更新方法 - 每幀調用
     */
    void update();
    
    /**
     * @brief 設置更新頻率
     */
    void setUpdateInterval(int milliseconds);
    
    /**
     * @brief 啟動/停止AI
     */
    void start();
    void stop();
    bool isRunning() const;

    // ===== 環境感知 =====
    
    /**
     * @brief 更新世界狀態
     */
    void updateWorldState(const QJsonObject &worldState);
    
    /**
     * @brief 獲取感知資料
     */
    PerceptionData getCurrentPerception() const;
    
    /**
     * @brief 設置感知範圍
     */
    void setPerceptionRange(float range);

    // ===== 行為執行 =====
    
    /**
     * @brief 執行動作
     */
    void executeAction(const AIAction &action);
    
    /**
     * @brief 獲取當前動作
     */
    AIAction getCurrentAction() const;
    
    /**
     * @brief 是否正在執行動作
     */
    bool isExecutingAction() const;

    // ===== 學習系統 =====
    
    /**
     * @brief 學習經驗
     */
    void learnFromOutcome(const QString &actionType, const QString &outcome, bool successful);
    
    /**
     * @brief 載入訓練模型
     */
    bool loadTrainingModel(const QString &modelPath);
    
    /**
     * @brief 保存訓練模型
     */
    bool saveTrainingModel(const QString &modelPath);

    // ===== 配置管理 =====
    
    /**
     * @brief 載入配置
     */
    void loadConfiguration(const QString &configPath);
    
    /**
     * @brief 更新配置
     */
    void updateConfiguration(const QJsonObject &config);
    
    /**
     * @brief 獲取當前配置
     */
    QJsonObject getCurrentConfiguration() const;

    // ===== 調試和監控 =====
    
    /**
     * @brief 獲取決策日誌
     */
    QJsonArray getDecisionLog() const;
    
    /**
     * @brief 獲取性能統計
     */
    QJsonObject getPerformanceStats() const;
    
    /**
     * @brief 獲取當前狀態
     */
    QString getCurrentState() const;
    
    /**
     * @brief 重置統計資料
     */
    void resetStats();

    // ===== 社交行為 =====
    
    /**
     * @brief 設置隊友
     */
    void setTeammates(const QStringList &teammateIds);
    
    /**
     * @brief 發送團隊訊息
     */
    void sendTeamMessage(const QString &message);
    
    /**
     * @brief 接收團隊訊息
     */
    void receiveTeamMessage(const QString &senderId, const QString &message);

signals:
    void actionExecuted(const QString &playerId, const AIAction &action);
    void decisionMade(const QString &playerId, const AIAction &decision);
    void learningCompleted(const QString &playerId, const QString &experience);
    void stateChanged(const QString &playerId, const QString &newState);
    void errorOccurred(const QString &playerId, const QString &error);
    void teamMessageSent(const QString &playerId, const QString &message);
    void performanceReport(const QString &playerId, const QJsonObject &stats);

private slots:
    void onUpdateTimer();
    void onActionCompleted();
    void onDecisionMade(const AIAction &action);

private:
    // ===== 初始化方法 =====
    void initializeDecisionCore();
    void initializePerception();
    void initializeBehavior();
    void setupTimers();
    
    // ===== 感知處理 =====
    PerceptionData perceiveEnvironment();
    void processWorldState(const QJsonObject &worldState, PerceptionData &perception);
    void detectThreats(PerceptionData &perception);
    void detectOpportunities(PerceptionData &perception);
    
    // ===== 行為執行 =====
    void startActionExecution(const AIAction &action);
    void updateActionProgress();
    bool isActionComplete() const;
    void finishAction();
    
    // ===== 狀態管理 =====
    void updateInternalState();
    QString calculateCurrentState() const;
    void transitionToState(const QString &newState);
    
    // ===== 學習邏輯 =====
    void recordExperience(const AIAction &action, const QString &outcome, bool successful);
    void processLearningData();
    
    // ===== 團隊協作 =====
    void updateTeamCoordination();
    void processTeamMessages();
    AIAction considerTeamAction(const AIAction &individualAction);

private:
    // ===== 核心組件 =====
    std::unique_ptr<AIDecisionCore> m_decisionCore;
    QString m_playerId;
    RANOnline::AI::AIPlayerData m_playerData;
    
    // ===== 狀態管理 =====
    QString m_currentState;
    PerceptionData m_currentPerception;
    QJsonObject m_worldState;
    AIAction m_currentAction;
    
    // ===== 計時器 =====
    QTimer *m_updateTimer;
    QTimer *m_actionTimer;
    int m_updateInterval;
    bool m_isRunning;
    
    // ===== 動作執行 =====
    bool m_isExecutingAction;
    qint64 m_actionStartTime;
    int m_actionDuration;
    float m_actionProgress;
    
    // ===== 感知系統 =====
    float m_perceptionRange;
    QJsonObject m_sensorData;
    
    // ===== 學習系統 =====
    std::vector<DecisionHistory> m_experienceHistory;
    int m_maxExperienceHistory;
    QString m_modelPath;
    
    // ===== 配置 =====
    QJsonObject m_configuration;
    QString m_configPath;
    
    // ===== 團隊系統 =====
    QStringList m_teammates;
    QJsonArray m_teamMessages;
    QString m_teamRole; // leader, member, solo
    
    // ===== 性能監控 =====
    QJsonObject m_performanceStats;
    QJsonArray m_decisionLog;
    qint64 m_totalDecisions;
    qint64 m_successfulActions;
    qint64 m_startTime;
};

// ========================================================================
// AI玩家管理器
// ========================================================================

/**
 * @brief AI玩家管理器 - 管理多個AI玩家
 */
class AIPlayerManager : public QObject
{
    Q_OBJECT

public:
    explicit AIPlayerManager(QObject *parent = nullptr);
    virtual ~AIPlayerManager();

    // ===== 玩家管理 =====
    
    /**
     * @brief 創建AI玩家
     */
    QString createAIPlayer(const RANOnline::AI::AIPlayerData &playerData);
    
    /**
     * @brief 移除AI玩家
     */
    bool removeAIPlayer(const QString &playerId);
    
    /**
     * @brief 獲取AI玩家
     */
    AIPlayerBrain* getAIPlayer(const QString &playerId);
    
    /**
     * @brief 獲取所有AI玩家ID
     */
    QStringList getAllPlayerIds() const;
    
    /**
     * @brief 獲取AI玩家數量
     */
    int getPlayerCount() const;

    // ===== 批量操作 =====
    
    /**
     * @brief 批量創建AI玩家
     */
    QStringList createMultipleAIPlayers(const QJsonArray &playerDataArray);
    
    /**
     * @brief 啟動所有AI玩家
     */
    void startAllPlayers();
    
    /**
     * @brief 停止所有AI玩家
     */
    void stopAllPlayers();
    
    /**
     * @brief 更新所有玩家世界狀態
     */
    void updateWorldStateForAll(const QJsonObject &worldState);

    // ===== 策略管理 =====
    
    /**
     * @brief 設置所有玩家策略
     */
    void setStrategyForAll(DecisionStrategy strategy);
    
    /**
     * @brief 設置特定玩家策略
     */
    void setStrategyForPlayer(const QString &playerId, DecisionStrategy strategy);
    
    /**
     * @brief 註冊全局自定義策略
     */
    void registerGlobalStrategy(const QString &name, 
                               std::shared_ptr<IDecisionStrategy> strategy);

    // ===== 團隊管理 =====
    
    /**
     * @brief 創建團隊
     */
    QString createTeam(const QStringList &playerIds);
    
    /**
     * @brief 解散團隊
     */
    bool disbandTeam(const QString &teamId);
    
    /**
     * @brief 獲取玩家團隊
     */
    QString getPlayerTeam(const QString &playerId) const;

    // ===== 訓練系統 =====
    
    /**
     * @brief 開始訓練模式
     */
    void startTrainingMode(const QString &trainingScenario);
    
    /**
     * @brief 停止訓練模式
     */
    void stopTrainingMode();
    
    /**
     * @brief 保存所有訓練模型
     */
    bool saveAllModels(const QString &directory);
    
    /**
     * @brief 載入所有訓練模型
     */
    bool loadAllModels(const QString &directory);

    // ===== 監控和統計 =====
    
    /**
     * @brief 獲取整體性能統計
     */
    QJsonObject getOverallStats() const;
    
    /**
     * @brief 獲取所有玩家狀態
     */
    QJsonArray getAllPlayerStates() const;
    
    /**
     * @brief 重置所有統計
     */
    void resetAllStats();

signals:
    void playerCreated(const QString &playerId);
    void playerRemoved(const QString &playerId);
    void teamCreated(const QString &teamId, const QStringList &memberIds);
    void teamDisbanded(const QString &teamId);
    void trainingCompleted(const QString &scenario, const QJsonObject &results);
    void overallStatsUpdated(const QJsonObject &stats);

private slots:
    void onPlayerAction(const QString &playerId, const AIAction &action);
    void onPlayerDecision(const QString &playerId, const AIAction &decision);
    void onPlayerError(const QString &playerId, const QString &error);
    void onStatsTimer();

private:
    // ===== 初始化 =====
    void initializeManager();
    void setupGlobalStrategies();
    void startStatsCollection();
    
    // ===== 玩家管理 =====
    QString generatePlayerId();
    void setupPlayerConnections(AIPlayerBrain *player);
    void cleanupPlayer(const QString &playerId);
    
    // ===== 團隊管理 =====
    QString generateTeamId();
    void updateTeamCoordination(const QString &teamId);
    
    // ===== 統計收集 =====
    void collectPlayerStats();
    void updateOverallStats();

private:
    // ===== 玩家管理 =====
    std::unordered_map<QString, std::unique_ptr<AIPlayerBrain>> m_players;
    std::unordered_map<QString, QStringList> m_teams; // teamId -> playerIds
    std::unordered_map<QString, QString> m_playerTeams; // playerId -> teamId
    
    // ===== 全局策略 =====
    std::unordered_map<QString, std::shared_ptr<IDecisionStrategy>> m_globalStrategies;
    
    // ===== 訓練系統 =====
    bool m_trainingMode;
    QString m_currentTrainingScenario;
    QJsonObject m_trainingResults;
    
    // ===== 統計系統 =====
    QTimer *m_statsTimer;
    QJsonObject m_overallStats;
    qint64 m_managerStartTime;
    
    // ===== 配置 =====
    QJsonObject m_managerConfig;
    int m_maxPlayers;
    int m_statsUpdateInterval;
};

} // namespace JyAI
