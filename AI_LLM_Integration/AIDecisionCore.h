/**
 * @file AIDecisionCore.h
 * @brief AI決策核心模組 - 集成強化決策邏輯
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 * 
 * 🎯 強化決策邏輯特性:
 * ✅ 效用函數系統 (Utility Function)
 * ✅ 分層決策架構 (Hierarchical Decision Making)
 * ✅ 行為樹決策 (Behavior Tree)
 * ✅ 強化學習集成 (Q-Learning)
 * ✅ 環境感知能力 (Environment Perception)
 * ✅ 學習能力進化 (Learning Evolution)
 * ✅ 策略模式支援 (Strategy Pattern)
 * ✅ 熱更新配置 (Hot-Swap Configuration)
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace JyAI {

// ========================================================================
// 核心資料結構
// ========================================================================

/**
 * @brief 環境感知資料
 */
struct PerceptionData {
    // 基本感知
    QJsonObject worldState;        // 世界狀態
    QJsonArray visibleEnemies;     // 可見敵人
    QJsonArray visibleAllies;      // 可見盟友
    QJsonObject mapInfo;           // 地圖資訊
    
    // 戰鬥感知
    float healthRatio;             // 血量比例
    float manaRatio;               // 法力比例
    QStringList activeBuffs;       // 當前Buff
    QStringList activeDebuffs;     // 當前Debuff
    
    // 戰術感知
    QString currentThreat;         // 當前威脅
    QString teamFormation;         // 隊形狀態
    int enemyCount;                // 敵人數量
    int allyCount;                 // 盟友數量
    
    // 環境感知
    QString weather;               // 天氣
    QString timeOfDay;             // 時間
    float visibility;              // 可見度
    QStringList environmentEffects; // 環境效果
};

/**
 * @brief 決策歷史記錄
 */
struct DecisionHistory {
    QString actionType;            // 行動類型
    QString targetId;              // 目標ID
    QString skillUsed;             // 使用技能
    float utilityScore;            // 效用分數
    QString outcome;               // 結果
    qint64 timestamp;              // 時間戳
    QString situation;             // 當時情況
    bool wasSuccessful;            // 是否成功
};

/**
 * @brief 行為樹節點類型
 */
enum class BehaviorNodeType {
    ACTION,        // 行動節點
    CONDITION,     // 條件節點
    SEQUENCE,      // 序列節點
    SELECTOR,      // 選擇器節點
    PARALLEL,      // 並行節點
    DECORATOR      // 裝飾器節點
};

/**
 * @brief 行為樹節點狀態
 */
enum class NodeStatus {
    SUCCESS,       // 成功
    FAILURE,       // 失敗
    RUNNING        // 執行中
};

/**
 * @brief 決策策略類型
 */
enum class DecisionStrategy {
    UTILITY_BASED,     // 效用函數
    BEHAVIOR_TREE,     // 行為樹
    Q_LEARNING,        // Q學習
    HIERARCHICAL,      // 分層決策
    HYBRID,            // 混合策略
    CUSTOM             // 自定義策略
};

/**
 * @brief AI決策動作
 */
struct AIAction {
    QString type;                  // 動作類型 (attack, move, cast, defend, etc.)
    QString target;                // 目標
    QString skill;                 // 技能
    QJsonObject parameters;        // 參數
    float confidence;              // 信心度
    QString reasoning;             // 決策理由
    float expectedUtility;         // 預期效用
};

// ========================================================================
// 前置聲明
// ========================================================================

class IDecisionStrategy;
class UtilitySystem;
class BehaviorTree;
class QLearningAgent;
class HierarchicalPlanner;
class EnvironmentPerceptor;
class ConfigManager;

// ========================================================================
// AI決策核心類
// ========================================================================

/**
 * @brief AI決策核心 - 主要決策大腦
 */
class AIDecisionCore : public QObject
{
    Q_OBJECT

public:
    explicit AIDecisionCore(QObject *parent = nullptr);
    virtual ~AIDecisionCore();

    // ===== 主要決策接口 =====
    
    /**
     * @brief 主決策方法
     * @param perception 感知資料
     * @param currentState 當前狀態
     * @param history 歷史記錄
     * @return 決策動作
     */
    AIAction decide(const PerceptionData &perception, 
                   const QJsonObject &currentState,
                   const std::vector<DecisionHistory> &history);

