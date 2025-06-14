/**
 * @file AISystemIntegration.cpp
 * @brief AI系統集成介面實現 - 連接新AI決策核心與現有RANOnline AI系統
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 4.0.0
 */

#include "AISystemIntegration.h"
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <algorithm>

namespace JyAI {

// ========================================================================
// AISystemIntegrationManager 實現
// ========================================================================

AISystemIntegrationManager::AISystemIntegrationManager(QObject *parent)
    : QObject(parent)
    , m_isInitialized(false)
    , m_isRunning(false)
    , m_debugMode(false)
    , m_legacyEngine(nullptr)
    , m_legacyGenerator(nullptr)
    , m_legacyWidget(nullptr)
{
    // 初始化更新定時器
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &AISystemIntegrationManager::onUpdateTimer);
    m_updateTimer->setInterval(1000); // 1秒更新間隔

    // 初始化統計資料
    m_stats = {};
    m_startTime = QDateTime::currentDateTime();

    qDebug() << "🔗 AISystemIntegrationManager created";
}

AISystemIntegrationManager::~AISystemIntegrationManager()
{
    stop();
    qDebug() << "🔗 AISystemIntegrationManager destroyed";
}

bool AISystemIntegrationManager::initialize(RANOnline::AI::AIDecisionEngine *existingEngine,
                                          RANOnline::AI::AIPlayerGenerator *existingGenerator,
                                          RANOnline::AI::AIManagementWidget *existingWidget)
{
    if (m_isInitialized) {
        qDebug() << "⚠️ AISystemIntegrationManager already initialized";
        return true;
    }

    try {
        // 保存現有系統組件的引用
        m_legacyEngine = existingEngine;
        m_legacyGenerator = existingGenerator;
        m_legacyWidget = existingWidget;

        // 創建新AI管理器
        m_newAIManager = std::make_unique<AIPlayerManager>(this);

        // 設置連接
        setupConnections();

        // 載入配置
        m_configFilePath = "ai_decision_config.json";
        reloadConfiguration();

        m_isInitialized = true;
        logSystemEvent("System Initialized", "Integration manager successfully initialized");
        
        emit systemInitialized();
        qDebug() << "✅ AISystemIntegrationManager initialized successfully";
        
        return true;

    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to initialize AISystemIntegrationManager:" << e.what();
        emit systemError(QString("Initialization failed: %1").arg(e.what()));
        return false;
    }
}

void AISystemIntegrationManager::start()
{
    if (!m_isInitialized) {
        qDebug() << "⚠️ Cannot start: system not initialized";
        emit systemError("Cannot start: system not initialized");
        return;
    }

    if (m_isRunning) {
        qDebug() << "⚠️ AISystemIntegrationManager already running";
        return;
    }

    try {
        // 啟動新AI管理器
        if (m_newAIManager) {
            m_newAIManager->start();
        }

        // 啟動更新定時器
        m_updateTimer->start();

        m_isRunning = true;
        m_startTime = QDateTime::currentDateTime();
        
        logSystemEvent("System Started", "Integration manager started successfully");
        emit systemStarted();
        
        qDebug() << "▶️ AISystemIntegrationManager started";

    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to start AISystemIntegrationManager:" << e.what();
        emit systemError(QString("Start failed: %1").arg(e.what()));
    }
}

void AISystemIntegrationManager::stop()
{
    if (!m_isRunning) {
        return;
    }

    try {
        // 停止更新定時器
        m_updateTimer->stop();

        // 停止新AI管理器
        if (m_newAIManager) {
            m_newAIManager->stop();
        }

        m_isRunning = false;
        
        logSystemEvent("System Stopped", "Integration manager stopped successfully");
        emit systemStopped();
        
        qDebug() << "⏹️ AISystemIntegrationManager stopped";

    } catch (const std::exception &e) {
        qDebug() << "❌ Error stopping AISystemIntegrationManager:" << e.what();
        emit systemError(QString("Stop failed: %1").arg(e.what()));
    }
}

