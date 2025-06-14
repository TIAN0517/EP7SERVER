// LLMGUI.cpp - 第二部分
// 包含GameEventSyncer和LLMMainWindow的完整實現

#include "LLMGUI.h"
#include <QObject>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QString>
#include <QUuid>
#include <QStringList>
#include <memory>

#ifdef HAVE_WEBSOCKETS
#include <QWebSocket>
#endif

// ====================================================================
// GameEventSyncer Implementation
// ====================================================================

#ifdef HAVE_WEBSOCKETS
GameEventSyncer::GameEventSyncer(QObject *parent)
    : QObject(parent)
    , m_webSocket(std::make_unique<QWebSocket>())
{
    connect(m_webSocket.get(), &QWebSocket::connected, this, &GameEventSyncer::onWebSocketConnected);
    connect(m_webSocket.get(), &QWebSocket::disconnected, this, &GameEventSyncer::onWebSocketDisconnected);
    connect(m_webSocket.get(), &QWebSocket::textMessageReceived, this, &GameEventSyncer::onTextMessageReceived);
    connect(m_webSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &GameEventSyncer::onWebSocketError);
}
#else
// WebSocket不可用时的简化实现
GameEventSyncer::GameEventSyncer(QObject *parent)
    : QObject(parent)
{
}

void GameEventSyncer::connectToGameServer(const QString &url)
{
    m_serverUrl = url;
    qDebug() << "連接到遊戲服務器:" << url;
    
#ifdef HAVE_WEBSOCKETS
    QNetworkRequest request(QUrl(url));
    request.setRawHeader("User-Agent", "RANOnline-AI-LLM-Client/2.0");
    request.setRawHeader("Origin", "https://ranonline.ai");
    
    m_webSocket->open(request);
#else
    qWarning() << "WebSocket支援未啟用，無法連接到遊戲服務器";
    Q_EMIT connectionStatusChanged(false);
#endif
}

void GameEventSyncer::disconnect()
{
#ifdef HAVE_WEBSOCKETS
    if (m_webSocket && m_isConnected) {
        m_webSocket->close();
    }
#else
    m_isConnected = false;
    Q_EMIT connectionStatusChanged(false);
#endif
}

bool GameEventSyncer::isConnected() const
{
    return m_isConnected;
}

