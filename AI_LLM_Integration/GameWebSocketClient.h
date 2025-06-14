/**
 * @file GameWebSocketClient.h
 * @brief RANOnline EP7 AI前端 WebSocket 客戶端
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtNetwork/QNetworkReply>
#include <QtWebSockets/QWebSocket>
#include <memory>

#include "GameAIProtocol.h"

namespace RANOnline {
namespace AI {

/**
 * @brief 遊戲WebSocket客戶端
 * 
 * 功能特性:
 * - 自動重連機制
 * - 心跳保活
 * - 請求隊列管理
 * - 批量操作支援
 * - 錯誤恢復
 */
class GameWebSocketClient : public QObject
{
    Q_OBJECT

public:
    explicit GameWebSocketClient(QObject *parent = nullptr);
    ~GameWebSocketClient();

    // 連接管理
    void connectToServer(const QString &url);
    void disconnectFromServer();
    bool isConnected() const;
    
    // 設置參數
    void setReconnectInterval(int seconds);
    void setHeartbeatInterval(int seconds);
    void setRequestTimeout(int seconds);
    
    // AI批量生成
    QString spawnAI(Academy academy, Department department, int count, int teamId = 0);
    
    // AI動作指令
    QString sendAICommand(const QString &aiId, const QString &action, const QJsonObject &params = QJsonObject());
    QString moveAI(const QString &aiId, const Position &target);
    QString attackTarget(const QString &aiId, const QString &targetId);
    QString useSkill(const QString &aiId, const QString &skillId, const QJsonObject &skillParams = QJsonObject());
    
    // AI隊伍管理
    QString assignTeam(const QStringList &aiIds, int teamId);
    
    // AI狀態查詢
    QString getAIStatus(const QStringList &aiIds = QStringList());
    
    // 批量操作
    QString sendBatchRequest(const QJsonArray &operations);
    
    // 系統操作
    QString deleteAI(const QString &aiId);
    QString deleteTeam(int teamId);
    QString pauseAllAI();
    QString resumeAllAI();

signals:
    // 連接狀態
    void connected();
    void disconnected();
    void connectionError(const QString &error);
    
    // 響應信號
    void aiSpawned(const QString &requestId, const QJsonArray &aiList);
    void commandExecuted(const QString &requestId, const QJsonObject &result);
    void teamAssigned(const QString &requestId, const QJsonObject &result);
    void statusReceived(const QString &requestId, const QJsonArray &aiStatus);
    void batchCompleted(const QString &requestId, const QJsonArray &results);
    void aiDeleted(const QString &requestId, const QString &aiId);
    
    // 錯誤信號
    void requestFailed(const QString &requestId, const QString &error);
    
    // 即時事件
    void aiStateChanged(const QString &aiId, AIState newState, const Position &position);
    void aiBattleEvent(const QString &aiId, const QString &eventType, const QJsonObject &data);
    void aiLevelUp(const QString &aiId, int newLevel);
    void aiDeath(const QString &aiId, const QString &killer);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);
    void onHeartbeatTimer();
    void onReconnectTimer();
    void processMessageQueue();
    void onRequestTimeout();

private:
    // 網路組件
    std::unique_ptr<QWebSocket> m_webSocket;
    QString m_serverUrl;
    bool m_isConnected = false;
    
    // 定時器
    QTimer *m_heartbeatTimer;
    QTimer *m_reconnectTimer;
    QTimer *m_messageQueueTimer;
    QTimer *m_requestTimeoutTimer;
    
    // 重連參數
    int m_reconnectAttempts = 0;
    int m_maxReconnectAttempts = 10;
    int m_reconnectInterval = 5000; // 5秒
    int m_heartbeatInterval = 30000; // 30秒
    int m_requestTimeout = 30000; // 30秒
    
    // 消息隊列
    QQueue<QJsonObject> m_messageQueue;
    QMutex m_queueMutex;
    
    // 請求追蹤
    struct PendingRequest {
        QString requestId;
        QJsonObject request;
        qint64 timestamp;
        int retryCount = 0;
    };
    QMap<QString, PendingRequest> m_pendingRequests;
    QMutex m_requestMutex;
    
    // 統計信息
    struct Statistics {
        qint64 sentMessages = 0;
        qint64 receivedMessages = 0;
        qint64 failedMessages = 0;
        qint64 connectionTime = 0;
        QDateTime lastActivity;
    } m_stats;
    
    // 私有方法
    void setupTimers();
    void setupConnections();
    QString sendRequest(const QJsonObject &request);
    void queueMessage(const QJsonObject &message);
    void processResponse(const QJsonObject &response);
    void processNotification(const QJsonObject &notification);
    void handleRequestTimeout(const QString &requestId);
    void retryFailedRequest(const QString &requestId);
    void updateStatistics();
    
    // 響應處理
    void handleSpawnResponse(const QJsonObject &response);
    void handleCommandResponse(const QJsonObject &response);
    void handleTeamResponse(const QJsonObject &response);
    void handleStatusResponse(const QJsonObject &response);
    void handleBatchResponse(const QJsonObject &response);
    void handleDeleteResponse(const QJsonObject &response);
    void handleErrorResponse(const QJsonObject &response);
    
    // 通知處理
    void handleAIStateNotification(const QJsonObject &notification);
    void handleBattleNotification(const QJsonObject &notification);
    void handleSystemNotification(const QJsonObject &notification);
};

} // namespace AI
} // namespace RANOnline
