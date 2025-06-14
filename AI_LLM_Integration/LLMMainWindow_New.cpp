#include "LLMMainWindow.h"
#include "OllamaLLMManager.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QMessageBox>
#include <QFile>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

// 學院主題定義 - 繁體中文化
const QMap<QString, AcademyTheme> LLMMainWindow::ACADEMY_THEMES = {
    {"賽博藍", {
        "賽博藍",
        QColor(70, 130, 255),   // 主色
        QColor(30, 60, 120),    // 次色
        QColor(100, 200, 255),  // 強調色
        QColor(255, 255, 255),  // 文字色
        "linear-gradient(45deg, #4682ff, #1e3c78)"
    }},
    {"霓虹綠", {
        "霓虹綠", 
        QColor(0, 255, 127),
        QColor(0, 150, 75),
        QColor(50, 255, 150),
        QColor(255, 255, 255),
        "linear-gradient(45deg, #00ff7f, #00964b)"
    }},
    {"聖門金", {
        "聖門金",
        QColor(255, 215, 0),
        QColor(184, 134, 11),
        QColor(255, 255, 224),
        QColor(255, 255, 255),
        "linear-gradient(45deg, #ffd700, #b8860b)"
    }},
    {"懸岩墨", {
        "懸岩墨",
        QColor(105, 105, 105),
        QColor(64, 64, 64),
        QColor(169, 169, 169),
        QColor(255, 255, 255),
        "linear-gradient(45deg, #696969, #404040)"
    }},
    {"鳳凰紅", {
        "鳳凰紅",
        QColor(220, 20, 60),
        QColor(139, 0, 0),
        QColor(255, 69, 0),
        QColor(255, 255, 255),
        "linear-gradient(45deg, #dc143c, #8b0000)"
    }}
};

LLMMainWindow::LLMMainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , m_llmManager(nullptr)
    , m_aiManagementWidget(nullptr)
    , m_aiPlayerGenerator(nullptr)
    , m_aiDecisionEngine(nullptr)
    , m_logoLabel(nullptr)
    , m_logoPixmap(nullptr)
    , m_brandLabel(nullptr)
    , m_settings(new QSettings(this))
{
    setWindowTitle(tr("蘭線上 AI 大型語言模型整合系統 - Jy技術團隊 v3.0.0"));
    setMinimumSize(1400, 900);
    resize(1600, 1000);
    
    // 載入品牌資源和設置圖示
    loadBrandResources();
    setupBrandingAndIcons();
    
    // 初始化元件
    m_llmManager = new OllamaLLMManager(this);
    
    // 初始化AI管控系統
    initializeAIManagement();
    
    // 設置UI
    setupUI();
    setupConnections();
    
    // 載入設定
    loadSettings();
    applyTheme();
}

LLMMainWindow::~LLMMainWindow()
{
    saveSettings();
    if (m_logoPixmap) {
        delete m_logoPixmap;
    }
}

void LLMMainWindow::setupUI()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // 頂部品牌區域
    auto *headerLayout = new QHBoxLayout();
    
    // 設置Logo顯示
    if (m_logoLabel && m_brandLabel) {
        headerLayout->addWidget(m_logoLabel);
        headerLayout->addWidget(m_brandLabel);
    }
    
    // 添加彈性空間
    headerLayout->addStretch();
    
    // 版本資訊標籤
    auto *versionLabel = new QLabel("v3.0.0 - AI管控系統", this);
    versionLabel->setStyleSheet(
        "font-size: 12px; color: #666666; font-style: italic;"
    );
    headerLayout->addWidget(versionLabel);
    
    layout->addLayout(headerLayout);
    
    // 標題
    auto *titleLabel = new QLabel(tr("蘭線上 AI 大型語言模型整合系統"), this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4682ff; margin: 10px 0;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 主要內容區域
    m_tabWidget = new QTabWidget(this);
    layout->addWidget(m_tabWidget);
    
    setupRequestTab();
    setupBatchTab();
    setupServerTab();
    setupAIManagementTab();  // AI管控標籤頁
    
    // 狀態列
    m_statusLabel = new QLabel(tr("準備就緒"), this);
    m_statusLabel->setStyleSheet("color: #00ff7f; padding: 5px;");
    layout->addWidget(m_statusLabel);
}

