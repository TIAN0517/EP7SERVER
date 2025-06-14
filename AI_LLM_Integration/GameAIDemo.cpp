/**
 * @file GameAIDemo.cpp
 * @brief RANOnline EP7 四部門AI通訊系統演示
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QThread>

#include "GameWebSocketServer.h"
#include "GameWebSocketClient.h"
#include "GameAIProtocol.h"

using namespace RANOnline::AI;

/**
 * @brief 演示程序類
 */
class GameAIDemo : public QObject
{
    Q_OBJECT

public:
    explicit GameAIDemo(QObject *parent = nullptr)
        : QObject(parent)
        , m_server(new GameWebSocketServer(this))
        , m_client(new GameWebSocketClient(this))
    {
        setupServer();
        setupClient();
        
        // 延遲啟動演示
        QTimer::singleShot(1000, this, &GameAIDemo::startDemo);
    }

private slots:
    void setupServer()
    {
        // 連接服務器信號
        connect(m_server, &GameWebSocketServer::serverStarted,
                this, [](const QString &host, quint16 port) {
                    qDebug() << "🚀 服務器已啟動:" << host << ":" << port;
                });
        
        connect(m_server, &GameWebSocketServer::clientConnected,
                this, [](const QString &clientInfo) {
                    qDebug() << "📱 客戶端已連接:" << clientInfo;
                });
        
        connect(m_server, &GameWebSocketServer::messageReceived,
                this, [](const QString &from, const QString &message) {
                    qDebug() << "📨 收到消息 [" << from << "]:" << message.left(100) << "...";
                });
        
        // 啟動服務器
        if (!m_server->startServer("localhost", 8765)) {
            qCritical() << "❌ 服務器啟動失敗！";
            QCoreApplication::exit(1);
        }
    }
    
    void setupClient()
    {
        // 連接客戶端信號
        connect(m_client, &GameWebSocketClient::connected,
                this, []() {
                    qDebug() << "✅ 客戶端已連接到服務器";
                });
        
        connect(m_client, &GameWebSocketClient::aiSpawned,
                this, [this](const QString &requestId, const QJsonArray &aiList) {
                    qDebug() << "🤖 AI生成完成 [" << requestId << "] 數量:" << aiList.size();
                    
                    // 保存AI ID用於後續操作
                    for (const auto &value : aiList) {
                        QJsonObject ai = value.toObject();
                        m_spawnedAIs.append(ai["ai_id"].toString());
                    }
                    
                    // 繼續演示
                    QTimer::singleShot(2000, this, &GameAIDemo::demoAICommands);
                });
        
        connect(m_client, &GameWebSocketClient::commandExecuted,
                this, [](const QString &requestId, const QJsonObject &result) {
                    QString aiId = result["ai_id"].toString();
                    QString action = result["action"].toString();
                    qDebug() << "⚔️ AI指令執行完成 [" << requestId << "] AI:" << aiId << "動作:" << action;
                });
        
        connect(m_client, &GameWebSocketClient::statusReceived,
                this, [this](const QString &requestId, const QJsonArray &aiStatus) {
                    qDebug() << "📊 AI狀態查詢完成 [" << requestId << "] 數量:" << aiStatus.size();
                    
                    for (const auto &value : aiStatus) {
                        QJsonObject ai = value.toObject();
                        QString aiId = ai["ai_id"].toString();
                        QString state = ai["state"].toString();
                        int hp = ai["hp"].toInt();
                        qDebug() << "   AI:" << aiId << "狀態:" << state << "HP:" << hp;
                    }
                    
                    // 繼續演示
                    QTimer::singleShot(3000, this, &GameAIDemo::demoBatchOperations);
                });
        
        connect(m_client, &GameWebSocketClient::aiStateChanged,
                this, [](const QString &aiId, AIState newState, const Position &position) {
                    qDebug() << "🔄 AI狀態變化:" << aiId 
                             << "新狀態:" << ProtocolUtils::stateToString(newState)
                             << "位置:(" << position.x << "," << position.y << ")";
                });
        
        connect(m_client, &GameWebSocketClient::aiBattleEvent,
                this, [](const QString &aiId, const QString &eventType, const QJsonObject &data) {
                    qDebug() << "⚔️ AI戰鬥事件:" << aiId << "類型:" << eventType;
                });
    }
    
    void startDemo()
    {
        qDebug() << "\n🎮 開始RANOnline EP7 四部門AI通訊演示";
        qDebug() << "==============================================";
        
        // 連接到服務器
        m_client->connectToServer("ws://localhost:8765");
        
        // 等待連接後開始演示
        QTimer::singleShot(2000, this, &GameAIDemo::demoSpawnAI);
    }
    
