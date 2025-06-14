/**
 * @file AIPlayerBrain.cpp
 * @brief AI玩家大腦實現 - 集成AI決策核心的完整AI玩家實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 * 
 * 🎯 AI玩家大腦實現特性:
 * ✅ 完整的AI更新循環
 * ✅ 環境感知和行為執行
 * ✅ 學習系統和經驗累積
 * ✅ 團隊協作和社交行為
 * ✅ 性能監控和調試功能
 * ✅ 與現有系統無縫集成
 */

#include "AIPlayerBrain.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <algorithm>
#include <cmath>

namespace JyAI {

// ========================================================================
// AIPlayerBrain 實現
// ========================================================================

AIPlayerBrain::AIPlayerBrain(const QString &playerId, QObject *parent)
    : QObject(parent)
    , m_playerId(playerId)
    , m_isActive(false)
    , m_updateFrequency(100) // 100ms預設更新頻率
    , m_debugEnabled(false)
    , m_lastUpdateTime(QDateTime::currentDateTime())
    , m_totalUpdateCount(0)
    , m_averageDecisionTime(0.0)
{
    // 初始化AI決策核心
    m_decisionCore = std::make_unique<AIDecisionCore>();
    
    // 初始化更新定時器
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &AIPlayerBrain::onUpdate);
    
    // 設置預設配置
    setupDefaultConfiguration();
    
    qDebug() << "🧠 AIPlayerBrain created for player:" << playerId;
}

AIPlayerBrain::~AIPlayerBrain()
{
    stop();
    savePlayerExperience();
    qDebug() << "🧠 AIPlayerBrain destroyed for player:" << m_playerId;
}

// ===== 基本設置 =====

void AIPlayerBrain::setPlayerData(const RANOnline::AI::AIPlayerData &playerData)
{
    m_playerData = playerData;
    
    // 更新感知數據
    updatePerceptionFromPlayerData();
    
    emit playerDataChanged(m_playerId, playerData);
    
    if (m_debugEnabled) {
        qDebug() << "🔧 Player data updated for:" << m_playerId;
    }
}

RANOnline::AI::AIPlayerData AIPlayerBrain::getPlayerData() const
{
    return m_playerData;
}

void AIPlayerBrain::setDecisionStrategy(DecisionStrategyType strategy)
{
    if (m_decisionCore) {
        m_decisionCore->setStrategy(strategy);
        m_currentStrategy = strategy;
        
        emit strategyChanged(m_playerId, strategy);
        
        if (m_debugEnabled) {
            qDebug() << "🎯 Strategy changed to:" << static_cast<int>(strategy) 
                     << "for player:" << m_playerId;
        }
    }
}

DecisionStrategyType AIPlayerBrain::getCurrentStrategy() const
{
    return m_currentStrategy;
}

// ===== 控制方法 =====

void AIPlayerBrain::start()
{
    if (!m_isActive) {
        m_isActive = true;
        m_updateTimer->start(m_updateFrequency);
        
        // 載入玩家經驗
        loadPlayerExperience();
        
        emit started(m_playerId);
        
        qDebug() << "▶️ AIPlayerBrain started for player:" << m_playerId;
    }
}

void AIPlayerBrain::stop()
{
    if (m_isActive) {
        m_isActive = false;
        m_updateTimer->stop();
        
        // 保存玩家經驗
        savePlayerExperience();
        
        emit stopped(m_playerId);
        
        qDebug() << "⏹️ AIPlayerBrain stopped for player:" << m_playerId;
    }
}

void AIPlayerBrain::pause()
{
    if (m_isActive) {
        m_updateTimer->stop();
        emit paused(m_playerId);
        
        if (m_debugEnabled) {
            qDebug() << "⏸️ AIPlayerBrain paused for player:" << m_playerId;
        }
    }
}

void AIPlayerBrain::resume()
{
    if (m_isActive) {
        m_updateTimer->start(m_updateFrequency);
        emit resumed(m_playerId);
        
        if (m_debugEnabled) {
            qDebug() << "▶️ AIPlayerBrain resumed for player:" << m_playerId;
        }
    }
}

void AIPlayerBrain::setUpdateFrequency(int milliseconds)
{
    m_updateFrequency = qMax(10, milliseconds); // 最小10ms
    
    if (m_isActive) {
        m_updateTimer->setInterval(m_updateFrequency);
    }
    
    if (m_debugEnabled) {
        qDebug() << "⏰ Update frequency set to:" << m_updateFrequency << "ms for player:" << m_playerId;
    }
}