void LLMMainWindow::setupRequestTab()
{
    m_requestTab = new QWidget();
    auto *layout = new QVBoxLayout(m_requestTab);
    
    // 模型選擇
    auto *modelLayout = new QHBoxLayout();
    modelLayout->addWidget(new QLabel(tr("模型:"), this));
    m_modelCombo = new QComboBox(this);
    m_modelCombo->addItems({"llama3:latest", "phi3:latest", "mistral:latest", "gemma:latest", "qwen2:latest", "deepseek:latest"});
    modelLayout->addWidget(m_modelCombo);
    
    // 學院選擇
    modelLayout->addWidget(new QLabel(tr("學院:"), this));
    m_academyCombo = new QComboBox(this);
    m_academyCombo->addItems({"聖門學院", "懸岩學院", "鳳凰學院"});
    modelLayout->addWidget(m_academyCombo);
    
    layout->addLayout(modelLayout);
    
    // 提示輸入
    layout->addWidget(new QLabel(tr("提示詞:"), this));
    m_promptEdit = new QTextEdit(this);
    m_promptEdit->setPlaceholderText(tr("請在此輸入您的提示詞..."));
    m_promptEdit->setMaximumHeight(150);
    layout->addWidget(m_promptEdit);
    
    // 提交按鈕
    m_submitButton = new QPushButton(tr("提交請求"), this);
    m_submitButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["賽博藍"]));
    layout->addWidget(m_submitButton);
    
    // 請求容器
    m_requestScrollArea = new QScrollArea(this);
    m_requestContainer = new QWidget();
    m_requestContainerLayout = new QVBoxLayout(m_requestContainer);
    m_requestScrollArea->setWidget(m_requestContainer);
    m_requestScrollArea->setWidgetResizable(true);
    layout->addWidget(m_requestScrollArea);
    
    m_tabWidget->addTab(m_requestTab, tr("🤖 AI 請求"));
}

void LLMMainWindow::setupBatchTab()
{
    m_batchTab = new QWidget();
    auto *layout = new QVBoxLayout(m_batchTab);
    
    // 批次處理標題
    auto *titleLabel = new QLabel(tr("批次處理介面"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4682ff; margin-bottom: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 批次提示輸入
    layout->addWidget(new QLabel(tr("批次提示詞模板:"), this));
    m_batchPromptEdit = new QTextEdit(this);
    m_batchPromptEdit->setPlaceholderText(tr("輸入批次處理的提示詞模板，可使用變數 {name}, {academy}, {department}"));
    m_batchPromptEdit->setMaximumHeight(120);
    layout->addWidget(m_batchPromptEdit);
    
    // 批次數量設定
    auto *countLayout = new QHBoxLayout();
    countLayout->addWidget(new QLabel(tr("批次數量:"), this));
    m_batchCountSpin = new QSpinBox(this);
    m_batchCountSpin->setRange(1, 100);
    m_batchCountSpin->setValue(10);
    countLayout->addWidget(m_batchCountSpin);
    countLayout->addStretch();
    layout->addLayout(countLayout);
    
    // 批次操作按鈕
    auto *buttonLayout = new QHBoxLayout();
    m_batchSubmitButton = new QPushButton(tr("開始批次處理"), this);
    m_batchSubmitButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["霓虹綠"]));
    m_batchCancelButton = new QPushButton(tr("取消批次處理"), this);
    m_batchCancelButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["鳳凰紅"]));
    
    buttonLayout->addWidget(m_batchSubmitButton);
    buttonLayout->addWidget(m_batchCancelButton);
    layout->addLayout(buttonLayout);
    
    // 批次結果列表
    layout->addWidget(new QLabel(tr("批次處理結果:"), this));
    m_batchList = new QListWidget(this);
    layout->addWidget(m_batchList);
    
    m_tabWidget->addTab(m_batchTab, tr("📋 批次處理"));
}

void LLMMainWindow::setupServerTab()
{
    m_serverTab = new QWidget();
    auto *layout = new QVBoxLayout(m_serverTab);
    
    // 伺服器管理標題
    auto *titleLabel = new QLabel(tr("伺服器管理介面"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4682ff; margin-bottom: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 伺服器操作按鈕
    auto *buttonLayout = new QHBoxLayout();
    m_addServerButton = new QPushButton(tr("新增伺服器"), this);
    m_removeServerButton = new QPushButton(tr("移除伺服器"), this);
    m_testConnectionButton = new QPushButton(tr("測試連線"), this);
    
    m_addServerButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["霓虹綠"]));
    m_removeServerButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["鳳凰紅"]));
    m_testConnectionButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["賽博藍"]));
    
    buttonLayout->addWidget(m_addServerButton);
    buttonLayout->addWidget(m_removeServerButton);
    buttonLayout->addWidget(m_testConnectionButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    // 伺服器列表
    layout->addWidget(new QLabel(tr("伺服器清單:"), this));
    m_serverList = new QListWidget(this);
    // 添加預設伺服器
    m_serverList->addItem("本地 Ollama 伺服器 (localhost:11434)");
    m_serverList->addItem("遠端 AI 伺服器 (待設定)");
    layout->addWidget(m_serverList);
    
    m_tabWidget->addTab(m_serverTab, tr("🌐 伺服器管理"));
}

