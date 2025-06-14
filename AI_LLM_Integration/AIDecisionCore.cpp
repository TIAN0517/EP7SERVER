/**
 * @file AIDecisionCore.cpp
 * @brief AI決策核心模組實現 - 集成強化決策邏輯
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 */

#include "AIDecisionCore.h"
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <algorithm>
#include <random>

namespace JyAI {

// ========================================================================
// AIDecisionCore 實現
// ========================================================================

AIDecisionCore::AIDecisionCore(QObject *parent)
    : QObject(parent)
    , m_currentStrategy(DecisionStrategy::HYBRID)
    , m_utilitySystem(std::make_unique<UtilitySystem>())
    , m_behaviorTree(std::make_unique<BehaviorTree>())
    , m_qLearningAgent(std::make_unique<QLearningAgent>())
    , m_hierarchicalPlanner(std::make_unique<HierarchicalPlanner>())
    , m_environmentPerceptor(std::make_unique<EnvironmentPerceptor>())
    , m_configManager(std::make_unique<ConfigManager>(this))
    , m_performanceTimer(new QTimer(this))
    , m_hotUpdateEnabled(true)
{
    qDebug() << "[AIDecisionCore] 初始化AI決策核心 v4.0.0";
    
    initializeStrategies();
    initializePerformanceMonitoring();
    setupConfigurationWatcher();
    
    // 載入默認配置
    QString defaultConfigPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ai_config.json";
    if (QFile::exists(defaultConfigPath)) {
        loadConfiguration(defaultConfigPath);
    } else {
        // 創建默認配置
        QJsonObject defaultConfig;
        defaultConfig["strategy"] = "hybrid";
        defaultConfig["learning_enabled"] = true;
        defaultConfig["hot_update_enabled"] = true;
        defaultConfig["performance_monitoring"] = true;
        updateConfiguration(defaultConfig);
    }
    
    qDebug() << "[AIDecisionCore] 初始化完成，當前策略:" << static_cast<int>(m_currentStrategy);
}

AIDecisionCore::~AIDecisionCore()
{
    qDebug() << "[AIDecisionCore] 銷毀AI決策核心";
}

AIAction AIDecisionCore::decide(const PerceptionData &perception, 
                               const QJsonObject &currentState,
                               const std::vector<DecisionHistory> &history)
{
    auto startTime = QDateTime::currentMSecsSinceEpoch();
    
    // 更新感知資料
    m_currentPerception = perception;
    m_currentState = currentState;
    
    AIAction action;
    
    try {
        // 根據當前策略進行決策
        switch (m_currentStrategy) {
            case DecisionStrategy::UTILITY_BASED:
                action = makeUtilityBasedDecision(perception, currentState);
                break;
            case DecisionStrategy::BEHAVIOR_TREE:
                action = makeBehaviorTreeDecision(perception, currentState);
                break;
            case DecisionStrategy::Q_LEARNING:
                action = makeQLearningDecision(perception, currentState);
                break;
            case DecisionStrategy::HIERARCHICAL:
                action = makeHierarchicalDecision(perception, currentState);
                break;
            case DecisionStrategy::HYBRID:
                action = makeHybridDecision(perception, currentState);
                break;
            case DecisionStrategy::CUSTOM:
                if (m_activeStrategy) {
                    action = m_activeStrategy->makeDecision(perception, currentState, history);
                } else {
                    action = makeHybridDecision(perception, currentState);
                }
                break;
        }
        
        // 生成決策理由
        action.reasoning = generateReasoningText(action, perception);
        
        // 更新性能統計
        auto endTime = QDateTime::currentMSecsSinceEpoch();
        updatePerformanceStats(action, endTime - startTime);
        
        // 記錄決策
        logDecision(action, perception);
        
        emit decisionMade(action);
        
    } catch (const std::exception &e) {
        qWarning() << "[AIDecisionCore] 決策過程發生錯誤:" << e.what();
        emit errorOccurred(QString("決策錯誤: %1").arg(e.what()));
        
        // 返回安全的默認動作
        action.type = "idle";
        action.confidence = 0.1f;
        action.reasoning = "錯誤恢復：使用默認待機動作";
    }
    
    return action;
}

void AIDecisionCore::setDecisionStrategy(DecisionStrategy strategy)
{
    if (m_currentStrategy != strategy) {
        m_currentStrategy = strategy;
        qDebug() << "[AIDecisionCore] 切換決策策略至:" << static_cast<int>(strategy);
        emit strategyChanged(QString::number(static_cast<int>(strategy)));
    }
}

void AIDecisionCore::registerCustomStrategy(const QString &name, 
                                           std::shared_ptr<IDecisionStrategy> strategy)
{
    if (strategy) {
        m_strategies[name] = strategy;
        qDebug() << "[AIDecisionCore] 註冊自定義策略:" << name;
    }
}

void AIDecisionCore::switchStrategy(const QString &strategyName)
{
    auto it = m_strategies.find(strategyName);
    if (it != m_strategies.end()) {
        m_activeStrategy = it->second;
        m_currentStrategy = DecisionStrategy::CUSTOM;
        qDebug() << "[AIDecisionCore] 切換至自定義策略:" << strategyName;
        emit strategyChanged(strategyName);
    } else {
        qWarning() << "[AIDecisionCore] 找不到策略:" << strategyName;
    }
}

void AIDecisionCore::loadConfiguration(const QString &configPath)
{
    m_configPath = configPath;
    
    QFile file(configPath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull() && doc.isObject()) {
            updateConfiguration(doc.object());
            qDebug() << "[AIDecisionCore] 載入配置文件:" << configPath;
        } else {
            qWarning() << "[AIDecisionCore] 配置文件格式錯誤:" << configPath;
        }
    } else {
        qWarning() << "[AIDecisionCore] 無法載入配置文件:" << configPath;
    }
}