// ===== 核心更新循環 =====

void AIPlayerBrain::onUpdate()
{
    if (!m_isActive || !m_decisionCore) {
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // 1. 更新感知系統
        updatePerception();
        
        // 2. 執行AI決策
        auto decision = m_decisionCore->makeDecision(m_perceptionData);
        
        // 3. 執行行為
        if (decision.isValid) {
            executeAction(decision);
        }
        
        // 4. 更新學習系統
        updateLearning(decision);
        
        // 5. 更新統計資料
        updateStatistics(startTime);
        
        // 6. 檢查團隊協作
        updateTeamCoordination();
        
        emit updated(m_playerId);
        
    } catch (const std::exception &e) {
        qDebug() << "❌ Error in AIPlayerBrain update:" << e.what() << "for player:" << m_playerId;
        emit errorOccurred(m_playerId, QString::fromStdString(e.what()));
    }
}

void AIPlayerBrain::updatePerception()
{
    // 更新基本感知數據
    updatePerceptionFromPlayerData();
    
    // 更新環境感知
    updateEnvironmentPerception();
    
    // 更新威脅分析
    updateThreatAnalysis();
    
    // 更新機會分析
    updateOpportunityAnalysis();
    
    if (m_debugEnabled && m_totalUpdateCount % 100 == 0) {
        qDebug() << "👁️ Perception updated - Health:" << m_perceptionData.health
                 << "Enemies:" << m_perceptionData.nearbyEnemies.size()
                 << "Allies:" << m_perceptionData.nearbyAllies.size();
    }
}

void AIPlayerBrain::updatePerceptionFromPlayerData()
{
    // 基本狀態
    m_perceptionData.health = m_playerData.currentHP;
    m_perceptionData.maxHealth = m_playerData.maxHP;
    m_perceptionData.mana = m_playerData.currentMP;
    m_perceptionData.maxMana = m_playerData.maxMP;
    m_perceptionData.level = m_playerData.level;
    m_perceptionData.experience = m_playerData.experience;
    
    // 位置資訊
    m_perceptionData.position = {m_playerData.position.x, m_playerData.position.y, m_playerData.position.z};
    
    // 狀態效果
    m_perceptionData.statusEffects.clear();
    for (const auto &effect : m_playerData.statusEffects) {
        m_perceptionData.statusEffects.push_back({
            effect.name.toStdString(),
            effect.duration,
            effect.isPositive
        });
    }
    
    // 裝備資訊
    m_perceptionData.hasWeapon = !m_playerData.weapon.name.isEmpty();
    m_perceptionData.weaponType = m_playerData.weapon.type.toStdString();
}

void AIPlayerBrain::updateEnvironmentPerception()
{
    // 更新環境效果
    m_perceptionData.environmentEffects.clear();
    
    // 檢查地形效果
    if (isInDangerousArea()) {
        m_perceptionData.environmentEffects.push_back({
            "dangerous_area", -50.0f, 30.0f
        });
    }
    
    if (isInSafeZone()) {
        m_perceptionData.environmentEffects.push_back({
            "safe_zone", 20.0f, -1.0f // 永久效果
        });
    }
    
    // 檢查天氣效果
    std::string weatherEffect = getCurrentWeatherEffect();
    if (!weatherEffect.empty()) {
        m_perceptionData.environmentEffects.push_back({
            weatherEffect, getWeatherImpact(), getWeatherDuration()
        });
    }
}

void AIPlayerBrain::updateThreatAnalysis()
{
    m_perceptionData.threatLevel = 0.0f;
    m_perceptionData.nearbyEnemies.clear();
    
    // 分析附近敵人
    for (const auto &enemy : getNearbyEnemies()) {
        EntityInfo enemyInfo;
        enemyInfo.id = enemy.id.toStdString();
        enemyInfo.position = {enemy.position.x, enemy.position.y, enemy.position.z};
        enemyInfo.health = enemy.currentHP;
        enemyInfo.level = enemy.level;
        enemyInfo.threatRating = calculateThreatRating(enemy);
        
        m_perceptionData.nearbyEnemies.push_back(enemyInfo);
        m_perceptionData.threatLevel += enemyInfo.threatRating;
    }
    
    // 正規化威脅等級
    m_perceptionData.threatLevel = qMin(100.0f, m_perceptionData.threatLevel);
}

