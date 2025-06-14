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
    }}
};

LLMMainWindow::LLMMainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , m_llmManager(nullptr)
    // , m_gameEventSyncer(nullptr)  // 暫時註釋
    , m_aiManagementWidget(nullptr)  // 新增AI管控組件初始化
    , m_aiPlayerGenerator(nullptr)
    , m_aiDecisionEngine(nullptr)
    , m_logoLabel(nullptr)
    , m_logoPixmap(nullptr)
    , m_brandLabel(nullptr)
    , m_settings(new QSettings(this))
{
    setWindowTitle(tr("RANOnline AI LLM 整合系統 - Jy技術團隊 v3.0.0"));
    setMinimumSize(1400, 900);
    resize(1600, 1000);
      // 加載品牌資源和設置圖標
    loadBrandResources();
    setupBrandingAndIcons();
    
    // 初始化組件
    m_llmManager = new OllamaLLMManager(this);
    
    // 初始化AI管控系統
    initializeAIManagement();
      // 設置UI
    setupUI();
    setupConnections();
    
    // 連接AI管控系統信號
    // connectAISignals();  // 已在initializeAIManagement中處理
    
    // 載入設置
    loadSettings();
    applyTheme();
}

LLMMainWindow::~LLMMainWindow()
{
    saveSettings();
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
    setupLogoDisplay();
    if (m_logoLabel && m_brandLabel) {
        headerLayout->addWidget(m_logoLabel);
        headerLayout->addWidget(m_brandLabel);
    }
    
    // 添加彈性空間
    headerLayout->addStretch();
    
    // 版本信息標籤
    auto *versionLabel = new QLabel("v3.0.0 - AI管控系統", this);
    versionLabel->setStyleSheet(
        "font-size: 12px; color: #666666; font-style: italic;"
    );
    headerLayout->addWidget(versionLabel);
    
    layout->addLayout(headerLayout);
    
    // 標題
    auto *titleLabel = new QLabel(tr("RANOnline AI LLM 整合系統"), this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4682ff; margin: 10px 0;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 主要內容區域
    m_tabWidget = new QTabWidget(this);
    layout->addWidget(m_tabWidget);
    
    setupRequestTab();
    setupBatchTab();
    setupServerTab();
    setupAIManagementTab();  // 新增AI管控標籤頁
    
    // 狀態列
    m_statusLabel = new QLabel(tr("Ready"), this);
    m_statusLabel->setStyleSheet("color: #00ff7f; padding: 5px;");
    layout->addWidget(m_statusLabel);
}

void LLMMainWindow::setupRequestTab()
{
    m_requestTab = new QWidget();
    auto *layout = new QVBoxLayout(m_requestTab);
    
    // 模型選擇
    auto *modelLayout = new QHBoxLayout();
    modelLayout->addWidget(new QLabel(tr("Model:"), this));
    m_modelCombo = new QComboBox(this);
    m_modelCombo->addItems({"llama3:latest", "phi3:latest", "mistral:latest"});
    modelLayout->addWidget(m_modelCombo);
    
    // 學院選擇
    modelLayout->addWidget(new QLabel(tr("Academy:"), this));
    m_academyCombo = new QComboBox(this);
    m_academyCombo->addItems({"Warrior", "Mage", "Archer", "Thief", "Priest"});
    modelLayout->addWidget(m_academyCombo);
    
    layout->addLayout(modelLayout);
    
    // 提示輸入
    layout->addWidget(new QLabel(tr("Prompt:"), this));
    m_promptEdit = new QTextEdit(this);
    m_promptEdit->setPlaceholderText(tr("Enter your prompt here..."));
    m_promptEdit->setMaximumHeight(150);
    layout->addWidget(m_promptEdit);
    
    // 提交按鈕
    m_submitButton = new QPushButton(tr("Submit Request"), this);
    m_submitButton->setStyleSheet(getCyberButtonStyle(ACADEMY_THEMES["Cyber Blue"]));
    layout->addWidget(m_submitButton);
    
    // 請求容器
    m_requestScrollArea = new QScrollArea(this);
    m_requestContainer = new QWidget();
    m_requestContainerLayout = new QVBoxLayout(m_requestContainer);
    m_requestScrollArea->setWidget(m_requestContainer);
    m_requestScrollArea->setWidgetResizable(true);
    layout->addWidget(m_requestScrollArea);
    
    m_tabWidget->addTab(m_requestTab, tr("AI Requests"));
}

void LLMMainWindow::setupBatchTab()
{
    m_batchTab = new QWidget();
    auto *layout = new QVBoxLayout(m_batchTab);
    
    layout->addWidget(new QLabel(tr("Batch Processing"), this));
    
    // 佔位文本
    auto *placeholder = new QLabel(tr("Batch processing interface will be implemented here."), this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #888; font-style: italic;");
    layout->addWidget(placeholder);
    
    m_tabWidget->addTab(m_batchTab, tr("Batch Processing"));
}

void LLMMainWindow::setupServerTab()
{
    m_serverTab = new QWidget();
    auto *layout = new QVBoxLayout(m_serverTab);
    
    layout->addWidget(new QLabel(tr("Server Management"), this));
    
    // 佔位文本
    auto *placeholder = new QLabel(tr("Server management interface will be implemented here."), this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #888; font-style: italic;");
    layout->addWidget(placeholder);
    
    m_tabWidget->addTab(m_serverTab, tr("Server Management"));
}

void LLMMainWindow::initializeAIManagement()
{
    try {
        m_aiPlayerGenerator = new RANOnline::AI::AIPlayerGenerator(this);
        m_aiDecisionEngine = new RANOnline::AI::AIDecisionEngine(this);
        m_aiManagementWidget = new RANOnline::AI::AIManagementWidget(this);
          // 連接AI管控信號
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
        
        // 連接AI管控Widget信號
        connect(m_aiManagementWidget, &RANOnline::AI::AIManagementWidget::generateAIRequested,
                m_aiPlayerGenerator, &RANOnline::AI::AIPlayerGenerator::generateAIPlayers);
        connect(m_aiManagementWidget, &RANOnline::AI::AIManagementWidget::testAIRequested,
                this, &LLMMainWindow::onTestAIRequested);
        
        // 添加命名系統測試功能
        testAcademyNamingSystem();
          } catch (const std::exception &e) {
        qDebug() << "AI管控系統初始化失敗：" << e.what();
    }
}

void LLMMainWindow::setupAIManagementTab()
{
    m_aiManagementTab = new QWidget();
    auto *layout = new QVBoxLayout(m_aiManagementTab);
    
    // 標題
    auto *titleLabel = new QLabel(tr("四部門AI智能管控系統"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4682ff; margin-bottom: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 添加AI管控主界面
    layout->addWidget(m_aiManagementWidget);
    
    // 添加到主標籤頁
    m_tabWidget->addTab(m_aiManagementTab, tr("🤖 AI管控系統"));
}

void LLMMainWindow::setupConnections()
{
    connect(m_submitButton, &QPushButton::clicked, this, &LLMMainWindow::onSubmitRequest);
    
    if (m_llmManager) {
        connect(m_llmManager, &OllamaLLMManager::requestCompleted, 
                this, &LLMMainWindow::onLLMRequestCompleted);
        connect(m_llmManager, &OllamaLLMManager::requestFailed,
                this, &LLMMainWindow::onLLMRequestFailed);
    }
}

/*
void LLMMainWindow::connectAISignals()
{
    // 已在 initializeAIManagement() 中處理信號連接
    // 這個方法暫時註釋掉以避免重複連接和參數類型錯誤
}
*/

void LLMMainWindow::onSubmitRequest()
{
    QString prompt = m_promptEdit->toPlainText().trimmed();
    if (prompt.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter a prompt."));
        return;
    }
    
    LLMRequestConfig config;
    config.requestId = QString("req_%1").arg(QDateTime::currentMSecsSinceEpoch());
    config.prompt = prompt;
    config.model = m_modelCombo->currentText();
    config.academy = m_academyCombo->currentText();
    
    if (m_llmManager) {
        m_llmManager->submitRequest(config);
        m_statusLabel->setText(tr("Request submitted: %1").arg(config.requestId));
        m_promptEdit->clear();
    }
}

void LLMMainWindow::onLLMRequestCompleted(const LLMResponse &response)
{
    m_statusLabel->setText(tr("Request completed: %1").arg(response.requestId));
    
    // 簡單顯示結果
    QMessageBox::information(this, tr("Request Completed"), 
                           tr("Response: %1").arg(response.content.left(200) + "..."));
}

void LLMMainWindow::onLLMRequestFailed(const QString &requestId, const QString &error)
{
    m_statusLabel->setText(tr("Request failed: %1").arg(requestId));
    QMessageBox::warning(this, tr("Request Failed"), tr("Error: %1").arg(error));
}

void LLMMainWindow::loadSettings()
{
    // 載入窗口幾何
    restoreGeometry(m_settings->value("geometry").toByteArray());
    
    // 載入主題
    QString themeName = m_settings->value("theme", "Cyber Blue").toString();
    if (ACADEMY_THEMES.contains(themeName)) {
        m_currentTheme = ACADEMY_THEMES[themeName];
    } else {
        m_currentTheme = ACADEMY_THEMES["Cyber Blue"];
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

void LLMMainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    FramelessWindow::closeEvent(event);
}

void LLMMainWindow::showEvent(QShowEvent *event)
{
    FramelessWindow::showEvent(event);
}

// 佔位實現 - 其他方法的空實現
void LLMMainWindow::onSubmitBatchRequests() {}
void LLMMainWindow::onCancelAllRequests() {}
void LLMMainWindow::onRefreshModels() {}
void LLMMainWindow::onExportLogs() {}
void LLMMainWindow::onImportTemplates() {}
void LLMMainWindow::onExportTemplates() {}
void LLMMainWindow::onAddServer() {}
void LLMMainWindow::onRemoveServer() {}
void LLMMainWindow::onTestConnection() {}
void LLMMainWindow::onThemeChanged() {}
void LLMMainWindow::onShowSettings() {}
void LLMMainWindow::onShowAbout() {}
void LLMMainWindow::onToggleFullscreen() {}
void LLMMainWindow::onMinimizeToTray() {}
void LLMMainWindow::onLLMStreamUpdate(const QString &, const QString &) {}
void LLMMainWindow::onLLMRequestProgress(const QString &, int) {}
void LLMMainWindow::onLLMModelsRefreshed(const QList<LLMModelInfo> &) {}
void LLMMainWindow::onLLMServerStatusChanged(const QString &, bool) {}
void LLMMainWindow::onLLMStatisticsUpdated() {}
void LLMMainWindow::onGameEventReceived(const QString &, const QJsonObject &) {}
void LLMMainWindow::onPlayerMessageReceived(const QString &, const QString &) {}
void LLMMainWindow::onSkillGenerationRequest(const QString &, int) {}
void LLMMainWindow::onGameConnectionStatusChanged(bool) {}
void LLMMainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason) {}
void LLMMainWindow::onAIManagementTabActivated() {}
void LLMMainWindow::onAIPlayersGenerated(const QJsonArray &players) {
    Q_UNUSED(players)
    qDebug() << "AI玩家生成完成";
}

void LLMMainWindow::onAIBatchDecisionCompleted(const QJsonArray &results) {
    Q_UNUSED(results)
    qDebug() << "AI批量決策完成";
}

void LLMMainWindow::onAITestCompleted(const RANOnline::AI::TestReport &report) {
    Q_UNUSED(report)
    qDebug() << "AI測試完成";
}

void LLMMainWindow::onAILogGenerated(const QString &logEntry) {
    Q_UNUSED(logEntry)
    // 實際實現時會將日誌顯示在UI中
}

void LLMMainWindow::onAutoRepairTriggered(const QString &aiId, const QString &repairAction) {
    Q_UNUSED(aiId)
    Q_UNUSED(repairAction)
    qDebug() << "AI自動修復觸發";
}

void LLMMainWindow::testAcademyNamingSystem()
{
    if (!m_aiPlayerGenerator) {
        qDebug() << "AI Player Generator 未初始化";
        return;
    }
    
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
    
    // 輸出統計結果到狀態欄和日誌
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
    } else {
        testReport += "\n⚠️ 發現超長名字：\n";
        for (const QString &name : overlengthNames) {
            testReport += QString("    %1\n").arg(name);
        }
    }
    
    qDebug() << testReport;
      // 發送到日誌系統
    qDebug() << QString("[%1] 學院命名系統測試完成")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

// AI管控系統槽函數實現
void LLMMainWindow::onAIPlayerGenerated(const RANOnline::AI::AIPlayerData &player)
{
    QString logEntry = QString("[%1] 新AI玩家生成: %2 (%3-%4)")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(player.name).arg(player.academy).arg(player.department);
    qDebug() << logEntry;
}

void LLMMainWindow::onAIErrorOccurred(const QString &error)
{
    QString logEntry = QString("[%1] AI系統錯誤: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(error);
    qDebug() << logEntry;
    qDebug() << "AI系統錯誤:" << error;
}

void LLMMainWindow::onAIDecisionMade(const RANOnline::AI::AIDecisionResponse &response)
{
    QString logEntry = QString("[%1] AI決策完成: %2 -> %3")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(response.skill).arg(response.target);
    qDebug() << logEntry;
}

void LLMMainWindow::onAIDecisionError(const QString &error)
{
    QString logEntry = QString("[%1] AI決策錯誤: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(error);
    qDebug() << logEntry;
    qDebug() << "AI決策錯誤:" << error;
}

void LLMMainWindow::onTestAIRequested(int count, const QString &modelType)
{
    if (!m_aiDecisionEngine) {
        qDebug() << "AI決策引擎未初始化";
        return;
    }
      QString logEntry = QString("[%1] 開始AI批量測試: %2個AI, 模型: %3")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(count).arg(modelType);
    qDebug() << logEntry;
    
    // 執行批量AI測試
    QJsonArray testData = m_aiPlayerGenerator->generateTestData(count);
    m_aiDecisionEngine->runBatchTest(testData, modelType);
}

// ========== Jy技術團隊品牌和圖標設置方法 ==========

void LLMMainWindow::loadBrandResources()
{
    // 加載應用程式圖標
    m_appIcon = QIcon(":/logo/app_icon.ico");
    if (m_appIcon.isNull()) {
        // 如果資源文件不可用，使用本地文件
        m_appIcon = QIcon("Resources/jy1.ico");
    }
    
    // 加載任務欄圖標
    m_taskbarIcon = QIcon(":/logo/taskbar_icon.ico");
    if (m_taskbarIcon.isNull()) {
        m_taskbarIcon = QIcon("Resources/jy2.ico");
    }
    
    // 加載Logo圖片
    QString logoPath = ":/logo/jy_tech_team.ico";
    if (!QFile::exists(logoPath)) {
        logoPath = "Resources/jy1.ico";
    }
    
    m_logoPixmap = new QPixmap(logoPath);
    if (!m_logoPixmap->isNull()) {
        // 調整Logo大小為48x48像素，保持高品質
        *m_logoPixmap = m_logoPixmap->scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    qDebug() << "品牌資源加載完成";
}

void LLMMainWindow::setupApplicationIcon()
{
    if (!m_appIcon.isNull()) {
        setWindowIcon(m_appIcon);
        
        // 設置應用程式圖標
        QApplication::setWindowIcon(m_appIcon);
        
        qDebug() << "應用程式圖標設置完成";
    } else {
        qWarning() << "無法加載應用程式圖標";
    }
}

void LLMMainWindow::setupTaskbarIcon()
{
    if (!m_taskbarIcon.isNull()) {
        // 如果有系統托盤圖標，更新它
        if (m_trayIcon) {
            m_trayIcon->setIcon(m_taskbarIcon);
            m_trayIcon->setToolTip("RANOnline AI管控系統 - Jy技術團隊");
        }
        
        qDebug() << "任務欄圖標設置完成";
    } else {
        qWarning() << "無法加載任務欄圖標";
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
    
    qDebug() << "品牌標識和圖標設置完成";
}
