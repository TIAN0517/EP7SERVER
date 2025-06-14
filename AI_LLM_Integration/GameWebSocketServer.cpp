/**
 * @file GameWebSocketServer.cpp
 * @brief RANOnline EP7 遊戲服務端 WebSocket 服務器實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "GameWebSocketServer.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtCore/QDateTime>
#include <QtCore/QRandomGenerator>

namespace RANOnline {
namespace AI {

// ==== AICharacterManager 實現 ====

AICharacterManager::AICharacterManager(QObject *parent)
    : QObject(parent)
    , m_updateTimer(new QTimer(this))
{
    // 設置AI邏輯更新定時器（每秒更新一次）
    m_updateTimer->setInterval(1000);
    m_updateTimer->setSingleShot(false);
    connect(m_updateTimer, &QTimer::timeout, this, &AICharacterManager::updateAILogic);
    m_updateTimer->start();
    
    qDebug() << "AICharacterManager initialized";
}

QStringList AICharacterManager::spawnAI(Academy academy, Department department, int count, int teamId)
{
    QMutexLocker locker(&m_dataMutex);
    
    QStringList newAIs;
    
    for (int i = 0; i < count; ++i) {
        AICharacterInfo ai;
        ai.aiId = generateAIId();
        ai.academy = academy;
        ai.department = department;
        ai.teamId = teamId;
        ai.name = generateAIName(academy, department);
        ai.level = 1;
        ai.hp = 1000;
        ai.maxHp = 1000;
        ai.mp = 500;
        ai.maxMp = 500;
        ai.position = generateRandomPosition();
        ai.state = AIState::IDLE;
        ai.lastUpdate = QDateTime::currentDateTime();
        
        m_aiCharacters[ai.aiId] = ai;
        newAIs.append(ai.aiId);
        
        // 添加到隊伍
        if (teamId > 0) {
            if (!m_teams.contains(teamId)) {
                m_teams[teamId] = QStringList();
            }
            m_teams[teamId].append(ai.aiId);
        }
    }
    
    qDebug() << "Spawned" << count << "AI characters for academy:" 
             << ProtocolUtils::academyToString(academy) 
             << "department:" << ProtocolUtils::departmentToString(department);
    
    return newAIs;
}

bool AICharacterManager::deleteAI(const QString &aiId)
{
    QMutexLocker locker(&m_dataMutex);
    
    if (!m_aiCharacters.contains(aiId)) {
        return false;
    }
    
    AICharacterInfo ai = m_aiCharacters[aiId];
    m_aiCharacters.remove(aiId);
    
    // 從隊伍中移除
    if (ai.teamId > 0 && m_teams.contains(ai.teamId)) {
        m_teams[ai.teamId].removeOne(aiId);
        if (m_teams[ai.teamId].isEmpty()) {
            m_teams.remove(ai.teamId);
        }
    }
    
    qDebug() << "Deleted AI:" << aiId;
    return true;
}

bool AICharacterManager::deleteTeam(int teamId)
{
    QMutexLocker locker(&m_dataMutex);
    
    if (!m_teams.contains(teamId)) {
        return false;
    }
    
    QStringList teamMembers = m_teams[teamId];
    
    for (const QString &aiId : teamMembers) {
        m_aiCharacters.remove(aiId);
    }
    
    m_teams.remove(teamId);
    
    qDebug() << "Deleted team:" << teamId << "with" << teamMembers.size() << "members";
    return true;
}

bool AICharacterManager::moveAI(const QString &aiId, const Position &target)
{
    QMutexLocker locker(&m_dataMutex);
    
    if (!m_aiCharacters.contains(aiId)) {
        return false;
    }
    
    AICharacterInfo &ai = m_aiCharacters[aiId];
    
    if (ai.state == AIState::DEAD) {
        return false;
    }
    
    ai.position = target;
    ai.state = AIState::MOVING;
    ai.lastUpdate = QDateTime::currentDateTime();
    
    emit aiStateChanged(aiId, ai.state, ai.position);
    
    qDebug() << "AI" << aiId << "moving to" << target.x << target.y;
    return true;
}

bool AICharacterManager::attackTarget(const QString &aiId, const QString &targetId)
{
    QMutexLocker locker(&m_dataMutex);
    
    if (!m_aiCharacters.contains(aiId) || !m_aiCharacters.contains(targetId)) {
        return false;
    }
    
    AICharacterInfo &attacker = m_aiCharacters[aiId];
    AICharacterInfo &target = m_aiCharacters[targetId];
    
    if (attacker.state == AIState::DEAD || target.state == AIState::DEAD) {
        return false;
    }
    
    attacker.state = AIState::FIGHTING;
    attacker.lastUpdate = QDateTime::currentDateTime();
    
    // 模擬戰鬥
    int damage = QRandomGenerator::global()->bounded(50, 150);
    target.hp = qMax(0, target.hp - damage);
    
    if (target.hp <= 0) {
        target.state = AIState::DEAD;
        emit aiDeath(targetId, aiId);
    }
    
    QJsonObject battleData{
        {"attacker", aiId},
        {"target", targetId},
        {"damage", damage},
        {"target_hp", target.hp}
    };
    
    emit aiBattleEvent(aiId, "attack", battleData);
    emit aiStateChanged(aiId, attacker.state, attacker.position);
    
    qDebug() << "AI" << aiId << "attacked" << targetId << "for" << damage << "damage";
    return true;
}

bool AICharacterManager::useSkill(const QString &aiId, const QString &skillId, const QJsonObject &params)
{
    QMutexLocker locker(&m_dataMutex);
    
    if (!m_aiCharacters.contains(aiId)) {
        return false;
    }
    
    AICharacterInfo &ai = m_aiCharacters[aiId];
    
    if (ai.state == AIState::DEAD || ai.mp < 50) {
        return false;
    }
    
    ai.state = AIState::USING_SKILL;
    ai.mp = qMax(0, ai.mp - 50);
    ai.lastUpdate = QDateTime::currentDateTime();
    
    QJsonObject skillData{
        {"skill_id", skillId},
        {"caster", aiId},
        {"mp_cost", 50},
        {"remaining_mp", ai.mp},
        {"params", params}
    };
    
    emit aiBattleEvent(aiId, "skill_use", skillData);
    emit aiStateChanged(aiId, ai.state, ai.position);
    
    qDebug() << "AI" << aiId << "used skill" << skillId;
    return true;
}

bool AICharacterManager::assignTeam(const QStringList &aiIds, int teamId)
{
    QMutexLocker locker(&m_dataMutex);
    
    // 清理目標隊伍
    if (m_teams.contains(teamId)) {
        for (const QString &oldId : m_teams[teamId]) {
            if (m_aiCharacters.contains(oldId)) {
                m_aiCharacters[oldId].teamId = 0;
            }
        }
    }
    
    m_teams[teamId] = QStringList();
    
    // 分配新成員
    for (const QString &aiId : aiIds) {
        if (m_aiCharacters.contains(aiId)) {
            // 從舊隊伍移除
            int oldTeam = m_aiCharacters[aiId].teamId;
            if (oldTeam > 0 && m_teams.contains(oldTeam)) {
                m_teams[oldTeam].removeOne(aiId);
                if (m_teams[oldTeam].isEmpty()) {
                    m_teams.remove(oldTeam);
                }
            }
            
            // 添加到新隊伍
            m_aiCharacters[aiId].teamId = teamId;
            m_teams[teamId].append(aiId);
        }
    }
    
    qDebug() << "Assigned" << aiIds.size() << "AIs to team" << teamId;
    return true;
}

QList<AICharacterInfo> AICharacterManager::getAIStatus(const QStringList &aiIds)
{
    QMutexLocker locker(&m_dataMutex);
    
    QList<AICharacterInfo> result;
    
    if (aiIds.isEmpty()) {
        // 返回所有AI
        result = m_aiCharacters.values();
    } else {
        // 返回指定AI
        for (const QString &aiId : aiIds) {
            if (m_aiCharacters.contains(aiId)) {
                result.append(m_aiCharacters[aiId]);
            }
        }
    }
    
    return result;
}

AICharacterInfo AICharacterManager::getAIInfo(const QString &aiId)
{
    QMutexLocker locker(&m_dataMutex);
    return m_aiCharacters.value(aiId);
}

QStringList AICharacterManager::getTeamMembers(int teamId)
{
    QMutexLocker locker(&m_dataMutex);
    return m_teams.value(teamId);
}

void AICharacterManager::pauseAllAI()
{
    m_systemPaused = true;
    m_updateTimer->stop();
    qDebug() << "All AI paused";
}

void AICharacterManager::resumeAllAI()
{
    m_systemPaused = false;
    m_updateTimer->start();
    qDebug() << "All AI resumed";
}

void AICharacterManager::resetAllAI()
{
    QMutexLocker locker(&m_dataMutex);
    
    m_aiCharacters.clear();
    m_teams.clear();
    m_nextAIId = 10001;
    m_nextTeamId = 1;
    
    qDebug() << "All AI reset";
}

int AICharacterManager::getTotalAICount() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_aiCharacters.size();
}

int AICharacterManager::getActiveAICount() const
{
    QMutexLocker locker(&m_dataMutex);
    
    int count = 0;
    for (const auto &ai : m_aiCharacters) {
        if (ai.state != AIState::DEAD) {
            count++;
        }
    }
    return count;
}

int AICharacterManager::getTeamCount() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_teams.size();
}

void AICharacterManager::updateAILogic()
{
    if (m_systemPaused) {
        return;
    }
    
    QMutexLocker locker(&m_dataMutex);
    
    for (auto &ai : m_aiCharacters) {
        simulateAIBehavior(ai);
    }
}

QString AICharacterManager::generateAIId()
{
    return QString("AI_%1").arg(m_nextAIId++);
}

QString AICharacterManager::generateAIName(Academy academy, Department department)
{
    QStringList names = {
        "戰士", "守護者", "劍客", "弓手", "法師", "武者", "忍者", "聖騎士"
    };
    
    QString academyPrefix = ProtocolUtils::academyToString(academy).left(1);
    QString deptPrefix = ProtocolUtils::departmentToString(department).left(1);
    QString baseName = names[QRandomGenerator::global()->bounded(names.size())];
    
    return QString("%1%2_%3_%4")
        .arg(academyPrefix)
        .arg(deptPrefix)
        .arg(baseName)
        .arg(QRandomGenerator::global()->bounded(1000, 9999));
}

Position AICharacterManager::generateRandomPosition()
{
    return Position{
        QRandomGenerator::global()->bounded(0, 1000),
        QRandomGenerator::global()->bounded(0, 1000),
        0
    };
}

void AICharacterManager::simulateAIBehavior(AICharacterInfo &ai)
{
    if (ai.state == AIState::DEAD) {
        return;
    }
    
    // 模擬AI自動行為
    switch (ai.state) {
        case AIState::IDLE: {
            // 隨機移動或進入戰鬥
            if (QRandomGenerator::global()->bounded(100) < 10) { // 10%機率移動
                ai.position = generateRandomPosition();
                ai.state = AIState::MOVING;
                emit aiStateChanged(ai.aiId, ai.state, ai.position);
            }
            break;
        }
        
        case AIState::MOVING: {
            // 移動完成，回到空閒
            if (QRandomGenerator::global()->bounded(100) < 30) { // 30%機率停止移動
                ai.state = AIState::IDLE;
                emit aiStateChanged(ai.aiId, ai.state, ai.position);
            }
            break;
        }
        
        case AIState::FIGHTING: {
            // 戰鬥結束，回到空閒
            if (QRandomGenerator::global()->bounded(100) < 20) { // 20%機率結束戰鬥
                ai.state = AIState::IDLE;
                emit aiStateChanged(ai.aiId, ai.state, ai.position);
            }
            break;
        }
        
        case AIState::USING_SKILL: {
            // 技能使用完成
            ai.state = AIState::IDLE;
            emit aiStateChanged(ai.aiId, ai.state, ai.position);
            break;
        }
        
        default:
            break;
    }
    
    // MP恢復
    if (ai.mp < ai.maxMp) {
        ai.mp = qMin(ai.maxMp, ai.mp + 10);
    }
    
    // HP緩慢恢復
    if (ai.hp < ai.maxHp && ai.state != AIState::FIGHTING) {
        ai.hp = qMin(ai.maxHp, ai.hp + 5);
    }
    
    ai.lastUpdate = QDateTime::currentDateTime();
}

// ==== GameWebSocketServer 實現 ====

GameWebSocketServer::GameWebSocketServer(QObject *parent)
    : QObject(parent)
    , m_server(std::make_unique<QWebSocketServer>("RANOnline AI Game Server", 
                                                  QWebSocketServer::NonSecureMode, this))
    , m_aiManager(std::make_unique<AICharacterManager>(this))
    , m_cleanupTimer(new QTimer(this))
{
    // 連接AI管理器信號
    connect(m_aiManager.get(), &AICharacterManager::aiStateChanged,
            this, &GameWebSocketServer::onAIStateChanged);
    connect(m_aiManager.get(), &AICharacterManager::aiBattleEvent,
            this, &GameWebSocketServer::onAIBattleEvent);
    connect(m_aiManager.get(), &AICharacterManager::aiLevelUp,
            this, &GameWebSocketServer::onAILevelUp);
    connect(m_aiManager.get(), &AICharacterManager::aiDeath,
            this, &GameWebSocketServer::onAIDeath);
    
    // 設置清理定時器
    m_cleanupTimer->setInterval(60000); // 每分鐘清理一次
    connect(m_cleanupTimer, &QTimer::timeout, this, &GameWebSocketServer::onCleanupTimer);
    
    qDebug() << "GameWebSocketServer initialized";
}

GameWebSocketServer::~GameWebSocketServer()
{
    stopServer();
}

bool GameWebSocketServer::startServer(const QString &host, quint16 port)
{
    if (m_server->isListening()) {
        qWarning() << "Server is already listening";
        return false;
    }
    
    QHostAddress address = (host == "localhost") ? QHostAddress::LocalHost : QHostAddress(host);
    
    if (!m_server->listen(address, port)) {
        QString error = m_server->errorString();
        qCritical() << "Failed to start server:" << error;
        emit errorOccurred(error);
        return false;
    }
    
    connect(m_server.get(), &QWebSocketServer::newConnection,
            this, &GameWebSocketServer::onNewConnection);
    
    m_stats.startTime = QDateTime::currentDateTime();
    m_cleanupTimer->start();
    
    QString actualHost = m_server->serverAddress().toString();
    quint16 actualPort = m_server->serverPort();
    
    qDebug() << "Server started on" << actualHost << ":" << actualPort;
    emit serverStarted(actualHost, actualPort);
    
    return true;
}

void GameWebSocketServer::stopServer()
{
    if (!m_server->isListening()) {
        return;
    }
    
    qDebug() << "Stopping server...";
    
    m_cleanupTimer->stop();
    
    // 關閉所有客戶端連接
    QMutexLocker locker(&m_clientsMutex);
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        QWebSocket *client = it.key();
        client->close();
    }
    m_clients.clear();
    locker.unlock();
    
    m_server->close();
    
    qDebug() << "Server stopped";
    emit serverStopped();
}

bool GameWebSocketServer::isListening() const
{
    return m_server->isListening();
}

quint16 GameWebSocketServer::serverPort() const
{
    return m_server->serverPort();
}

QHostAddress GameWebSocketServer::serverAddress() const
{
    return m_server->serverAddress();
}

int GameWebSocketServer::getConnectedClients() const
{
    QMutexLocker locker(&m_clientsMutex);
    return m_clients.size();
}

qint64 GameWebSocketServer::getTotalMessages() const
{
    return m_stats.totalMessages;
}

void GameWebSocketServer::onNewConnection()
{
    QWebSocket *client = m_server->nextPendingConnection();
    
    if (!client) {
        return;
    }
    
    QString clientInfo = getClientInfo(client);
    
    QMutexLocker locker(&m_clientsMutex);
    m_clients[client] = clientInfo;
    m_stats.totalConnections++;
    m_stats.currentConnections++;
    locker.unlock();
    
    connect(client, &QWebSocket::textMessageReceived,
            this, &GameWebSocketServer::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected,
            this, &GameWebSocketServer::onClientDisconnected);
    
    qDebug() << "New client connected:" << clientInfo;
    emit clientConnected(clientInfo);
}

void GameWebSocketServer::onClientDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    if (!client) {
        return;
    }
    
    QMutexLocker locker(&m_clientsMutex);
    QString clientInfo = m_clients.value(client, "Unknown");
    m_clients.remove(client);
    m_stats.currentConnections--;
    locker.unlock();
    
    client->deleteLater();
    
    qDebug() << "Client disconnected:" << clientInfo;
    emit clientDisconnected(clientInfo);
}

void GameWebSocketServer::onTextMessageReceived(const QString &message)
{
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    if (!client) {
        return;
    }
    
    m_stats.totalMessages++;
    
    QString clientInfo = getClientInfo(client);
    
    qDebug() << "Message from" << clientInfo << ":" << message.left(100) << "...";
    emit messageReceived(clientInfo, message);
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        sendError(client, "", "Invalid JSON format", 400);
        return;
    }
    
    QJsonObject request = doc.object();
    processRequest(client, request);
}

void GameWebSocketServer::onCleanupTimer()
{
    cleanup();
}

void GameWebSocketServer::processRequest(QWebSocket *client, const QJsonObject &request)
{
    QString cmd = request["cmd"].toString();
    QString requestId = request["request_id"].toString();
    
    if (cmd.isEmpty()) {
        sendError(client, requestId, "Missing command", 400);
        return;
    }
    
    try {
        if (cmd == Commands::SPAWN_AI) {
            processSpawnRequest(client, request);
        } else if (cmd == Commands::AI_COMMAND) {
            processCommandRequest(client, request);
        } else if (cmd == Commands::ASSIGN_TEAM) {
            processTeamRequest(client, request);
        } else if (cmd == Commands::GET_STATUS) {
            processStatusRequest(client, request);
        } else if (cmd == Commands::BATCH_OPERATION) {
            processBatchRequest(client, request);
        } else if (cmd == Commands::DELETE_AI) {
            processDeleteRequest(client, request);
        } else if (cmd == Commands::SYSTEM_CONTROL) {
            processSystemRequest(client, request);
        } else if (cmd == "heartbeat") {
            // 心跳響應
            QJsonObject response{
                {"cmd", "heartbeat"},
                {"status", "alive"},
                {"timestamp", QDateTime::currentMSecsSinceEpoch()}
            };
            QJsonDocument doc(response);
            client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
        } else {
            sendError(client, requestId, "Unknown command: " + cmd, 404);
        }
        
        m_stats.successfulRequests++;
    } catch (const std::exception &e) {
        m_stats.failedRequests++;
        sendError(client, requestId, QString("Internal error: %1").arg(e.what()), 500);
    }
}

void GameWebSocketServer::processSpawnRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QString academyStr = request["academy"].toString();
    QString departmentStr = request["department"].toString();
    int count = request["count"].toInt();
    int teamId = request["team"].toInt();
    
    if (academyStr.isEmpty() || departmentStr.isEmpty() || count <= 0) {
        sendError(client, requestId, "Invalid spawn parameters", 400);
        return;
    }
    
    Academy academy = ProtocolUtils::stringToAcademy(academyStr);
    Department department = ProtocolUtils::stringToDepartment(departmentStr);
    
    QStringList aiIds = m_aiManager->spawnAI(academy, department, count, teamId);
    
    QJsonArray aiList;
    for (const QString &aiId : aiIds) {
        AICharacterInfo info = m_aiManager->getAIInfo(aiId);
        aiList.append(info.toJson());
    }
    
    QJsonObject data{
        {"ai_list", aiList},
        {"count", aiIds.size()}
    };
    
    sendResponse(client, requestId, data);
}

void GameWebSocketServer::processCommandRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QString aiId = request["ai_id"].toString();
    QString action = request["action"].toString();
    QJsonObject params = request["params"].toObject();
    
    bool success = false;
    QString errorMsg;
    
    if (action == Actions::MOVE) {
        Position target = Position::fromJson(params["target"].toObject());
        success = m_aiManager->moveAI(aiId, target);
        errorMsg = success ? "" : "Failed to move AI";
        
    } else if (action == Actions::ATTACK) {
        QString targetId = params["target_id"].toString();
        success = m_aiManager->attackTarget(aiId, targetId);
        errorMsg = success ? "" : "Failed to attack target";
        
    } else if (action == Actions::USE_SKILL) {
        QString skillId = params["skill_id"].toString();
        QJsonObject skillParams = params["skill_params"].toObject();
        success = m_aiManager->useSkill(aiId, skillId, skillParams);
        errorMsg = success ? "" : "Failed to use skill";
        
    } else {
        sendError(client, requestId, "Unknown action: " + action, 400);
        return;
    }
    
    if (success) {
        QJsonObject data{
            {"ai_id", aiId},
            {"action", action},
            {"success", true}
        };
        sendResponse(client, requestId, data);
    } else {
        sendError(client, requestId, errorMsg, 400);
    }
}

void GameWebSocketServer::processTeamRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QJsonArray aiIdsArray = request["ai_ids"].toArray();
    int teamId = request["team"].toInt();
    
    QStringList aiIds;
    for (const QJsonValue &value : aiIdsArray) {
        aiIds.append(value.toString());
    }
    
    bool success = m_aiManager->assignTeam(aiIds, teamId);
    
    if (success) {
        QJsonObject data{
            {"team_id", teamId},
            {"ai_ids", aiIdsArray},
            {"success", true}
        };
        sendResponse(client, requestId, data);
    } else {
        sendError(client, requestId, "Failed to assign team", 400);
    }
}

void GameWebSocketServer::processStatusRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QJsonArray aiIdsArray = request["ai_ids"].toArray();
    
    QStringList aiIds;
    for (const QJsonValue &value : aiIdsArray) {
        aiIds.append(value.toString());
    }
    
    QList<AICharacterInfo> aiStatus = m_aiManager->getAIStatus(aiIds);
    
    QJsonArray statusArray;
    for (const AICharacterInfo &info : aiStatus) {
        statusArray.append(info.toJson());
    }
    
    QJsonObject data{
        {"ai_status", statusArray},
        {"count", statusArray.size()}
    };
    
    sendResponse(client, requestId, data);
}

void GameWebSocketServer::processBatchRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QJsonArray operations = request["operations"].toArray();
    
    QJsonArray results;
    
    for (const QJsonValue &opValue : operations) {
        QJsonObject op = opValue.toObject();
        
        // 遞迴處理每個操作
        // 這裡簡化處理，實際可以更複雜
        QJsonObject result{
            {"operation", op},
            {"success", true}
        };
        results.append(result);
    }
    
    QJsonObject data{
        {"results", results},
        {"count", results.size()}
    };
    
    sendResponse(client, requestId, data);
}

void GameWebSocketServer::processDeleteRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QString aiId = request["ai_id"].toString();
    int teamId = request["team_id"].toInt();
    
    bool success = false;
    
    if (!aiId.isEmpty()) {
        success = m_aiManager->deleteAI(aiId);
    } else if (teamId > 0) {
        success = m_aiManager->deleteTeam(teamId);
    }
    
    if (success) {
        QJsonObject data{
            {"ai_id", aiId},
            {"team_id", teamId},
            {"success", true}
        };
        sendResponse(client, requestId, data);
    } else {
        sendError(client, requestId, "Failed to delete", 400);
    }
}

void GameWebSocketServer::processSystemRequest(QWebSocket *client, const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    QString action = request["action"].toString();
    
    if (action == "pause_all") {
        m_aiManager->pauseAllAI();
    } else if (action == "resume_all") {
        m_aiManager->resumeAllAI();
    } else if (action == "reset_all") {
        m_aiManager->resetAllAI();
    } else {
        sendError(client, requestId, "Unknown system action: " + action, 400);
        return;
    }
    
    QJsonObject data{
        {"action", action},
        {"success", true}
    };
    
    sendResponse(client, requestId, data);
}

void GameWebSocketServer::sendResponse(QWebSocket *client, const QString &requestId, const QJsonObject &data)
{
    QJsonObject response{
        {"status", "success"},
        {"request_id", requestId},
        {"data", data},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    QJsonDocument doc(response);
    client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
}

void GameWebSocketServer::sendError(QWebSocket *client, const QString &requestId, const QString &error, int code)
{
    QJsonObject response{
        {"status", "error"},
        {"request_id", requestId},
        {"error", error},
        {"error_code", code},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    QJsonDocument doc(response);
    client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
}

void GameWebSocketServer::broadcastNotification(const QJsonObject &notification)
{
    QJsonDocument doc(notification);
    QString message = doc.toJson(QJsonDocument::Compact);
    
    QMutexLocker locker(&m_clientsMutex);
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        QWebSocket *client = it.key();
        client->sendTextMessage(message);
    }
}

QString GameWebSocketServer::getClientInfo(QWebSocket *client)
{
    return QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
}

void GameWebSocketServer::updateStatistics()
{
    // 更新統計信息
}

void GameWebSocketServer::cleanup()
{
    // 清理斷開的連接
    QMutexLocker locker(&m_clientsMutex);
    
    auto it = m_clients.begin();
    while (it != m_clients.end()) {
        QWebSocket *client = it.key();
        if (client->state() != QAbstractSocket::ConnectedState) {
            it = m_clients.erase(it);
            client->deleteLater();
        } else {
            ++it;
        }
    }
}

void GameWebSocketServer::onAIStateChanged(const QString &aiId, AIState newState, const Position &position)
{
    QJsonObject notification{
        {"type", "ai_state_change"},
        {"ai_id", aiId},
        {"new_state", ProtocolUtils::stateToString(newState)},
        {"position", position.toJson()},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    broadcastNotification(notification);
}

void GameWebSocketServer::onAIBattleEvent(const QString &aiId, const QString &eventType, const QJsonObject &data)
{
    QJsonObject notification{
        {"type", "battle_event"},
        {"ai_id", aiId},
        {"event_type", eventType},
        {"data", data},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    broadcastNotification(notification);
}

void GameWebSocketServer::onAILevelUp(const QString &aiId, int newLevel)
{
    QJsonObject notification{
        {"type", "system_event"},
        {"event_type", "ai_level_up"},
        {"ai_id", aiId},
        {"new_level", newLevel},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    broadcastNotification(notification);
}

void GameWebSocketServer::onAIDeath(const QString &aiId, const QString &killer)
{
    QJsonObject notification{
        {"type", "system_event"},
        {"event_type", "ai_death"},
        {"ai_id", aiId},
        {"killer", killer},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    broadcastNotification(notification);
}

} // namespace AI
} // namespace RANOnline

#include "GameWebSocketServer.moc"