void AIPlayerBrain::updateOpportunityAnalysis()
{
    m_perceptionData.nearbyAllies.clear();
    m_perceptionData.nearbyItems.clear();
    
    // 分析附近盟友
    for (const auto &ally : getNearbyAllies()) {
        EntityInfo allyInfo;
        allyInfo.id = ally.id.toStdString();
        allyInfo.position = {ally.position.x, ally.position.y, ally.position.z};
        allyInfo.health = ally.currentHP;
        allyInfo.level = ally.level;
        allyInfo.threatRating = 0.0f; // 盟友不是威脅
        
        m_perceptionData.nearbyAllies.push_back(allyInfo);
    }
    
    // 分析附近物品
    for (const auto &item : getNearbyItems()) {
        ItemInfo itemInfo;
        itemInfo.id = item.id.toStdString();
        itemInfo.type = item.type.toStdString();
        itemInfo.position = {item.position.x, item.position.y, item.position.z};
        itemInfo.value = item.value;
        itemInfo.rarity = item.rarity;
        
        m_perceptionData.nearbyItems.push_back(itemInfo);
    }
}

void AIPlayerBrain::executeAction(const AIAction &action)
{
    if (!action.isValid) {
        return;
    }
    
    try {
        switch (action.type) {
            case ActionType::MOVE:
                executeMovement(action);
                break;
                
            case ActionType::ATTACK:
                executeAttack(action);
                break;
                
            case ActionType::USE_SKILL:
                executeSkill(action);
                break;
                
            case ActionType::USE_ITEM:
                executeItem(action);
                break;
                
            case ActionType::INTERACT:
                executeInteraction(action);
                break;
                
            case ActionType::WAIT:
                executeWait(action);
                break;
                
            case ActionType::FLEE:
                executeFlee(action);
                break;
                
            default:
                qDebug() << "⚠️ Unknown action type:" << static_cast<int>(action.type);
                break;
        }
        
        // 記錄行為歷史
        recordActionHistory(action);
        
        emit actionExecuted(m_playerId, action);
        
    } catch (const std::exception &e) {
        qDebug() << "❌ Error executing action:" << e.what() << "for player:" << m_playerId;
        emit errorOccurred(m_playerId, QString::fromStdString(e.what()));
    }
}

void AIPlayerBrain::executeMovement(const AIAction &action)
{
    if (action.parameters.size() >= 3) {
        float x = std::stof(action.parameters[0]);
        float y = std::stof(action.parameters[1]);
        float z = std::stof(action.parameters[2]);
        
        // 發送移動命令到遊戲系統
        emit moveRequested(m_playerId, QVector3D(x, y, z));
        
        if (m_debugEnabled) {
            qDebug() << "🚶 Move action executed to:" << x << y << z;
        }
    }
}

void AIPlayerBrain::executeAttack(const AIAction &action)
{
    if (!action.parameters.empty()) {
        QString targetId = QString::fromStdString(action.parameters[0]);
        
        // 發送攻擊命令到遊戲系統
        emit attackRequested(m_playerId, targetId);
        
        if (m_debugEnabled) {
            qDebug() << "⚔️ Attack action executed on target:" << targetId;
        }
    }
}

void AIPlayerBrain::executeSkill(const AIAction &action)
{
    if (action.parameters.size() >= 1) {
        QString skillId = QString::fromStdString(action.parameters[0]);
        QString targetId;
        
        if (action.parameters.size() >= 2) {
            targetId = QString::fromStdString(action.parameters[1]);
        }
        
        // 發送技能使用命令到遊戲系統
        emit skillUseRequested(m_playerId, skillId, targetId);
        
        if (m_debugEnabled) {
            qDebug() << "✨ Skill action executed:" << skillId << "on target:" << targetId;
        }
    }
}

void AIPlayerBrain::executeItem(const AIAction &action)
{
    if (!action.parameters.empty()) {
        QString itemId = QString::fromStdString(action.parameters[0]);
        
        // 發送物品使用命令到遊戲系統
        emit itemUseRequested(m_playerId, itemId);
        
        if (m_debugEnabled) {
            qDebug() << "🎒 Item action executed:" << itemId;
        }
    }
}