    // ===== 策略管理 =====
    
    /**
     * @brief 設置決策策略
     */
    void setDecisionStrategy(DecisionStrategy strategy);
    
    /**
     * @brief 註冊自定義策略
     */
    void registerCustomStrategy(const QString &name, 
                               std::shared_ptr<IDecisionStrategy> strategy);
    
    /**
     * @brief 切換策略 (支援熱切換)
     */
    void switchStrategy(const QString &strategyName);

    // ===== 配置管理 =====
    
    /**
     * @brief 載入配置 (支援熱更新)
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

    // ===== 學習能力 =====
    
    /**
     * @brief 從經驗學習
     */
    void learnFromExperience(const DecisionHistory &experience);
    
    /**
     * @brief 批量學習
     */
    void batchLearn(const std::vector<DecisionHistory> &experiences);
    
    /**
     * @brief 保存學習模型
     */
    bool saveModel(const QString &modelPath);
    
    /**
     * @brief 載入學習模型
     */
    bool loadModel(const QString &modelPath);

    // ===== 調試和監控 =====
    
    /**
     * @brief 獲取決策解釋
     */
    QString getDecisionExplanation() const;
    
    /**
     * @brief 獲取性能統計
     */
    QJsonObject getPerformanceStats() const;
    
    /**
     * @brief 重置統計資料
     */
    void resetStats();

    // ===== 環境感知 =====
    
    /**
     * @brief 更新環境感知
     */
    void updatePerception(const PerceptionData &perception);
    
    /**
     * @brief 預測環境變化
     */
    PerceptionData predictEnvironmentChange(const PerceptionData &current, float deltaTime);

signals:
    void decisionMade(const AIAction &action);
    void strategyChanged(const QString &newStrategy);
    void learningCompleted(const QString &modelInfo);
    void configurationUpdated(const QJsonObject &newConfig);
    void performanceReport(const QJsonObject &stats);
    void errorOccurred(const QString &error);

private slots:
    void onConfigurationChanged();
    void onPerformanceTimer();

private:
    // ===== 初始化方法 =====
    void initializeStrategies();
    void initializePerformanceMonitoring();
    void setupConfigurationWatcher();
    
    // ===== 內部決策邏輯 =====
    AIAction makeUtilityBasedDecision(const PerceptionData &perception, 
                                     const QJsonObject &currentState);
    AIAction makeBehaviorTreeDecision(const PerceptionData &perception, 
                                     const QJsonObject &currentState);
    AIAction makeQLearningDecision(const PerceptionData &perception, 
                                  const QJsonObject &currentState);
    AIAction makeHierarchicalDecision(const PerceptionData &perception, 
                                     const QJsonObject &currentState);
    AIAction makeHybridDecision(const PerceptionData &perception, 
                               const QJsonObject &currentState);

    // ===== 工具方法 =====
    void updatePerformanceStats(const AIAction &action, float executionTime);
    QString generateReasoningText(const AIAction &action, const PerceptionData &perception);
    void logDecision(const AIAction &action, const PerceptionData &perception);

private:
    // ===== 策略系統 =====
    DecisionStrategy m_currentStrategy;
    std::unordered_map<QString, std::shared_ptr<IDecisionStrategy>> m_strategies;
    std::shared_ptr<IDecisionStrategy> m_activeStrategy;

    // ===== 決策系統組件 =====
    std::unique_ptr<UtilitySystem> m_utilitySystem;
    std::unique_ptr<BehaviorTree> m_behaviorTree;
    std::unique_ptr<QLearningAgent> m_qLearningAgent;
    std::unique_ptr<HierarchicalPlanner> m_hierarchicalPlanner;
    std::unique_ptr<EnvironmentPerceptor> m_environmentPerceptor;
    std::unique_ptr<ConfigManager> m_configManager;

    // ===== 狀態管理 =====
    PerceptionData m_currentPerception;
    QJsonObject m_currentState;
    std::vector<DecisionHistory> m_decisionHistory;
    
    // ===== 性能監控 =====
    QTimer *m_performanceTimer;
    QJsonObject m_performanceStats;
    QString m_lastDecisionExplanation;
    
    // ===== 配置 =====
    QString m_configPath;
    QJsonObject m_configuration;
    bool m_hotUpdateEnabled;
};

// ========================================================================
// 策略接口
// ========================================================================

