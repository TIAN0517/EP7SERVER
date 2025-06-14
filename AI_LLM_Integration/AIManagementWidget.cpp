/**
 * @file AIManagementWidget.cpp
 * @brief RAN Online四部門AI智能管控界面實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "AIManagementWidget.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtCore/QDateTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QJsonDocument>
#include <QtCore/QDebug>

namespace RANOnline {
namespace AI {

// AIStatsWidget Implementation
AIStatsWidget::AIStatsWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QGridLayout(this);
    
    // 總請求數
    layout->addWidget(new QLabel("總請求數:", this), 0, 0);
    m_totalRequestsLabel = new QLabel("0", this);
    m_totalRequestsLabel->setStyleSheet("font-weight: bold; color: #4682ff;");
    layout->addWidget(m_totalRequestsLabel, 0, 1);
    
    // 成功率
    layout->addWidget(new QLabel("成功率:", this), 1, 0);
    m_successRateLabel = new QLabel("0%", this);
    m_successRateLabel->setStyleSheet("font-weight: bold; color: #00ff7f;");
    layout->addWidget(m_successRateLabel, 1, 1);
    
    m_successRateBar = new QProgressBar(this);
    m_successRateBar->setMaximum(100);
    layout->addWidget(m_successRateBar, 1, 2);
    
    // 平均回應時間
    layout->addWidget(new QLabel("平均回應時間:", this), 2, 0);
    m_avgResponseTimeLabel = new QLabel("0ms", this);
    m_avgResponseTimeLabel->setStyleSheet("font-weight: bold; color: #ffa500;");
    layout->addWidget(m_avgResponseTimeLabel, 2, 1);
    
    // 錯誤數量
    layout->addWidget(new QLabel("錯誤數量:", this), 3, 0);
    m_errorCountLabel = new QLabel("0", this);
    m_errorCountLabel->setStyleSheet("font-weight: bold; color: #ff4500;");
    layout->addWidget(m_errorCountLabel, 3, 1);
}

void AIStatsWidget::updateStats(const TestReport &report)
{
    m_totalRequestsLabel->setText(QString::number(report.totalRequests));
    m_successRateLabel->setText(QString("%.2f%%").arg(report.successRate));
    m_successRateBar->setValue(static_cast<int>(report.successRate));
    m_avgResponseTimeLabel->setText(QString("%.2fms").arg(report.averageResponseTime));
    m_errorCountLabel->setText(QString::number(report.failedRequests));
}

// AILogWidget Implementation
AILogWidget::AILogWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    
    // 按鈕列
    m_buttonLayout = new QHBoxLayout();
    m_clearButton = new QPushButton("清空日誌", this);
    m_exportButton = new QPushButton("導出日誌", this);
    
    m_clearButton->setStyleSheet("QPushButton { background-color: #ff4500; color: white; border: none; padding: 8px; border-radius: 4px; }");
    m_exportButton->setStyleSheet("QPushButton { background-color: #4682ff; color: white; border: none; padding: 8px; border-radius: 4px; }");
    
    m_buttonLayout->addWidget(m_clearButton);
    m_buttonLayout->addWidget(m_exportButton);
    m_buttonLayout->addStretch();
    
    layout->addLayout(m_buttonLayout);
    
    // 日誌顯示區域
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #00ff7f; font-family: 'Consolas', monospace; }");
    layout->addWidget(m_logTextEdit);
    
    // 連接信號
    connect(m_clearButton, &QPushButton::clicked, this, &AILogWidget::onClearButtonClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &AILogWidget::onExportButtonClicked);
}

void AILogWidget::addLogEntry(const QString &entry)
{
    QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ");
    m_logTextEdit->append(timestamp + entry);
    
    // 保持滾動到底部
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);
}

void AILogWidget::clearLogs()
{
    m_logTextEdit->clear();
}

void AILogWidget::onClearButtonClicked()
{
    int ret = QMessageBox::question(this, "確認", "確定要清空所有日誌嗎？",
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        clearLogs();
    }
}

void AILogWidget::onExportButtonClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "導出日誌", 
                                                   QString("AI_Logs_%1.txt")
                                                   .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
                                                   "文本文件 (*.txt)");
    if (!fileName.isEmpty()) {
        exportLogs(fileName);
    }
}

bool AILogWidget::exportLogs(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "錯誤", "無法創建日誌文件");
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << m_logTextEdit->toPlainText();
    
    QMessageBox::information(this, "成功", QString("日誌已導出到：%1").arg(filePath));
    return true;
}

// AIPlayerTableWidget Implementation
AIPlayerTableWidget::AIPlayerTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setupTable();
}

void AIPlayerTableWidget::setupTable()
{
    setColumnCount(8);
    QStringList headers = {"AI ID", "學院", "部門", "姓名", "性格", "戰鬥風格", "狀態", "操作"};
    setHorizontalHeaderLabels(headers);
    
    // 設置表格樣式
    setStyleSheet(
        "QTableWidget {"
        "    background-color: #2d2d2d;"
        "    color: white;"
        "    gridline-color: #555555;"
        "    selection-background-color: #4682ff;"
        "}"
        "QHeaderView::section {"
        "    background-color: #3d3d3d;"
        "    color: white;"
        "    padding: 8px;"
        "    border: 1px solid #555555;"
        "    font-weight: bold;"
        "}"
    );
    
    // 設置列寬
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    setColumnWidth(0, 120);  // AI ID
    setColumnWidth(1, 80);   // 學院
    setColumnWidth(2, 80);   // 部門
    setColumnWidth(3, 120);  // 姓名
    setColumnWidth(4, 120);  // 性格
    setColumnWidth(5, 120);  // 戰鬥風格
    setColumnWidth(6, 80);   // 狀態
    setColumnWidth(7, 100);  // 操作
    
    // 其他設置
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void AIPlayerTableWidget::addAIPlayer(const AIPlayerData &player)
{
    int row = rowCount();
    insertRow(row);
    
    setItem(row, 0, new QTableWidgetItem(player.aiId));
    setItem(row, 1, new QTableWidgetItem(player.academy));
    setItem(row, 2, new QTableWidgetItem(player.department));
    setItem(row, 3, new QTableWidgetItem(player.name));
    setItem(row, 4, new QTableWidgetItem(player.personality));
    setItem(row, 5, new QTableWidgetItem(player.combatStyle));
    setItem(row, 6, new QTableWidgetItem(player.state));
    
    // 添加移除按鈕
    QPushButton *removeButton = createRemoveButton(player.aiId);
    setCellWidget(row, 7, removeButton);
    
    // 記錄AI ID到行號的映射
    m_aiRowMap[player.aiId] = row;
}

QPushButton *AIPlayerTableWidget::createRemoveButton(const QString &aiId)
{
    QPushButton *button = new QPushButton("移除", this);
    button->setStyleSheet("QPushButton { background-color: #ff4500; color: white; border: none; padding: 4px 8px; border-radius: 3px; }");
    button->setProperty("aiId", aiId);
    
    connect(button, &QPushButton::clicked, this, &AIPlayerTableWidget::onRemoveButtonClicked);
    
    return button;
}

void AIPlayerTableWidget::updateAIStatus(const QString &aiId, const QString &status)
{
    if (m_aiRowMap.contains(aiId)) {
        int row = m_aiRowMap[aiId];
        if (item(row, 6)) {
            item(row, 6)->setText(status);
        }
    }
}

void AIPlayerTableWidget::clearAllPlayers()
{
    setRowCount(0);
    m_aiRowMap.clear();
}

void AIPlayerTableWidget::onCellClicked(int row, int column)
{
    Q_UNUSED(column)
    if (row < rowCount() && item(row, 0)) {
        QString aiId = item(row, 0)->text();
        emit aiPlayerSelected(aiId);
    }
}

void AIPlayerTableWidget::onRemoveButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString aiId = button->property("aiId").toString();
        
        int ret = QMessageBox::question(this, "確認", 
                                       QString("確定要移除AI玩家 %1 嗎？").arg(aiId),
                                       QMessageBox::Yes | QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            // 找到並移除行
            if (m_aiRowMap.contains(aiId)) {
                int row = m_aiRowMap[aiId];
                removeRow(row);
                m_aiRowMap.remove(aiId);
                
                // 更新其他行的映射
                for (auto it = m_aiRowMap.begin(); it != m_aiRowMap.end(); ++it) {
                    if (it.value() > row) {
                        it.value()--;
                    }
                }
                
                emit aiPlayerRemoved(aiId);
            }
        }
    }
}

// AIManagementWidget Implementation
AIManagementWidget::AIManagementWidget(QWidget *parent)
    : QWidget(parent)    , m_playerGenerator(new AIPlayerGenerator(this))
    , m_decisionEngine(new AIDecisionEngine(this))
    // , m_gameEventSyncer(nullptr)  // 暫時註釋
    , m_isConnectedToGameServer(false)
    , m_updateTimer(new QTimer(this))
    , m_totalAICount(0)
{
    setupUI();
    
    // 連接信號
    connect(m_playerGenerator, &AIPlayerGenerator::aiPlayerGenerated,
            this, &AIManagementWidget::onAIPlayerGenerated);
    connect(m_playerGenerator, &AIPlayerGenerator::logGenerated,
            this, &AIManagementWidget::onLogGenerated);
    
    connect(m_decisionEngine, &AIDecisionEngine::batchDecisionCompleted,
            this, &AIManagementWidget::onBatchDecisionCompleted);
    connect(m_decisionEngine, &AIDecisionEngine::testCompleted,
            this, &AIManagementWidget::onTestCompleted);
    connect(m_decisionEngine, &AIDecisionEngine::testProgressUpdated,
            this, &AIManagementWidget::onTestProgressUpdated);
    connect(m_decisionEngine, &AIDecisionEngine::logGenerated,
            this, &AIManagementWidget::onLogGenerated);
    
    // 定時更新
    connect(m_updateTimer, &QTimer::timeout, this, &AIManagementWidget::updateAICount);
    m_updateTimer->start(5000); // 每5秒更新一次
}

AIManagementWidget::~AIManagementWidget()
{
}

void AIManagementWidget::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // 創建標籤頁控件
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "    border: 1px solid #555555;"
        "    background-color: #2d2d2d;"
        "}"
        "QTabBar::tab {"
        "    background-color: #3d3d3d;"
        "    color: white;"
        "    padding: 8px 16px;"
        "    margin-right: 2px;"
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #4682ff;"
        "}"
    );
    
    setupGenerationTab();
    setupDecisionTab();
    setupTestingTab();
    setupLogsTab();
    setupSettingsTab();
    setupStatsTab();
    
    mainLayout->addWidget(m_tabWidget);
}

void AIManagementWidget::setupGenerationTab()
{
    m_generationTab = new QWidget();
    auto *layout = new QVBoxLayout(m_generationTab);
    
    // 控制面板
    auto *controlGroup = new QGroupBox("AI生成控制", this);
    auto *controlLayout = new QGridLayout(controlGroup);
    
    // AI數量
    controlLayout->addWidget(new QLabel("生成數量:", this), 0, 0);
    m_aiCountSpinBox = new QSpinBox(this);
    m_aiCountSpinBox->setRange(1, 10000);
    m_aiCountSpinBox->setValue(100);
    controlLayout->addWidget(m_aiCountSpinBox, 0, 1);
    
    // 學院篩選
    controlLayout->addWidget(new QLabel("學院篩選:", this), 0, 2);
    m_academyFilterCombo = new QComboBox(this);
    m_academyFilterCombo->addItems({"全部", "聖門", "懸岩", "鳳凰"});
    controlLayout->addWidget(m_academyFilterCombo, 0, 3);
    
    // 部門篩選
    controlLayout->addWidget(new QLabel("部門篩選:", this), 1, 0);
    m_departmentFilterCombo = new QComboBox(this);
    m_departmentFilterCombo->addItems({"全部", "劍道", "弓箭", "格鬥", "氣功"});
    controlLayout->addWidget(m_departmentFilterCombo, 1, 1);
    
    // 按鈕
    m_generateButton = new QPushButton("🎲 生成AI玩家", this);
    m_generateButton->setStyleSheet("QPushButton { background-color: #00ff7f; color: black; font-weight: bold; padding: 10px; border-radius: 5px; }");
    controlLayout->addWidget(m_generateButton, 1, 2);
    
    m_clearAIButton = new QPushButton("🗑️ 清空全部", this);
    m_clearAIButton->setStyleSheet("QPushButton { background-color: #ff4500; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    controlLayout->addWidget(m_clearAIButton, 1, 3);
    
    layout->addWidget(controlGroup);
    
    // AI統計
    m_aiCountLabel = new QLabel("當前AI數量: 0", this);
    m_aiCountLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #4682ff;");
    layout->addWidget(m_aiCountLabel);
    
    // AI玩家表格
    m_playerTable = new AIPlayerTableWidget(this);
    layout->addWidget(m_playerTable);
    
    // 連接信號
    connect(m_generateButton, &QPushButton::clicked, this, &AIManagementWidget::onGenerateAIClicked);
    connect(m_clearAIButton, &QPushButton::clicked, this, &AIManagementWidget::onClearAIClicked);
    
    m_tabWidget->addTab(m_generationTab, "🎮 AI生成");
}

void AIManagementWidget::setupDecisionTab()
{
    m_decisionTab = new QWidget();
    auto *layout = new QVBoxLayout(m_decisionTab);
    
    // 決策控制面板
    auto *decisionGroup = new QGroupBox("AI決策控制", this);
    auto *decisionLayout = new QGridLayout(decisionGroup);
    
    // 模型選擇
    decisionLayout->addWidget(new QLabel("LLM模型:", this), 0, 0);
    m_modelSelectionCombo = new QComboBox(this);
    m_modelSelectionCombo->addItems({"gemma:latest", "qwen2:latest", "llama3:latest", "phi3:latest", "mistral:latest", "deepseek-coder:latest"});
    decisionLayout->addWidget(m_modelSelectionCombo, 0, 1);
    
    // 自動修復
    m_autoRepairCheckBox = new QCheckBox("啟用自動修復", this);
    m_autoRepairCheckBox->setChecked(true);
    decisionLayout->addWidget(m_autoRepairCheckBox, 0, 2);
    
    // 決策按鈕
    m_batchDecisionButton = new QPushButton("🧠 批量決策", this);
    m_batchDecisionButton->setStyleSheet("QPushButton { background-color: #4682ff; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    decisionLayout->addWidget(m_batchDecisionButton, 1, 0);
    
    m_teamDecisionButton = new QPushButton("👥 團隊決策", this);
    m_teamDecisionButton->setStyleSheet("QPushButton { background-color: #ffa500; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    decisionLayout->addWidget(m_teamDecisionButton, 1, 1);
    
    layout->addWidget(decisionGroup);
    
    // 決策結果顯示
    auto *resultsGroup = new QGroupBox("決策結果", this);
    auto *resultsLayout = new QVBoxLayout(resultsGroup);
    
    m_decisionResultsEdit = new QTextEdit(this);
    m_decisionResultsEdit->setReadOnly(true);
    m_decisionResultsEdit->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #00ff7f; font-family: 'Consolas', monospace; }");
    resultsLayout->addWidget(m_decisionResultsEdit);
    
    layout->addWidget(resultsGroup);
    
    // 連接信號
    connect(m_batchDecisionButton, &QPushButton::clicked, this, &AIManagementWidget::onBatchDecisionClicked);
    connect(m_teamDecisionButton, &QPushButton::clicked, this, &AIManagementWidget::onTeamDecisionClicked);
    connect(m_autoRepairCheckBox, &QCheckBox::toggled, this, &AIManagementWidget::onAutoRepairToggled);
    
    m_tabWidget->addTab(m_decisionTab, "🤖 AI決策");
}

void AIManagementWidget::setupTestingTab()
{
    m_testingTab = new QWidget();
    auto *layout = new QVBoxLayout(m_testingTab);
    
    // 測試控制面板
    auto *testGroup = new QGroupBox("自動化測試", this);
    auto *testLayout = new QGridLayout(testGroup);
    
    // 測試數量
    testLayout->addWidget(new QLabel("測試數量:", this), 0, 0);
    m_testCountSpinBox = new QSpinBox(this);
    m_testCountSpinBox->setRange(10, 10000);
    m_testCountSpinBox->setValue(1000);
    testLayout->addWidget(m_testCountSpinBox, 0, 1);
    
    // 開始測試按鈕
    m_autoTestButton = new QPushButton("🚀 開始自動測試", this);
    m_autoTestButton->setStyleSheet("QPushButton { background-color: #ff6347; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    testLayout->addWidget(m_autoTestButton, 0, 2);
    
    layout->addWidget(testGroup);
    
    // 測試進度
    auto *progressGroup = new QGroupBox("測試進度", this);
    auto *progressLayout = new QVBoxLayout(progressGroup);
    
    m_testProgressBar = new QProgressBar(this);
    m_testProgressBar->setStyleSheet("QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center; } QProgressBar::chunk { background-color: #4682ff; }");
    progressLayout->addWidget(m_testProgressBar);
    
    m_testStatusLabel = new QLabel("等待開始測試...", this);
    m_testStatusLabel->setStyleSheet("color: #00ff7f; font-weight: bold;");
    progressLayout->addWidget(m_testStatusLabel);
    
    layout->addWidget(progressGroup);
    
    // 統計面板（預留空間）
    layout->addStretch();
    
    // 連接信號
    connect(m_autoTestButton, &QPushButton::clicked, this, &AIManagementWidget::onAutoTestClicked);
    
    m_tabWidget->addTab(m_testingTab, "🧪 自動測試");
}

void AIManagementWidget::setupLogsTab()
{
    m_logsTab = new QWidget();
    auto *layout = new QVBoxLayout(m_logsTab);
    
    // 日誌控制面板
    auto *logControlGroup = new QGroupBox("日誌管理", this);
    auto *logControlLayout = new QHBoxLayout(logControlGroup);
    
    m_exportAllLogsButton = new QPushButton("📁 導出全部日誌", this);
    m_exportAllLogsButton->setStyleSheet("QPushButton { background-color: #4682ff; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
    logControlLayout->addWidget(m_exportAllLogsButton);
    logControlLayout->addStretch();
    
    layout->addWidget(logControlGroup);
    
    // 日誌顯示組件
    m_logWidget = new AILogWidget(this);
    layout->addWidget(m_logWidget);
    
    // 連接信號
    connect(m_exportAllLogsButton, &QPushButton::clicked, this, &AIManagementWidget::onExportAllLogsClicked);
    
    m_tabWidget->addTab(m_logsTab, "📋 日誌管理");
}

void AIManagementWidget::setupSettingsTab()
{
    m_settingsTab = new QWidget();
    auto *layout = new QVBoxLayout(m_settingsTab);
    
    // Ollama設定
    auto *ollamaGroup = new QGroupBox("Ollama服務器設定", this);
    auto *ollamaLayout = new QGridLayout(ollamaGroup);
    
    ollamaLayout->addWidget(new QLabel("主機:", this), 0, 0);
    m_ollamaHostEdit = new QLineEdit("localhost", this);
    ollamaLayout->addWidget(m_ollamaHostEdit, 0, 1);
    
    ollamaLayout->addWidget(new QLabel("端口:", this), 0, 2);
    m_ollamaPortSpinBox = new QSpinBox(this);
    m_ollamaPortSpinBox->setRange(1, 65535);
    m_ollamaPortSpinBox->setValue(11434);
    ollamaLayout->addWidget(m_ollamaPortSpinBox, 0, 3);
    
    layout->addWidget(ollamaGroup);
    
    // 遊戲服務器設定
    auto *gameServerGroup = new QGroupBox("遊戲服務器連接", this);
    auto *gameServerLayout = new QGridLayout(gameServerGroup);
    
    gameServerLayout->addWidget(new QLabel("WebSocket URL:", this), 0, 0);
    m_gameServerUrlEdit = new QLineEdit("ws://localhost:8080/ai", this);
    gameServerLayout->addWidget(m_gameServerUrlEdit, 0, 1);
    
    m_connectGameServerButton = new QPushButton("🔗 連接遊戲服務器", this);
    m_connectGameServerButton->setStyleSheet("QPushButton { background-color: #00ff7f; color: black; font-weight: bold; padding: 8px; border-radius: 4px; }");
    gameServerLayout->addWidget(m_connectGameServerButton, 0, 2);
    
    m_connectionStatusLabel = new QLabel("未連接", this);
    m_connectionStatusLabel->setStyleSheet("color: #ff4500; font-weight: bold;");
    gameServerLayout->addWidget(m_connectionStatusLabel, 1, 0, 1, 3);
    
    layout->addWidget(gameServerGroup);
    
    // 空白填充
    layout->addStretch();
    
    // 連接信號
    connect(m_ollamaHostEdit, &QLineEdit::textChanged, this, &AIManagementWidget::onOllamaSettingsChanged);
    connect(m_ollamaPortSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &AIManagementWidget::onOllamaSettingsChanged);
    connect(m_connectGameServerButton, &QPushButton::clicked, this, &AIManagementWidget::onConnectGameServerClicked);
    
    m_tabWidget->addTab(m_settingsTab, "⚙️ 系統設定");
}

void AIManagementWidget::setupStatsTab()
{
    m_statsTab = new QWidget();
    auto *layout = new QVBoxLayout(m_statsTab);
    
    // 統計組件
    auto *statsGroup = new QGroupBox("系統統計", this);
    auto *statsLayout = new QVBoxLayout(statsGroup);
    
    m_statsWidget = new AIStatsWidget(this);
    statsLayout->addWidget(m_statsWidget);
    
    layout->addWidget(statsGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_statsTab, "📊 統計資料");
}

// 槽函數實現
void AIManagementWidget::onGenerateAIClicked()
{
    int count = m_aiCountSpinBox->value();
    
    QJsonArray players = m_playerGenerator->generateAIPlayers(count);
    m_currentAIPlayers = players;
    
    emit aiPlayersGenerated(players);
    emit logGenerated(QString("成功生成 %1 個AI玩家").arg(count));
    
    updateAICount();
}

void AIManagementWidget::onClearAIClicked()
{
    int ret = QMessageBox::question(this, "確認", "確定要清空所有AI玩家嗎？",
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        m_playerTable->clearAllPlayers();
        m_currentAIPlayers = QJsonArray();
        m_totalAICount = 0;
        updateAICount();
        emit logGenerated("已清空所有AI玩家");
    }
}

void AIManagementWidget::onAIPlayerGenerated(const AIPlayerData &player)
{
    m_playerTable->addAIPlayer(player);
    m_totalAICount++;
    
    QString logEntry = QString("生成AI: %1 (%2-%3)").arg(player.name).arg(player.academy).arg(player.department);
    m_logWidget->addLogEntry(logEntry);
}

void AIManagementWidget::onBatchDecisionClicked()
{
    if (m_currentAIPlayers.isEmpty()) {
        QMessageBox::warning(this, "警告", "請先生成AI玩家");
        return;
    }
    
    m_decisionEngine->makeBatchDecisions(m_currentAIPlayers);
    emit logGenerated("開始批量AI決策");
}

void AIManagementWidget::onTeamDecisionClicked()
{
    if (m_currentAIPlayers.isEmpty()) {
        QMessageBox::warning(this, "警告", "請先生成AI玩家");
        return;
    }
    
    QJsonObject teamData = buildTeamData();
    m_decisionEngine->makeTeamDecisions(teamData);
    emit logGenerated("開始團隊AI決策");
}

void AIManagementWidget::onAutoTestClicked()
{
    int testCount = m_testCountSpinBox->value();
    
    m_testProgressBar->setValue(0);
    m_testStatusLabel->setText(QString("開始測試 %1 個請求...").arg(testCount));
    m_autoTestButton->setEnabled(false);
    
    m_decisionEngine->runAutomatedTest(testCount);
    emit logGenerated(QString("開始自動化測試：%1個請求").arg(testCount));
}

void AIManagementWidget::onTestProgressUpdated(int completed, int total)
{
    int progress = (completed * 100) / total;
    m_testProgressBar->setValue(progress);
    m_testStatusLabel->setText(QString("測試進度: %1/%2 (%3%)").arg(completed).arg(total).arg(progress));
}

void AIManagementWidget::onTestCompleted(const TestReport &report)
{
    m_autoTestButton->setEnabled(true);
    m_testStatusLabel->setText(QString("測試完成 - 成功率: %.2f%%").arg(report.successRate));
    
    // 更新統計
    m_statsWidget->updateStats(report);
    
    emit testCompleted(report);
    emit logGenerated(QString("自動化測試完成：成功率%.2f%%").arg(report.successRate));
}

void AIManagementWidget::onBatchDecisionCompleted(const QJsonArray &results)
{
    QString resultText;
    for (const QJsonValue &value : results) {
        QJsonObject result = value.toObject();
        resultText += QString("AI: %1 -> %2: %3 對 %4\n")
                     .arg(result["ai_id"].toString())
                     .arg(result["action"].toString())
                     .arg(result["skill"].toString())
                     .arg(result["target"].toString());
    }
    
    m_decisionResultsEdit->setPlainText(resultText);
    emit batchDecisionCompleted(results);
}

void AIManagementWidget::onLogGenerated(const QString &logEntry)
{
    m_logWidget->addLogEntry(logEntry);
    emit logGenerated(logEntry);
}

void AIManagementWidget::onExportAllLogsClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "導出全部日誌",
                                                   QString("AI_Management_Logs_%1.csv")
                                                   .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
                                                   "CSV文件 (*.csv)");
    if (!fileName.isEmpty()) {
        // 導出決策引擎日誌
        if (m_decisionEngine->exportDecisionLogs(fileName)) {
            QMessageBox::information(this, "成功", QString("日誌已導出到：%1").arg(fileName));
        } else {
            QMessageBox::warning(this, "錯誤", "日誌導出失敗");
        }
    }
}

void AIManagementWidget::onOllamaSettingsChanged()
{
    QString host = m_ollamaHostEdit->text();
    int port = m_ollamaPortSpinBox->value();
    
    m_decisionEngine->setOllamaServer(host, port);
    emit logGenerated(QString("Ollama服務器設定已更新：%1:%2").arg(host).arg(port));
}

void AIManagementWidget::onAutoRepairToggled(bool enabled)
{
    m_decisionEngine->setAutoRepairEnabled(enabled);
    emit logGenerated(QString("自動修復功能：%1").arg(enabled ? "已啟用" : "已停用"));
}

void AIManagementWidget::onConnectGameServerClicked()
{
    QString url = m_gameServerUrlEdit->text();
    connectToGameServer(url);
}

void AIManagementWidget::connectToGameServer(const QString &url)
{
    // if (m_gameEventSyncer) {
    //     m_gameEventSyncer->connectToGameServer(url);
    //     emit logGenerated(QString("正在連接遊戲服務器：%1").arg(url));
    // }
    emit logGenerated(QString("遊戲服務器連接功能暫未實現：%1").arg(url));
}

void AIManagementWidget::setOllamaServer(const QString &host, int port)
{
    m_ollamaHostEdit->setText(host);
    m_ollamaPortSpinBox->setValue(port);
    onOllamaSettingsChanged();
}

void AIManagementWidget::updateConnectionStatus(bool connected)
{
    m_isConnectedToGameServer = connected;
    if (connected) {
        m_connectionStatusLabel->setText("✅ 已連接");
        m_connectionStatusLabel->setStyleSheet("color: #00ff7f; font-weight: bold;");
        m_connectGameServerButton->setText("🔗 重新連接");
    } else {
        m_connectionStatusLabel->setText("❌ 未連接");
        m_connectionStatusLabel->setStyleSheet("color: #ff4500; font-weight: bold;");
        m_connectGameServerButton->setText("🔗 連接遊戲服務器");
    }
}

void AIManagementWidget::updateAICount()
{
    m_aiCountLabel->setText(QString("當前AI數量: %1").arg(m_totalAICount));
}

QJsonObject AIManagementWidget::buildTeamData()
{
    QJsonObject teamData;
    teamData["members"] = m_currentAIPlayers;
    teamData["situation"] = "PVP戰鬥";
    teamData["enemies"] = QJsonArray(); // 簡化處理
    teamData["allies"] = QJsonArray();
    teamData["map"] = "競技場";
    
    return teamData;
}

void AIManagementWidget::onConnectionStatusChanged(bool connected)
{
    updateConnectionStatus(connected);
    emit logGenerated(QString("遊戲服務器連接狀態：%1").arg(connected ? "已連接" : "已斷開"));
}

void AIManagementWidget::onModelSelectionChanged()
{
    // 處理模型選擇變更（如果需要）
}

void AIManagementWidget::onDecisionCompleted(const QString &aiId, const AIDecisionResponse &response)
{
    QString logEntry = QString("AI決策完成：%1 使用 %2 對 %3").arg(aiId).arg(response.skill).arg(response.target);
    m_logWidget->addLogEntry(logEntry);
    emit logGenerated(logEntry);
}

} // namespace AI
} // namespace RANOnline