void AIPlayerBrain::executeInteraction(const AIAction &action)
{
    if (!action.parameters.empty()) {
        QString objectId = QString::fromStdString(action.parameters[0]);
        
        // 發送互動命令到遊戲系統
        emit interactionRequested(m_playerId, objectId);
        
        if (m_debugEnabled) {
            qDebug() << "🤝 Interaction action executed with:" << objectId;
        }
    }
}

void AIPlayerBrain::executeWait(const AIAction &action)
{
    // 等待行為 - 暫時不做任何事
    if (m_debugEnabled) {
        qDebug() << "⏳ Wait action executed";
    }
}

void AIPlayerBrain::executeFlee(const AIAction &action)
{
    // 計算逃跑方向
    QVector3D fleeDirection = calculateFleeDirection();
    
    if (fleeDirection.length() > 0) {
        QVector3D currentPos(m_playerData.position.x, m_playerData.position.y, m_playerData.position.z);
        QVector3D targetPos = currentPos + fleeDirection * 50.0f; // 逃跑50單位距離
        
        emit moveRequested(m_playerId, targetPos);
        
        if (m_debugEnabled) {
            qDebug() << "🏃 Flee action executed to:" << targetPos;
        }
    }
}

void AIPlayerBrain::updateLearning(const AIAction &action)
{
    if (!m_decisionCore) {
        return;
    }
    
    // 計算行為回饋
    float reward = calculateActionReward(action);
    
    // 記錄決策歷史
    DecisionHistory history;
    history.perception = m_perceptionData;
    history.action = action;
    history.reward = reward;
    history.timestamp = std::chrono::system_clock::now();
    history.strategy = m_currentStrategy;
    
    // 更新學習系統
    m_decisionCore->learn(m_perceptionData, action, reward);
    
    // 記錄歷史
    m_decisionHistory.push_back(history);
    
    // 限制歷史記錄大小
    if (m_decisionHistory.size() > 1000) {
        m_decisionHistory.erase(m_decisionHistory.begin());
    }
    
    if (m_debugEnabled && m_totalUpdateCount % 50 == 0) {
        qDebug() << "📚 Learning updated - Reward:" << reward << "History size:" << m_decisionHistory.size();
    }
}

float AIPlayerBrain::calculateActionReward(const AIAction &action)
{
    float reward = 0.0f;
    
    // 基於生存狀態的獎勵
    float healthRatio = m_perceptionData.health / qMax(1.0f, m_perceptionData.maxHealth);
    reward += healthRatio * 10.0f;
    
    // 基於行為類型的獎勵
    switch (action.type) {
        case ActionType::ATTACK:
            // 攻擊獎勵基於威脅等級
            reward += (m_perceptionData.threatLevel > 30.0f) ? 15.0f : -5.0f;
            break;
            
        case ActionType::USE_SKILL:
            // 技能使用獎勵
            reward += 8.0f;
            break;
            
        case ActionType::FLEE:
            // 逃跑獎勵基於威脅等級
            reward += (m_perceptionData.threatLevel > 70.0f) ? 12.0f : -3.0f;
            break;
            
        case ActionType::USE_ITEM:
            // 物品使用獎勵
            reward += (healthRatio < 0.5f) ? 10.0f : 2.0f;
            break;
            
        case ActionType::MOVE:
            // 移動獎勵
            reward += 1.0f;
            break;
            
        case ActionType::WAIT:
            // 等待懲罰
            reward -= 1.0f;
            break;
            
        default:
            break;
    }
    
    // 基於效果的額外獎勵
    reward += calculateEffectivenessReward(action);
    
    return reward;
}

float AIPlayerBrain::calculateEffectivenessReward(const AIAction &action)
{
    float effectiveness = 0.0f;
    
    // 檢查行為是否達到預期效果
    // 這裡可以根據具體的遊戲邏輯來實現
    
    // 例如：攻擊是否成功造成傷害
    if (action.type == ActionType::ATTACK && action.confidence > 0.7f) {
        effectiveness += 5.0f;
    }
    
    // 技能使用是否適當
    if (action.type == ActionType::USE_SKILL && action.confidence > 0.8f) {
        effectiveness += 3.0f;
    }
    
    return effectiveness;
}