void LLMMainWindow::setupAIManagementTab()
{
    m_aiManagementTab = new QWidget();
    auto *layout = new QVBoxLayout(m_aiManagementTab);
    
    // 標題
    auto *titleLabel = new QLabel(tr("四部門AI智慧管控系統"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4682ff; margin-bottom: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // AI 管控功能按鈕
    auto *aiButtonLayout = new QHBoxLayout();
    
    auto *generateAIButton = new QPushButton(tr("生成AI玩家"), this);
    generateAIButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["聖門金"]));
    connect(generateAIButton, &QPushButton::clicked, [this]() {
        onTestAIRequested(20, "llama3:latest");
    });
    
    auto *testNamingButton = new QPushButton(tr("測試命名系統"), this);
    testNamingButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["懸岩墨"]));
    connect(testNamingButton, &QPushButton::clicked, [this]() {
        testAcademyNamingSystem();
    });
    
    auto *testDecisionButton = new QPushButton(tr("測試決策引擎"), this);
    testDecisionButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["鳳凰紅"]));
    connect(testDecisionButton, &QPushButton::clicked, [this]() {
        if (m_aiDecisionEngine) {
            QJsonArray testData = m_aiPlayerGenerator->generateTestData(5);
            m_aiDecisionEngine->runBatchTest(testData, "llama3:latest");
        }
    });
    
    aiButtonLayout->addWidget(generateAIButton);
    aiButtonLayout->addWidget(testNamingButton);
    aiButtonLayout->addWidget(testDecisionButton);
    aiButtonLayout->addStretch();
    layout->addLayout(aiButtonLayout);
    
    // 添加AI管控主介面
    if (m_aiManagementWidget) {
        layout->addWidget(m_aiManagementWidget);
    } else {
        auto *placeholder = new QLabel(tr("AI管控系統載入中..."), this);
        placeholder->setAlignment(Qt::AlignCenter);
        placeholder->setStyleSheet("color: #888; font-style: italic; font-size: 14px;");
        layout->addWidget(placeholder);
    }
    
    // 添加到主標籤頁
    m_tabWidget->addTab(m_aiManagementTab, tr("🤖 AI管控系統"));
}

void LLMMainWindow::initializeAIManagement()
{
    try {
        m_aiPlayerGenerator = new RANOnline::AI::AIPlayerGenerator(this);
        m_aiDecisionEngine = new RANOnline::AI::AIDecisionEngine(this);
        m_aiManagementWidget = new RANOnline::AI::AIManagementWidget(this);
        
        // 連接AI管控訊號
        connect(m_aiPlayerGenerator, &RANOnline::AI::AIPlayerGenerator::aiPlayerGenerated,
                this, &LLMMainWindow::onAIPlayerGenerated);
        connect(m_aiPlayerGenerator, &RANOnline::AI::AIPlayerGenerator::logGenerated,
                this, &LLMMainWindow::onAILogGenerated);
        connect(m_aiPlayerGenerator, &RANOnline::AI::AIPlayerGenerator::errorOccurred,
                this, &LLMMainWindow::onAIErrorOccurred);
        
        connect(m_aiDecisionEngine, &RANOnline::AI::AIDecisionEngine::decisionMade,
                this, &LLMMainWindow::onAIDecisionMade);
        connect(m_aiDecisionEngine, &RANOnline::AI::AIDecisionEngine::testCompleted,
                this, &LLMMainWindow::onAITestCompleted);
        connect(m_aiDecisionEngine, &RANOnline::AI::AIDecisionEngine::errorOccurred,
                this, &LLMMainWindow::onAIDecisionError);
        
        // 連接AI管控Widget訊號
        if (m_aiManagementWidget) {
            connect(m_aiManagementWidget, &RANOnline::AI::AIManagementWidget::generateAIRequested,
                    m_aiPlayerGenerator, &RANOnline::AI::AIPlayerGenerator::generateAIPlayers);
            connect(m_aiManagementWidget, &RANOnline::AI::AIManagementWidget::testAIRequested,
                    this, &LLMMainWindow::onTestAIRequested);
        }
        
        qDebug() << "AI管控系統初始化完成";
        
    } catch (const std::exception &e) {
        qDebug() << "AI管控系統初始化失敗：" << e.what();
    }
}