bool AISystemIntegrationManager::createAIPlayer(const QString &playerId, 
                                               const RANOnline::AI::AIPlayerData &playerData,
                                               bool useNewAI)
{
    if (!m_isInitialized) {
        qDebug() << "⚠️ Cannot create AI player: system not initialized";
        return false;
    }

    if (m_playerAIType.contains(playerId)) {
        qDebug() << "⚠️ AI player already exists:" << playerId;
        return false;
    }

    try {
        bool success = false;

        if (useNewAI && m_newAIManager) {
            // 使用新AI系統
            success = m_newAIManager->addPlayer(playerId, playerData);
            if (success) {
                m_playerAIType[playerId] = true;
                m_stats.newAIPlayers++;
            }
        } else if (!useNewAI && m_legacyGenerator) {
            // 使用舊AI系統
            success = true; // 假設舊系統創建成功
            if (success) {
                m_playerAIType[playerId] = false;
                m_stats.legacyAIPlayers++;
            }
        }

        if (success) {
            m_playerData[playerId] = playerData;
            m_playerLastUpdate[playerId] = QDateTime::currentDateTime();
            m_playerDecisionCount[playerId] = 0;
            m_playerDecisionTime[playerId] = 0.0;
            m_stats.totalAIPlayers++;

            logSystemEvent("AI Player Created", 
                         QString("Player %1 created using %2 AI system")
                         .arg(playerId)
                         .arg(useNewAI ? "new" : "legacy"));
            
            emit aiPlayerCreated(playerId, useNewAI);
            
            if (m_debugMode) {
                qDebug() << "➕ AI player created:" << playerId << "using" << (useNewAI ? "new" : "legacy") << "AI";
            }
        }

        return success;

    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to create AI player:" << playerId << "Error:" << e.what();
        emit systemError(QString("Failed to create AI player %1: %2").arg(playerId, e.what()));
        return false;
    }
}

bool AISystemIntegrationManager::removeAIPlayer(const QString &playerId)
{
    if (!m_playerAIType.contains(playerId)) {
        qDebug() << "⚠️ AI player not found:" << playerId;
        return false;
    }

    try {
        bool success = false;
        bool wasNewAI = m_playerAIType[playerId];

        if (wasNewAI && m_newAIManager) {
            // 從新AI系統移除
            success = m_newAIManager->removePlayer(playerId);
            if (success) {
                m_stats.newAIPlayers--;
            }
        } else {
            // 從舊AI系統移除
            success = true; // 假設舊系統移除成功
            if (success) {
                m_stats.legacyAIPlayers--;
            }
        }

        if (success) {
            m_playerAIType.remove(playerId);
            m_playerData.remove(playerId);
            m_playerLastUpdate.remove(playerId);
            m_playerDecisionCount.remove(playerId);
            m_playerDecisionTime.remove(playerId);
            m_stats.totalAIPlayers--;

            logSystemEvent("AI Player Removed", 
                         QString("Player %1 removed from %2 AI system")
                         .arg(playerId)
                         .arg(wasNewAI ? "new" : "legacy"));
            
            emit aiPlayerRemoved(playerId);
            
            if (m_debugMode) {
                qDebug() << "➖ AI player removed:" << playerId;
            }
        }

        return success;

    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to remove AI player:" << playerId << "Error:" << e.what();
        emit systemError(QString("Failed to remove AI player %1: %2").arg(playerId, e.what()));
        return false;
    }
}

