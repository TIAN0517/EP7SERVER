#pragma once

// Qt 核心標頭檔
#include <QWidget>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QListWidget>
#include <QScrollArea>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QSettings>
#include <QApplication>
#include <QIcon>
#include <QPixmap>
#include <QSplashScreen>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QCloseEvent>
#include <QShowEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QStandardPaths>
#include <QMessageBox>
#include <QProgressBar>
#include <QSplitter>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>
#include <QDial>
#include <QTextBrowser>
#include <QToolButton>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QActionGroup>

// 專案特定標頭檔
#include "FramelessWindow.h"
#include "AcademyTheme.h"
#include "LLMRequestItem_New.h"

// 新增AI管控系統頭檔
#include "AIManagementWidget.h"
#include "AIPlayerGenerator.h"
#include "AIDecisionEngine.h"

// 前向聲明
class OllamaLLMManager;
class GameEventSyncer;

/**
 * @brief RANOnline LLM Integration 主窗口 - 包含四部門AI智能管控系統
 * @version 3.0.0 - 添加Jy技術團隊品牌標識和圖標支持
 */
class LLMMainWindow : public FramelessWindow
{
    Q_OBJECT

public:
    explicit LLMMainWindow(QWidget *parent = nullptr);
    ~LLMMainWindow();

private slots:
    void onSubmitRequest();
    void onSubmitBatchRequests();
    void onCancelAllRequests();
    void onRefreshModels();
    void onExportLogs();
    void onImportTemplates();
    void onExportTemplates();
    void onAddServer();
    void onRemoveServer();
    void onTestConnection();
    void onThemeChanged();
    void onShowSettings();
    void onShowAbout();
    void onToggleFullscreen();
    void onMinimizeToTray();
    
    // LLM Manager 回調
    void onLLMRequestCompleted(const LLMResponse &response);
    void onLLMRequestFailed(const QString &requestId, const QString &error);
    void onLLMStreamUpdate(const QString &requestId, const QString &content);
    void onLLMRequestProgress(const QString &requestId, int progress);
    void onLLMModelsRefreshed(const QList<LLMModelInfo> &models);
    void onLLMServerStatusChanged(const QString &serverName, bool isOnline);
    void onLLMStatisticsUpdated();
    
    // 遊戲事件回調
    void onGameEventReceived(const QString &eventType, const QJsonObject &data);
    void onPlayerMessageReceived(const QString &playerId, const QString &message);
    void onSkillGenerationRequest(const QString &academy, int level);
    void onGameConnectionStatusChanged(bool connected);
    
    // 托盤圖標
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    // 新增AI管控系統槽函數
    void onAIManagementTabActivated();
    void onAIPlayersGenerated(const QJsonArray &players);
    void onAIBatchDecisionCompleted(const QJsonArray &results);
    void onAITestCompleted(const RANOnline::AI::TestReport &report);
    void onAILogGenerated(const QString &logEntry);
    void onAutoRepairTriggered(const QString &aiId, const QString &repairAction);
    
    // AI玩家生成器回調
    void onAIPlayerGenerated(const RANOnline::AI::AIPlayerData &player);
    void onAIErrorOccurred(const QString &error);
    
    // AI決策引擎回調
    void onAIDecisionMade(const RANOnline::AI::AIDecisionResponse &response);
    void onAIDecisionError(const QString &error);
    void onTestAIRequested(int count, const QString &modelType);

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    // 核心組件
    OllamaLLMManager *m_llmManager;
    // GameEventSyncer *m_gameEventSyncer;  // 暫時註釋，待實現
    
    // 新增AI管控系統組件
    RANOnline::AI::AIManagementWidget *m_aiManagementWidget;
    RANOnline::AI::AIPlayerGenerator *m_aiPlayerGenerator;
    RANOnline::AI::AIDecisionEngine *m_aiDecisionEngine;
    