void GameEventSyncer::sendAIResponse(const QString &playerId, const QString &response)
{
#ifdef HAVE_WEBSOCKETS
    if (!m_isConnected) return;
    
    QJsonObject message;
    message["type"] = "ai_response";
    message["player_id"] = playerId;
    message["response"] = response;
    message["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonDocument doc(message);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    
    qDebug() << "發送AI響應到遊戲:" << playerId << response.left(50) + "...";
#else
    Q_UNUSED(playerId)
    Q_UNUSED(response)
    qWarning() << "WebSocket支援未啟用，無法發送AI響應";
#endif
}

void GameEventSyncer::sendSkillGeneration(const QString &skillId, const QString &description)
{
#ifdef HAVE_WEBSOCKETS
    if (!m_isConnected) return;
    
    QJsonObject message;
    message["type"] = "skill_generated";
    message["skill_id"] = skillId;
    message["description"] = description;
    message["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonDocument doc(message);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    
    qDebug() << "發送技能生成結果:" << skillId;
#else
    Q_UNUSED(skillId)
    Q_UNUSED(description)
    qWarning() << "WebSocket支援未啟用，無法發送技能生成結果";
#endif
}

void GameEventSyncer::sendBatchResult(const QStringList &results)
{
#ifdef HAVE_WEBSOCKETS
    if (!m_isConnected) return;
    
    QJsonObject message;
    message["type"] = "batch_result";
    message["results"] = QJsonArray::fromStringList(results);
    message["count"] = results.size();
    message["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonDocument doc(message);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    
    qDebug() << "發送批量結果，數量:" << results.size();
#else
    Q_UNUSED(results)
    qWarning() << "WebSocket支援未啟用，無法發送批量結果";
#endif
}

void GameEventSyncer::onWebSocketConnected()
{
#ifdef HAVE_WEBSOCKETS
    m_isConnected = true;
    Q_EMIT connectionStatusChanged(true);
    qDebug() << "WebSocket連接成功";
    
    // 發送連接確認
    QJsonObject hello;
    hello["type"] = "hello";
    hello["client"] = "ai_llm_manager";
    hello["version"] = "2.0.0";
    hello["capabilities"] = QJsonArray{"ai_responses", "skill_generation", "batch_processing"};
    
    QJsonDocument doc(hello);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
#endif
}

void GameEventSyncer::onWebSocketDisconnected()
{
    m_isConnected = false;
    Q_EMIT connectionStatusChanged(false);
    qDebug() << "WebSocket連接斷開";
}

void GameEventSyncer::onWebSocketError(QAbstractSocket::SocketError error)
{
#ifdef HAVE_WEBSOCKETS
    m_isConnected = false;
    Q_EMIT connectionStatusChanged(false);
    qWarning() << "WebSocket錯誤:" << static_cast<int>(error);
#else
    Q_UNUSED(error)
#endif
}

void GameEventSyncer::onTextMessageReceived(const QString &message)
{
#ifdef HAVE_WEBSOCKETS
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "解析WebSocket訊息失敗:" << parseError.errorString();
        return;
    }
    
    QJsonObject obj = doc.object();
    QString eventType = obj["type"].toString();
    
    Q_EMIT gameEventReceived(eventType, obj);
    
    processGameEvent(obj);
#else
    Q_UNUSED(message)
#endif
}

void GameEventSyncer::processGameEvent(const QJsonObject &event)
{
#ifdef HAVE_WEBSOCKETS
    QString type = event["type"].toString();
    
    if (type == "player_message") {
        QString playerId = event["player_id"].toString();
        QString message = event["message"].toString();
        Q_EMIT playerMessageReceived(playerId, message);
        
    } else if (type == "skill_generation_request") {
        QString skillId = event["skill_id"].toString();
        QJsonObject requirements = event["requirements"].toObject();
        Q_EMIT skillGenerationRequest(skillId, requirements);
        
    } else if (type == "ping") {
        // 回應心跳
        QJsonObject pong;
        pong["type"] = "pong";
        pong["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        
        QJsonDocument doc(pong);
        m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
    }
#else
    Q_UNUSED(event)
#endif
}

// ====================================================================
// LLMMainWindow Implementation
// ====================================================================

LLMMainWindow::LLMMainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , m_llmManager(std::make_unique<OllamaLLMManager>(this))
    , m_templateManager(std::make_unique<PromptTemplateManager>(this))
    , m_gameSync(std::make_unique<GameEventSyncer>(this))
    , m_settings(new QSettings("JyTech", "RANOnline-LLM", this))
    , m_fadeAnimation(new QPropertyAnimation(this, "windowOpacity", this))
    , m_statusUpdateTimer(new QTimer(this))
{
    setWindowTitle("RANOnline EP7 AI - LLM控制中心 v2.0");
    setMinimumSize(1400, 900);
    resize(1600, 1000);
    
    // 設置窗口圖標
    setWindowIcon(QIcon(":/icons/ai_brain.png"));
    
    setupUI();
    setupTrayIcon();
    setupConnections();
    setupAnimations();
    
    // 初始化組件
    m_llmManager->initialize();
    m_templateManager->loadTemplates();
    
    loadSettings();
    applyTheme(m_currentTheme);
    
    // 啟動狀態更新定時器
    m_statusUpdateTimer->setInterval(2000);
    m_statusUpdateTimer->start();
    
    m_isInitialized = true;
}

LLMMainWindow::~LLMMainWindow()
{
    saveSettings();
    m_llmManager->shutdown();
}

void LLMMainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(10);
    
    setupTitleBar();
    
    // 主要分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainLayout->addWidget(m_mainSplitter);
    
    setupControlPanel();
    setupResultPanel();
    setupStatusBar();
}

void LLMMainWindow::setupTitleBar()
{
    m_titleBar = new QFrame(this);
    m_titleBar->setFixedHeight(50);
    m_titleBar->setFrameStyle(QFrame::NoFrame);
    
    m_titleLayout = new QHBoxLayout(m_titleBar);
    m_titleLayout->setContentsMargins(15, 5, 15, 5);
    
    // Logo
    m_logoLabel = new QLabel(this);
    m_logoLabel->setPixmap(QPixmap(":/icons/ran_logo.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_titleLayout->addWidget(m_logoLabel);
    
    // 標題
    m_titleLabel = new QLabel("RANOnline EP7 AI - LLM控制中心", this);
    m_titleLabel->setFont(QFont("Arial", 14, QFont::Bold));
    m_titleLabel->setStyleSheet("color: #FFFFFF; margin-left: 10px;");
    m_titleLayout->addWidget(m_titleLabel);
    
    m_titleLayout->addStretch();
    
    // 主題切換按鈕
    m_themeButton = new QPushButton("🎨", this);
    m_themeButton->setFixedSize(35, 35);
    m_themeButton->setToolTip("切換主題");
    connect(m_themeButton, &QPushButton::clicked, this, &LLMMainWindow::onThemeChanged);
    m_titleLayout->addWidget(m_themeButton);
    
    // 窗口控制按鈕
    m_minimizeButton = new QPushButton("−", this);
    m_minimizeButton->setFixedSize(35, 35);
    connect(m_minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);
    m_titleLayout->addWidget(m_minimizeButton);
    
    m_maximizeButton = new QPushButton("□", this);
    m_maximizeButton->setFixedSize(35, 35);
    connect(m_maximizeButton, &QPushButton::clicked, this, &LLMMainWindow::onToggleFullscreen);
    m_titleLayout->addWidget(m_maximizeButton);
    
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setFixedSize(35, 35);
    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
    m_titleLayout->addWidget(m_closeButton);
    
    m_mainLayout->addWidget(m_titleBar);
}

void LLMMainWindow::setupControlPanel()
{
    // 左側控制面板
    m_controlTabs = new QTabWidget();
    m_controlTabs->setMaximumWidth(450);
    m_controlTabs->setMinimumWidth(400);
    
    // 請求標籤頁
    m_requestTab = new QWidget();
    setupRequestTab();
    m_controlTabs->addTab(m_requestTab, "🤖 AI請求");
    
    // 批量標籤頁
    m_batchTab = new QWidget();
    setupBatchTab();
    m_controlTabs->addTab(m_batchTab, "📦 批量處理");
    
    // 服務器標籤頁
    m_serverTab = new QWidget();
    setupServerTab();
    m_controlTabs->addTab(m_serverTab, "🖥️ 服務器");
    
    // 模板標籤頁
    m_templateTab = new QWidget();
    setupTemplateTab();
    m_controlTabs->addTab(m_templateTab, "📝 模板");
    
    // 設置標籤頁
    m_settingsTab = new QWidget();
    setupSettingsTab();
    m_controlTabs->addTab(m_settingsTab, "⚙️ 設置");
    
    m_mainSplitter->addWidget(m_controlTabs);
}

void LLMMainWindow::setupRequestTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_requestTab);
    
    // 請求表單
    QGroupBox *requestGroup = new QGroupBox("AI請求配置", this);
    m_requestForm = new QFormLayout(requestGroup);
    
    m_modelCombo = new QComboBox(this);
    m_modelCombo->setEditable(true);
    m_requestForm->addRow("模型:", m_modelCombo);
    
    m_academyCombo = new QComboBox(this);
    m_academyCombo->addItems({"戰士", "法師", "弓手", "盜賊", "牧師"});
    m_requestForm->addRow("學院:", m_academyCombo);
    
    m_templateCombo = new QComboBox(this);
    m_templateCombo->addItem("自定義", "");
    m_requestForm->addRow("模板:", m_templateCombo);
    
    m_promptEdit = new QTextEdit(this);
    m_promptEdit->setMaximumHeight(120);
    m_promptEdit->setPlaceholderText("輸入提示詞...");
    m_requestForm->addRow("提示詞:", m_promptEdit);
    
    m_systemPromptEdit = new QTextEdit(this);
    m_systemPromptEdit->setMaximumHeight(80);
    m_systemPromptEdit->setPlaceholderText("系統提示詞（可選）");
    m_requestForm->addRow("系統提示:", m_systemPromptEdit);
    
    // 高級參數
    QGroupBox *advancedGroup = new QGroupBox("高級參數", this);
    QFormLayout *advancedLayout = new QFormLayout(advancedGroup);
    
    m_temperatureSpin = new QDoubleSpinBox(this);
    m_temperatureSpin->setRange(0.1, 2.0);
    m_temperatureSpin->setSingleStep(0.1);
    m_temperatureSpin->setValue(0.7);
    m_temperatureSpin->setDecimals(1);
    advancedLayout->addRow("溫度:", m_temperatureSpin);
    
    m_maxTokensSpin = new QSpinBox(this);
    m_maxTokensSpin->setRange(100, 4000);
    m_maxTokensSpin->setValue(1000);
    advancedLayout->addRow("最大Token:", m_maxTokensSpin);
    
    m_topKSpin = new QSpinBox(this);
    m_topKSpin->setRange(1, 100);
    m_topKSpin->setValue(40);
    advancedLayout->addRow("Top-K:", m_topKSpin);
    
    m_topPSpin = new QDoubleSpinBox(this);
    m_topPSpin->setRange(0.1, 1.0);
    m_topPSpin->setSingleStep(0.1);
    m_topPSpin->setValue(0.9);
    m_topPSpin->setDecimals(1);
    advancedLayout->addRow("Top-P:", m_topPSpin);
    
    // 控制按鈕
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_submitButton = new QPushButton("🚀 發送請求", this);
    m_submitButton->setMinimumHeight(40);
    buttonLayout->addWidget(m_submitButton);
    
    m_submitBatchButton = new QPushButton("📦 批量發送", this);
    m_submitBatchButton->setMinimumHeight(40);
    buttonLayout->addWidget(m_submitBatchButton);
    
    m_cancelAllButton = new QPushButton("⏹️ 全部取消", this);
    m_cancelAllButton->setMinimumHeight(40);
    buttonLayout->addWidget(m_cancelAllButton);
    
    layout->addWidget(requestGroup);
    layout->addWidget(advancedGroup);
    layout->addLayout(buttonLayout);
    layout->addStretch();
}

void LLMMainWindow::setupBatchTab()
{
    m_batchLayout = new QVBoxLayout(m_batchTab);
    
    QGroupBox *batchGroup = new QGroupBox("批量請求配置", this);
    QFormLayout *batchForm = new QFormLayout(batchGroup);
    
    m_batchCountSpin = new QSpinBox(this);
    m_batchCountSpin->setRange(2, 50);
    m_batchCountSpin->setValue(5);
    batchForm->addRow("請求數量:", m_batchCountSpin);
    
    m_batchModelCombo = new QComboBox(this);
    batchForm->addRow("批量模型:", m_batchModelCombo);
    
    m_batchPromptEdit = new QTextEdit(this);
    m_batchPromptEdit->setMaximumHeight(100);
    m_batchPromptEdit->setPlaceholderText("批量提示詞模板，使用{{變量}}表示替換內容");
    batchForm->addRow("提示模板:", m_batchPromptEdit);
    
    m_autoVariationCheck = new QCheckBox("自動變化提示詞", this);
    m_autoVariationCheck->setChecked(true);
    batchForm->addRow("", m_autoVariationCheck);
    
    m_generateBatchButton = new QPushButton("🎲 生成批量任務", this);
    batchForm->addRow("", m_generateBatchButton);
    
    // 預覽列表
    QGroupBox *previewGroup = new QGroupBox("任務預覽", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    
    m_batchPreviewList = new QListWidget(this);
    m_batchPreviewList->setMaximumHeight(200);
    previewLayout->addWidget(m_batchPreviewList);
    
    QPushButton *executeBatchButton = new QPushButton("⚡ 執行批量任務", this);
    executeBatchButton->setMinimumHeight(40);
    previewLayout->addWidget(executeBatchButton);
    
    m_batchLayout->addWidget(batchGroup);
    m_batchLayout->addWidget(previewGroup);
    m_batchLayout->addStretch();
}

void LLMMainWindow::setupServerTab()
{
    m_serverLayout = new QVBoxLayout(m_serverTab);
    
    QGroupBox *serverGroup = new QGroupBox("Ollama服務器管理", this);
    QVBoxLayout *serverGroupLayout = new QVBoxLayout(serverGroup);
    
    // 服務器列表
    m_serverList = new QListWidget(this);
    m_serverList->setMaximumHeight(200);
    serverGroupLayout->addWidget(m_serverList);
    
    // 添加服務器表單
    QGroupBox *addServerGroup = new QGroupBox("添加服務器", this);
    QFormLayout *addServerForm = new QFormLayout(addServerGroup);
    
    m_serverHostEdit = new QLineEdit(this);
    m_serverHostEdit->setPlaceholderText("localhost");
    addServerForm->addRow("主機:", m_serverHostEdit);
    
    m_serverPortSpin = new QSpinBox(this);
    m_serverPortSpin->setRange(1, 65535);
    m_serverPortSpin->setValue(11434);
    addServerForm->addRow("端口:", m_serverPortSpin);
    
    m_serverNameEdit = new QLineEdit(this);
    m_serverNameEdit->setPlaceholderText("服務器名稱");
    addServerForm->addRow("名稱:", m_serverNameEdit);
    
    QHBoxLayout *serverButtonLayout = new QHBoxLayout();
    
    m_addServerButton = new QPushButton("➕ 添加", this);
    serverButtonLayout->addWidget(m_addServerButton);
    
    m_removeServerButton = new QPushButton("➖ 移除", this);
    serverButtonLayout->addWidget(m_removeServerButton);
    
    m_testConnectionButton = new QPushButton("🔍 測試", this);
    serverButtonLayout->addWidget(m_testConnectionButton);
    
    addServerForm->addRow("操作:", serverButtonLayout);
    
    m_connectionStatusLabel = new QLabel("狀態: 未連接", this);
    addServerForm->addRow("連接狀態:", m_connectionStatusLabel);
    
    m_serverLayout->addWidget(serverGroup);
    m_serverLayout->addWidget(addServerGroup);
    m_serverLayout->addStretch();
}

void LLMMainWindow::setupTemplateTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_templateTab);
    
    QGroupBox *templateGroup = new QGroupBox("提示詞模板管理", this);
    QVBoxLayout *templateLayout = new QVBoxLayout(templateGroup);
    
    QHBoxLayout *templateButtonLayout = new QHBoxLayout();
    
    QPushButton *refreshTemplateButton = new QPushButton("🔄 刷新", this);
    templateButtonLayout->addWidget(refreshTemplateButton);
    
    QPushButton *importTemplateButton = new QPushButton("📁 導入", this);
    connect(importTemplateButton, &QPushButton::clicked, this, &LLMMainWindow::onImportTemplates);
    templateButtonLayout->addWidget(importTemplateButton);
    
    QPushButton *exportTemplateButton = new QPushButton("💾 導出", this);
    connect(exportTemplateButton, &QPushButton::clicked, this, &LLMMainWindow::onExportTemplates);
    templateButtonLayout->addWidget(exportTemplateButton);
    
    templateLayout->addLayout(templateButtonLayout);
    
    // TODO: 添加模板編輯界面
    QLabel *templateLabel = new QLabel("模板編輯功能開發中...", this);
    templateLabel->setAlignment(Qt::AlignCenter);
    templateLayout->addWidget(templateLabel);
    
    layout->addWidget(templateGroup);
    layout->addStretch();
}

void LLMMainWindow::setupSettingsTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_settingsTab);
    
    QGroupBox *gameGroup = new QGroupBox("遊戲同步設置", this);
    QFormLayout *gameForm = new QFormLayout(gameGroup);
    
    QLineEdit *gameServerEdit = new QLineEdit(this);
    gameServerEdit->setPlaceholderText("ws://localhost:8080/websocket");
    gameForm->addRow("遊戲服務器:", gameServerEdit);
    
    QPushButton *connectGameButton = new QPushButton("🔗 連接遊戲", this);
    gameForm->addRow("", connectGameButton);
    
    QGroupBox *logGroup = new QGroupBox("日誌設置", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    
    QPushButton *exportLogButton = new QPushButton("📄 導出日誌", this);
    connect(exportLogButton, &QPushButton::clicked, this, &LLMMainWindow::onExportLogs);
    logButtonLayout->addWidget(exportLogButton);
    
    QPushButton *clearLogButton = new QPushButton("🗑️ 清除日誌", this);
    logButtonLayout->addWidget(clearLogButton);
    
    logLayout->addLayout(logButtonLayout);
    
    QGroupBox *aboutGroup = new QGroupBox("關於", this);
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutGroup);
    
    QLabel *aboutLabel = new QLabel(
        "RANOnline EP7 AI - LLM控制中心 v2.0\n\n"
        "開發團隊: Jy技術團隊\n"
        "發布日期: 2025年6月14日\n\n"
        "支援多個Ollama LLM模型的統一管理\n"
        "包含批量請求、遊戲同步、智能分流等功能", this);
    aboutLabel->setWordWrap(true);
    aboutLayout->addWidget(aboutLabel);
    
    QPushButton *aboutButton = new QPushButton("ℹ️ 更多信息", this);
    connect(aboutButton, &QPushButton::clicked, this, &LLMMainWindow::onShowAbout);
    aboutLayout->addWidget(aboutButton);
    
    layout->addWidget(gameGroup);
    layout->addWidget(logGroup);
    layout->addWidget(aboutGroup);
    layout->addStretch();
}