void LLMMainWindow::setupConnections()
{
    // 基本按鈕連接
    connect(m_submitButton, &QPushButton::clicked, this, &LLMMainWindow::onSubmitRequest);
    
    // 批次處理按鈕連接
    if (m_batchSubmitButton && m_batchCancelButton) {
        connect(m_batchSubmitButton, &QPushButton::clicked, this, &LLMMainWindow::onSubmitBatchRequests);
        connect(m_batchCancelButton, &QPushButton::clicked, this, &LLMMainWindow::onCancelAllRequests);
    }
    
    // 伺服器管理按鈕連接
    if (m_addServerButton && m_removeServerButton && m_testConnectionButton) {
        connect(m_addServerButton, &QPushButton::clicked, this, &LLMMainWindow::onAddServer);
        connect(m_removeServerButton, &QPushButton::clicked, this, &LLMMainWindow::onRemoveServer);
        connect(m_testConnectionButton, &QPushButton::clicked, this, &LLMMainWindow::onTestConnection);
    }
    
    // LLM 管理器連接
    if (m_llmManager) {
        connect(m_llmManager, &OllamaLLMManager::requestCompleted, 
                this, &LLMMainWindow::onLLMRequestCompleted);
        connect(m_llmManager, &OllamaLLMManager::requestFailed,
                this, &LLMMainWindow::onLLMRequestFailed);
    }
}

void LLMMainWindow::onSubmitRequest()
{
    QString prompt = m_promptEdit->toPlainText().trimmed();
    if (prompt.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("請輸入提示詞。"));
        return;
    }
    
    LLMRequestConfig config;
    config.requestId = QString("req_%1").arg(QDateTime::currentMSecsSinceEpoch());
    config.prompt = prompt;
    config.model = m_modelCombo->currentText();
    config.academy = m_academyCombo->currentText();
    
    if (m_llmManager) {
        m_llmManager->submitRequest(config);
        m_statusLabel->setText(tr("請求已提交: %1").arg(config.requestId));
        m_promptEdit->clear();
    }
}

void LLMMainWindow::onSubmitBatchRequests()
{
    QString batchPrompt = m_batchPromptEdit->toPlainText().trimmed();
    if (batchPrompt.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("請輸入批次提示詞模板。"));
        return;
    }
    
    int count = m_batchCountSpin->value();
    m_statusLabel->setText(tr("開始批次處理 %1 個請求...").arg(count));
    
    // 執行批次請求
    for (int i = 0; i < count; ++i) {
        QString processedPrompt = batchPrompt;
        processedPrompt.replace("{index}", QString::number(i + 1));
        processedPrompt.replace("{academy}", m_academyCombo->currentText());
        
        LLMRequestConfig config;
        config.requestId = QString("batch_req_%1_%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(i);
        config.prompt = processedPrompt;
        config.model = m_modelCombo->currentText();
        config.academy = m_academyCombo->currentText();
        
        if (m_llmManager) {
            m_llmManager->submitRequest(config);
        }
        
        m_batchList->addItem(tr("批次請求 %1: %2").arg(i + 1).arg(config.requestId));
    }
    
    m_statusLabel->setText(tr("批次請求已全部提交"));
}

void LLMMainWindow::onLLMRequestCompleted(const LLMResponse &response)
{
    m_statusLabel->setText(tr("請求完成: %1").arg(response.requestId));
    
    // 顯示結果
    QMessageBox::information(this, tr("請求完成"), 
                           tr("回應: %1").arg(response.content.left(200) + "..."));
    
    // 如果是批次請求，更新列表
    if (response.requestId.startsWith("batch_")) {
        for (int i = 0; i < m_batchList->count(); ++i) {
            QListWidgetItem *item = m_batchList->item(i);
            if (item->text().contains(response.requestId)) {
                item->setText(item->text() + " - ✅ 完成");
                break;
            }
        }
    }
}

void LLMMainWindow::onLLMRequestFailed(const QString &requestId, const QString &error)
{
    m_statusLabel->setText(tr("請求失敗: %1").arg(requestId));
    QMessageBox::warning(this, tr("請求失敗"), tr("錯誤: %1").arg(error));
    
    // 如果是批次請求，更新列表
    if (requestId.startsWith("batch_")) {
        for (int i = 0; i < m_batchList->count(); ++i) {
            QListWidgetItem *item = m_batchList->item(i);
            if (item->text().contains(requestId)) {
                item->setText(item->text() + " - ❌ 失敗");
                break;
            }
        }
    }
}

