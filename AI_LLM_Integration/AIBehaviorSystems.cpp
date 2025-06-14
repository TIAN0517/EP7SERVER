/**
 * @file AIBehaviorSystems.cpp
 * @brief AI行為系統實現 - 行為樹、Q學習、分層決策
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 */

#include "AIDecisionCore.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <algorithm>
#include <random>

namespace JyAI {

// ========================================================================
// 行為樹節點實現
// ========================================================================

/**
 * @brief 行動節點 - 執行具體行動
 */
class ActionNode : public BehaviorNode
{
public:
    ActionNode(const QString &actionType, const QJsonObject &parameters)
        : m_actionType(actionType), m_parameters(parameters) {}
    
    NodeStatus execute(const PerceptionData &perception, const QJsonObject &currentState) override
    {
        // 檢查行動是否可執行
        if (canExecuteAction(perception, currentState)) {
            // 執行行動
            executeAction(perception, currentState);
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }
    
    void reset() override {
        // 重置節點狀態
    }
    
    BehaviorNodeType getType() const override {
        return BehaviorNodeType::ACTION;
    }

private:
    QString m_actionType;
    QJsonObject m_parameters;
    
    bool canExecuteAction(const PerceptionData &perception, const QJsonObject &currentState)
    {
        if (m_actionType == "attack") {
            return perception.enemyCount > 0 && perception.healthRatio > 0.2f;
        } else if (m_actionType == "heal") {
            return perception.healthRatio < 0.5f && perception.manaRatio > 0.3f;
        } else if (m_actionType == "defend") {
            return perception.currentThreat == "high";
        }
        return true; // 默認可執行
    }
    
    void executeAction(const PerceptionData &perception, const QJsonObject &currentState)
    {
        qDebug() << "[ActionNode] 執行行動:" << m_actionType;
    }
};

/**
 * @brief 條件節點 - 檢查條件
 */
class ConditionNode : public BehaviorNode
{
public:
    ConditionNode(const QString &conditionType, const QJsonObject &parameters)
        : m_conditionType(conditionType), m_parameters(parameters) {}
    
    NodeStatus execute(const PerceptionData &perception, const QJsonObject &currentState) override
    {
        if (checkCondition(perception, currentState)) {
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }
    
    void reset() override {
        // 條件節點通常不需要重置
    }
    
    BehaviorNodeType getType() const override {
        return BehaviorNodeType::CONDITION;
    }

private:
    QString m_conditionType;
    QJsonObject m_parameters;
    
    bool checkCondition(const PerceptionData &perception, const QJsonObject &currentState)
    {
        if (m_conditionType == "low_health") {
            float threshold = m_parameters.value("threshold").toDouble(0.3);
            return perception.healthRatio < threshold;
        } else if (m_conditionType == "enemy_nearby") {
            return perception.enemyCount > 0;
        } else if (m_conditionType == "high_threat") {
            return perception.currentThreat == "high";
        } else if (m_conditionType == "sufficient_mana") {
            float threshold = m_parameters.value("threshold").toDouble(0.5);
            return perception.manaRatio > threshold;
        }
        return false;
    }
};

/**
 * @brief 序列節點 - 按順序執行子節點
 */
class SequenceNode : public BehaviorNode
{
public:
    SequenceNode() : m_currentChild(0) {}
    
    NodeStatus execute(const PerceptionData &perception, const QJsonObject &currentState) override
    {
        while (m_currentChild < m_children.size()) {
            NodeStatus status = m_children[m_currentChild]->execute(perception, currentState);
            
            if (status == NodeStatus::FAILURE) {
                reset();
                return NodeStatus::FAILURE;
            } else if (status == NodeStatus::RUNNING) {
                return NodeStatus::RUNNING;
            }
            
            m_currentChild++;
        }
        
        reset();
        return NodeStatus::SUCCESS;
    }
    
    void reset() override {
        m_currentChild = 0;
        for (auto &child : m_children) {
            child->reset();
        }
    }
    
    BehaviorNodeType getType() const override {
        return BehaviorNodeType::SEQUENCE;
    }

private:
    size_t m_currentChild;
};

/**
 * @brief 選擇器節點 - 選擇第一個成功的子節點
 */
class SelectorNode : public BehaviorNode
{
public:
    SelectorNode() : m_currentChild(0) {}
    