/**
 * @brief 決策策略接口
 */
class IDecisionStrategy
{
public:
    virtual ~IDecisionStrategy() = default;
    
    /**
     * @brief 執行決策
     */
    virtual AIAction makeDecision(const PerceptionData &perception,
                                 const QJsonObject &currentState,
                                 const std::vector<DecisionHistory> &history) = 0;
    
    /**
     * @brief 學習更新
     */
    virtual void learn(const DecisionHistory &experience) = 0;
    
    /**
     * @brief 配置更新
     */
    virtual void updateConfiguration(const QJsonObject &config) = 0;
    
    /**
     * @brief 獲取策略名稱
     */
    virtual QString getStrategyName() const = 0;
    
    /**
     * @brief 獲取決策解釋
     */
    virtual QString getExplanation() const = 0;
};

// ========================================================================
// 效用函數系統
// ========================================================================

/**
 * @brief 效用函數系統
 */
class UtilitySystem : public IDecisionStrategy
{
public:
    explicit UtilitySystem();
    virtual ~UtilitySystem() = default;

    // IDecisionStrategy 實現
    AIAction makeDecision(const PerceptionData &perception,
                         const QJsonObject &currentState,
                         const std::vector<DecisionHistory> &history) override;
    
    void learn(const DecisionHistory &experience) override;
    void updateConfiguration(const QJsonObject &config) override;
    QString getStrategyName() const override { return "UtilityBased"; }
    QString getExplanation() const override { return m_lastExplanation; }

    // 效用函數方法
    float calculateActionUtility(const QString &actionType, 
                                const PerceptionData &perception,
                                const QJsonObject &currentState);
    
    void registerUtilityFunction(const QString &actionType, 
                                std::function<float(const PerceptionData&, const QJsonObject&)> func);

private:
    std::unordered_map<QString, std::function<float(const PerceptionData&, const QJsonObject&)>> m_utilityFunctions;
    QJsonObject m_utilityWeights;
    QString m_lastExplanation;
    
    void initializeDefaultUtilityFunctions();
    float calculateCombatUtility(const PerceptionData &perception, const QJsonObject &currentState);
    float calculateSurvivalUtility(const PerceptionData &perception, const QJsonObject &currentState);
    float calculateSupportUtility(const PerceptionData &perception, const QJsonObject &currentState);
};

// ========================================================================
// 行為樹系統
// ========================================================================

/**
 * @brief 行為樹節點基類
 */
class BehaviorNode
{
public:
    virtual ~BehaviorNode() = default;
    virtual NodeStatus execute(const PerceptionData &perception, const QJsonObject &currentState) = 0;
    virtual void reset() = 0;
    virtual BehaviorNodeType getType() const = 0;
    
    void addChild(std::shared_ptr<BehaviorNode> child) { m_children.push_back(child); }
    
protected:
    std::vector<std::shared_ptr<BehaviorNode>> m_children;
};

/**
 * @brief 行為樹系統
 */
class BehaviorTree : public IDecisionStrategy
{
public:
    explicit BehaviorTree();
    virtual ~BehaviorTree() = default;

    // IDecisionStrategy 實現
    AIAction makeDecision(const PerceptionData &perception,
                         const QJsonObject &currentState,
                         const std::vector<DecisionHistory> &history) override;
    
    void learn(const DecisionHistory &experience) override;
    void updateConfiguration(const QJsonObject &config) override;
    QString getStrategyName() const override { return "BehaviorTree"; }
    QString getExplanation() const override { return m_lastExplanation; }

    // 行為樹方法
    void setRootNode(std::shared_ptr<BehaviorNode> root);
    void loadTreeFromJson(const QJsonObject &treeConfig);
    NodeStatus tick(const PerceptionData &perception, const QJsonObject &currentState);

private:
    std::shared_ptr<BehaviorNode> m_rootNode;
    QString m_lastExplanation;
    AIAction m_pendingAction;
    
    void initializeDefaultTree();
    std::shared_ptr<BehaviorNode> createNodeFromConfig(const QJsonObject &nodeConfig);
};

// ========================================================================
// Q學習系統
// ========================================================================

/**
 * @brief Q學習代理
 */
class QLearningAgent : public IDecisionStrategy
{
public:
    explicit QLearningAgent();
    virtual ~QLearningAgent() = default;