    void demoSpawnAI()
    {
        qDebug() << "\n📝 演示1: 批量生成四部門AI";
        qDebug() << "-------------------------------";
        
        // 生成聖門學院劍術部AI (隊伍1)
        QString request1 = m_client->spawnAI(Academy::SEONGMUN, Department::SWORD, 5, 1);
        qDebug() << "🗡️ 生成聖門劍術AI x5 (隊伍1) 請求:" << request1;
        
        // 生成懸岩學院弓術部AI (隊伍2)  
        QString request2 = m_client->spawnAI(Academy::HYONRYON, Department::BOW, 3, 2);
        qDebug() << "🏹 生成懸岩弓術AI x3 (隊伍2) 請求:" << request2;
        
        // 生成鳳凰學院格鬥部AI (隊伍1)
        QString request3 = m_client->spawnAI(Academy::BONGCHEON, Department::MARTIAL, 4, 1);
        qDebug() << "👊 生成鳳凰格鬥AI x4 (隊伍1) 請求:" << request3;
        
        // 生成氣功部AI (無隊伍)
        QString request4 = m_client->spawnAI(Academy::SEONGMUN, Department::QIGONG, 2, 0);
        qDebug() << "🌀 生成聖門氣功AI x2 (無隊伍) 請求:" << request4;
    }
    
    void demoAICommands()
    {
        if (m_spawnedAIs.isEmpty()) {
            qWarning() << "❌ 沒有可用的AI進行演示";
            return;
        }
        
        qDebug() << "\n📝 演示2: AI動作指令";
        qDebug() << "----------------------";
        
        // 移動指令
        QString aiId1 = m_spawnedAIs.first();
        Position target{150, 200, 0};
        QString moveRequest = m_client->moveAI(aiId1, target);
        qDebug() << "🚶 移動AI" << aiId1 << "到位置(150,200) 請求:" << moveRequest;
        
        // 攻擊指令 (如果有足夠的AI)
        if (m_spawnedAIs.size() > 1) {
            QString aiId2 = m_spawnedAIs[1];
            QString attackRequest = m_client->attackTarget(aiId1, aiId2);
            qDebug() << "⚔️ " << aiId1 << "攻擊" << aiId2 << "請求:" << attackRequest;
        }
        
        // 使用技能
        QJsonObject skillParams{
            {"power", 100},
            {"range", 50}
        };
        QString skillRequest = m_client->useSkill(aiId1, "fireball", skillParams);
        qDebug() << "🔥 " << aiId1 << "使用火球術 請求:" << skillRequest;
        
        // 查詢AI狀態
        QStringList statusAIs = m_spawnedAIs.mid(0, 3); // 前3個AI
        QString statusRequest = m_client->getAIStatus(statusAIs);
        qDebug() << "📊 查詢AI狀態 請求:" << statusRequest;
    }
    
    void demoBatchOperations()
    {
        qDebug() << "\n📝 演示3: 批量操作";
        qDebug() << "-------------------";
        
        // 創建批量操作
        QJsonArray operations;
        
        // 批量移動操作
        for (int i = 0; i < qMin(3, m_spawnedAIs.size()); ++i) {
            QJsonObject moveOp{
                {"cmd", Commands::AI_COMMAND},
                {"ai_id", m_spawnedAIs[i]},
                {"action", Actions::MOVE},
                {"params", QJsonObject{
                    {"target", Position{i * 100, i * 100, 0}.toJson()}
                }}
            };
            operations.append(moveOp);
        }
        
        QString batchRequest = m_client->sendBatchRequest(operations);
        qDebug() << "📦 批量移動操作 請求:" << batchRequest;
        
        // 隊伍重新分配
        if (m_spawnedAIs.size() >= 4) {
            QStringList teamMembers = m_spawnedAIs.mid(0, 4);
            QString teamRequest = m_client->assignTeam(teamMembers, 10);
            qDebug() << "👥 重新分配隊伍10 請求:" << teamRequest;
        }
        
        // 演示結束
        QTimer::singleShot(5000, this, &GameAIDemo::demoComplete);
    }
    
    void demoComplete()
    {
        qDebug() << "\n🎉 演示完成！";
        qDebug() << "=============";
        qDebug() << "總生成AI數量:" << m_spawnedAIs.size();
        qDebug() << "服務器連接客戶端:" << m_server->getConnectedClients();
        qDebug() << "服務器處理消息:" << m_server->getTotalMessages();
        
        // 清理演示
        QTimer::singleShot(2000, this, &GameAIDemo::cleanup);
    }
    
    void cleanup()
    {
        qDebug() << "\n🧹 清理資源...";
        
        // 斷開客戶端
        m_client->disconnectFromServer();
        
        // 停止服務器
        m_server->stopServer();
        
        // 退出應用程序
        QTimer::singleShot(1000, []() {
            qDebug() << "✅ 演示結束，程序退出";
            QCoreApplication::quit();
        });
    }

private:
    GameWebSocketServer *m_server;
    GameWebSocketClient *m_client;
    QStringList m_spawnedAIs;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "🎮 RANOnline EP7 四部門AI通訊系統演示";
    qDebug() << "作者: Jy技術團隊";
    qDebug() << "日期: 2025年6月14日";
    qDebug() << "版本: 2.0.0\n";
    
    // 創建演示實例
    GameAIDemo demo;
    Q_UNUSED(demo);
    
    return app.exec();
}

#include "GameAIDemo.moc"