bool AISystemIntegrationManager::switchAISystem(const QString &playerId, bool useNewAI)
{
    if (!m_playerAIType.contains(playerId)) {
        qDebug() << "⚠️ AI player not found:" << playerId;
        return false;
    }

    bool currentlyNewAI = m_playerAIType[playerId];
    if (currentlyNewAI == useNewAI) {
        qDebug() << "⚠️ AI player already using" << (useNewAI ? "new" : "legacy") << "AI system:" << playerId;
        return true;
    }

    try {
        // 獲取當前玩家數據
        auto playerData = m_playerData[playerId];

        // 從當前系統移除
        bool removeSuccess = false;
        if (currentlyNewAI && m_newAIManager) {
            removeSuccess = m_newAIManager->removePlayer(playerId);
            if (removeSuccess) m_stats.newAIPlayers--;
        } else {
            removeSuccess = true; // 假設從舊系統移除成功
            if (removeSuccess) m_stats.legacyAIPlayers--;
        }

        if (!removeSuccess) {
            qDebug() << "❌ Failed to remove player from current AI system:" << playerId;
            return false;
        }

        // 添加到目標系統
        bool addSuccess = false;
        if (useNewAI && m_newAIManager) {
            addSuccess = m_newAIManager->addPlayer(playerId, playerData);
            if (addSuccess) m_stats.newAIPlayers++;
        } else {
            addSuccess = true; // 假設添加到舊系統成功
            if (addSuccess) m_stats.legacyAIPlayers++;
        }

        if (addSuccess) {
            m_playerAIType[playerId] = useNewAI;
            
            // 遷移玩家數據
            migratePlayerData(playerId, useNewAI);

            logSystemEvent("AI System Switched", 
                         QString("Player %1 switched from %2 to %3 AI system")
                         .arg(playerId)
                         .arg(currentlyNewAI ? "new" : "legacy")
                         .arg(useNewAI ? "new" : "legacy"));
            
            emit aiSystemSwitched(playerId, useNewAI);
            
            if (m_debugMode) {
                qDebug() << "🔄 AI system switched for player:" << playerId 
                         << "to" << (useNewAI ? "new" : "legacy") << "AI";
            }

            return true;
        } else {
            // 切換失敗，嘗試恢復到原系統
            qDebug() << "❌ Failed to add player to target AI system, attempting to restore:" << playerId;
            
            bool restoreSuccess = false;
            if (currentlyNewAI && m_newAIManager) {
                restoreSuccess = m_newAIManager->addPlayer(playerId, playerData);
                if (restoreSuccess) m_stats.newAIPlayers++;
            } else {
                restoreSuccess = true; // 假設恢復到舊系統成功
                if (restoreSuccess) m_stats.legacyAIPlayers++;
            }

            if (!restoreSuccess) {
                qDebug() << "❌ Critical: Failed to restore player to original AI system:" << playerId;
                // 玩家已丟失，從記錄中移除
                m_playerAIType.remove(playerId);
                m_playerData.remove(playerId);
                m_stats.totalAIPlayers--;
            }

            return false;
        }

    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to switch AI system for player:" << playerId << "Error:" << e.what();
        emit systemError(QString("Failed to switch AI system for player %1: %2").arg(playerId, e.what()));
        return false;
    }
}

bool AISystemIntegrationManager::setPlayerStrategy(const QString &playerId, DecisionStrategyType strategy)
{
    if (!m_playerAIType.contains(playerId)) {
        qDebug() << "⚠️ AI player not found:" << playerId;
        return false;
    }

    bool isNewAI = m_playerAIType[playerId];
    if (!isNewAI) {
        qDebug() << "⚠️ Cannot set strategy: player using legacy AI system:" << playerId;
        return false;
    }

    try {
        if (m_newAIManager) {
            auto player = m_newAIManager->getPlayer(playerId);
            if (player) {
                player->setDecisionStrategy(strategy);
                
                logSystemEvent("Strategy Changed", 
                             QString("Player %1 strategy changed to %2")
                             .arg(playerId)
                             .arg(static_cast<int>(strategy)));
                
                if (m_debugMode) {
                    qDebug() << "🎯 Strategy set for player:" << playerId 
                             << "to strategy:" << static_cast<int>(strategy);
                }
                
                return true;
            }
        }

        qDebug() << "❌ Failed to find player in new AI manager:" << playerId;
        return false;

    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to set strategy for player:" << playerId << "Error:" << e.what();
        emit systemError(QString("Failed to set strategy for player %1: %2").arg(playerId, e.what()));
        return false;
    }
}