void AIDecisionCore::updateConfiguration(const QJsonObject &config)
{
    m_configuration = config;
    
    // 更新各子系統配置
    if (m_utilitySystem) {
        m_utilitySystem->updateConfiguration(config.value("utility_system").toObject());
    }
    if (m_behaviorTree) {
        m_behaviorTree->updateConfiguration(config.value("behavior_tree").toObject());
    }
    if (m_qLearningAgent) {
        m_qLearningAgent->updateConfiguration(config.value("q_learning").toObject());
    }
    if (m_hierarchicalPlanner) {
        m_hierarchicalPlanner->updateConfiguration(config.value("hierarchical_planner").toObject());
    }
    
    // 更新策略
    QString strategyName = config.value("strategy").toString("hybrid");
    if (strategyName == "utility") {
        setDecisionStrategy(DecisionStrategy::UTILITY_BASED);
    } else if (strategyName == "behavior_tree") {
        setDecisionStrategy(DecisionStrategy::BEHAVIOR_TREE);
    } else if (strategyName == "q_learning") {
        setDecisionStrategy(DecisionStrategy::Q_LEARNING);
    } else if (strategyName == "hierarchical") {
        setDecisionStrategy(DecisionStrategy::HIERARCHICAL);
    } else {
        setDecisionStrategy(DecisionStrategy::HYBRID);
    }
    
    // 更新熱更新設置
    m_hotUpdateEnabled = config.value("hot_update_enabled").toBool(true);
    
    emit configurationUpdated(config);
    qDebug() << "[AIDecisionCore] 配置已更新";
}

QJsonObject AIDecisionCore::getCurrentConfiguration() const
{
    return m_configuration;
}

