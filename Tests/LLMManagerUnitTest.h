#ifndef LLMMANAGER_UNITTEST_H
#define LLMMANAGER_UNITTEST_H

#include <QTest>
#include <QObject>
#include <QSignalSpy>
#include <QEventLoop>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <memory>

#include "OllamaLLMManager.h"
#include "PromptTemplateManager.h"

/**
 * @brief Ollama LLM管理器單元測試類
 * 
 * 測試覆蓋範圍:
 * - 初始化和配置
 * - 模型管理和選擇
 * - 同步和異步請求
 * - 錯誤處理和重試機制
 * - 提示詞模板管理
 * - 統計和監控
 * 
 * @author Jy技術團隊
 * @date 2025-06-14
 * @version 2.0.0
 */

using namespace RANOnline::AI;

class OllamaLLMManagerTest : public QObject
{
    Q_OBJECT

public:
    OllamaLLMManagerTest();

private slots:
    // ============================================================
    // 🔧 測試初始化和清理
    // ============================================================
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // ============================================================
    // 🔌 初始化和連接測試
    // ============================================================
    void testInitialization();
    void testServiceAvailability();
    void testConfigurationLoadSave();
    void testInvalidURL();

    // ============================================================
    // 📋 模型管理測試
    // ============================================================
    void testModelListRefresh();
    void testModelSelection();
    void testModelHealthCheck();
    void testModelStatistics();

    // ============================================================
    // 🚀 核心功能測試
    // ============================================================
    void testSyncGeneration();
    void testAsyncGeneration();
    void testStreamGeneration();
    void testBatchRequest();
    void testRequestCancellation();

    // ============================================================
    // 🎯 便捷方法測試
    // ============================================================
    void testQuickChat();
    void testCodeGeneration();
    void testTechnicalSupport();

    // ============================================================
    // ⚠️ 錯誤處理測試
    // ============================================================
    void testErrorHandling();
    void testRetryMechanism();
    void testTimeoutHandling();
    void testModelSwitching();

    // ============================================================
    // ⚙️ 設置和控制測試
    // ============================================================
    void testGlobalOptions();
    void testConcurrentLimits();
    void testQueueManagement();

    // ============================================================
    // 📊 統計和監控測試
    // ============================================================
    void testStatisticsTracking();
    void testPerformanceMetrics();

private:
    // ============================================================
    // 🔧 輔助方法
    // ============================================================
    void waitForSignal(QObject *sender, const char *signal, int timeout = 5000);
    bool isOllamaServiceRunning();
    void mockServiceResponse();
    
    std::unique_ptr<OllamaLLMManager> m_manager;
    QString m_testConfigPath;
    bool m_serviceAvailable;
};

/**
 * @brief 提示詞模板管理器單元測試類
 */
class PromptTemplateManagerTest : public QObject
{
    Q_OBJECT

public:
    PromptTemplateManagerTest();

private slots:
    // ============================================================
    // 🔧 測試初始化和清理
    // ============================================================
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // ============================================================
    // 📋 模板管理測試
    // ============================================================
    void testTemplateCreation();
    void testTemplateAddUpdate();
    void testTemplateRemoval();
    void testTemplateSearch();
    void testTemplateCategories();

    // ============================================================
    // 🎯 模板處理測試
    // ============================================================
    void testTemplateRendering();
    void testVariableValidation();
    void testVariableExtraction();
    void testDefaultValues();

    // ============================================================
    // 💾 序列化測試
    // ============================================================
    void testJsonSerialization();
    void testFileImportExport();
    void testBulkOperations();

    // ============================================================
    // 🔧 實用工具測試
    // ============================================================
    void testQuickTemplateCreation();
    void testTemplateDuplication();
    void testBuilderPattern();

private:
    // ============================================================
    // 🔧 輔助方法
    // ============================================================
    PromptTemplate createTestTemplate(const QString &name);
    void validateTemplateContent(const PromptTemplate &template);
    
    std::unique_ptr<PromptTemplateManager> m_manager;
    QString m_testTemplateDir;
};

/**
 * @brief 集成測試類
 */
class LLMIntegrationTest : public QObject
{
    Q_OBJECT

public:
    LLMIntegrationTest();

private slots:
    // ============================================================
    // 🔧 測試初始化和清理
    // ============================================================
    void initTestCase();
    void cleanupTestCase();

    // ============================================================
    // 🔗 集成測試
    // ============================================================
    void testLLMWithTemplates();
    void testEndToEndWorkflow();
    void testPerformanceBenchmark();
    void testErrorRecovery();

private:
    std::unique_ptr<OllamaLLMManager> m_llmManager;
    std::unique_ptr<PromptTemplateManager> m_templateManager;
};

#endif // LLMMANAGER_UNITTEST_H