    // IDecisionStrategy 實現
    AIAction makeDecision(const PerceptionData &perception,
                         const QJsonObject &currentState,
                         const std::vector<DecisionHistory> &history) override;
    
    void learn(const DecisionHistory &experience) override;
    void updateConfiguration(const QJsonObject &config) override;
    QString getStrategyName() const override { return "QLearning"; }
    QString getExplanation() const override { return m_lastExplanation; }

    // Q學習方法
    void updateQValue(const QString &state, const QString &action, float reward, const QString &nextState);
    float getQValue(const QString &state, const QString &action);
    void saveQTable(const QString &filePath);
    void loadQTable(const QString &filePath);

private:
    std::unordered_map<QString, std::unordered_map<QString, float>> m_qTable;
    float m_learningRate;
    float m_discountFactor;
    float m_explorationRate;
    QString m_lastExplanation;
    
    QString stateToString(const PerceptionData &perception, const QJsonObject &currentState);
    QString selectAction(const QString &state, const QStringList &availableActions);
    QStringList getAvailableActions(const PerceptionData &perception, const QJsonObject &currentState);
    float calculateReward(const DecisionHistory &experience);
};

// ========================================================================
// 分層決策系統
// ========================================================================

/**
 * @brief 分層決策規劃器
 */
class HierarchicalPlanner : public IDecisionStrategy
{
public:
    explicit HierarchicalPlanner();
    virtual ~HierarchicalPlanner() = default;

    // IDecisionStrategy 實現
    AIAction makeDecision(const PerceptionData &perception,
                         const QJsonObject &currentState,
                         const std::vector<DecisionHistory> &history) override;
    
    void learn(const DecisionHistory &experience) override;
    void updateConfiguration(const QJsonObject &config) override;
    QString getStrategyName() const override { return "Hierarchical"; }
    QString getExplanation() const override { return m_lastExplanation; }

    // 分層決策方法
    AIAction makeStrategicDecision(const PerceptionData &perception, const QJsonObject &currentState);
    AIAction makeTacticalDecision(const PerceptionData &perception, const QJsonObject &currentState);
    AIAction makeOperationalDecision(const PerceptionData &perception, const QJsonObject &currentState);

private:
    QString m_currentStrategicGoal;
    QString m_currentTacticalPlan;
    QStringList m_operationalTasks;
    QString m_lastExplanation;
    
    void updateStrategicGoal(const PerceptionData &perception);
    void planTacticalActions(const QString &strategicGoal, const PerceptionData &perception);
    void executeOperationalTask(const QString &tacticalPlan, const PerceptionData &perception);
};

// ========================================================================
// 環境感知器
// ========================================================================

/**
 * @brief 環境感知器
 */
class EnvironmentPerceptor
{
public:
    explicit EnvironmentPerceptor();
    virtual ~EnvironmentPerceptor() = default;

    // 感知方法
    PerceptionData perceive(const QJsonObject &worldState);
    void updateSensorData(const QJsonObject &sensorData);
    float predictThreatLevel(const PerceptionData &perception);
    QStringList identifyOpportunities(const PerceptionData &perception);

private:
    QJsonObject m_sensorConfiguration;
    std::vector<PerceptionData> m_perceptionHistory;
    
    void processSensorData(const QJsonObject &rawData, PerceptionData &perception);
    void analyzeThreats(PerceptionData &perception);
    void analyzeOpportunities(PerceptionData &perception);
    void updateSpatialAwareness(PerceptionData &perception);
};

// ========================================================================
// 配置管理器
// ========================================================================

/**
 * @brief 配置管理器 - 支援熱更新
 */
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);
    virtual ~ConfigManager() = default;

    // 配置方法
    void loadConfiguration(const QString &configPath);
    void saveConfiguration(const QString &configPath);
    void updateConfiguration(const QJsonObject &newConfig);
    QJsonObject getConfiguration() const;
    
    // 熱更新支援
    void enableHotUpdate(bool enabled);
    bool isHotUpdateEnabled() const;

signals:
    void configurationChanged(const QJsonObject &newConfig);

private slots:
    void onConfigFileChanged();

private:
    QJsonObject m_configuration;
    QString m_configPath;
    bool m_hotUpdateEnabled;
    QTimer *m_configWatcher;
    
    void setupFileWatcher();
    void validateConfiguration(const QJsonObject &config);
};

} // namespace JyAI