void AIDecisionCore::learnFromExperience(const DecisionHistory &experience)
{
    // 將經驗傳遞給各學習系統
    if (m_qLearningAgent) {
        m_qLearningAgent->learn(experience);
    }
    if (m_utilitySystem) {
        m_utilitySystem->learn(experience);
    }
    if (m_behaviorTree) {
        m_behaviorTree->learn(experience);
    }
    if (m_hierarchicalPlanner) {
        m_hierarchicalPlanner->learn(experience);
    }
    
    // 記錄到歷史
    m_decisionHistory.push_back(experience);
    
    // 限制歷史記錄大小
    if (m_decisionHistory.size() > 10000) {
        m_decisionHistory.erase(m_decisionHistory.begin(), 
                               m_decisionHistory.begin() + 1000);
    }
}

void AIDecisionCore::batchLearn(const std::vector<DecisionHistory> &experiences)
{
    for (const auto &exp : experiences) {
        learnFromExperience(exp);
    }
    
    qDebug() << "[AIDecisionCore] 批量學習完成，處理" << experiences.size() << "個經驗";
    emit learningCompleted(QString("批量學習: %1個經驗").arg(experiences.size()));
}

bool AIDecisionCore::saveModel(const QString &modelPath)
{
    try {
        QJsonObject modelData;
        
        // 保存Q學習模型
        if (m_qLearningAgent) {
            QString qTablePath = modelPath + "_qtable.json";
            m_qLearningAgent->saveQTable(qTablePath);
            modelData["q_table_path"] = qTablePath;
        }
        
        // 保存歷史決策資料
        QJsonArray historyArray;
        for (const auto &history : m_decisionHistory) {
            QJsonObject historyObj;
            historyObj["action_type"] = history.actionType;
            historyObj["target_id"] = history.targetId;
            historyObj["skill_used"] = history.skillUsed;
            historyObj["utility_score"] = history.utilityScore;
            historyObj["outcome"] = history.outcome;
            historyObj["timestamp"] = static_cast<qint64>(history.timestamp);
            historyObj["situation"] = history.situation;
            historyObj["was_successful"] = history.wasSuccessful;
            historyArray.append(historyObj);
        }
        modelData["decision_history"] = historyArray;
        
        // 保存配置
        modelData["configuration"] = m_configuration;
        
        // 寫入文件
        QFile file(modelPath);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(modelData);
            file.write(doc.toJson());
            file.close();
            
            qDebug() << "[AIDecisionCore] 模型已保存至:" << modelPath;
            return true;
        }
    } catch (const std::exception &e) {
        qWarning() << "[AIDecisionCore] 保存模型失敗:" << e.what();
    }
    
    return false;
}

bool AIDecisionCore::loadModel(const QString &modelPath)
{
    try {
        QFile file(modelPath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (doc.isObject()) {
                QJsonObject modelData = doc.object();
                
                // 載入Q學習模型
                if (modelData.contains("q_table_path") && m_qLearningAgent) {
                    QString qTablePath = modelData["q_table_path"].toString();
                    m_qLearningAgent->loadQTable(qTablePath);
                }
                
                // 載入歷史決策資料
                if (modelData.contains("decision_history")) {
                    QJsonArray historyArray = modelData["decision_history"].toArray();
                    m_decisionHistory.clear();
                    
                    for (const auto &value : historyArray) {
                        QJsonObject historyObj = value.toObject();
                        DecisionHistory history;
                        history.actionType = historyObj["action_type"].toString();
                        history.targetId = historyObj["target_id"].toString();
                        history.skillUsed = historyObj["skill_used"].toString();
                        history.utilityScore = historyObj["utility_score"].toDouble();
                        history.outcome = historyObj["outcome"].toString();
                        history.timestamp = historyObj["timestamp"].toVariant().toLongLong();
                        history.situation = historyObj["situation"].toString();
                        history.wasSuccessful = historyObj["was_successful"].toBool();
                        m_decisionHistory.push_back(history);
                    }
                }
                
                // 載入配置
                if (modelData.contains("configuration")) {
                    updateConfiguration(modelData["configuration"].toObject());
                }
                
                qDebug() << "[AIDecisionCore] 模型已載入:" << modelPath;
                return true;
            }
        }
    } catch (const std::exception &e) {
        qWarning() << "[AIDecisionCore] 載入模型失敗:" << e.what();
    }
    
    return false;
}