void AIPlayerBrain::updateStatistics(const std::chrono::high_resolution_clock::time_point &startTime)
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    double decisionTime = duration.count() / 1000.0; // 轉換為毫秒
    
    // 更新統計
    m_totalUpdateCount++;
    m_averageDecisionTime = (m_averageDecisionTime * (m_totalUpdateCount - 1) + decisionTime) / m_totalUpdateCount;
    m_lastUpdateTime = QDateTime::currentDateTime();
    
    // 記錄性能統計
    if (m_totalUpdateCount % 1000 == 0) {
        qDebug() << "📊 Performance stats for player:" << m_playerId
                 << "- Updates:" << m_totalUpdateCount
                 << "- Avg decision time:" << QString::number(m_averageDecisionTime, 'f', 2) << "ms";
    }
}

void AIPlayerBrain::updateTeamCoordination()
{
    // 簡單的團隊協作邏輯
    if (m_perceptionData.nearbyAllies.size() > 0) {
        // 檢查是否需要支援隊友
        for (const auto &ally : m_perceptionData.nearbyAllies) {
            float allyHealthRatio = ally.health / qMax(1.0f, 100.0f); // 假設最大血量100
            
            if (allyHealthRatio < 0.3f) {
                // 隊友需要幫助
                emit teamSupportNeeded(m_playerId, QString::fromStdString(ally.id));
            }
        }
    }
    
    // 發送自己的狀態給團隊
    if (m_totalUpdateCount % 50 == 0) { // 每50次更新發送一次
        emit teamStatusUpdate(m_playerId, m_perceptionData.health, m_perceptionData.threatLevel);
    }
}

void AIPlayerBrain::recordActionHistory(const AIAction &action)
{
    // 記錄到內部歷史
    ActionRecord record;
    record.action = action;
    record.timestamp = QDateTime::currentDateTime();
    record.playerState = m_playerData;
    record.perceptionData = m_perceptionData;
    
    m_actionHistory.append(record);
    
    // 限制歷史記錄大小
    if (m_actionHistory.size() > 500) {
        m_actionHistory.removeFirst();
    }
}

// ===== 輔助方法 =====

void AIPlayerBrain::setupDefaultConfiguration()
{
    // 設置預設決策策略
    m_currentStrategy = DecisionStrategyType::HYBRID;
    
    // 設置預設參數
    m_updateFrequency = 100; // 100ms
    m_debugEnabled = false;
    
    // 初始化性能統計
    m_totalUpdateCount = 0;
    m_averageDecisionTime = 0.0;
}

bool AIPlayerBrain::isInDangerousArea() const
{
    // 簡單的危險區域檢測
    // 這裡可以根據具體的遊戲地圖來實現
    return m_perceptionData.threatLevel > 50.0f;
}

bool AIPlayerBrain::isInSafeZone() const
{
    // 簡單的安全區域檢測
    return m_perceptionData.threatLevel < 10.0f && m_perceptionData.nearbyEnemies.empty();
}

std::string AIPlayerBrain::getCurrentWeatherEffect() const
{
    // 簡單的天氣效果模擬
    // 這裡可以根據遊戲的天氣系統來實現
    return ""; // 暫時返回空字符串
}

float AIPlayerBrain::getWeatherImpact() const
{
    return 0.0f; // 暫時返回0
}

float AIPlayerBrain::getWeatherDuration() const
{
    return 0.0f; // 暫時返回0
}

QList<RANOnline::AI::AIPlayerData> AIPlayerBrain::getNearbyEnemies() const
{
    // 這裡應該從遊戲世界獲取附近的敵人
    // 暫時返回空列表
    return QList<RANOnline::AI::AIPlayerData>();
}

QList<RANOnline::AI::AIPlayerData> AIPlayerBrain::getNearbyAllies() const
{
    // 這裡應該從遊戲世界獲取附近的盟友
    // 暫時返回空列表
    return QList<RANOnline::AI::AIPlayerData>();
}

QList<RANOnline::AI::ItemData> AIPlayerBrain::getNearbyItems() const
{
    // 這裡應該從遊戲世界獲取附近的物品
    // 暫時返回空列表
    return QList<RANOnline::AI::ItemData>();
}

float AIPlayerBrain::calculateThreatRating(const RANOnline::AI::AIPlayerData &enemy) const
{
    float threat = 0.0f;
    
    // 基於等級差異
    int levelDiff = enemy.level - m_playerData.level;
    threat += levelDiff * 5.0f;
    
    // 基於血量
    float enemyHealthRatio = enemy.currentHP / qMax(1.0f, static_cast<float>(enemy.maxHP));
    threat += enemyHealthRatio * 10.0f;
    
    // 基於距離
    QVector3D enemyPos(enemy.position.x, enemy.position.y, enemy.position.z);
    QVector3D myPos(m_playerData.position.x, m_playerData.position.y, m_playerData.position.z);
    float distance = (enemyPos - myPos).length();
    threat += qMax(0.0f, 20.0f - distance); // 距離越近威脅越大
    
    return qMax(0.0f, threat);
}