QStringList AISystemIntegrationManager::getAIPlayerList() const
{
    return m_playerAIType.keys();
}

QStringList AISystemIntegrationManager::getNewAIPlayerList() const
{
    QStringList newAIPlayers;
    for (auto it = m_playerAIType.constBegin(); it != m_playerAIType.constEnd(); ++it) {
        if (it.value()) {
            newAIPlayers.append(it.key());
        }
    }
    return newAIPlayers;
}

QStringList AISystemIntegrationManager::getLegacyAIPlayerList() const
{
    QStringList legacyAIPlayers;
    for (auto it = m_playerAIType.constBegin(); it != m_playerAIType.constEnd(); ++it) {
        if (!it.value()) {
            legacyAIPlayers.append(it.key());
        }
    }
    return legacyAIPlayers;
}

void AISystemIntegrationManager::setGlobalAIConfig(const QVariantMap &config)
{
    m_globalConfig = config;
    
    // Apply configuration to new AI system
    if (m_newAIManager) {
        // 這裡可以將配置應用到新AI系統
        // 例如：設置更新頻率、調試模式等
        
        if (config.contains("updateFrequency")) {
            int frequency = config["updateFrequency"].toInt();
            m_newAIManager->setAllPlayersUpdateFrequency(frequency);
        }
        
        if (config.contains("debugMode")) {
            bool debug = config["debugMode"].toBool();
            enableDebugMode(debug);
        }
    }
    
    logSystemEvent("Configuration Updated", "Global AI configuration updated");
    
    if (m_debugMode) {
        qDebug() << "⚙️ Global AI configuration updated";
    }
}

QVariantMap AISystemIntegrationManager::getGlobalAIConfig() const
{
    return m_globalConfig;
}

bool AISystemIntegrationManager::reloadConfiguration()
{
    try {
        QFile configFile(m_configFilePath);
        if (configFile.open(QIODevice::ReadOnly)) {
            QByteArray data = configFile.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (doc.isObject()) {
                QJsonObject configObj = doc.object();
                QVariantMap config = configObj.toVariantMap();
                setGlobalAIConfig(config);
                
                logSystemEvent("Configuration Reloaded", "Configuration reloaded from file");
                
                if (m_debugMode) {
                    qDebug() << "📄 Configuration reloaded from:" << m_configFilePath;
                }
                
                return true;
            }
        }
        
        qDebug() << "⚠️ Failed to reload configuration from:" << m_configFilePath;
        return false;
        
    } catch (const std::exception &e) {
        qDebug() << "❌ Error reloading configuration:" << e.what();
        emit systemError(QString("Configuration reload failed: %1").arg(e.what()));
        return false;
    }
}

void AISystemIntegrationManager::enableDebugMode(bool enabled)
{
    m_debugMode = enabled;
    
    // 啟用新AI系統的調試模式
    if (m_newAIManager) {
        auto playerIds = getNewAIPlayerList();
        for (const QString &playerId : playerIds) {
            auto player = m_newAIManager->getPlayer(playerId);
            if (player) {
                player->enableDebug(enabled);
            }
        }
    }
    
    logSystemEvent("Debug Mode Changed", 
                 QString("Debug mode %1").arg(enabled ? "enabled" : "disabled"));
    
    qDebug() << "🐛 Debug mode" << (enabled ? "enabled" : "disabled");
}

AISystemIntegrationManager::SystemStats AISystemIntegrationManager::getSystemStats() const
{
    // 更新統計資料
    m_stats.totalAIPlayers = m_playerAIType.size();
    m_stats.newAIPlayers = getNewAIPlayerList().size();
    m_stats.legacyAIPlayers = getLegacyAIPlayerList().size();
    m_stats.lastUpdateTime = QDateTime::currentDateTime();
    
    // 計算平均決策時間
    if (!m_playerDecisionTime.isEmpty()) {
        double totalTime = 0.0;
        for (auto time : m_playerDecisionTime.values()) {
            totalTime += time;
        }
        m_stats.averageDecisionTime = totalTime / m_playerDecisionTime.size();
    }
    
    // 計算總決策數
    m_stats.totalDecisions = 0;
    for (auto count : m_playerDecisionCount.values()) {
        m_stats.totalDecisions += count;
    }
    
    return m_stats;
}

