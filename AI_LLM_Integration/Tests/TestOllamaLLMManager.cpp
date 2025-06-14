#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "../OllamaLLMManager.h"

/**
 * @class TestOllamaLLMManager
 * @brief OllamaLLMManager類的單元測試
 * 
 * 測試LLM管理器的核心功能：
 * - 連接管理和健康檢查
 * - 負載均衡策略
 * - 流式響應處理
 * - 錯誤處理和重試機制
 * - 性能監控
 */
class TestOllamaLLMManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 連接管理測試
    void testServerConnection();
    void testHealthCheck();
    void testConnectionPooling();
    void testAutoReconnect();

    // 負載均衡測試
    void testRoundRobinBalancing();
    void testLeastConnectionsBalancing();
    void testWeightedBalancing();
    void testFailover();

    // 請求處理測試
    void testSingleRequest();
    void testStreamingRequest();
    void testBatchRequests();
    void testConcurrentRequests();

    // 錯誤處理測試
    void testNetworkError();
    void testTimeoutHandling();
    void testRetryMechanism();
    void testCircuitBreaker();

    // 性能監控測試
    void testPerformanceMetrics();
    void testResourceUsage();
    void testStatisticsCollection();

private:
    OllamaLLMManager* m_manager;
    QNetworkAccessManager* m_networkManager;
    QString m_testConfigPath;
};