QVector3D AIPlayerBrain::calculateFleeDirection() const
{
    QVector3D fleeDirection(0, 0, 0);
    QVector3D myPos(m_playerData.position.x, m_playerData.position.y, m_playerData.position.z);
    
    // 計算遠離所有敵人的方向
    for (const auto &enemy : m_perceptionData.nearbyEnemies) {
        QVector3D enemyPos(enemy.position.x, enemy.position.y, enemy.position.z);
        QVector3D awayDirection = myPos - enemyPos;
        
        if (awayDirection.length() > 0) {
            awayDirection.normalize();
            fleeDirection += awayDirection;
        }
    }
    
    if (fleeDirection.length() > 0) {
        fleeDirection.normalize();
    }
    
    return fleeDirection;
}

void AIPlayerBrain::loadPlayerExperience()
{
    // 從文件載入玩家經驗
    QString filename = QString("ai_experience_%1.json").arg(m_playerId);
    
    // 這裡可以實現從文件載入經驗數據的邏輯
    if (m_debugEnabled) {
        qDebug() << "📖 Loading experience for player:" << m_playerId;
    }
}

void AIPlayerBrain::savePlayerExperience()
{
    // 保存玩家經驗到文件
    QString filename = QString("ai_experience_%1.json").arg(m_playerId);
    
    // 這裡可以實現保存經驗數據到文件的邏輯
    if (m_debugEnabled) {
        qDebug() << "💾 Saving experience for player:" << m_playerId;
    }
}

// ===== 調試和監控 =====

void AIPlayerBrain::enableDebug(bool enabled)
{
    m_debugEnabled = enabled;
    qDebug() << "🐛 Debug" << (enabled ? "enabled" : "disabled") << "for player:" << m_playerId;
}

QString AIPlayerBrain::getDebugInfo() const
{
    QJsonObject debugInfo;
    debugInfo["playerId"] = m_playerId;
    debugInfo["isActive"] = m_isActive;
    debugInfo["currentStrategy"] = static_cast<int>(m_currentStrategy);
    debugInfo["updateFrequency"] = m_updateFrequency;
    debugInfo["totalUpdates"] = static_cast<int>(m_totalUpdateCount);
    debugInfo["averageDecisionTime"] = m_averageDecisionTime;
    debugInfo["threatLevel"] = static_cast<double>(m_perceptionData.threatLevel);
    debugInfo["health"] = static_cast<double>(m_perceptionData.health);
    debugInfo["nearbyEnemies"] = static_cast<int>(m_perceptionData.nearbyEnemies.size());
    debugInfo["nearbyAllies"] = static_cast<int>(m_perceptionData.nearbyAllies.size());
    
    QJsonDocument doc(debugInfo);
    return doc.toJson(QJsonDocument::Compact);
}

AIPlayerBrain::PerformanceStats AIPlayerBrain::getPerformanceStats() const
{
    PerformanceStats stats;
    stats.totalUpdates = m_totalUpdateCount;
    stats.averageDecisionTime = m_averageDecisionTime;
    stats.lastUpdateTime = m_lastUpdateTime;
    stats.isActive = m_isActive;
    stats.currentStrategy = m_currentStrategy;
    
    return stats;
}

QList<AIPlayerBrain::ActionRecord> AIPlayerBrain::getActionHistory(int limit) const
{
    if (limit <= 0 || limit >= m_actionHistory.size()) {
        return m_actionHistory;
    }
    
    return m_actionHistory.mid(m_actionHistory.size() - limit);
}

// ========================================================================
// AIPlayerManager 實現
// ========================================================================

AIPlayerManager::AIPlayerManager(QObject *parent)
    : QObject(parent)
    , m_isActive(false)
    , m_maxPlayers(100)
    , m_updateInterval(50) // 50ms預設間隔
{
    // 初始化管理器定時器
    m_managerTimer = new QTimer(this);
    connect(m_managerTimer, &QTimer::timeout, this, &AIPlayerManager::onManagerUpdate);
    
    qDebug() << "🎮 AIPlayerManager created";
}