QString AISystemIntegrationManager::getDetailedReport() const
{
    auto stats = getSystemStats();
    
    QJsonObject report;
    report["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    report["systemRunning"] = m_isRunning;
    report["debugMode"] = m_debugMode;
    report["uptime"] = m_startTime.secsTo(QDateTime::currentDateTime());
    
    QJsonObject statsObj;
    statsObj["totalAIPlayers"] = stats.totalAIPlayers;
    statsObj["newAIPlayers"] = stats.newAIPlayers;
    statsObj["legacyAIPlayers"] = stats.legacyAIPlayers;
    statsObj["averageDecisionTime"] = stats.averageDecisionTime;
    statsObj["totalDecisions"] = stats.totalDecisions;
    statsObj["totalErrors"] = stats.totalErrors;
    
    report["statistics"] = statsObj;
    
    // 添加玩家詳細資訊
    QJsonArray playersArray;
    for (auto it = m_playerAIType.constBegin(); it != m_playerAIType.constEnd(); ++it) {
        QJsonObject playerObj;
        playerObj["id"] = it.key();
        playerObj["aiType"] = it.value() ? "new" : "legacy";
        playerObj["decisionCount"] = m_playerDecisionCount.value(it.key(), 0);
        playerObj["averageDecisionTime"] = m_playerDecisionTime.value(it.key(), 0.0);
        playerObj["lastUpdate"] = m_playerLastUpdate.value(it.key()).toString(Qt::ISODate);
        playersArray.append(playerObj);
    }
    report["players"] = playersArray;
    
    QJsonDocument doc(report);
    return doc.toJson(QJsonDocument::Indented);
}

bool AISystemIntegrationManager::exportSystemLog(const QString &filePath) const
{
    try {
        QFile logFile(filePath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&logFile);
            
            stream << "RANOnline AI System Integration Log\n";
            stream << "Generated: " << QDateTime::currentDateTime().toString() << "\n";
            stream << "========================================\n\n";
            
            for (const QString &logEntry : m_systemLog) {
                stream << logEntry << "\n";
            }
            
            stream << "\n========================================\n";
            stream << "Detailed Report:\n";
            stream << getDetailedReport();
            
            return true;
        }
        
        return false;
        
    } catch (const std::exception &e) {
        qDebug() << "❌ Failed to export system log:" << e.what();
        return false;
    }
}

// ===== 私有槽函數 =====

void AISystemIntegrationManager::onUpdateTimer()
{
    if (!m_isRunning) {
        return;
    }
    
    try {
        updateStatistics();
        
        auto stats = getSystemStats();
        emit performanceStatsUpdated(stats);
        
        // 檢查性能警告
        if (stats.averageDecisionTime > 10.0) { // 超過10ms
            emit performanceWarning("SYSTEM", "Average decision time too high");
        }
        
        if (m_debugMode && (QDateTime::currentDateTime().second() % 10 == 0)) {
            qDebug() << "📊 System stats - Total:" << stats.totalAIPlayers 
                     << "New:" << stats.newAIPlayers 
                     << "Legacy:" << stats.legacyAIPlayers
                     << "Avg time:" << QString::number(stats.averageDecisionTime, 'f', 2) << "ms";
        }
        
    } catch (const std::exception &e) {
        qDebug() << "❌ Error in update timer:" << e.what();
        emit systemError(QString("Update timer error: %1").arg(e.what()));
    }
}