void TestOllamaLLMManager::initTestCase()
{
    // 創建測試配置
    m_testConfigPath = QDir::temp().filePath("test_llm_config.json");
    
    QJsonObject config;
    QJsonArray servers;
    
    QJsonObject server1;
    server1["id"] = "test_server_1";
    server1["name"] = "Test Server 1";
    server1["host"] = "localhost";
    server1["port"] = 11434;
    server1["enabled"] = true;
    server1["weight"] = 100;
    servers.append(server1);
    
    config["servers"] = servers;
    
    QJsonDocument doc(config);
    QFile file(m_testConfigPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void TestOllamaLLMManager::cleanupTestCase()
{
    // 清理測試文件
    QFile::remove(m_testConfigPath);
}

void TestOllamaLLMManager::init()
{
    m_manager = new OllamaLLMManager(this);
    m_networkManager = new QNetworkAccessManager(this);
}

void TestOllamaLLMManager::cleanup()
{
    delete m_manager;
    delete m_networkManager;
}

void TestOllamaLLMManager::testServerConnection()
{
    // 測試伺服器連接
    QSignalSpy spy(m_manager, &OllamaLLMManager::serverConnected);
    
    m_manager->addServer("test_server", "localhost", 11434);
    m_manager->connectToServers();
    
    // 等待連接信號
    QVERIFY(spy.wait(5000));
    QCOMPARE(spy.count(), 1);
    
    // 檢查連接狀態
    QVERIFY(m_manager->isServerConnected("test_server"));
}

void TestOllamaLLMManager::testHealthCheck()
{
    // 測試健康檢查
    m_manager->addServer("test_server", "localhost", 11434);
    
    QSignalSpy healthSpy(m_manager, &OllamaLLMManager::healthCheckCompleted);
    
    m_manager->performHealthCheck("test_server");
    
    QVERIFY(healthSpy.wait(3000));
    QCOMPARE(healthSpy.count(), 1);
    
    auto args = healthSpy.takeFirst();
    QString serverId = args.at(0).toString();
    bool isHealthy = args.at(1).toBool();
    
    QCOMPARE(serverId, "test_server");
    QVERIFY(isHealthy);
}

void TestOllamaLLMManager::testRoundRobinBalancing()
{
    // 測試輪詢負載均衡
    m_manager->addServer("server1", "localhost", 11434);
    m_manager->addServer("server2", "localhost", 11435);
    
    m_manager->setLoadBalancingStrategy(LoadBalancer::RoundRobin);
    
    // 發送多個請求，檢查是否輪詢分配
    QString server1 = m_manager->selectServer();
    QString server2 = m_manager->selectServer();
    QString server3 = m_manager->selectServer();
    
    QVERIFY(server1 != server2);
    QCOMPARE(server1, server3); // 應該回到第一個伺服器
}

void TestOllamaLLMManager::testStreamingRequest()
{
    // 測試流式請求
    m_manager->addServer("test_server", "localhost", 11434);
    
    QSignalSpy streamSpy(m_manager, &OllamaLLMManager::responseChunkReceived);
    QSignalSpy completeSpy(m_manager, &OllamaLLMManager::responseCompleted);
    
    QString requestId = m_manager->sendStreamingRequest("test_server", "llama3", "Hello, world!");
    
    // 等待流式響應
    QVERIFY(streamSpy.wait(10000));
    QVERIFY(streamSpy.count() > 0);
    
    // 等待響應完成
    QVERIFY(completeSpy.wait(5000));
    QCOMPARE(completeSpy.count(), 1);
    
    auto args = completeSpy.takeFirst();
    QString responseId = args.at(0).toString();
    QCOMPARE(responseId, requestId);
}

void TestOllamaLLMManager::testRetryMechanism()
{
    // 測試重試機制
    m_manager->addServer("invalid_server", "invalid_host", 99999);
    m_manager->setMaxRetryAttempts(3);
    
    QSignalSpy errorSpy(m_manager, &OllamaLLMManager::requestFailed);
    
    QString requestId = m_manager->sendRequest("invalid_server", "llama3", "Test message");
    
    // 等待重試失敗
    QVERIFY(errorSpy.wait(15000));
    QCOMPARE(errorSpy.count(), 1);
    
    auto args = errorSpy.takeFirst();
    QString failedId = args.at(0).toString();
    QString error = args.at(1).toString();
    
    QCOMPARE(failedId, requestId);
    QVERIFY(error.contains("retry"));
}

void TestOllamaLLMManager::testPerformanceMetrics()
{
    // 測試性能指標收集
    m_manager->addServer("test_server", "localhost", 11434);
    
    // 發送請求並檢查指標
    QString requestId = m_manager->sendRequest("test_server", "llama3", "Performance test");
    
    QSignalSpy completeSpy(m_manager, &OllamaLLMManager::responseCompleted);
    QVERIFY(completeSpy.wait(10000));
    
    // 獲取性能指標
    auto metrics = m_manager->getPerformanceMetrics("test_server");
    
    QVERIFY(metrics.contains("total_requests"));
    QVERIFY(metrics.contains("successful_requests"));
    QVERIFY(metrics.contains("average_response_time"));
    
    QVERIFY(metrics["total_requests"].toInt() > 0);
    QVERIFY(metrics["successful_requests"].toInt() > 0);
    QVERIFY(metrics["average_response_time"].toDouble() > 0);
}

void TestOllamaLLMManager::testBatchRequests()
{
    // 測試批量請求處理
    m_manager->addServer("test_server", "localhost", 11434);
    
    QStringList prompts = {
        "First prompt",
        "Second prompt", 
        "Third prompt"
    };
    
    QSignalSpy batchSpy(m_manager, &OllamaLLMManager::batchCompleted);
    
    QString batchId = m_manager->sendBatchRequests("test_server", "llama3", prompts);
    
    // 等待批量處理完成
    QVERIFY(batchSpy.wait(30000));
    QCOMPARE(batchSpy.count(), 1);
    
    auto args = batchSpy.takeFirst();
    QString completedBatchId = args.at(0).toString();
    QVariantList results = args.at(1).toList();
    
    QCOMPARE(completedBatchId, batchId);
    QCOMPARE(results.size(), prompts.size());
}

void TestOllamaLLMManager::testCircuitBreaker()
{
    // 測試熔斷器機制
    m_manager->addServer("unstable_server", "localhost", 99999);
    m_manager->setCircuitBreakerThreshold(3);
    
    QSignalSpy circuitSpy(m_manager, &OllamaLLMManager::circuitBreakerTripped);
    
    // 發送多個失敗請求觸發熔斷器
    for (int i = 0; i < 5; ++i) {
        m_manager->sendRequest("unstable_server", "llama3", QString("Test %1").arg(i));
        QTest::qWait(100);
    }
    
    // 等待熔斷器觸發
    QVERIFY(circuitSpy.wait(10000));
    QCOMPARE(circuitSpy.count(), 1);
    
    auto args = circuitSpy.takeFirst();
    QString serverId = args.at(0).toString();
    QCOMPARE(serverId, "unstable_server");
    
    // 檢查伺服器是否被標記為不可用
    QVERIFY(!m_manager->isServerAvailable("unstable_server"));
}

QTEST_MAIN(TestOllamaLLMManager)
#include "TestOllamaLLMManager.moc"
