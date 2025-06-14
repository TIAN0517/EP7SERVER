#include <QtTest/QtTest>
#include <QApplication>
#include <QTimer>
#include <QSignalSpy>
#include "../LLMGUI.h"

/**
 * @class TestLLMGUI
 * @brief LLMGUI類的單元測試
 * 
 * 測試GUI組件的功能：
 * - 無邊框窗口拖拽
 * - 主題切換
 * - 進度條動畫
 * - 請求管理
 * - WebSocket通信
 */
class TestLLMGUI : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 窗口測試
    void testFramelessWindow();
    void testWindowDragging();
    void testWindowResizing();

    // 主題測試
    void testThemeSwitching();
    void testAcademyColors();
    void testThemeAnimations();

    // 進度條測試
    void testProgressBarAnimation();
    void testProgressBarColors();
    void testProgressBarCompletion();

    // 請求管理測試
    void testRequestCreation();
    void testRequestCancellation();
    void testBatchRequestManagement();

    // WebSocket測試
    void testWebSocketConnection();
    void testGameEventHandling();
    void testMessageForwarding();

    // 設置測試
    void testSettingsSaving();
    void testSettingsLoading();
    void testConfigurationPersistence();

private:
    QApplication* m_app;
    LLMGUI* m_gui;
    QWidget* m_testWidget;
};

void TestLLMGUI::initTestCase()
{
    // 初始化測試應用程序
    int argc = 0;
    char* argv[] = {nullptr};
    m_app = new QApplication(argc, argv);
}

void TestLLMGUI::cleanupTestCase()
{
    delete m_app;
}

void TestLLMGUI::init()
{
    m_gui = new LLMGUI();
    m_testWidget = new QWidget();
}

void TestLLMGUI::cleanup()
{
    delete m_gui;
    delete m_testWidget;
}

void TestLLMGUI::testFramelessWindow()
{
    // 測試無邊框窗口
    FramelessWindow* window = new FramelessWindow();
    
    // 檢查窗口標志
    QVERIFY(window->windowFlags() & Qt::FramelessWindowHint);
    QVERIFY(window->windowFlags() & Qt::WindowSystemMenuHint);
    
    // 檢查窗口屬性
    QVERIFY(window->testAttribute(Qt::WA_TranslucentBackground));
    
    delete window;
}

void TestLLMGUI::testThemeSwitching()
{
    // 測試主題切換
    QSignalSpy themeSpy(m_gui, &LLMGUI::themeChanged);
    
    // 切換到不同主題
    m_gui->setTheme(LLMGUI::CyberBlue);
    QCOMPARE(themeSpy.count(), 1);
    QCOMPARE(m_gui->getCurrentTheme(), LLMGUI::CyberBlue);
    
    m_gui->setTheme(LLMGUI::NeonGreen);
    QCOMPARE(themeSpy.count(), 2);
    QCOMPARE(m_gui->getCurrentTheme(), LLMGUI::NeonGreen);
    
    m_gui->setTheme(LLMGUI::PurpleHaze);
    QCOMPARE(themeSpy.count(), 3);
    QCOMPARE(m_gui->getCurrentTheme(), LLMGUI::PurpleHaze);
}

void TestLLMGUI::testProgressBarAnimation()
{
    // 測試進度條動畫
    CyberProgressBar* progressBar = new CyberProgressBar(m_testWidget);
    progressBar->setAcademy(CyberProgressBar::Warrior);
    
    QSignalSpy valueSpy(progressBar, &CyberProgressBar::valueChanged);
    
    // 測試進度更新
    progressBar->setValue(50);
    QCOMPARE(progressBar->value(), 50);
    QCOMPARE(valueSpy.count(), 1);
    
    // 測試動畫完成
    progressBar->setValue(100);
    QSignalSpy completeSpy(progressBar, &CyberProgressBar::animationFinished);
    QVERIFY(completeSpy.wait(2000));
    
    delete progressBar;
}

void TestLLMGUI::testRequestCreation()
{
    // 測試請求創建
    QSignalSpy requestSpy(m_gui, &LLMGUI::requestCreated);
    
    // 創建新請求
    QString prompt = "Test prompt for AI generation";
    QString model = "llama3:latest";
    QString academy = "Warrior";
    
    m_gui->createNewRequest(prompt, model, academy);
    
    QCOMPARE(requestSpy.count(), 1);
    
    auto args = requestSpy.takeFirst();
    QString requestId = args.at(0).toString();
    QVERIFY(!requestId.isEmpty());
    
    // 檢查請求是否添加到列表
    QVERIFY(m_gui->hasRequest(requestId));
}

void TestLLMGUI::testRequestCancellation()
{
    // 測試請求取消
    QString requestId = m_gui->createNewRequest("Test prompt", "llama3", "Mage");
    
    QSignalSpy cancelSpy(m_gui, &LLMGUI::requestCancelled);
    
    m_gui->cancelRequest(requestId);
    
    QCOMPARE(cancelSpy.count(), 1);
    
    auto args = cancelSpy.takeFirst();
    QString cancelledId = args.at(0).toString();
    QCOMPARE(cancelledId, requestId);
}

