/**
 * @file GameWebSocketClient.cpp
 * @brief RANOnline EP7 AI前端 WebSocket 客戶端實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "GameWebSocketClient.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtCore/QDateTime>

namespace RANOnline {
namespace AI {

GameWebSocketClient::GameWebSocketClient(QObject *parent)
    : QObject(parent)
    , m_webSocket(std::make_unique<QWebSocket>())
    , m_heartbeatTimer(new QTimer(this))
    , m_reconnectTimer(new QTimer(this))
    , m_messageQueueTimer(new QTimer(this))
    , m_requestTimeoutTimer(new QTimer(this))
{
    setupTimers();
    setupConnections();
    
    qDebug() << "GameWebSocketClient initialized";
}

GameWebSocketClient::~GameWebSocketClient()
{
    if (m_isConnected) {
        disconnectFromServer();
    }
}

void GameWebSocketClient::setupTimers()
{
    // 心跳定時器
    m_heartbeatTimer->setSingleShot(false);
    m_heartbeatTimer->setInterval(m_heartbeatInterval);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &GameWebSocketClient::onHeartbeatTimer);
    
    // 重連定時器
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &GameWebSocketClient::onReconnectTimer);
    
    // 消息隊列處理定時器
    m_messageQueueTimer->setSingleShot(false);
    m_messageQueueTimer->setInterval(100); // 100ms處理一次
    connect(m_messageQueueTimer, &QTimer::timeout, this, &GameWebSocketClient::processMessageQueue);
    
    // 請求超時定時器
    m_requestTimeoutTimer->setSingleShot(false);
    m_requestTimeoutTimer->setInterval(5000); // 5秒檢查一次
    connect(m_requestTimeoutTimer, &QTimer::timeout, this, &GameWebSocketClient::onRequestTimeout);
}

void GameWebSocketClient::setupConnections()
{
    connect(m_webSocket.get(), &QWebSocket::connected, this, &GameWebSocketClient::onConnected);
    connect(m_webSocket.get(), &QWebSocket::disconnected, this, &GameWebSocketClient::onDisconnected);
    connect(m_webSocket.get(), &QWebSocket::textMessageReceived, this, &GameWebSocketClient::onTextMessageReceived);
    connect(m_webSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &GameWebSocketClient::onError);
}

void GameWebSocketClient::connectToServer(const QString &url)
{
    if (m_isConnected) {
        qWarning() << "Already connected to server";
        return;
    }
    
    m_serverUrl = url;
    qDebug() << "Connecting to game server:" << url;
    
    // 設置請求標頭
    QNetworkRequest request(QUrl(url));
    request.setRawHeader("User-Agent", "RANOnline-AI-Frontend/2.0");
    request.setRawHeader("Origin", "https://ranonline-ai.local");
    
    m_webSocket->open(request);
}

void GameWebSocketClient::disconnectFromServer()
{
    if (!m_isConnected) {
        return;
    }
    
    qDebug() << "Disconnecting from game server";
    
    m_heartbeatTimer->stop();
    m_messageQueueTimer->stop();
    m_requestTimeoutTimer->stop();
    
    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        m_webSocket->close();
    }
    
    m_isConnected = false;
}

bool GameWebSocketClient::isConnected() const
{
    return m_isConnected && m_webSocket->state() == QAbstractSocket::ConnectedState;
}

void GameWebSocketClient::setReconnectInterval(int seconds)
{
    m_reconnectInterval = seconds * 1000;
}

void GameWebSocketClient::setHeartbeatInterval(int seconds)
{
    m_heartbeatInterval = seconds * 1000;
    if (m_heartbeatTimer->isActive()) {
        m_heartbeatTimer->setInterval(m_heartbeatInterval);
    }
}

void GameWebSocketClient::setRequestTimeout(int seconds)
{
    m_requestTimeout = seconds * 1000;
}

QString GameWebSocketClient::spawnAI(Academy academy, Department department, int count, int teamId)
{
    QJsonObject request = ProtocolUtils::createSpawnRequest(academy, department, count, teamId);
    return sendRequest(request);
}

QString GameWebSocketClient::sendAICommand(const QString &aiId, const QString &action, const QJsonObject &params)
{
    QJsonObject request = ProtocolUtils::createCommandRequest(aiId, action, params);
    return sendRequest(request);
}

QString GameWebSocketClient::moveAI(const QString &aiId, const Position &target)
{
    QJsonObject params{
        {"target", target.toJson()}
    };
    return sendAICommand(aiId, Actions::MOVE, params);
}

QString GameWebSocketClient::attackTarget(const QString &aiId, const QString &targetId)
{
    QJsonObject params{
        {"target_id", targetId}
    };
    return sendAICommand(aiId, Actions::ATTACK, params);
}

QString GameWebSocketClient::useSkill(const QString &aiId, const QString &skillId, const QJsonObject &skillParams)
{
    QJsonObject params{
        {"skill_id", skillId},
        {"skill_params", skillParams}
    };
    return sendAICommand(aiId, Actions::USE_SKILL, params);
}

QString GameWebSocketClient::assignTeam(const QStringList &aiIds, int teamId)
{
    QJsonObject request = ProtocolUtils::createTeamAssignRequest(aiIds, teamId);
    return sendRequest(request);
}

QString GameWebSocketClient::getAIStatus(const QStringList &aiIds)
{
    QJsonObject request = ProtocolUtils::createStatusRequest(aiIds);
    return sendRequest(request);
}

QString GameWebSocketClient::sendBatchRequest(const QJsonArray &operations)
{
    QJsonObject request = ProtocolUtils::createBatchRequest(operations);
    return sendRequest(request);
}

QString GameWebSocketClient::deleteAI(const QString &aiId)
{
    QJsonObject request{
        {"cmd", Commands::DELETE_AI},
        {"ai_id", aiId},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
    return sendRequest(request);
}

QString GameWebSocketClient::deleteTeam(int teamId)
{
    QJsonObject request{
        {"cmd", Commands::DELETE_AI},
        {"team_id", teamId},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
    return sendRequest(request);
}

QString GameWebSocketClient::pauseAllAI()
{
    QJsonObject request{
        {"cmd", Commands::SYSTEM_CONTROL},
        {"action", "pause_all"},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
    return sendRequest(request);
}

QString GameWebSocketClient::resumeAllAI()
{
    QJsonObject request{
        {"cmd", Commands::SYSTEM_CONTROL},
        {"action", "resume_all"},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
    return sendRequest(request);
}

// ==== 私有方法實現 ====

QString GameWebSocketClient::sendRequest(const QJsonObject &request)
{
    QString requestId = request["request_id"].toString();
    
    if (!isConnected()) {
        queueMessage(request);
        qWarning() << "Not connected, queued message:" << requestId;
        return requestId;
    }
    
    // 記錄待處理請求
    QMutexLocker locker(&m_requestMutex);
    PendingRequest pending;
    pending.requestId = requestId;
    pending.request = request;
    pending.timestamp = QDateTime::currentMSecsSinceEpoch();
    m_pendingRequests[requestId] = pending;
    
    // 發送請求
    QJsonDocument doc(request);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    
    m_stats.sentMessages++;
    updateStatistics();
    
    qDebug() << "Sent request:" << requestId << "Type:" << request["cmd"].toString();
    return requestId;
}

void GameWebSocketClient::queueMessage(const QJsonObject &message)
{
    QMutexLocker locker(&m_queueMutex);
    m_messageQueue.enqueue(message);
}

void GameWebSocketClient::onConnected()
{
    qDebug() << "Connected to game server";
    
    m_isConnected = true;
    m_reconnectAttempts = 0;
    m_stats.connectionTime = QDateTime::currentMSecsSinceEpoch();
    
    m_heartbeatTimer->start();
    m_messageQueueTimer->start();
    m_requestTimeoutTimer->start();
    
    emit connected();
}

void GameWebSocketClient::onDisconnected()
{
    qDebug() << "Disconnected from game server";
    
    m_isConnected = false;
    
    m_heartbeatTimer->stop();
    m_messageQueueTimer->stop();
    
    emit disconnected();
    
    // 自動重連
    if (m_reconnectAttempts < m_maxReconnectAttempts) {
        m_reconnectTimer->start(m_reconnectInterval);
        m_reconnectAttempts++;
        qDebug() << "Scheduling reconnect attempt" << m_reconnectAttempts;
    }
}

void GameWebSocketClient::onTextMessageReceived(const QString &message)
{
    m_stats.receivedMessages++;
    m_stats.lastActivity = QDateTime::currentDateTime();
    
    qDebug() << "Received message:" << message.left(100) << "...";
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return;
    }
    
    QJsonObject obj = doc.object();
    
    if (obj.contains("request_id")) {
        // 這是對請求的響應
        processResponse(obj);
    } else {
        // 這是服務器主動發送的通知
        processNotification(obj);
    }
}

void GameWebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorString = m_webSocket->errorString();
    qWarning() << "WebSocket error:" << error << errorString;
    
    m_stats.failedMessages++;
    emit connectionError(errorString);
}

void GameWebSocketClient::onHeartbeatTimer()
{
    if (isConnected()) {
        QJsonObject heartbeat{
            {"cmd", "heartbeat"},
            {"timestamp", QDateTime::currentMSecsSinceEpoch()}
        };
        
        QJsonDocument doc(heartbeat);
        m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    }
}

void GameWebSocketClient::onReconnectTimer()
{
    qDebug() << "Attempting to reconnect...";
    connectToServer(m_serverUrl);
}

void GameWebSocketClient::processMessageQueue()
{
    if (!isConnected() || m_messageQueue.isEmpty()) {
        return;
    }
    
    QMutexLocker locker(&m_queueMutex);
    
    // 批量發送隊列中的消息
    int processed = 0;
    const int maxBatch = 10; // 每次最多處理10條消息
    
    while (!m_messageQueue.isEmpty() && processed < maxBatch) {
        QJsonObject message = m_messageQueue.dequeue();
        sendRequest(message);
        processed++;
    }
    
    if (processed > 0) {
        qDebug() << "Processed" << processed << "queued messages";
    }
}

void GameWebSocketClient::onRequestTimeout()
{
    QMutexLocker locker(&m_requestMutex);
    
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QStringList timeoutRequests;
    
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
        if (now - it->timestamp > m_requestTimeout) {
            timeoutRequests.append(it.key());
        }
    }
    
    for (const QString &requestId : timeoutRequests) {
        handleRequestTimeout(requestId);
    }
}

void GameWebSocketClient::handleRequestTimeout(const QString &requestId)
{
    QMutexLocker locker(&m_requestMutex);
    
    if (!m_pendingRequests.contains(requestId)) {
        return;
    }
    
    PendingRequest &request = m_pendingRequests[requestId];
    
    if (request.retryCount < 3) {
        // 重試請求
        request.retryCount++;
        request.timestamp = QDateTime::currentMSecsSinceEpoch();
        
        QJsonDocument doc(request.request);
        m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
        
        qDebug() << "Retrying request:" << requestId << "Attempt:" << request.retryCount;
    } else {
        // 請求失敗
        m_pendingRequests.remove(requestId);
        emit requestFailed(requestId, "Request timeout after retries");
        
        qWarning() << "Request failed after retries:" << requestId;
    }
}

void GameWebSocketClient::processResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    
    // 移除待處理請求
    QMutexLocker locker(&m_requestMutex);
    m_pendingRequests.remove(requestId);
    locker.unlock();
    
    QString status = response["status"].toString();
    
    if (status == "success") {
        QString cmd = response["cmd"].toString();
        
        if (cmd == Commands::SPAWN_AI) {
            handleSpawnResponse(response);
        } else if (cmd == Commands::AI_COMMAND) {
            handleCommandResponse(response);
        } else if (cmd == Commands::ASSIGN_TEAM) {
            handleTeamResponse(response);
        } else if (cmd == Commands::GET_STATUS) {
            handleStatusResponse(response);
        } else if (cmd == Commands::BATCH_OPERATION) {
            handleBatchResponse(response);
        } else if (cmd == Commands::DELETE_AI) {
            handleDeleteResponse(response);
        }
    } else if (status == "error") {
        handleErrorResponse(response);
    }
}

void GameWebSocketClient::processNotification(const QJsonObject &notification)
{
    QString type = notification["type"].toString();
    
    if (type == "ai_state_change") {
        handleAIStateNotification(notification);
    } else if (type == "battle_event") {
        handleBattleNotification(notification);
    } else if (type == "system_event") {
        handleSystemNotification(notification);
    }
}

void GameWebSocketClient::handleSpawnResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QJsonArray aiList = response["data"]["ai_list"].toArray();
    
    emit aiSpawned(requestId, aiList);
}

void GameWebSocketClient::handleCommandResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QJsonObject result = response["data"].toObject();
    
    emit commandExecuted(requestId, result);
}

void GameWebSocketClient::handleTeamResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QJsonObject result = response["data"].toObject();
    
    emit teamAssigned(requestId, result);
}

void GameWebSocketClient::handleStatusResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QJsonArray aiStatus = response["data"]["ai_status"].toArray();
    
    emit statusReceived(requestId, aiStatus);
}

void GameWebSocketClient::handleBatchResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QJsonArray results = response["data"]["results"].toArray();
    
    emit batchCompleted(requestId, results);
}

void GameWebSocketClient::handleDeleteResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QString aiId = response["data"]["ai_id"].toString();
    
    emit aiDeleted(requestId, aiId);
}

void GameWebSocketClient::handleErrorResponse(const QJsonObject &response)
{
    QString requestId = response["request_id"].toString();
    QString error = response["error"].toString();
    
    emit requestFailed(requestId, error);
}

void GameWebSocketClient::handleAIStateNotification(const QJsonObject &notification)
{
    QString aiId = notification["ai_id"].toString();
    AIState newState = ProtocolUtils::stringToState(notification["new_state"].toString());
    Position position = Position::fromJson(notification["position"].toObject());
    
    emit aiStateChanged(aiId, newState, position);
}

void GameWebSocketClient::handleBattleNotification(const QJsonObject &notification)
{
    QString aiId = notification["ai_id"].toString();
    QString eventType = notification["event_type"].toString();
    QJsonObject data = notification["data"].toObject();
    
    emit aiBattleEvent(aiId, eventType, data);
}

void GameWebSocketClient::handleSystemNotification(const QJsonObject &notification)
{
    QString eventType = notification["event_type"].toString();
    
    if (eventType == "ai_level_up") {
        QString aiId = notification["ai_id"].toString();
        int newLevel = notification["new_level"].toInt();
        emit aiLevelUp(aiId, newLevel);
    } else if (eventType == "ai_death") {
        QString aiId = notification["ai_id"].toString();
        QString killer = notification["killer"].toString();
        emit aiDeath(aiId, killer);
    }
}

void GameWebSocketClient::updateStatistics()
{
    // 更新統計信息，可用於GUI顯示
    m_stats.lastActivity = QDateTime::currentDateTime();
}

} // namespace AI
} // namespace RANOnline