QString AIDecisionCore::getDecisionExplanation() const
{
    return m_lastDecisionExplanation;
}

QJsonObject AIDecisionCore::getPerformanceStats() const
{
    return m_performanceStats;
}

void AIDecisionCore::resetStats()
{
    m_performanceStats = QJsonObject();
    m_decisionHistory.clear();
    qDebug() << "[AIDecisionCore] 統計資料已重置";
}

void AIDecisionCore::updatePerception(const PerceptionData &perception)
{
    m_currentPerception = perception;
    
    if (m_environmentPerceptor) {
        // 這裡可以添加額外的感知處理邏輯
    }
}

PerceptionData AIDecisionCore::predictEnvironmentChange(const PerceptionData &current, float deltaTime)
{
    PerceptionData predicted = current;
    
    if (m_environmentPerceptor) {
        // 基於歷史資料預測環境變化
        // 這是一個簡單的實現，實際可以更複雜
        
        // 預測血量變化
        if (current.healthRatio < 0.5f) {
            predicted.healthRatio = std::max(0.0f, current.healthRatio - deltaTime * 0.1f);
        }
        
        // 預測威脅變化
        if (current.enemyCount > current.allyCount) {
            predicted.currentThreat = "high";
        } else {
            predicted.currentThreat = "low";
        }
    }
    
    return predicted;
}

// ===== 私有方法實現 =====

void AIDecisionCore::initializeStrategies()
{
    // 註冊內建策略
    m_strategies["utility"] = m_utilitySystem;
    m_strategies["behavior_tree"] = m_behaviorTree;
    m_strategies["q_learning"] = m_qLearningAgent;
    m_strategies["hierarchical"] = m_hierarchicalPlanner;
    
    qDebug() << "[AIDecisionCore] 策略系統初始化完成";
}

void AIDecisionCore::initializePerformanceMonitoring()
{
    // 初始化性能統計
    m_performanceStats["total_decisions"] = 0;
    m_performanceStats["average_decision_time"] = 0.0;
    m_performanceStats["successful_decisions"] = 0;
    m_performanceStats["failed_decisions"] = 0;
    m_performanceStats["strategy_usage"] = QJsonObject();
    
    // 設置性能監控計時器
    m_performanceTimer->setInterval(60000); // 每分鐘報告一次
    connect(m_performanceTimer, &QTimer::timeout, this, &AIDecisionCore::onPerformanceTimer);
    m_performanceTimer->start();
    
    qDebug() << "[AIDecisionCore] 性能監控系統啟動";
}

void AIDecisionCore::setupConfigurationWatcher()
{
    if (m_configManager) {
        connect(m_configManager.get(), &ConfigManager::configurationChanged,
                this, &AIDecisionCore::onConfigurationChanged);
        m_configManager->enableHotUpdate(m_hotUpdateEnabled);
    }
}

AIAction AIDecisionCore::makeUtilityBasedDecision(const PerceptionData &perception, 
                                                 const QJsonObject &currentState)
{
    if (m_utilitySystem) {
        return m_utilitySystem->makeDecision(perception, currentState, m_decisionHistory);
    }
    
    AIAction fallbackAction;
    fallbackAction.type = "idle";
    fallbackAction.confidence = 0.1f;
    fallbackAction.reasoning = "效用系統不可用";
    return fallbackAction;
}

AIAction AIDecisionCore::makeBehaviorTreeDecision(const PerceptionData &perception, 
                                                 const QJsonObject &currentState)
{
    if (m_behaviorTree) {
        return m_behaviorTree->makeDecision(perception, currentState, m_decisionHistory);
    }
    
    AIAction fallbackAction;
    fallbackAction.type = "idle";
    fallbackAction.confidence = 0.1f;
    fallbackAction.reasoning = "行為樹系統不可用";
    return fallbackAction;
}

