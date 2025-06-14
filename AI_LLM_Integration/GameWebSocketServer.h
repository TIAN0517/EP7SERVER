/**
 * @file GameWebSocketServer.h
 * @brief RANOnline EP7 遊戲服務端 WebSocket 服務器
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <memory>

#include "GameAIProtocol.h"

namespace RANOnline {
namespace AI {

/**
 * @brief AI角色管理器
 */
class AICharacterManager : public QObject
{
    Q_OBJECT

public:
    explicit AICharacterManager(QObject *parent = nullptr);
    
    // AI生成和管理
    QStringList spawnAI(Academy academy, Department department, int count, int teamId);
    bool deleteAI(const QString &aiId);
    bool deleteTeam(int teamId);
    
    // AI控制
    bool moveAI(const QString &aiId, const Position &target);
    bool attackTarget(const QString &aiId, const QString &targetId);
    bool useSkill(const QString &aiId, const QString &skillId, const QJsonObject &params);
    bool assignTeam(const QStringList &aiIds, int teamId);
    
    // AI查詢
    QList<AICharacterInfo> getAIStatus(const QStringList &aiIds = QStringList());
    AICharacterInfo getAIInfo(const QString &aiId);
    QStringList getTeamMembers(int teamId);
    
    // 系統控制
    void pauseAllAI();
    void resumeAllAI();
    void resetAllAI();
    
    // 統計信息
    int getTotalAICount() const;
    int getActiveAICount() const;
    int getTeamCount() const;

signals:
    void aiStateChanged(const QString &aiId, AIState newState, const Position &position);
    void aiBattleEvent(const QString &aiId, const QString &eventType, const QJsonObject &data);
    void aiLevelUp(const QString &aiId, int newLevel);
    void aiDeath(const QString &aiId, const QString &killer);

private slots:
    void updateAILogic();

private:
    QMap<QString, AICharacterInfo> m_aiCharacters;
    QMap<int, QStringList> m_teams;
    QMutex m_dataMutex;
    QTimer *m_updateTimer;
    
    bool m_systemPaused = false;
    int m_nextAIId = 10001;
    int m_nextTeamId = 1;
    
    QString generateAIId();
    QString generateAIName(Academy academy, Department department);
    Position generateRandomPosition();
    void simulateAIBehavior(AICharacterInfo &ai);
    void handleAIMovement(AICharacterInfo &ai);
    void handleAIBattle(AICharacterInfo &ai);
};

/**
 * @brief 遊戲WebSocket服務器
 */
class GameWebSocketServer : public QObject
{
    Q_OBJECT

public:
    explicit GameWebSocketServer(QObject *parent = nullptr);
    ~GameWebSocketServer();

    bool startServer(const QString &host = "localhost", quint16 port = 8765);
    void stopServer();
    bool isListening() const;
    
    quint16 serverPort() const;
    QHostAddress serverAddress() const;
    
    int getConnectedClients() const;
    qint64 getTotalMessages() const;

signals:
    void serverStarted(const QString &host, quint16 port);
    void serverStopped();
    void clientConnected(const QString &clientInfo);
    void clientDisconnected(const QString &clientInfo);
    void messageReceived(const QString &from, const QString &message);
    void errorOccurred(const QString &error);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onTextMessageReceived(const QString &message);
    void onCleanupTimer();

private:
    std::unique_ptr<QWebSocketServer> m_server;
    std::unique_ptr<AICharacterManager> m_aiManager;
    
    QMap<QWebSocket*, QString> m_clients;
    QMutex m_clientsMutex;
    
    QTimer *m_cleanupTimer;
    
    // 統計信息
    struct ServerStatistics {
        qint64 totalConnections = 0;
        qint64 currentConnections = 0;
        qint64 totalMessages = 0;
        qint64 successfulRequests = 0;
        qint64 failedRequests = 0;
        QDateTime startTime;
    } m_stats;
    
    // 請求處理
    void processRequest(QWebSocket *client, const QJsonObject &request);
    void processSpawnRequest(QWebSocket *client, const QJsonObject &request);
    void processCommandRequest(QWebSocket *client, const QJsonObject &request);
    void processTeamRequest(QWebSocket *client, const QJsonObject &request);
    void processStatusRequest(QWebSocket *client, const QJsonObject &request);
    void processBatchRequest(QWebSocket *client, const QJsonObject &request);
    void processDeleteRequest(QWebSocket *client, const QJsonObject &request);
    void processSystemRequest(QWebSocket *client, const QJsonObject &request);
    
    // 響應發送
    void sendResponse(QWebSocket *client, const QString &requestId, const QJsonObject &data);
    void sendError(QWebSocket *client, const QString &requestId, const QString &error, int code = 1);
    void broadcastNotification(const QJsonObject &notification);
    
    // 工具方法
    QString getClientInfo(QWebSocket *client);
    void updateStatistics();
    void cleanup();
    
    // AI事件處理
    void onAIStateChanged(const QString &aiId, AIState newState, const Position &position);
    void onAIBattleEvent(const QString &aiId, const QString &eventType, const QJsonObject &data);
    void onAILevelUp(const QString &aiId, int newLevel);
    void onAIDeath(const QString &aiId, const QString &killer);
};

} // namespace AI
} // namespace RANOnline