void LLMMainWindow::testAcademyNamingSystem()
{
    if (!m_aiPlayerGenerator) {
        qDebug() << "AI Player Generator 未初始化";
        m_statusLabel->setText(tr("AI玩家生成器未初始化"));
        return;
    }
    
    m_statusLabel->setText(tr("執行學院命名系統測試中..."));
    
    qDebug() << "=== RAN Online 四部門AI學院命名系統測試 ===";
    qDebug() << "測試新的三學院命名規則：";
    qDebug() << "• 聖門：God主題（威嚴神聖）";
    qDebug() << "• 懸岩：水墨風格（詩意文雅）";
    qDebug() << "• 鳳凰：心情感系列（情感豐富）";
    qDebug() << "";
    
    // 生成100個AI進行統計測試
    QJsonArray testPlayers = m_aiPlayerGenerator->generateAIPlayers(100);
    
    QMap<QString, int> academyCount;
    QMap<QString, int> nameLength;
    QMap<QString, QStringList> sampleNames;
    QStringList overlengthNames;
    
    for (const QJsonValue &value : testPlayers) {
        QJsonObject player = value.toObject();
        QString academy = player["academy"].toString();
        QString name = player["name"].toString();
        
        academyCount[academy]++;
        nameLength[QString::number(name.length())]++;
        
        // 收集樣本名字
        if (sampleNames[academy].size() < 5) {
            sampleNames[academy].append(name);
        }
        
        // 檢查超長名字
        if (name.length() > 6) {
            overlengthNames.append(QString("%1 (%2字符)").arg(name).arg(name.length()));
        }
    }
    
    // 輸出統計結果
    QString testReport;
    testReport += "=== 學院命名系統測試報告 ===\n";
    
    testReport += "學院分佈：\n";
    for (auto it = academyCount.begin(); it != academyCount.end(); ++it) {
        testReport += QString("  %1: %2名AI\n").arg(it.key()).arg(it.value());
        testReport += QString("    樣本名字: %1\n").arg(sampleNames[it.key()].join(", "));
    }
    
    testReport += "\n名字長度分佈：\n";
    for (auto it = nameLength.begin(); it != nameLength.end(); ++it) {
        testReport += QString("  %1字符: %2個名字\n").arg(it.key()).arg(it.value());
    }
    
    if (overlengthNames.isEmpty()) {
        testReport += "\n✅ 所有名字都符合6字符限制！\n";
        m_statusLabel->setText(tr("命名系統測試完成 - 全部通過"));
    } else {
        testReport += "\n⚠️ 發現超長名字：\n";
        for (const QString &name : overlengthNames) {
            testReport += QString("    %1\n").arg(name);
        }
        m_statusLabel->setText(tr("命名系統測試完成 - 發現 %1 個超長名字").arg(overlengthNames.size()));
    }
    
    qDebug() << testReport;
    
    // 顯示測試結果對話框
    QMessageBox::information(this, tr("命名系統測試結果"), testReport);
}

// ========== Jy技術團隊品牌和圖示設置方法 ==========