AIAction AIDecisionCore::makeQLearningDecision(const PerceptionData &perception, 
                                              const QJsonObject &currentState)
{
    if (m_qLearningAgent) {
        return m_qLearningAgent->makeDecision(perception, currentState, m_decisionHistory);
    }
    
    AIAction fallbackAction;
    fallbackAction.type = "idle";
    fallbackAction.confidence = 0.1f;
    fallbackAction.reasoning = "Q學習系統不可用";
    return fallbackAction;
}

AIAction AIDecisionCore::makeHierarchicalDecision(const PerceptionData &perception, 
                                                 const QJsonObject &currentState)
{
    if (m_hierarchicalPlanner) {
        return m_hierarchicalPlanner->makeDecision(perception, currentState, m_decisionHistory);
    }
    
    AIAction fallbackAction;
    fallbackAction.type = "idle";
    fallbackAction.confidence = 0.1f;
    fallbackAction.reasoning = "分層決策系統不可用";
    return fallbackAction;
}

AIAction AIDecisionCore::makeHybridDecision(const PerceptionData &perception, 
                                           const QJsonObject &currentState)
{
    // 混合策略：根據情況選擇最佳策略
    std::vector<AIAction> candidateActions;
    
    // 獲取各策略的決策
    if (m_utilitySystem) {
        auto utilityAction = m_utilitySystem->makeDecision(perception, currentState, m_decisionHistory);
        utilityAction.confidence *= 0.8f; // 調整權重
        candidateActions.push_back(utilityAction);
    }
    
    if (m_behaviorTree) {
        auto treeAction = m_behaviorTree->makeDecision(perception, currentState, m_decisionHistory);
        treeAction.confidence *= 0.9f; // 行為樹通常更可靠
        candidateActions.push_back(treeAction);
    }
    
    if (m_qLearningAgent) {
        auto qAction = m_qLearningAgent->makeDecision(perception, currentState, m_decisionHistory);
        qAction.confidence *= 0.7f; // Q學習需要更多訓練
        candidateActions.push_back(qAction);
    }
    
    if (m_hierarchicalPlanner) {
        auto hierarchicalAction = m_hierarchicalPlanner->makeDecision(perception, currentState, m_decisionHistory);
        hierarchicalAction.confidence *= 0.85f;
        candidateActions.push_back(hierarchicalAction);
    }
    
    // 選擇最高信心度的動作
    if (!candidateActions.empty()) {
        auto bestAction = *std::max_element(candidateActions.begin(), candidateActions.end(),
            [](const AIAction &a, const AIAction &b) {
                return a.confidence < b.confidence;
            });
        
        bestAction.reasoning = QString("混合決策：選擇信心度最高的動作 (%.2f)").arg(bestAction.confidence);
        return bestAction;
    }
    
    // 後備動作
    AIAction fallbackAction;
    fallbackAction.type = "idle";
    fallbackAction.confidence = 0.1f;
    fallbackAction.reasoning = "混合決策：所有子系統不可用";
    return fallbackAction;
}

void AIDecisionCore::updatePerformanceStats(const AIAction &action, float executionTime)
{
    int totalDecisions = m_performanceStats["total_decisions"].toInt() + 1;
    double avgTime = m_performanceStats["average_decision_time"].toDouble();
    
    // 更新平均決策時間
    avgTime = (avgTime * (totalDecisions - 1) + executionTime) / totalDecisions;
    
    m_performanceStats["total_decisions"] = totalDecisions;
    m_performanceStats["average_decision_time"] = avgTime;
    
    // 更新成功/失敗統計
    if (action.confidence > 0.5f) {
        int successful = m_performanceStats["successful_decisions"].toInt() + 1;
        m_performanceStats["successful_decisions"] = successful;
    } else {
        int failed = m_performanceStats["failed_decisions"].toInt() + 1;
        m_performanceStats["failed_decisions"] = failed;
    }
    
    // 更新策略使用統計
    QJsonObject strategyUsage = m_performanceStats["strategy_usage"].toObject();
    QString strategyName = QString::number(static_cast<int>(m_currentStrategy));
    int usage = strategyUsage[strategyName].toInt() + 1;
    strategyUsage[strategyName] = usage;
    m_performanceStats["strategy_usage"] = strategyUsage;
}