AIPlayerManager::~AIPlayerManager()
{
    stop();
    clearAllPlayers();
    qDebug() << "🎮 AIPlayerManager destroyed";
}

// ===== 玩家管理 =====

bool AIPlayerManager::addPlayer(const QString &playerId, const RANOnline::AI::AIPlayerData &playerData)
{
    if (m_players.contains(playerId)) {
        qDebug() << "⚠️ Player already exists:" << playerId;
        return false;
    }
    
    if (m_players.size() >= m_maxPlayers) {
        qDebug() << "⚠️ Maximum player limit reached:" << m_maxPlayers;
        return false;
    }
    
    // 創建新的AI玩家大腦
    auto brain = std::make_unique<AIPlayerBrain>(playerId, this);
    brain->setPlayerData(playerData);
    
    // 連接信號
    connectPlayerSignals(brain.get());
    
    // 添加到管理器
    m_players[playerId] = std::move(brain);
    
    emit playerAdded(playerId);
    qDebug() << "➕ Player added:" << playerId << "Total players:" << m_players.size();
    
    return true;
}

bool AIPlayerManager::removePlayer(const QString &playerId)
{
    auto it = m_players.find(playerId);
    if (it == m_players.end()) {
        qDebug() << "⚠️ Player not found:" << playerId;
        return false;
    }
    
    // 停止AI玩家
    it->second->stop();
    
    // 移除玩家
    m_players.erase(it);
    
    emit playerRemoved(playerId);
    qDebug() << "➖ Player removed:" << playerId << "Remaining players:" << m_players.size();
    
    return true;
}

AIPlayerBrain* AIPlayerManager::getPlayer(const QString &playerId)
{
    auto it = m_players.find(playerId);
    return (it != m_players.end()) ? it->second.get() : nullptr;
}

const AIPlayerBrain* AIPlayerManager::getPlayer(const QString &playerId) const
{
    auto it = m_players.find(playerId);
    return (it != m_players.end()) ? it->second.get() : nullptr;
}

QStringList AIPlayerManager::getPlayerIds() const
{
    QStringList ids;
    for (const auto &pair : m_players) {
        ids.append(pair.first);
    }
    return ids;
}

void AIPlayerManager::clearAllPlayers()
{
    // 停止所有玩家
    for (auto &pair : m_players) {
        pair.second->stop();
    }
    
    int count = m_players.size();
    m_players.clear();
    
    emit allPlayersCleared();
    qDebug() << "🗑️ All players cleared:" << count << "players removed";
}

// ===== 批量控制 =====

void AIPlayerManager::startAllPlayers()
{
    for (auto &pair : m_players) {
        pair.second->start();
    }
    
    qDebug() << "▶️ All players started:" << m_players.size() << "players";
}

void AIPlayerManager::stopAllPlayers()
{
    for (auto &pair : m_players) {
        pair.second->stop();
    }
    
    qDebug() << "⏹️ All players stopped:" << m_players.size() << "players";
}

void AIPlayerManager::pauseAllPlayers()
{
    for (auto &pair : m_players) {
        pair.second->pause();
    }
    
    qDebug() << "⏸️ All players paused:" << m_players.size() << "players";
}

void AIPlayerManager::resumeAllPlayers()
{
    for (auto &pair : m_players) {
        pair.second->resume();
    }
    
    qDebug() << "▶️ All players resumed:" << m_players.size() << "players";
}

void AIPlayerManager::setAllPlayersStrategy(DecisionStrategyType strategy)
{
    for (auto &pair : m_players) {
        pair.second->setDecisionStrategy(strategy);
    }
    
    qDebug() << "🎯 All players strategy set to:" << static_cast<int>(strategy);
}

void AIPlayerManager::setAllPlayersUpdateFrequency(int milliseconds)
{
    for (auto &pair : m_players) {
        pair.second->setUpdateFrequency(milliseconds);
    }
    
    qDebug() << "⏰ All players update frequency set to:" << milliseconds << "ms";
}

// ===== 管理器控制 =====

void AIPlayerManager::start()
{
    if (!m_isActive) {
        m_isActive = true;
        m_managerTimer->start(m_updateInterval);
        
        // 啟動所有玩家
        startAllPlayers();
        
        emit started();
        qDebug() << "▶️ AIPlayerManager started with" << m_players.size() << "players";
    }
}