void LLMMainWindow::loadBrandResources()
{
    // 載入應用程式圖示
    m_appIcon = QIcon(":/logo/app_icon.ico");
    if (m_appIcon.isNull()) {
        // 如果資源檔案不可用，使用本地檔案
        m_appIcon = QIcon("Resources/jy1.ico");
    }
    
    // 載入任務欄圖示
    m_taskbarIcon = QIcon(":/logo/taskbar_icon.ico");
    if (m_taskbarIcon.isNull()) {
        m_taskbarIcon = QIcon("Resources/jy2.ico");
    }
    
    // 載入Logo圖片
    QString logoPath = ":/logo/jy_tech_team.ico";
    if (!QFile::exists(logoPath)) {
        logoPath = "Resources/jy1.ico";
    }
    
    m_logoPixmap = new QPixmap(logoPath);
    if (m_logoPixmap && !m_logoPixmap->isNull()) {
        // 調整Logo大小為48x48像素，保持高品質
        *m_logoPixmap = m_logoPixmap->scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    qDebug() << "品牌資源載入完成";
}

void LLMMainWindow::setupApplicationIcon()
{
    if (!m_appIcon.isNull()) {
        setWindowIcon(m_appIcon);
        
        // 設置應用程式圖示
        QApplication::setWindowIcon(m_appIcon);
        
        qDebug() << "應用程式圖示設置完成";
    } else {
        qWarning() << "無法載入應用程式圖示";
    }
}

void LLMMainWindow::setupTaskbarIcon()
{
    if (!m_taskbarIcon.isNull()) {
        // 如果有系統托盤圖示，更新它
        if (m_trayIcon) {
            m_trayIcon->setIcon(m_taskbarIcon);
            m_trayIcon->setToolTip("蘭線上 AI管控系統 - Jy技術團隊");
        }
        
        qDebug() << "任務欄圖示設置完成";
    } else {
        qWarning() << "無法載入任務欄圖示";
    }
}

void LLMMainWindow::setupLogoDisplay()
{
    if (!m_logoPixmap || m_logoPixmap->isNull()) {
        qWarning() << "Logo圖片不可用";
        return;
    }
    
    // 創建Logo標籤
    m_logoLabel = new QLabel(this);
    m_logoLabel->setPixmap(*m_logoPixmap);
    m_logoLabel->setFixedSize(48, 48);
    m_logoLabel->setScaledContents(true);
    m_logoLabel->setStyleSheet(
        "QLabel {"
        "   border: 2px solid #4682ff;"
        "   border-radius: 8px;"
        "   background-color: rgba(70, 130, 255, 0.1);"
        "   padding: 2px;"
        "}"
    );
    
    // 創建品牌文字標籤
    m_brandLabel = new QLabel("Jy技術團隊", this);
    m_brandLabel->setStyleSheet(
        "QLabel {"
        "   font-family: 'Microsoft YaHei', '微软雅黑';"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   color: #4682ff;"
        "   margin-left: 8px;"
        "}"
    );
    
    qDebug() << "Logo顯示設置完成";
}

void LLMMainWindow::setupBrandingAndIcons()
{
    setupApplicationIcon();
    setupTaskbarIcon();
    setupLogoDisplay();
    
    qDebug() << "品牌標識和圖示設置完成";
}

// ========== 設定和主題方法 ==========

void LLMMainWindow::loadSettings()
{
    // 載入視窗幾何
    restoreGeometry(m_settings->value("geometry").toByteArray());
    
    // 載入主題
    QString themeName = m_settings->value("theme", "賽博藍").toString();
    if (ACADEMY_THEMES.contains(themeName)) {
        m_currentTheme = ACADEMY_THEMES[themeName];
    } else {
        m_currentTheme = ACADEMY_THEMES["賽博藍"];
    }
}

void LLMMainWindow::saveSettings()
{
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("theme", m_currentTheme.name);
}

void LLMMainWindow::applyTheme()
{
    QString styleSheet = getThemeStyleSheet(m_currentTheme);
    setStyleSheet(styleSheet);
}

QString LLMMainWindow::getThemeStyleSheet(const AcademyTheme &theme)
{
    return QString(
        "QTabWidget::pane { "
        "    border: 1px solid %1; "
        "    background-color: rgba(20, 25, 35, 200); "
        "} "
        "QTabBar::tab { "
        "    background-color: rgba(30, 35, 45, 180); "
        "    color: %4; "
        "    padding: 8px 16px; "
        "    margin-right: 2px; "
        "} "
        "QTabBar::tab:selected { "
        "    background-color: %1; "
        "    color: white; "
        "} "
        "QTextEdit, QComboBox { "
        "    background-color: rgba(25, 30, 40, 200); "
        "    color: %4; "
        "    border: 1px solid %1; "
        "    border-radius: 4px; "
        "    padding: 5px; "
        "} "
        "QListWidget { "
        "    background-color: rgba(25, 30, 40, 200); "
        "    color: %4; "
        "    border: 1px solid %1; "
        "    border-radius: 4px; "
        "} "
        "QLabel { "
        "    color: %4; "
        "} "
    ).arg(theme.primaryColor.name(), 
          theme.secondaryColor.name(),
          theme.accentColor.name(),
          theme.textColor.name());
}

QString LLMMainWindow::getCyberButtonStyle(const AcademyTheme &theme)
{
    return QString(
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 %1, stop:1 %2); "
        "    color: white; "
        "    border: 1px solid %3; "
        "    border-radius: 8px; "
        "    padding: 10px 20px; "
        "    font-weight: bold; "
        "    font-size: 14px; "
        "} "
        "QPushButton:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                stop:0 %3, stop:1 %1); "
        "} "
        "QPushButton:pressed { "
        "    background: %2; "
        "} "
    ).arg(theme.primaryColor.name(),
          theme.secondaryColor.name(), 
          theme.accentColor.name());
}

// ========== 事件處理方法 ==========

void LLMMainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    FramelessWindow::closeEvent(event);
}

void LLMMainWindow::showEvent(QShowEvent *event)
{
    FramelessWindow::showEvent(event);
}

// ========== AI管控系統槽函數實現 ==========

void LLMMainWindow::onAIPlayerGenerated(const RANOnline::AI::AIPlayerData &player)
{
    QString logEntry = QString("[%1] 新AI玩家生成: %2 (%3-%4)")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(player.name).arg(player.academy).arg(player.department);
    qDebug() << logEntry;
    m_statusLabel->setText(tr("AI玩家已生成: %1").arg(player.name));
}