void AISystemIntegrationManager::onNewAIPlayerAction(const QString &playerId, const AIAction &action)
{
    // 處理新AI系統的玩家行為
    handleAIPlayerAction(playerId, action);
    
    // 轉換並發送信號
    QVariantMap actionMap = AIActionConverter::convertNewToLegacy(action);
    emit aiActionExecuted(playerId, actionMap["type"].toString(), actionMap["parameters"].toMap());
    
    // 更新統計
    m_playerDecisionCount[playerId]++;
    m_playerLastUpdate[playerId] = QDateTime::currentDateTime();
    
    if (m_debugMode) {
        qDebug() << "🎯 New AI action:" << playerId << "Type:" << static_cast<int>(action.type);
    }
}

void AISystemIntegrationManager::onLegacyAIPlayerAction(const QString &playerId, const QVariantMap &action)
{
    // 處理舊AI系統的玩家行為
    emit aiActionExecuted(playerId, action["type"].toString(), action["parameters"].toMap());
    
    // 更新統計
    m_playerDecisionCount[playerId]++;
    m_playerLastUpdate[playerId] = QDateTime::currentDateTime();
    
    if (m_debugMode) {
        qDebug() << "🎯 Legacy AI action:" << playerId << "Type:" << action["type"].toString();
    }
}

void AISystemIntegrationManager::onAIPlayerError(const QString &playerId, const QString &error)
{
    m_stats.totalErrors++;
    
    logSystemEvent("AI Player Error", QString("Player %1: %2").arg(playerId, error));
    emit systemError(QString("AI Player %1 error: %2").arg(playerId, error));
    
    if (m_debugMode) {
        qDebug() << "❌ AI Player error:" << playerId << "-" << error;
    }
}

// ===== 私有方法 =====

void AISystemIntegrationManager::setupConnections()
{
    if (m_newAIManager) {
        // 連接新AI管理器的信號
        connect(m_newAIManager.get(), &AIPlayerManager::playerActionExecuted,
                this, &AISystemIntegrationManager::onNewAIPlayerAction);
        connect(m_newAIManager.get(), &AIPlayerManager::playerErrorOccurred,
                this, &AISystemIntegrationManager::onAIPlayerError);
    }
    
    // 這裡可以添加與舊AI系統的連接
}

void AISystemIntegrationManager::updateStatistics()
{
    // 更新各種統計資料
    m_stats.lastUpdateTime = QDateTime::currentDateTime();
    
    // 計算平均更新頻率
    auto now = QDateTime::currentDateTime();
    qint64 uptime = m_startTime.secsTo(now);
    if (uptime > 0) {
        m_stats.averageUpdateFrequency = static_cast<double>(m_stats.totalDecisions) / uptime;
    }
}

void AISystemIntegrationManager::handleAIPlayerAction(const QString &playerId, const AIAction &action)
{
    // 處理AI玩家行為
    forwardActionToGame(playerId, action);
    
    // 記錄行為執行時間
    // 這裡可以記錄更詳細的性能數據
}

void AISystemIntegrationManager::forwardActionToGame(const QString &playerId, const AIAction &action)
{
    // 將AI行為轉發到遊戲系統
    // 這裡需要根據具體的遊戲系統接口來實現
    
    // 示例：根據行為類型轉發到不同的遊戲系統
    switch (action.type) {
        case ActionType::MOVE:
            // 轉發移動命令
            break;
        case ActionType::ATTACK:
            // 轉發攻擊命令
            break;
        case ActionType::USE_SKILL:
            // 轉發技能使用命令
            break;
        case ActionType::USE_ITEM:
            // 轉發物品使用命令
            break;
        default:
            break;
    }
}

void AISystemIntegrationManager::migratePlayerData(const QString &playerId, bool toNewAI)
{
    // 遷移玩家數據
    if (toNewAI) {
        // 從舊AI系統遷移到新AI系統
        // 可能需要轉換數據格式
    } else {
        // 從新AI系統遷移到舊AI系統
        // 可能需要簡化數據
    }
}

void AISystemIntegrationManager::logSystemEvent(const QString &event, const QString &details)
{
    QString logEntry = QString("[%1] %2: %3")
                      .arg(QDateTime::currentDateTime().toString())
                      .arg(event)
                      .arg(details);
    
    m_systemLog.append(logEntry);
    
    // 限制日誌大小
    if (m_systemLog.size() > 1000) {
        m_systemLog.removeFirst();
    }
}