    // UI 組件
    QTabWidget *m_tabWidget;
    QWidget *m_requestTab;
    QWidget *m_batchTab;
    QWidget *m_serverTab;
    QWidget *m_templateTab;
    QWidget *m_settingsTab;
    QWidget *m_aiManagementTab;  // 新增AI管控標籤頁
    
    // 請求頁面
    QVBoxLayout *m_requestLayout;
    QComboBox *m_modelCombo;
    QComboBox *m_academyCombo;
    QTextEdit *m_promptEdit;
    QPushButton *m_submitButton;
    QScrollArea *m_requestScrollArea;
    QWidget *m_requestContainer;
    QVBoxLayout *m_requestContainerLayout;
    
    // 批量頁面
    QVBoxLayout *m_batchLayout;
    QTextEdit *m_batchPromptEdit;
    QSpinBox *m_batchCountSpin;
    QPushButton *m_batchSubmitButton;
    QPushButton *m_batchCancelButton;
    QListWidget *m_batchList;
    
    // 服務器頁面
    QVBoxLayout *m_serverLayout;
    QListWidget *m_serverList;
    QPushButton *m_addServerButton;
    QPushButton *m_removeServerButton;
    QPushButton *m_testConnectionButton;
    
    // 狀態和控制
    QLabel *m_statusLabel;
    QLabel *m_statsLabel;
    QPushButton *m_themeButton;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    
    // Jy技術團隊品牌標識
    QLabel *m_logoLabel;           // 左上角Logo標籤
    QPixmap *m_logoPixmap;         // Logo圖片
    QLabel *m_brandLabel;          // 品牌文字標籤
    QIcon m_appIcon;               // 應用程式圖標
    QIcon m_taskbarIcon;           // 任務欄圖標
    
    // 設置和主題
    QSettings *m_settings;
    AcademyTheme m_currentTheme;
    static const QMap<QString, AcademyTheme> ACADEMY_THEMES;
      // 初始化方法
    void setupUI();
    void setupTitleBar();
    void setupControlPanel();
    void setupRequestTab();
    void setupBatchTab();
    void setupServerTab();
    void setupTemplateTab();
    void setupSettingsTab();
    void setupResultPanel();
    void setupStatusBar();
    void setupTrayIcon();
    void setupAIManagementTab();
    void setupConnections();
    void applyTheme(const AcademyTheme &theme);
    
    // Ollama管理器初始化
    void initializeOllamaManager();
    
    // Jy技術團隊品牌和圖標設置
    void setupBrandingAndIcons();
    void setupApplicationIcon();
    void setupTaskbarIcon();
    void setupLogoDisplay();
    void loadBrandResources();
    
    // 請求管理
    void addRequest(const QString &requestId, const LLMRequestConfig &config);
    void updateRequest(const QString &requestId, const LLMResponse &response);
    void updateProgress(const QString &requestId, int progress);
    
    // 設置和主題
    void loadSettings();
    void saveSettings();
    void applyTheme();
    void updateThemeColors();
    void updateServerList();
    void updateModelCombo();
    void updateStats();
    void addLogMessage(const QString &message, const QString &level = "INFO");
    void exportLogsToFile();
    void showNotification(const QString &title, const QString &message);
    
    // 工具方法
    QString formatDuration(qint64 milliseconds);
    QStringList collectTemplateVariables(const QString &templateText);
    LLMRequestConfig createRequestConfig();
    void handleGameSkillGeneration(const QString &academy, int level);
    void handlePlayerInteraction(const QString &playerId, const QString &message);
      // 主題相關
    AcademyTheme getAcademyTheme(const QString &academy);
    QString getThemeStyleSheet(const AcademyTheme &theme);
    QString getCyberButtonStyle(const AcademyTheme &theme);
    QString getGlowEffectStyle(const AcademyTheme &theme);
    QString getCyberTechStyleSheet(const AcademyTheme &theme);

    // 新增AI管控初始化和測試方法
    void initializeAIManagement();
    void testAcademyNamingSystem();
    void connectAISignals();
};
