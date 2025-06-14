/**
 * @file AIManagementWidget.h
 * @brief RAN Online四部門AI智能管控界面
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtCore/QTimer>
#include "AIPlayerGenerator.h"
#include "AIDecisionEngine.h"
#include "GameEventSyncer.h"

namespace RANOnline {
namespace AI {

/**
 * @brief AI統計資料顯示
 */
class AIStatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIStatsWidget(QWidget *parent = nullptr);
    void updateStats(const TestReport &report);

private:
    QLabel *m_totalRequestsLabel;
    QLabel *m_successRateLabel;
    QLabel *m_avgResponseTimeLabel;
    QLabel *m_errorCountLabel;
    QProgressBar *m_successRateBar;
};

/**
 * @brief AI決策日誌顯示
 */
class AILogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AILogWidget(QWidget *parent = nullptr);
    void addLogEntry(const QString &entry);
    void clearLogs();
    bool exportLogs(const QString &filePath);

private slots:
    void onClearButtonClicked();
    void onExportButtonClicked();

private:
    QTextEdit *m_logTextEdit;
    QPushButton *m_clearButton;
    QPushButton *m_exportButton;
    QHBoxLayout *m_buttonLayout;
};

/**
 * @brief AI玩家表格顯示
 */
class AIPlayerTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit AIPlayerTableWidget(QWidget *parent = nullptr);
    void addAIPlayer(const AIPlayerData &player);
    void updateAIStatus(const QString &aiId, const QString &status);
    void clearAllPlayers();

signals:
    void aiPlayerSelected(const QString &aiId);
    void aiPlayerRemoved(const QString &aiId);

private slots:
    void onCellClicked(int row, int column);
    void onRemoveButtonClicked();

private:
    void setupTable();
    QPushButton *createRemoveButton(const QString &aiId);
    
    QMap<QString, int> m_aiRowMap; // AI ID -> 行號映射
};

/**
 * @brief AI管控主界面
 */
class AIManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIManagementWidget(QWidget *parent = nullptr);
    ~AIManagementWidget();
    
    // 連接到遊戲服務器
    void connectToGameServer(const QString &url);
    
    // 設定Ollama服務器
    void setOllamaServer(const QString &host, int port);

signals:
    void aiPlayersGenerated(const QJsonArray &players);
    void batchDecisionCompleted(const QJsonArray &results);
    void testCompleted(const TestReport &report);
    void logGenerated(const QString &logEntry);
    void generateAIRequested(int count);
    void testAIRequested(int count, const QString &modelType);

private slots:
    // AI生成相關
    void onGenerateAIClicked();
    void onClearAIClicked();
    void onAIPlayerGenerated(const AIPlayerData &player);
    
    // 決策相關
    void onBatchDecisionClicked();
    void onTeamDecisionClicked();
    void onAutoTestClicked();
    void onDecisionCompleted(const QString &aiId, const AIDecisionResponse &response);
    void onBatchDecisionCompleted(const QJsonArray &results);
    
    // 測試相關
    void onTestProgressUpdated(int completed, int total);
    void onTestCompleted(const TestReport &report);
    
    // 設定相關
    void onOllamaSettingsChanged();
    void onAutoRepairToggled(bool enabled);
    void onModelSelectionChanged();
    
    // 日誌相關
    void onLogGenerated(const QString &logEntry);
    void onExportAllLogsClicked();
    
    // 連接相關
    void onConnectGameServerClicked();
    void onConnectionStatusChanged(bool connected);

private:
    void setupUI();
    void setupGenerationTab();
    void setupDecisionTab();
    void setupTestingTab();
    void setupLogsTab();
    void setupSettingsTab();
    void setupStatsTab();
    
    void updateConnectionStatus(bool connected);
    void updateAICount();
    QJsonObject buildTeamData();

private:
    // 核心組件
    AIPlayerGenerator *m_playerGenerator;
    AIDecisionEngine *m_decisionEngine;
    // GameEventSyncer *m_gameEventSyncer;  // 暫時註釋，待實現
    
    // UI組件
    QTabWidget *m_tabWidget;
    
    // AI生成標籤頁
    QWidget *m_generationTab;
    QSpinBox *m_aiCountSpinBox;
    QComboBox *m_academyFilterCombo;
    QComboBox *m_departmentFilterCombo;
    QPushButton *m_generateButton;
    QPushButton *m_clearAIButton;
    AIPlayerTableWidget *m_playerTable;
    QLabel *m_aiCountLabel;
    
    // 決策標籤頁
    QWidget *m_decisionTab;
    QPushButton *m_batchDecisionButton;
    QPushButton *m_teamDecisionButton;
    QComboBox *m_modelSelectionCombo;
    QCheckBox *m_autoRepairCheckBox;
    QTextEdit *m_decisionResultsEdit;
    
    // 測試標籤頁
    QWidget *m_testingTab;
    QSpinBox *m_testCountSpinBox;
    QPushButton *m_autoTestButton;
    QProgressBar *m_testProgressBar;
    QLabel *m_testStatusLabel;
    
    // 日誌標籤頁
    QWidget *m_logsTab;
    AILogWidget *m_logWidget;
    QPushButton *m_exportAllLogsButton;
    
    // 設定標籤頁
    QWidget *m_settingsTab;
    QLineEdit *m_ollamaHostEdit;
    QSpinBox *m_ollamaPortSpinBox;
    QLineEdit *m_gameServerUrlEdit;
    QPushButton *m_connectGameServerButton;
    QLabel *m_connectionStatusLabel;
    
    // 統計標籤頁
    QWidget *m_statsTab;
    AIStatsWidget *m_statsWidget;
    
    // 狀態
    bool m_isConnectedToGameServer;
    QTimer *m_updateTimer;
    int m_totalAICount;
    QJsonArray m_currentAIPlayers;
};

} // namespace AI
} // namespace RANOnline