// ========================================================================
// AIActionConverter 實現
// ========================================================================

QHash<ActionType, QString> AIActionConverter::s_actionTypeMap;
QHash<QString, ActionType> AIActionConverter::s_reverseActionTypeMap;
bool AIActionConverter::s_mappingsInitialized = false;

AIActionConverter::AIActionConverter(QObject *parent)
    : QObject(parent)
{
    if (!s_mappingsInitialized) {
        initializeMappings();
    }
}

void AIActionConverter::initializeMappings()
{
    s_actionTypeMap[ActionType::MOVE] = "MOVE";
    s_actionTypeMap[ActionType::ATTACK] = "ATTACK";
    s_actionTypeMap[ActionType::USE_SKILL] = "USE_SKILL";
    s_actionTypeMap[ActionType::USE_ITEM] = "USE_ITEM";
    s_actionTypeMap[ActionType::INTERACT] = "INTERACT";
    s_actionTypeMap[ActionType::WAIT] = "WAIT";
    s_actionTypeMap[ActionType::FLEE] = "FLEE";
    
    // 創建反向映射
    for (auto it = s_actionTypeMap.constBegin(); it != s_actionTypeMap.constEnd(); ++it) {
        s_reverseActionTypeMap[it.value()] = it.key();
    }
    
    s_mappingsInitialized = true;
}

QVariantMap AIActionConverter::convertNewToLegacy(const AIAction &newAction)
{
    if (!s_mappingsInitialized) {
        const_cast<AIActionConverter*>(nullptr)->initializeMappings();
    }
    
    QVariantMap legacy;
    legacy["type"] = s_actionTypeMap.value(newAction.type, "UNKNOWN");
    legacy["confidence"] = static_cast<double>(newAction.confidence);
    legacy["priority"] = static_cast<double>(newAction.priority);
    legacy["isValid"] = newAction.isValid;
    
    QVariantList params;
    for (const std::string &param : newAction.parameters) {
        params.append(QString::fromStdString(param));
    }
    legacy["parameters"] = params;
    
    return legacy;
}

AIAction AIActionConverter::convertLegacyToNew(const QVariantMap &legacyAction)
{
    if (!s_mappingsInitialized) {
        const_cast<AIActionConverter*>(nullptr)->initializeMappings();
    }
    
    AIAction newAction;
    
    QString typeStr = legacyAction["type"].toString();
    newAction.type = s_reverseActionTypeMap.value(typeStr, ActionType::WAIT);
    newAction.confidence = static_cast<float>(legacyAction["confidence"].toDouble());
    newAction.priority = static_cast<float>(legacyAction["priority"].toDouble());
    newAction.isValid = legacyAction["isValid"].toBool();
    
    QVariantList params = legacyAction["parameters"].toList();
    for (const QVariant &param : params) {
        newAction.parameters.push_back(param.toString().toStdString());
    }
    
    return newAction;
}

QVariantMap AIActionConverter::convertPerceptionToLegacy(const PerceptionData &perception)
{
    QVariantMap legacy;
    
    legacy["health"] = static_cast<double>(perception.health);
    legacy["maxHealth"] = static_cast<double>(perception.maxHealth);
    legacy["mana"] = static_cast<double>(perception.mana);
    legacy["maxMana"] = static_cast<double>(perception.maxMana);
    legacy["level"] = perception.level;
    legacy["threatLevel"] = static_cast<double>(perception.threatLevel);
    
    QVariantList enemies;
    for (const EntityInfo &enemy : perception.nearbyEnemies) {
        QVariantMap enemyMap;
        enemyMap["id"] = QString::fromStdString(enemy.id);
        enemyMap["health"] = static_cast<double>(enemy.health);
        enemyMap["level"] = enemy.level;
        enemyMap["threatRating"] = static_cast<double>(enemy.threatRating);
        enemies.append(enemyMap);
    }
    legacy["nearbyEnemies"] = enemies;
    
    return legacy;
}