    NodeStatus execute(const PerceptionData &perception, const QJsonObject &currentState) override
    {
        while (m_currentChild < m_children.size()) {
            NodeStatus status = m_children[m_currentChild]->execute(perception, currentState);
            
            if (status == NodeStatus::SUCCESS) {
                reset();
                return NodeStatus::SUCCESS;
            } else if (status == NodeStatus::RUNNING) {
                return NodeStatus::RUNNING;
            }
            
            m_currentChild++;
        }
        
        reset();
        return NodeStatus::FAILURE;
    }
    
    void reset() override {
        m_currentChild = 0;
        for (auto &child : m_children) {
            child->reset();
        }
    }
    
    BehaviorNodeType getType() const override {
        return BehaviorNodeType::SELECTOR;
    }

private:
    size_t m_currentChild;
};

// ========================================================================
// BehaviorTree 實現
// ========================================================================

BehaviorTree::BehaviorTree()
{
    initializeDefaultTree();
}

AIAction BehaviorTree::makeDecision(const PerceptionData &perception,
                                   const QJsonObject &currentState,
                                   const std::vector<DecisionHistory> &history)
{
    if (!m_rootNode) {
        initializeDefaultTree();
    }
    
    // 執行行為樹
    NodeStatus status = tick(perception, currentState);
    
    // 根據執行結果生成動作
    AIAction action;
    
    if (status == NodeStatus::SUCCESS && !m_pendingAction.type.isEmpty()) {
        action = m_pendingAction;
        action.confidence = 0.8f;
    } else {
        // 默認動作
        action.type = "idle";
        action.confidence = 0.3f;
        action.reasoning = "行為樹執行失敗，使用默認動作";
    }
    
    m_lastExplanation = QString("行為樹決策：%1，狀態：%2")
                          .arg(action.type)
                          .arg(status == NodeStatus::SUCCESS ? "成功" : 
                               status == NodeStatus::FAILURE ? "失敗" : "執行中");
    
    return action;
}

void BehaviorTree::learn(const DecisionHistory &experience)
{
    // 行為樹的學習可以通過調整節點權重或結構來實現
    // 這裡實現一個簡單的學習機制
    
    if (experience.wasSuccessful) {
        qDebug() << "[BehaviorTree] 學習成功經驗:" << experience.actionType;
        // 可以增加成功行動對應節點的優先級
    } else {
        qDebug() << "[BehaviorTree] 學習失敗經驗:" << experience.actionType;
        // 可以降低失敗行動對應節點的優先級
    }
}

void BehaviorTree::updateConfiguration(const QJsonObject &config)
{
    if (config.contains("tree_structure")) {
        loadTreeFromJson(config["tree_structure"].toObject());
    }
}

void BehaviorTree::setRootNode(std::shared_ptr<BehaviorNode> root)
{
    m_rootNode = root;
}

void BehaviorTree::loadTreeFromJson(const QJsonObject &treeConfig)
{
    // 從JSON配置載入行為樹結構
    if (treeConfig.contains("root")) {
        m_rootNode = createNodeFromConfig(treeConfig["root"].toObject());
    }
}

NodeStatus BehaviorTree::tick(const PerceptionData &perception, const QJsonObject &currentState)
{
    if (m_rootNode) {
        return m_rootNode->execute(perception, currentState);
    }
    return NodeStatus::FAILURE;
}

void BehaviorTree::initializeDefaultTree()
{
    // 創建默認行為樹
    // 根節點是選擇器
    auto root = std::make_shared<SelectorNode>();
    
    // 生存序列
    auto survivalSequence = std::make_shared<SequenceNode>();
    survivalSequence->addChild(std::make_shared<ConditionNode>("low_health", QJsonObject{{"threshold", 0.3}}));
    survivalSequence->addChild(std::make_shared<ActionNode>("heal", QJsonObject()));
    
    // 戰鬥序列
    auto combatSequence = std::make_shared<SequenceNode>();
    combatSequence->addChild(std::make_shared<ConditionNode>("enemy_nearby", QJsonObject()));
    combatSequence->addChild(std::make_shared<ConditionNode>("sufficient_mana", QJsonObject{{"threshold", 0.3}}));
    combatSequence->addChild(std::make_shared<ActionNode>("attack", QJsonObject()));
    
    // 防禦序列
    auto defenseSequence = std::make_shared<SequenceNode>();
    defenseSequence->addChild(std::make_shared<ConditionNode>("high_threat", QJsonObject()));
    defenseSequence->addChild(std::make_shared<ActionNode>("defend", QJsonObject()));
    
    // 默認動作
    auto idleAction = std::make_shared<ActionNode>("idle", QJsonObject());
    
    // 組裝樹結構
    root->addChild(survivalSequence);
    root->addChild(combatSequence);
    root->addChild(defenseSequence);
    root->addChild(idleAction);
    
    m_rootNode = root;
    
    qDebug() << "[BehaviorTree] 初始化默認行為樹完成";
}

std::shared_ptr<BehaviorNode> BehaviorTree::createNodeFromConfig(const QJsonObject &nodeConfig)
{
    QString nodeType = nodeConfig["type"].toString();
    QJsonObject parameters = nodeConfig["parameters"].toObject();
    
    if (nodeType == "action") {
        QString actionType = nodeConfig["action_type"].toString();
        return std::make_shared<ActionNode>(actionType, parameters);
    } else if (nodeType == "condition") {
        QString conditionType = nodeConfig["condition_type"].toString();
        return std::make_shared<ConditionNode>(conditionType, parameters);
    } else if (nodeType == "sequence") {
        auto sequence = std::make_shared<SequenceNode>();
        QJsonArray children = nodeConfig["children"].toArray();
        for (const auto &child : children) {
            sequence->addChild(createNodeFromConfig(child.toObject()));
        }
        return sequence;
    } else if (nodeType == "selector") {
        auto selector = std::make_shared<SelectorNode>();
        QJsonArray children = nodeConfig["children"].toArray();
        for (const auto &child : children) {
            selector->addChild(createNodeFromConfig(child.toObject()));
        }
        return selector;
    }
    
    return nullptr;
}

// ========================================================================
// QLearningAgent 實現
// ========================================================================

QLearningAgent::QLearningAgent()
    : m_learningRate(0.1f)
    , m_discountFactor(0.9f)
    , m_explorationRate(0.1f)
{
    qDebug() << "[QLearningAgent] 初始化Q學習代理";
}

AIAction QLearningAgent::makeDecision(const PerceptionData &perception,
                                     const QJsonObject &currentState,
                                     const std::vector<DecisionHistory> &history)
{
    QString state = stateToString(perception, currentState);
    QStringList availableActions = getAvailableActions(perception, currentState);
    
    QString selectedAction = selectAction(state, availableActions);
    
    AIAction action;
    action.type = selectedAction;
    action.confidence = 1.0f - m_explorationRate; // 基於探索率調整信心度
    
    float qValue = getQValue(state, selectedAction);
    action.expectedUtility = qValue;
    
    m_lastExplanation = QString("Q學習決策：狀態=%1，動作=%2，Q值=%.3f")
                          .arg(state).arg(selectedAction).arg(qValue);
    
    return action;
}

void QLearningAgent::learn(const DecisionHistory &experience)
{
    // 根據經驗更新Q值
    QString state = QString("%1_%2_%3")
                      .arg(experience.situation)
                      .arg(experience.actionType)
                      .arg(experience.targetId);
    
    float reward = calculateReward(experience);
    
    // 簡化的Q學習更新（這裡沒有下一個狀態，所以直接更新）
    float oldQValue = getQValue(state, experience.actionType);
    float newQValue = oldQValue + m_learningRate * (reward - oldQValue);
    
    updateQValue(state, experience.actionType, newQValue, "");
    
    // 逐步降低探索率
    m_explorationRate = std::max(0.01f, m_explorationRate * 0.995f);
    
    qDebug() << "[QLearningAgent] 學習更新 - 狀態:" << state 
             << "動作:" << experience.actionType 
             << "獎勵:" << reward 
             << "新Q值:" << newQValue;
}

void QLearningAgent::updateConfiguration(const QJsonObject &config)
{
    if (config.contains("learning_rate")) {
        m_learningRate = config["learning_rate"].toDouble(0.1);
    }
    if (config.contains("discount_factor")) {
        m_discountFactor = config["discount_factor"].toDouble(0.9);
    }
    if (config.contains("exploration_rate")) {
        m_explorationRate = config["exploration_rate"].toDouble(0.1);
    }
}

void QLearningAgent::updateQValue(const QString &state, const QString &action, 
                                 float reward, const QString &nextState)
{
    if (m_qTable.find(state) == m_qTable.end()) {
        m_qTable[state] = std::unordered_map<QString, float>();
    }
    
    float oldValue = getQValue(state, action);
    float nextMaxQ = 0.0f;
    
    if (!nextState.isEmpty() && m_qTable.find(nextState) != m_qTable.end()) {
        auto &nextStateActions = m_qTable[nextState];
        for (const auto &pair : nextStateActions) {
            nextMaxQ = std::max(nextMaxQ, pair.second);
        }
    }
    
    float newValue = oldValue + m_learningRate * (reward + m_discountFactor * nextMaxQ - oldValue);
    m_qTable[state][action] = newValue;
}

float QLearningAgent::getQValue(const QString &state, const QString &action)
{
    if (m_qTable.find(state) != m_qTable.end() && 
        m_qTable[state].find(action) != m_qTable[state].end()) {
        return m_qTable[state][action];
    }
    return 0.0f; // 默認Q值
}

void QLearningAgent::saveQTable(const QString &filePath)
{
    QJsonObject qTableJson;
    
    for (const auto &statePair : m_qTable) {
        QJsonObject stateActions;
        for (const auto &actionPair : statePair.second) {
            stateActions[actionPair.first] = actionPair.second;
        }
        qTableJson[statePair.first] = stateActions;
    }
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(qTableJson);
        file.write(doc.toJson());
        file.close();
        qDebug() << "[QLearningAgent] Q表已保存至:" << filePath;
    }
}

void QLearningAgent::loadQTable(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (doc.isObject()) {
            QJsonObject qTableJson = doc.object();
            m_qTable.clear();
            
            for (auto stateIt = qTableJson.begin(); stateIt != qTableJson.end(); ++stateIt) {
                QJsonObject stateActions = stateIt.value().toObject();
                std::unordered_map<QString, float> actionMap;
                
                for (auto actionIt = stateActions.begin(); actionIt != stateActions.end(); ++actionIt) {
                    actionMap[actionIt.key()] = actionIt.value().toDouble();
                }
                
                m_qTable[stateIt.key()] = actionMap;
            }
            
            qDebug() << "[QLearningAgent] Q表已載入:" << filePath;
        }
    }
}

QString QLearningAgent::stateToString(const PerceptionData &perception, const QJsonObject &currentState)
{
    // 將感知狀態轉換為字符串表示
    QString healthLevel = perception.healthRatio > 0.7f ? "high" : 
                         perception.healthRatio > 0.3f ? "medium" : "low";
    QString manaLevel = perception.manaRatio > 0.7f ? "high" : 
                       perception.manaRatio > 0.3f ? "medium" : "low";
    QString enemyPresence = perception.enemyCount > 0 ? "present" : "absent";
    QString threatLevel = perception.currentThreat;
    
    return QString("%1_%2_%3_%4").arg(healthLevel).arg(manaLevel).arg(enemyPresence).arg(threatLevel);
}

QString QLearningAgent::selectAction(const QString &state, const QStringList &availableActions)
{
    // ε-貪婪策略
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    
    if (dis(gen) < m_explorationRate || availableActions.isEmpty()) {
        // 探索：隨機選擇
        if (!availableActions.isEmpty()) {
            std::uniform_int_distribution<> actionDis(0, availableActions.size() - 1);
            return availableActions[actionDis(gen)];
        }
        return "idle";
    } else {
        // 利用：選擇Q值最高的動作
        QString bestAction = "idle";
        float bestQValue = -1000.0f;
        
        for (const QString &action : availableActions) {
            float qValue = getQValue(state, action);
            if (qValue > bestQValue) {
                bestQValue = qValue;
                bestAction = action;
            }
        }
        
        return bestAction;
    }
}

QStringList QLearningAgent::getAvailableActions(const PerceptionData &perception, const QJsonObject &currentState)
{
    QStringList actions;
    
    // 基本動作
    actions << "idle" << "move";
    
    // 戰鬥動作
    if (perception.enemyCount > 0) {
        actions << "attack" << "defend";
        if (perception.manaRatio > 0.3f) {
            actions << "cast_skill";
        }
    }
    
    // 治療動作
    if (perception.healthRatio < 0.8f && perception.manaRatio > 0.2f) {
        actions << "heal";
    }
    
    // 支援動作
    if (perception.allyCount > 0 && perception.manaRatio > 0.4f) {
        actions << "buff" << "support";
    }
    
    return actions;
}

float QLearningAgent::calculateReward(const DecisionHistory &experience)
{
    float reward = 0.0f;
    
    if (experience.wasSuccessful) {
        reward += 1.0f; // 基礎成功獎勵
        
        // 根據動作類型給予額外獎勵
        if (experience.actionType == "attack" && experience.outcome.contains("enemy_defeated")) {
            reward += 2.0f;
        } else if (experience.actionType == "heal" && experience.outcome.contains("health_recovered")) {
            reward += 1.5f;
        } else if (experience.actionType == "defend" && experience.outcome.contains("damage_blocked")) {
            reward += 1.2f;
        }
    } else {
        reward -= 0.5f; // 失敗懲罰
        
        // 特別懲罰危險行為
        if (experience.actionType == "attack" && experience.situation.contains("HP:0.")) {
            reward -= 1.0f; // 低血量時攻擊的額外懲罰
        }
    }
    
    return reward;
}

// ========================================================================
// HierarchicalPlanner 實現
// ========================================================================

HierarchicalPlanner::HierarchicalPlanner()
    : m_currentStrategicGoal("survival")
    , m_currentTacticalPlan("defensive")
{
    qDebug() << "[HierarchicalPlanner] 初始化分層決策規劃器";
}

AIAction HierarchicalPlanner::makeDecision(const PerceptionData &perception,
                                          const QJsonObject &currentState,
                                          const std::vector<DecisionHistory> &history)
{
    // 分層決策：戰略 -> 戰術 -> 操作
    
    // 1. 戰略層決策
    updateStrategicGoal(perception);
    
    // 2. 戰術層決策
    planTacticalActions(m_currentStrategicGoal, perception);
    
    // 3. 操作層決策
    AIAction action;
    
    if (m_currentStrategicGoal == "survival") {
        action = makeStrategicDecision(perception, currentState);
    } else if (m_currentStrategicGoal == "combat") {
        action = makeTacticalDecision(perception, currentState);
    } else {
        action = makeOperationalDecision(perception, currentState);
    }
    
    m_lastExplanation = QString("分層決策：戰略=%1，戰術=%2，動作=%3")
                          .arg(m_currentStrategicGoal)
                          .arg(m_currentTacticalPlan)
                          .arg(action.type);
    
    return action;
}

void HierarchicalPlanner::learn(const DecisionHistory &experience)
{
    // 分層學習：根據結果調整各層決策
    if (experience.wasSuccessful) {
        qDebug() << "[HierarchicalPlanner] 學習成功經驗 - 戰略:" << m_currentStrategicGoal 
                 << "戰術:" << m_currentTacticalPlan;
        // 可以增強當前戰略和戰術的權重
    } else {
        qDebug() << "[HierarchicalPlanner] 學習失敗經驗 - 需要調整策略";
        // 可以考慮切換戰略或戰術
    }
}

void HierarchicalPlanner::updateConfiguration(const QJsonObject &config)
{
    if (config.contains("default_strategic_goal")) {
        m_currentStrategicGoal = config["default_strategic_goal"].toString();
    }
    if (config.contains("default_tactical_plan")) {
        m_currentTacticalPlan = config["default_tactical_plan"].toString();
    }
}

AIAction HierarchicalPlanner::makeStrategicDecision(const PerceptionData &perception, const QJsonObject &currentState)
{
    AIAction action;
    
    if (perception.healthRatio < 0.3f) {
        action.type = "heal";
        action.confidence = 0.9f;
        action.reasoning = "戰略決策：生存優先，血量危險";
    } else if (perception.currentThreat == "high" && perception.enemyCount > perception.allyCount) {
        action.type = "retreat";
        action.confidence = 0.8f;
        action.reasoning = "戰略決策：敵眾我寡，戰術撤退";
    } else {
        action.type = "advance";
        action.confidence = 0.7f;
        action.reasoning = "戰略決策：條件允許，主動進攻";
    }
    
    return action;
}

AIAction HierarchicalPlanner::makeTacticalDecision(const PerceptionData &perception, const QJsonObject &currentState)
{
    AIAction action;
    
    if (m_currentTacticalPlan == "aggressive") {
        action.type = "attack";
        action.confidence = 0.8f;
        action.reasoning = "戰術決策：攻擊性戰術";
    } else if (m_currentTacticalPlan == "defensive") {
        action.type = "defend";
        action.confidence = 0.7f;
        action.reasoning = "戰術決策：防禦性戰術";
    } else if (m_currentTacticalPlan == "supportive") {
        action.type = "buff";
        action.confidence = 0.6f;
        action.reasoning = "戰術決策：支援性戰術";
    } else {
        action.type = "move";
        action.confidence = 0.5f;
        action.reasoning = "戰術決策：機動戰術";
    }
    
    return action;
}

AIAction HierarchicalPlanner::makeOperationalDecision(const PerceptionData &perception, const QJsonObject &currentState)
{
    AIAction action;
    
    // 操作層決策：具體的行動選擇
    if (!m_operationalTasks.isEmpty()) {
        QString task = m_operationalTasks.first();
        m_operationalTasks.removeFirst();
        
        action.type = task;
        action.confidence = 0.6f;
        action.reasoning = "操作決策：執行預定任務";
    } else {
        action.type = "idle";
        action.confidence = 0.3f;
        action.reasoning = "操作決策：無任務，待機";
    }
    
    return action;
}

void HierarchicalPlanner::updateStrategicGoal(const PerceptionData &perception)
{
    QString newGoal = m_currentStrategicGoal;
    
    if (perception.healthRatio < 0.3f) {
        newGoal = "survival";
    } else if (perception.enemyCount > 0 && perception.healthRatio > 0.6f) {
        newGoal = "combat";
    } else if (perception.allyCount > 0 && perception.manaRatio > 0.7f) {
        newGoal = "support";
    } else {
        newGoal = "exploration";
    }
    
    if (newGoal != m_currentStrategicGoal) {
        m_currentStrategicGoal = newGoal;
        qDebug() << "[HierarchicalPlanner] 戰略目標更新為:" << newGoal;
    }
}

void HierarchicalPlanner::planTacticalActions(const QString &strategicGoal, const PerceptionData &perception)
{
    QString newPlan = m_currentTacticalPlan;
    
    if (strategicGoal == "survival") {
        newPlan = "defensive";
        m_operationalTasks = QStringList{"heal", "defend", "retreat"};
    } else if (strategicGoal == "combat") {
        if (perception.enemyCount > perception.allyCount) {
            newPlan = "guerrilla";
            m_operationalTasks = QStringList{"hit_and_run", "kite", "escape"};
        } else {
            newPlan = "aggressive";
            m_operationalTasks = QStringList{"attack", "cast_skill", "finish"};
        }
    } else if (strategicGoal == "support") {
        newPlan = "supportive";
        m_operationalTasks = QStringList{"buff", "heal_ally", "coordinate"};
    } else {
        newPlan = "exploratory";
        m_operationalTasks = QStringList{"scout", "move", "observe"};
    }
    
    if (newPlan != m_currentTacticalPlan) {
        m_currentTacticalPlan = newPlan;
        qDebug() << "[HierarchicalPlanner] 戰術計劃更新為:" << newPlan;
    }
}

// ========================================================================
// EnvironmentPerceptor 實現
// ========================================================================

EnvironmentPerceptor::EnvironmentPerceptor()
{
    // 初始化感知器配置
    m_sensorConfiguration["vision_range"] = 10.0;
    m_sensorConfiguration["threat_sensitivity"] = 0.8;
    m_sensorConfiguration["ally_detection"] = true;
    m_sensorConfiguration["environmental_awareness"] = true;
    
    qDebug() << "[EnvironmentPerceptor] 初始化環境感知器";
}

PerceptionData EnvironmentPerceptor::perceive(const QJsonObject &worldState)
{
    PerceptionData perception;
    
    // 處理感知器數據
    processSensorData(worldState, perception);
    
    // 分析威脅
    analyzeThreats(perception);
    
    // 分析機會
    analyzeOpportunities(perception);
    
    // 更新空間感知
    updateSpatialAwareness(perception);
    
    // 記錄感知歷史
    m_perceptionHistory.push_back(perception);
    if (m_perceptionHistory.size() > 100) {
        m_perceptionHistory.erase(m_perceptionHistory.begin());
    }
    
    return perception;
}

void EnvironmentPerceptor::updateSensorData(const QJsonObject &sensorData)
{
    // 更新感知器配置
    for (auto it = sensorData.begin(); it != sensorData.end(); ++it) {
        m_sensorConfiguration[it.key()] = it.value();
    }
}

float EnvironmentPerceptor::predictThreatLevel(const PerceptionData &perception)
{
    float threatLevel = 0.0f;
    
    // 基於敵人數量
    threatLevel += perception.enemyCount * 0.3f;
    
    // 基於血量狀態
    threatLevel += (1.0f - perception.healthRatio) * 0.4f;
    
    // 基於環境效果
    for (const QString &effect : perception.environmentEffects) {
        if (effect.contains("poison") || effect.contains("fire")) {
            threatLevel += 0.2f;
        }
    }
    
    return std::clamp(threatLevel, 0.0f, 1.0f);
}

QStringList EnvironmentPerceptor::identifyOpportunities(const PerceptionData &perception)
{
    QStringList opportunities;
    
    if (perception.enemyCount == 1 && perception.allyCount > 1) {
        opportunities << "gang_up_opportunity";
    }
    
    if (perception.manaRatio > 0.8f && perception.healthRatio > 0.8f) {
        opportunities << "full_power_available";
    }
    
    if (perception.currentThreat == "low" && perception.enemyCount == 0) {
        opportunities << "safe_exploration";
    }
    
    return opportunities;
}

void EnvironmentPerceptor::processSensorData(const QJsonObject &rawData, PerceptionData &perception)
{
    // 處理基本狀態資料
    perception.healthRatio = rawData["health_ratio"].toDouble(1.0);
    perception.manaRatio = rawData["mana_ratio"].toDouble(1.0);
    
    // 處理實體資料
    QJsonArray enemies = rawData["enemies"].toArray();
    QJsonArray allies = rawData["allies"].toArray();
    
    perception.enemyCount = enemies.size();
    perception.allyCount = allies.size();
    perception.visibleEnemies = enemies;
    perception.visibleAllies = allies;
    
    // 處理環境資料
    perception.weather = rawData["weather"].toString("clear");
    perception.timeOfDay = rawData["time_of_day"].toString("day");
    perception.visibility = rawData["visibility"].toDouble(1.0);
    
    // 處理Buff/Debuff
    QJsonArray buffs = rawData["buffs"].toArray();
    QJsonArray debuffs = rawData["debuffs"].toArray();
    
    for (const auto &buff : buffs) {
        perception.activeBuffs << buff.toString();
    }
    for (const auto &debuff : debuffs) {
        perception.activeDebuffs << debuff.toString();
    }
}

void EnvironmentPerceptor::analyzeThreats(PerceptionData &perception)
{
    float threatScore = predictThreatLevel(perception);
    
    if (threatScore > 0.7f) {
        perception.currentThreat = "high";
    } else if (threatScore > 0.4f) {
        perception.currentThreat = "medium";
    } else {
        perception.currentThreat = "low";
    }
}

void EnvironmentPerceptor::analyzeOpportunities(PerceptionData &perception)
{
    QStringList opportunities = identifyOpportunities(perception);
    
    // 將機會資訊添加到環境效果中
    for (const QString &opportunity : opportunities) {
        perception.environmentEffects << opportunity;
    }
}

void EnvironmentPerceptor::updateSpatialAwareness(PerceptionData &perception)
{
    // 分析隊形狀態
    if (perception.allyCount > 0) {
        if (perception.allyCount >= 3) {
            perception.teamFormation = "group";
        } else {
            perception.teamFormation = "pair";
        }
    } else {
        perception.teamFormation = "solo";
    }
    
    // 添加空間相關的環境效果
    if (perception.visibility < 0.5f) {
        perception.environmentEffects << "low_visibility";
    }
    
    if (perception.enemyCount > perception.allyCount * 2) {
        perception.environmentEffects << "outnumbered";
    }
}

// ========================================================================
// ConfigManager 實現
// ========================================================================

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_hotUpdateEnabled(false)
    , m_configWatcher(new QTimer(this))
{
    // 設置配置文件監控
    m_configWatcher->setInterval(1000); // 每秒檢查一次
    connect(m_configWatcher, &QTimer::timeout, this, &ConfigManager::onConfigFileChanged);
    
    qDebug() << "[ConfigManager] 初始化配置管理器";
}

void ConfigManager::loadConfiguration(const QString &configPath)
{
    m_configPath = configPath;
    
    QFile file(configPath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (doc.isObject()) {
            m_configuration = doc.object();
            validateConfiguration(m_configuration);
            
            qDebug() << "[ConfigManager] 載入配置:" << configPath;
            emit configurationChanged(m_configuration);
        }
    }
    
    if (m_hotUpdateEnabled) {
        setupFileWatcher();
    }
}

void ConfigManager::saveConfiguration(const QString &configPath)
{
    QFile file(configPath.isEmpty() ? m_configPath : configPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(m_configuration);
        file.write(doc.toJson());
        file.close();
        
        qDebug() << "[ConfigManager] 保存配置:" << file.fileName();
    }
}

void ConfigManager::updateConfiguration(const QJsonObject &newConfig)
{
    m_configuration = newConfig;
    validateConfiguration(m_configuration);
    
    emit configurationChanged(m_configuration);
    qDebug() << "[ConfigManager] 配置已更新";
}

QJsonObject ConfigManager::getConfiguration() const
{
    return m_configuration;
}

void ConfigManager::enableHotUpdate(bool enabled)
{
    m_hotUpdateEnabled = enabled;
    
    if (enabled && !m_configPath.isEmpty()) {
        setupFileWatcher();
    } else {
        m_configWatcher->stop();
    }
    
    qDebug() << "[ConfigManager] 熱更新" << (enabled ? "啟用" : "禁用");
}

bool ConfigManager::isHotUpdateEnabled() const
{
    return m_hotUpdateEnabled;
}

void ConfigManager::onConfigFileChanged()
{
    if (!m_configPath.isEmpty()) {
        QFile file(m_configPath);
        if (file.exists()) {
            static qint64 lastModified = 0;
            qint64 currentModified = QFileInfo(file).lastModified().toMSecsSinceEpoch();
            
            if (currentModified > lastModified) {
                lastModified = currentModified;
                loadConfiguration(m_configPath);
                qDebug() << "[ConfigManager] 檢測到配置文件變更，自動重載";
            }
        }
    }
}

void ConfigManager::setupFileWatcher()
{
    if (m_hotUpdateEnabled && !m_configPath.isEmpty()) {
        m_configWatcher->start();
        qDebug() << "[ConfigManager] 啟動配置文件監控:" << m_configPath;
    }
}

void ConfigManager::validateConfiguration(const QJsonObject &config)
{
    // 驗證配置的完整性和有效性
    QStringList requiredKeys = {"strategy", "learning_enabled", "hot_update_enabled"};
    
    for (const QString &key : requiredKeys) {
        if (!config.contains(key)) {
            qWarning() << "[ConfigManager] 配置缺少必要鍵:" << key;
        }
    }
    
    // 可以添加更多驗證邏輯
}

} // namespace JyAI