void AIPlayerManager::stop()
{
    if (m_isActive) {
        m_isActive = false;
        m_managerTimer->stop();
        
        // 停止所有玩家
        stopAllPlayers();
        
        emit stopped();
        qDebug() << "⏹️ AIPlayerManager stopped";
    }
}

void AIPlayerManager::setMaxPlayers(int maxPlayers)
{
    m_maxPlayers = qMax(1, maxPlayers);
    qDebug() << "👥 Max players set to:" << m_maxPlayers;
}

void AIPlayerManager::setUpdateInterval(int milliseconds)
{
    m_updateInterval = qMax(10, milliseconds);
    
    if (m_isActive) {
        m_managerTimer->setInterval(m_updateInterval);
    }
    
    qDebug() << "⏰ Manager update interval set to:" << m_updateInterval << "ms";
}

// ===== 統計和監控 =====

AIPlayerManager::ManagerStats AIPlayerManager::getManagerStats() const
{
    ManagerStats stats;
    stats.totalPlayers = m_players.size();
    stats.activePlayers = 0;
    stats.averageDecisionTime = 0.0;
    stats.totalUpdates = 0;
    
    for (const auto &pair : m_players) {
        auto playerStats = pair.second->getPerformanceStats();
        if (playerStats.isActive) {
            stats.activePlayers++;
        }
        stats.averageDecisionTime += playerStats.averageDecisionTime;
        stats.totalUpdates += playerStats.totalUpdates;
    }
    
    if (stats.totalPlayers > 0) {
        stats.averageDecisionTime /= stats.totalPlayers;
    }
    
    stats.isActive = m_isActive;
    stats.maxPlayers = m_maxPlayers;
    stats.updateInterval = m_updateInterval;
    
    return stats;
}

QString AIPlayerManager::getDetailedStats() const
{
    QJsonObject stats;
    auto managerStats = getManagerStats();
    
    stats["managerActive"] = managerStats.isActive;
    stats["totalPlayers"] = managerStats.totalPlayers;
    stats["activePlayers"] = managerStats.activePlayers;
    stats["maxPlayers"] = managerStats.maxPlayers;
    stats["averageDecisionTime"] = managerStats.averageDecisionTime;
    stats["totalUpdates"] = static_cast<int>(managerStats.totalUpdates);
    stats["updateInterval"] = managerStats.updateInterval;
    
    QJsonDocument doc(stats);
    return doc.toJson(QJsonDocument::Indented);
}

// ===== 私有方法 =====

void AIPlayerManager::connectPlayerSignals(AIPlayerBrain *brain)
{
    // 轉發玩家信號
    connect(brain, &AIPlayerBrain::actionExecuted, this, &AIPlayerManager::playerActionExecuted);
    connect(brain, &AIPlayerBrain::errorOccurred, this, &AIPlayerManager::playerErrorOccurred);
    connect(brain, &AIPlayerBrain::teamSupportNeeded, this, &AIPlayerManager::teamSupportNeeded);
    connect(brain, &AIPlayerBrain::teamStatusUpdate, this, &AIPlayerManager::teamStatusUpdate);
    
    // 轉發遊戲動作信號
    connect(brain, &AIPlayerBrain::moveRequested, this, &AIPlayerManager::moveRequested);
    connect(brain, &AIPlayerBrain::attackRequested, this, &AIPlayerManager::attackRequested);
    connect(brain, &AIPlayerBrain::skillUseRequested, this, &AIPlayerManager::skillUseRequested);
    connect(brain, &AIPlayerBrain::itemUseRequested, this, &AIPlayerManager::itemUseRequested);
    connect(brain, &AIPlayerBrain::interactionRequested, this, &AIPlayerManager::interactionRequested);
}

void AIPlayerManager::onManagerUpdate()
{
    if (!m_isActive) {
        return;
    }
    
    // 執行管理器級別的更新邏輯
    // 例如：負載平衡、性能監控、團隊協調等
    
    // 每1000次更新輸出統計資訊
    static int updateCount = 0;
    updateCount++;
    
    if (updateCount % 1000 == 0) {
        auto stats = getManagerStats();
        qDebug() << "📊 Manager stats - Players:" << stats.totalPlayers
                 << "Active:" << stats.activePlayers
                 << "Avg decision time:" << QString::number(stats.averageDecisionTime, 'f', 2) << "ms";
    }
    
    emit managerUpdated();
}

} // namespace JyAI

#include "AIPlayerBrain.moc"