void LLMMainWindow::setupResultPanel()
{
    // 右側結果面板
    m_resultTabs = new QTabWidget();
    m_resultTabs->setMinimumWidth(600);
    
    // 批量請求面板
    m_batchPanel = new BatchRequestPanel();
    m_resultTabs->addTab(m_batchPanel, "📋 請求列表");
    
    // 日誌面板
    m_logEdit = new QTextEdit();
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumBlockCount(1000); // 限制日誌行數
    m_resultTabs->addTab(m_logEdit, "📊 系統日誌");
    
    // 統計面板
    m_statsDashboard = new StatsDashboard();
    m_resultTabs->addTab(m_statsDashboard, "📈 統計儀表板");
    
    m_mainSplitter->addWidget(m_resultTabs);
    
    // 設置分割器比例
    m_mainSplitter->setSizes({400, 800});
}

void LLMMainWindow::setupStatusBar()
{
    m_statusBar = new QFrame(this);
    m_statusBar->setFixedHeight(35);
    m_statusBar->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    
    m_statusLayout = new QHBoxLayout(m_statusBar);
    m_statusLayout->setContentsMargins(10, 5, 10, 5);
    
    m_statusLabel = new QLabel("就緒", this);
    m_statusLayout->addWidget(m_statusLabel);
    
    m_statusLayout->addStretch();
    
    m_connectionLabel = new QLabel("🔴 未連接", this);
    m_statusLayout->addWidget(m_connectionLabel);
    
    m_requestCountLabel = new QLabel("請求: 0/0", this);
    m_statusLayout->addWidget(m_requestCountLabel);
    
    m_globalProgressBar = new QProgressBar(this);
    m_globalProgressBar->setMaximumWidth(200);
    m_globalProgressBar->setVisible(false);
    m_statusLayout->addWidget(m_globalProgressBar);
    
    m_mainLayout->addWidget(m_statusBar);
}