void LLMMainWindow::onAIErrorOccurred(const QString &error)
{
    QString logEntry = QString("[%1] AI系統錯誤: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(error);
    qDebug() << logEntry;
    m_statusLabel->setText(tr("AI系統錯誤: %1").arg(error));
}

void LLMMainWindow::onAIDecisionMade(const RANOnline::AI::AIDecisionResponse &response)
{
    QString logEntry = QString("[%1] AI決策完成: %2 -> %3")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(response.skill).arg(response.target);
    qDebug() << logEntry;
    m_statusLabel->setText(tr("AI決策完成"));
}

void LLMMainWindow::onAIDecisionError(const QString &error)
{
    QString logEntry = QString("[%1] AI決策錯誤: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(error);
    qDebug() << logEntry;
    m_statusLabel->setText(tr("AI決策錯誤: %1").arg(error));
}

void LLMMainWindow::onTestAIRequested(int count, const QString &modelType)
{
    if (!m_aiDecisionEngine || !m_aiPlayerGenerator) {
        qDebug() << "AI決策引擎或AI玩家生成器未初始化";
        m_statusLabel->setText(tr("AI系統元件未初始化"));
        return;
    }
    
    QString logEntry = QString("[%1] 開始AI批量測試: %2個AI, 模型: %3")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(count).arg(modelType);
    qDebug() << logEntry;
    m_statusLabel->setText(tr("開始AI批量測試: %1個AI").arg(count));
    
    // 執行批量AI測試
    QJsonArray testData = m_aiPlayerGenerator->generateTestData(count);
    m_aiDecisionEngine->runBatchTest(testData, modelType);
}

// ========== 佔位實現 - 其他方法的空實現（避免未使用參數警告）==========

void LLMMainWindow::onCancelAllRequests() 
{ 
    m_statusLabel->setText(tr("已取消所有請求"));
}

void LLMMainWindow::onRefreshModels() 
{ 
    m_statusLabel->setText(tr("正在重新整理模型清單..."));
}

void LLMMainWindow::onExportLogs() 
{ 
    m_statusLabel->setText(tr("匯出日誌功能開發中..."));
}

void LLMMainWindow::onImportTemplates() 
{ 
    m_statusLabel->setText(tr("匯入範本功能開發中..."));
}

void LLMMainWindow::onExportTemplates() 
{ 
    m_statusLabel->setText(tr("匯出範本功能開發中..."));
}

void LLMMainWindow::onAddServer() 
{ 
    m_statusLabel->setText(tr("新增伺服器功能開發中..."));
}

void LLMMainWindow::onRemoveServer() 
{ 
    m_statusLabel->setText(tr("移除伺服器功能開發中..."));
}

void LLMMainWindow::onTestConnection() 
{ 
    m_statusLabel->setText(tr("正在測試連線..."));
}

void LLMMainWindow::onThemeChanged() 
{ 
    m_statusLabel->setText(tr("主題已變更"));
}

void LLMMainWindow::onShowSettings() 
{ 
    m_statusLabel->setText(tr("設定介面開發中..."));
}

void LLMMainWindow::onShowAbout() 
{ 
    QMessageBox::about(this, tr("關於"), tr("Jy技術團隊 線上AI - 大型語言模型整合系統\n版本 3.0.0\n\n© 2025 Jy技術團隊\n所有權利保留"));
}

void LLMMainWindow::onToggleFullscreen() 
{ 
    if (isFullScreen()) {
        showNormal();
        m_statusLabel->setText(tr("已退出全螢幕模式"));
    } else {
        showFullScreen();
        m_statusLabel->setText(tr("已進入全螢幕模式"));
    }
}

void LLMMainWindow::onMinimizeToTray() 
{ 
    hide();
    m_statusLabel->setText(tr("已最小化至系統托盤"));
}

void LLMMainWindow::onLLMStreamUpdate(const QString &requestId, const QString &content) 
{ 
    Q_UNUSED(requestId)
    Q_UNUSED(content)
}

void LLMMainWindow::onLLMRequestProgress(const QString &requestId, int progress) 
{ 
    Q_UNUSED(requestId)
    m_statusLabel->setText(tr("請求進度: %1%").arg(progress));
}

void LLMMainWindow::onLLMModelsRefreshed(const QList<LLMModelInfo> &models) 
{ 
    Q_UNUSED(models)
    m_statusLabel->setText(tr("模型清單已更新"));
}

void LLMMainWindow::onLLMServerStatusChanged(const QString &serverName, bool isOnline) 
{ 
    Q_UNUSED(serverName)
    m_statusLabel->setText(isOnline ? tr("伺服器已連線") : tr("伺服器已離線"));
}

void LLMMainWindow::onLLMStatisticsUpdated() 
{ 
    m_statusLabel->setText(tr("統計資料已更新"));
}

void LLMMainWindow::onGameEventReceived(const QString &eventType, const QJsonObject &data) 
{ 
    Q_UNUSED(eventType)
    Q_UNUSED(data)
}

void LLMMainWindow::onPlayerMessageReceived(const QString &playerId, const QString &message) 
{ 
    Q_UNUSED(playerId)
    Q_UNUSED(message)
}

void LLMMainWindow::onSkillGenerationRequest(const QString &academy, int level) 
{ 
    Q_UNUSED(academy)
    Q_UNUSED(level)
}

void LLMMainWindow::onGameConnectionStatusChanged(bool connected) 
{ 
    m_statusLabel->setText(connected ? tr("遊戲已連線") : tr("遊戲已離線"));
}

void LLMMainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) 
{ 
    Q_UNUSED(reason)
    show();
    raise();
    activateWindow();
}

void LLMMainWindow::onAIManagementTabActivated() 
{ 
    m_statusLabel->setText(tr("AI管控系統已啟動"));
}

void LLMMainWindow::onAIPlayersGenerated(const QJsonArray &players) 
{
    Q_UNUSED(players)
    m_statusLabel->setText(tr("AI玩家批次生成完成"));
}

void LLMMainWindow::onAIBatchDecisionCompleted(const QJsonArray &results) 
{
    Q_UNUSED(results)
    m_statusLabel->setText(tr("AI批次決策完成"));
}

void LLMMainWindow::onAITestCompleted(const RANOnline::AI::TestReport &report) 
{
    Q_UNUSED(report)
    m_statusLabel->setText(tr("AI測試完成"));
}

void LLMMainWindow::onAILogGenerated(const QString &logEntry) 
{
    Q_UNUSED(logEntry)
    // 實際實現時會將日誌顯示在UI中
}

void LLMMainWindow::onAutoRepairTriggered(const QString &aiId, const QString &repairAction) 
{
    Q_UNUSED(aiId)
    Q_UNUSED(repairAction)
    m_statusLabel->setText(tr("AI自動修復已觸發"));
}

// ========== 其他工具方法的佔位實現 ==========

void LLMMainWindow::setupTitleBar() {}
void LLMMainWindow::setupControlPanel() {}
void LLMMainWindow::setupTemplateTab() {}
void LLMMainWindow::setupSettingsTab() {}
void LLMMainWindow::setupResultPanel() {}
void LLMMainWindow::setupStatusBar() {}
void LLMMainWindow::setupTrayIcon() {}
void LLMMainWindow::applyTheme(const AcademyTheme &theme) { Q_UNUSED(theme) }
void LLMMainWindow::addRequest(const QString &requestId, const LLMRequestConfig &config) { Q_UNUSED(requestId) Q_UNUSED(config) }
void LLMMainWindow::updateRequest(const QString &requestId, const LLMResponse &response) { Q_UNUSED(requestId) Q_UNUSED(response) }
void LLMMainWindow::updateProgress(const QString &requestId, int progress) { Q_UNUSED(requestId) Q_UNUSED(progress) }
void LLMMainWindow::updateThemeColors() {}
void LLMMainWindow::updateServerList() {}
void LLMMainWindow::updateModelCombo() {}
void LLMMainWindow::updateStats() {}
void LLMMainWindow::addLogMessage(const QString &message, const QString &level) { Q_UNUSED(message) Q_UNUSED(level) }
void LLMMainWindow::exportLogsToFile() {}
void LLMMainWindow::showNotification(const QString &title, const QString &message) { Q_UNUSED(title) Q_UNUSED(message) }
QString LLMMainWindow::formatDuration(qint64 milliseconds) { Q_UNUSED(milliseconds) return QString(); }
QStringList LLMMainWindow::collectTemplateVariables(const QString &templateText) { Q_UNUSED(templateText) return QStringList(); }
LLMRequestConfig LLMMainWindow::createRequestConfig() { return LLMRequestConfig(); }
void LLMMainWindow::handleGameSkillGeneration(const QString &academy, int level) { Q_UNUSED(academy) Q_UNUSED(level) }
void LLMMainWindow::handlePlayerInteraction(const QString &playerId, const QString &message) { Q_UNUSED(playerId) Q_UNUSED(message) }
AcademyTheme LLMMainWindow::getAcademyTheme(const QString &academy) { Q_UNUSED(academy) return ACADEMY_THEMES["賽博藍"]; }
QString LLMMainWindow::getGlowEffectStyle(const AcademyTheme &theme) { Q_UNUSED(theme) return QString(); }
void LLMMainWindow::connectAISignals() {}