PerceptionData AIActionConverter::convertPerceptionFromLegacy(const QVariantMap &legacyPerception)
{
    PerceptionData perception;
    
    perception.health = static_cast<float>(legacyPerception["health"].toDouble());
    perception.maxHealth = static_cast<float>(legacyPerception["maxHealth"].toDouble());
    perception.mana = static_cast<float>(legacyPerception["mana"].toDouble());
    perception.maxMana = static_cast<float>(legacyPerception["maxMana"].toDouble());
    perception.level = legacyPerception["level"].toInt();
    perception.threatLevel = static_cast<float>(legacyPerception["threatLevel"].toDouble());
    
    QVariantList enemies = legacyPerception["nearbyEnemies"].toList();
    for (const QVariant &enemyVar : enemies) {
        QVariantMap enemyMap = enemyVar.toMap();
        EntityInfo enemy;
        enemy.id = enemyMap["id"].toString().toStdString();
        enemy.health = static_cast<float>(enemyMap["health"].toDouble());
        enemy.level = enemyMap["level"].toInt();
        enemy.threatRating = static_cast<float>(enemyMap["threatRating"].toDouble());
        perception.nearbyEnemies.push_back(enemy);
    }
    
    return perception;
}

// ========================================================================
// AISystemBridge 實現
// ========================================================================

AISystemBridge::AISystemBridge(AISystemIntegrationManager *manager, QObject *parent)
    : QObject(parent)
    , m_manager(manager)
{
}

QVariantMap AISystemBridge::makeAIDecision(const QString &playerId, const QVariantMap &gameState)
{
    if (!m_manager || !m_manager->isInitialized()) {
        return QVariantMap();
    }
    
    // 轉換遊戲狀態為感知數據
    PerceptionData perception = AIActionConverter::convertPerceptionFromLegacy(gameState);
    
    // 執行AI決策 (這裡需要實現具體的決策邏輯)
    // 目前返回一個示例決策
    AIAction action;
    action.type = ActionType::WAIT;
    action.confidence = 0.5f;
    action.isValid = true;
    
    // 轉換為統一格式
    QVariantMap decision = AIActionConverter::convertNewToLegacy(action);
    
    emit aiDecisionMade(playerId, decision);
    
    return decision;
}

void AISystemBridge::updateAIState(const QString &playerId, const QVariantMap &stateUpdate)
{
    // 更新AI狀態
    // 這裡可以實現狀態更新邏輯
    
    emit aiStateUpdated(playerId, stateUpdate);
}

void AISystemBridge::provideAIFeedback(const QString &playerId, const QVariantMap &action, double reward)
{
    // 提供AI學習回饋
    // 如果是使用新AI系統的玩家，轉發學習回饋
    if (m_manager && m_manager->getNewAIPlayerList().contains(playerId)) {
        // 實現學習回饋邏輯
    }
}

QVariantMap AISystemBridge::getAIState(const QString &playerId) const
{
    QVariantMap state;
    
    if (m_manager && m_manager->isInitialized()) {
        auto playerList = m_manager->getAIPlayerList();
        if (playerList.contains(playerId)) {
            bool isNewAI = m_manager->getNewAIPlayerList().contains(playerId);
            state["aiType"] = isNewAI ? "new" : "legacy";
            state["exists"] = true;
        } else {
            state["exists"] = false;
        }
    } else {
        state["exists"] = false;
    }
    
    return state;
}

void AISystemBridge::setAIParameters(const QString &playerId, const QVariantMap &parameters)
{
    // 設置AI參數
    if (m_manager && m_manager->getNewAIPlayerList().contains(playerId)) {
        // 為新AI系統設置參數
        if (parameters.contains("strategy")) {
            int strategyInt = parameters["strategy"].toInt();
            auto strategy = static_cast<DecisionStrategyType>(strategyInt);
            m_manager->setPlayerStrategy(playerId, strategy);
        }
    }
}

} // namespace JyAI

#include "AISystemIntegration.moc"