QString AIDecisionCore::generateReasoningText(const AIAction &action, const PerceptionData &perception)
{
    QString reasoning = QString("決策：%1").arg(action.type);
    
    if (!action.target.isEmpty()) {
        reasoning += QString(" -> 目標：%1").arg(action.target);
    }
    
    if (!action.skill.isEmpty()) {
        reasoning += QString(" -> 技能：%1").arg(action.skill);
    }
    
    reasoning += QString(" -> 信心度：%.2f").arg(action.confidence);
    
    // 添加環境分析
    if (perception.healthRatio < 0.3f) {
        reasoning += " -> 血量危險";
    }
    if (perception.enemyCount > perception.allyCount) {
        reasoning += " -> 敵眾我寡";
    }
    if (perception.currentThreat == "high") {
        reasoning += " -> 高威脅環境";
    }
    
    return reasoning;
}

void AIDecisionCore::logDecision(const AIAction &action, const PerceptionData &perception)
{
    DecisionHistory history;
    history.actionType = action.type;
    history.targetId = action.target;
    history.skillUsed = action.skill;
    history.utilityScore = action.expectedUtility;
    history.outcome = "pending";
    history.timestamp = QDateTime::currentMSecsSinceEpoch();
    history.situation = QString("HP:%.2f MP:%.2f 敵:%1 友:%2")
                          .arg(perception.healthRatio)
                          .arg(perception.manaRatio)
                          .arg(perception.enemyCount)
                          .arg(perception.allyCount);
    history.wasSuccessful = action.confidence > 0.5f;
    
    m_decisionHistory.push_back(history);
    
    // 更新解釋文本
    m_lastDecisionExplanation = action.reasoning;
}

void AIDecisionCore::onConfigurationChanged()
{
    if (m_configManager && m_hotUpdateEnabled) {
        QJsonObject newConfig = m_configManager->getConfiguration();
        updateConfiguration(newConfig);
        qDebug() << "[AIDecisionCore] 配置熱更新完成";
    }
}

void AIDecisionCore::onPerformanceTimer()
{
    emit performanceReport(m_performanceStats);
    
    // 可以在這裡添加自動優化邏輯
    double successRate = 0.0;
    int total = m_performanceStats["total_decisions"].toInt();
    int successful = m_performanceStats["successful_decisions"].toInt();
    
    if (total > 0) {
        successRate = static_cast<double>(successful) / total;
    }
    
    qDebug() << "[AIDecisionCore] 性能報告 - 總決策:" << total 
             << "成功率:" << QString::number(successRate * 100, 'f', 1) << "%"
             << "平均時間:" << m_performanceStats["average_decision_time"].toDouble() << "ms";
}

// ========================================================================
// UtilitySystem 實現
// ========================================================================

UtilitySystem::UtilitySystem()
{
    initializeDefaultUtilityFunctions();
    
    // 初始化權重
    m_utilityWeights["combat"] = 0.4;
    m_utilityWeights["survival"] = 0.4;
    m_utilityWeights["support"] = 0.2;
}

AIAction UtilitySystem::makeDecision(const PerceptionData &perception,
                                   const QJsonObject &currentState,
                                   const std::vector<DecisionHistory> &history)
{
    QStringList availableActions = {"attack", "defend", "heal", "buff", "move", "cast_skill"};
    
    AIAction bestAction;
    float bestUtility = -1.0f;
    
    for (const QString &actionType : availableActions) {
        float utility = calculateActionUtility(actionType, perception, currentState);
        
        if (utility > bestUtility) {
            bestUtility = utility;
            bestAction.type = actionType;
            bestAction.confidence = utility;
            bestAction.expectedUtility = utility;
        }
    }
    
    // 生成解釋
    m_lastExplanation = QString("效用函數決策：選擇%1，效用值%.3f")
                          .arg(bestAction.type).arg(bestUtility);
    
    bestAction.reasoning = m_lastExplanation;
    
    return bestAction;
}