void LLMMainWindow::setupTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/ai_brain.png"));
    m_trayIcon->setToolTip("RANOnline LLM控制中心");
    
    m_trayMenu = new QMenu(this);
    
    QAction *showAction = m_trayMenu->addAction("顯示窗口");
    connect(showAction, &QAction::triggered, this, &QWidget::show);
    
    QAction *hideAction = m_trayMenu->addAction("隱藏窗口");
    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    
    m_trayMenu->addSeparator();
    
    QAction *quitAction = m_trayMenu->addAction("退出");
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    
    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
    
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &LLMMainWindow::onTrayIconActivated);
}

void LLMMainWindow::setupConnections()
{
    // LLM管理器信號
    connect(m_llmManager.get(), &OllamaLLMManager::requestCompleted, 
            this, &LLMMainWindow::onLLMRequestCompleted);
    connect(m_llmManager.get(), &OllamaLLMManager::requestFailed, 
            this, &LLMMainWindow::onLLMRequestFailed);
    connect(m_llmManager.get(), &OllamaLLMManager::streamUpdate, 
            this, &LLMMainWindow::onLLMStreamUpdate);
    connect(m_llmManager.get(), &OllamaLLMManager::requestProgress, 
            this, &LLMMainWindow::onLLMRequestProgress);
    connect(m_llmManager.get(), &OllamaLLMManager::modelsRefreshed, 
            this, &LLMMainWindow::onLLMModelsRefreshed);
    connect(m_llmManager.get(), &OllamaLLMManager::serverStatusChanged, 
            this, &LLMMainWindow::onLLMServerStatusChanged);
    connect(m_llmManager.get(), &OllamaLLMManager::statisticsUpdated, 
            this, &LLMMainWindow::onLLMStatisticsUpdated);
    
    // 遊戲同步信號
    connect(m_gameSync.get(), &GameEventSyncer::gameEventReceived, 
            this, &LLMMainWindow::onGameEventReceived);
    connect(m_gameSync.get(), &GameEventSyncer::playerMessageReceived, 
            this, &LLMMainWindow::onPlayerMessageReceived);
    connect(m_gameSync.get(), &GameEventSyncer::skillGenerationRequest, 
            this, &LLMMainWindow::onSkillGenerationRequest);
    connect(m_gameSync.get(), &GameEventSyncer::connectionStatusChanged, 
            this, &LLMMainWindow::onGameConnectionStatusChanged);
    
    // 批量面板信號
    connect(m_batchPanel, &BatchRequestPanel::requestCancelled, 
            m_llmManager.get(), &OllamaLLMManager::cancelRequest);
    connect(m_batchPanel, &BatchRequestPanel::requestRetry, 
            this, [this](const QString &requestId) {
        // 重試邏輯
        addLogMessage(QString("重試請求: %1").arg(requestId), "RETRY");
    });
    
    // UI控制信號
    connect(m_submitButton, &QPushButton::clicked, this, &LLMMainWindow::onSubmitRequest);
    connect(m_submitBatchButton, &QPushButton::clicked, this, &LLMMainWindow::onSubmitBatchRequests);
    connect(m_cancelAllButton, &QPushButton::clicked, this, &LLMMainWindow::onCancelAllRequests);
    connect(m_addServerButton, &QPushButton::clicked, this, &LLMMainWindow::onAddServer);
    connect(m_removeServerButton, &QPushButton::clicked, this, &LLMMainWindow::onRemoveServer);
    connect(m_testConnectionButton, &QPushButton::clicked, this, &LLMMainWindow::onTestConnection);
    
    // 狀態更新定時器
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &LLMMainWindow::updateStats);
}

void LLMMainWindow::setupAnimations()
{
    // 淡入動畫
    m_fadeAnimation->setDuration(500);
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
}

// 待續...下一部分將包含所有槽函數的實現