void TestLLMGUI::testBatchRequestManagement()
{
    // 測試批量請求管理
    QStringList prompts = {
        "Generate warrior skill",
        "Generate mage spell",
        "Generate archer ability"
    };
    
    QSignalSpy batchSpy(m_gui, &LLMGUI::batchRequestCreated);
    
    QString batchId = m_gui->createBatchRequest(prompts, "llama3", "Mixed");
    
    QCOMPARE(batchSpy.count(), 1);
    QVERIFY(!batchId.isEmpty());
    
    // 檢查批量請求狀態
    QVERIFY(m_gui->hasBatchRequest(batchId));
    QCOMPARE(m_gui->getBatchRequestCount(batchId), prompts.size());
}

void TestLLMGUI::testWebSocketConnection()
{
    // 測試WebSocket連接
    GameEventSyncer* syncer = m_gui->getGameEventSyncer();
    QVERIFY(syncer != nullptr);
    
    QSignalSpy connectSpy(syncer, &GameEventSyncer::connected);
    QSignalSpy disconnectSpy(syncer, &GameEventSyncer::disconnected);
    
    // 測試連接
    syncer->connectToGameServer("ws://localhost:8080");
    QVERIFY(connectSpy.wait(5000));
    QCOMPARE(connectSpy.count(), 1);
    
    // 測試斷線
    syncer->disconnectFromGameServer();
    QVERIFY(disconnectSpy.wait(3000));
    QCOMPARE(disconnectSpy.count(), 1);
}

void TestLLMGUI::testGameEventHandling()
{
    // 測試遊戲事件處理
    GameEventSyncer* syncer = m_gui->getGameEventSyncer();
    
    QSignalSpy eventSpy(syncer, &GameEventSyncer::gameEventReceived);
    
    // 模擬遊戲事件
    QJsonObject event;
    event["type"] = "player_message";
    event["player_id"] = "player123";
    event["message"] = "Need skill generation";
    event["academy"] = "Warrior";
    
    QJsonDocument doc(event);
    syncer->handleIncomingMessage(doc.toJson());
    
    QCOMPARE(eventSpy.count(), 1);
    
    auto args = eventSpy.takeFirst();
    QJsonObject receivedEvent = args.at(0).toJsonObject();
    QCOMPARE(receivedEvent["type"].toString(), "player_message");
    QCOMPARE(receivedEvent["player_id"].toString(), "player123");
}

void TestLLMGUI::testSettingsSaving()
{
    // 測試設置保存
    m_gui->setTheme(LLMGUI::RedAlert);
    m_gui->setWindowOpacity(0.8);
    m_gui->setDefaultModel("deepseek-coder");
    
    QSignalSpy saveSpy(m_gui, &LLMGUI::settingsSaved);
    
    m_gui->saveSettings();
    
    QCOMPARE(saveSpy.count(), 1);
    
    // 檢查設置是否正確保存
    QCOMPARE(m_gui->getCurrentTheme(), LLMGUI::RedAlert);
    QCOMPARE(m_gui->windowOpacity(), 0.8);
    QCOMPARE(m_gui->getDefaultModel(), QString("deepseek-coder"));
}

void TestLLMGUI::testSettingsLoading()
{
    // 測試設置加載
    // 先保存一些設置
    m_gui->setTheme(LLMGUI::GoldenGlow);
    m_gui->setWindowOpacity(0.9);
    m_gui->saveSettings();
    
    // 創建新的GUI實例
    LLMGUI* newGui = new LLMGUI();
    
    QSignalSpy loadSpy(newGui, &LLMGUI::settingsLoaded);
    
    newGui->loadSettings();
    
    QCOMPARE(loadSpy.count(), 1);
    
    // 檢查設置是否正確加載
    QCOMPARE(newGui->getCurrentTheme(), LLMGUI::GoldenGlow);
    QCOMPARE(newGui->windowOpacity(), 0.9);
    
    delete newGui;
}

void TestLLMGUI::testAcademyColors()
{
    // 測試學院顏色主題
    struct AcademyTest {
        CyberProgressBar::Academy academy;
        QColor expectedColor;
    };
    
    QList<AcademyTest> tests = {
        {CyberProgressBar::Warrior, QColor("#FF4500")},
        {CyberProgressBar::Mage, QColor("#4169E1")},
        {CyberProgressBar::Archer, QColor("#228B22")},
        {CyberProgressBar::Thief, QColor("#8B008B")},
        {CyberProgressBar::Priest, QColor("#FFD700")}
    };
    
    for (const auto& test : tests) {
        CyberProgressBar* progressBar = new CyberProgressBar(m_testWidget);
        progressBar->setAcademy(test.academy);
        
        QColor actualColor = progressBar->getAcademyColor();
        QCOMPARE(actualColor, test.expectedColor);
        
        delete progressBar;
    }
}

void TestLLMGUI::testWindowDragging()
{
    // 測試窗口拖拽功能
    FramelessWindow* window = new FramelessWindow();
    window->show();
    
    QPoint initialPos = window->pos();
    
    // 模擬鼠標拖拽
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, QPoint(50, 20));
    QTest::mouseMove(window, QPoint(100, 70));
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, QPoint(100, 70));
    
    // 檢查窗口是否移動
    QPoint newPos = window->pos();
    QVERIFY(newPos != initialPos);
    
    window->close();
    delete window;
}

QTEST_MAIN(TestLLMGUI)
#include "TestLLMGUI.moc"