void UtilitySystem::learn(const DecisionHistory &experience)
{
    // 基於經驗調整權重
    if (experience.wasSuccessful) {
        // 成功的決策增加對應動作類型的權重
        if (experience.actionType.contains("attack")) {
            m_utilityWeights["combat"] = std::min(1.0, m_utilityWeights["combat"].toDouble() + 0.01);
        } else if (experience.actionType.contains("heal") || experience.actionType.contains("defend")) {
            m_utilityWeights["survival"] = std::min(1.0, m_utilityWeights["survival"].toDouble() + 0.01);
        }
    } else {
        // 失敗的決策減少權重
        if (experience.actionType.contains("attack")) {
            m_utilityWeights["combat"] = std::max(0.1, m_utilityWeights["combat"].toDouble() - 0.005);
        }
    }
}

void UtilitySystem::updateConfiguration(const QJsonObject &config)
{
    if (config.contains("weights")) {
        QJsonObject weights = config["weights"].toObject();
        for (auto it = weights.begin(); it != weights.end(); ++it) {
            m_utilityWeights[it.key()] = it.value().toDouble();
        }
    }
}

float UtilitySystem::calculateActionUtility(const QString &actionType, 
                                           const PerceptionData &perception,
                                           const QJsonObject &currentState)
{
    float utility = 0.0f;
    
    if (actionType == "attack") {
        utility += calculateCombatUtility(perception, currentState);
        utility *= m_utilityWeights["combat"].toDouble();
    } else if (actionType == "heal" || actionType == "defend") {
        utility += calculateSurvivalUtility(perception, currentState);
        utility *= m_utilityWeights["survival"].toDouble();
    } else if (actionType == "buff") {
        utility += calculateSupportUtility(perception, currentState);
        utility *= m_utilityWeights["support"].toDouble();
    }
    
    return std::clamp(utility, 0.0f, 1.0f);
}

void UtilitySystem::initializeDefaultUtilityFunctions()
{
    // 這裡可以註冊更多自定義效用函數
    qDebug() << "[UtilitySystem] 初始化默認效用函數";
}

float UtilitySystem::calculateCombatUtility(const PerceptionData &perception, 
                                           const QJsonObject &currentState)
{
    float utility = 0.0f;
    
    // 基於敵人數量
    if (perception.enemyCount > 0) {
        utility += 0.3f;
    }
    
    // 基於血量
    if (perception.healthRatio > 0.7f) {
        utility += 0.4f;
    } else if (perception.healthRatio < 0.3f) {
        utility -= 0.3f; // 血量低時降低攻擊慾望
    }
    
    // 基於威脅等級
    if (perception.currentThreat == "high") {
        utility += 0.3f;
    }
    
    return utility;
}

float UtilitySystem::calculateSurvivalUtility(const PerceptionData &perception, 
                                             const QJsonObject &currentState)
{
    float utility = 0.0f;
    
    // 血量越低，生存慾望越高
    utility += (1.0f - perception.healthRatio) * 0.8f;
    
    // 敵人多時增加防禦慾望
    if (perception.enemyCount > perception.allyCount) {
        utility += 0.3f;
    }
    
    // 高威脅環境
    if (perception.currentThreat == "high") {
        utility += 0.4f;
    }
    
    return utility;
}

float UtilitySystem::calculateSupportUtility(const PerceptionData &perception, 
                                            const QJsonObject &currentState)
{
    float utility = 0.0f;
    
    // 有盟友時增加支援慾望
    if (perception.allyCount > 0) {
        utility += 0.4f;
    }
    
    // 法力充足時
    if (perception.manaRatio > 0.5f) {
        utility += 0.3f;
    }
    
    // 團隊戰時
    if (perception.allyCount > 1 && perception.enemyCount > 1) {
        utility += 0.3f;
    }
    
    return utility;
}

} // namespace JyAI
