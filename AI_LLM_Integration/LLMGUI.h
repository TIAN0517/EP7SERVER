#ifndef LLMGUI_H
#define LLMGUI_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QProgressBar>
#include <QListWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QUrl>
#ifdef HAVE_WEBSOCKETS
#include <QWebSocket>
#endif
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QStandardPaths>
#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QFontMetrics>
#include <memory>

// Forward declarations to avoid circular dependencies
class OllamaLLMManager;
class LLMRequestTask;
class LoadBalancer;
struct LLMRequestConfig;
struct LLMResponse;
struct LLMModelInfo;
struct OllamaServerConfig;

/**
 * @brief 科技感主題配色枚舉
 */
enum class ThemeMode {
    CyberBlue,    // 賽博藍
    NeonGreen,    // 霓虹綠
    PurpleHaze,   // 紫霧
    RedAlert,     // 紅色警戒
    GoldenGlow    // 金輝
};

/**
 * @brief 學院顏色映射
 */
struct AcademyTheme {
    QString name;
    QColor primaryColor;
    QColor secondaryColor;
    QColor accentColor;
    QColor textColor;
    QString gradient;
};

/**
 * @brief 自定義無邊框窗口基類
 */
class FramelessWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool m_isDragging = false;
    QPoint m_dragStartPosition;
    QPoint m_windowPositionAsDrag;
    
    void setupWindowFlags();
    void createShadowEffect();
};

/**
 * @brief 科技感進度條組件
 */
class CyberProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit CyberProgressBar(QWidget *parent = nullptr);
    
    void setAcademyTheme(const AcademyTheme &theme);
    void setGlowEffect(bool enabled);
    void setAnimationSpeed(int speed);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    AcademyTheme m_theme;
    bool m_glowEnabled = true;
    int m_animationSpeed = 1000;
    QTimer *m_pulseTimer;
    double m_pulseOpacity = 1.0;
    
    void setupPulseAnimation();
};

/**
 * @brief LLM請求項目組件
 */
class LLMRequestItem : public QFrame
{
    Q_OBJECT

public:
    explicit LLMRequestItem(const QString &requestId, const LLMRequestConfig &config, QWidget *parent = nullptr);
    
    void updateProgress(int progress);
    void updateResponse(const QString &content);
    void setCompleted(bool success, const QString &message = "");
    void setAcademyTheme(const AcademyTheme &theme);
    
    QString getRequestId() const { return m_requestId; }
    bool isCompleted() const { return m_isCompleted; }

signals:
    void cancelRequested(const QString &requestId);
    void retryRequested(const QString &requestId);

private slots:
    void onCancelClicked();
    void onRetryClicked();
    void updateAnimation();

private:
    QString m_requestId;
    LLMRequestConfig m_config;
    AcademyTheme m_theme;
    bool m_isCompleted = false;
    
    // UI組件
    QVBoxLayout *m_layout;
    QLabel *m_headerLabel;
    QLabel *m_modelLabel;
    QLabel *m_academyLabel;
    CyberProgressBar *m_progressBar;
    QTextEdit *m_responseEdit;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_cancelButton;
    QPushButton *m_retryButton;
    QLabel *m_statusLabel;
    
    // 動畫效果
    QPropertyAnimation *m_glowAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    void setupUI();
    void setupAnimations();
    void applyTheme();
};

/**
 * @brief 批量請求管理面板
 */
class BatchRequestPanel : public QScrollArea
{
    Q_OBJECT

public:
    explicit BatchRequestPanel(QWidget *parent = nullptr);
    
    void addRequest(const QString &requestId, const LLMRequestConfig &config);
    void updateRequest(const QString &requestId, const LLMResponse &response);
    void updateProgress(const QString &requestId, int progress);
    void updateStream(const QString &requestId, const QString &content);
    void removeRequest(const QString &requestId);
    void clearAll();
    
    int getActiveCount() const;
    int getCompletedCount() const;
    int getTotalCount() const;

signals:
    void requestCancelled(const QString &requestId);
    void requestRetry(const QString &requestId);

private slots:
    void onRequestCancel(const QString &requestId);
    void onRequestRetry(const QString &requestId);

private:
    QWidget *m_contentWidget;
    QVBoxLayout *m_layout;
    QMap<QString, LLMRequestItem*> m_requestItems;
    
    void setupUI();
    AcademyTheme getAcademyTheme(const QString &academy) const;
};

/**
 * @brief 統計儀表板組件
 */
class StatsDashboard : public QFrame
{
    Q_OBJECT

public:
    explicit StatsDashboard(QWidget *parent = nullptr);
    
    void updateStats(int active, int total, double avgTime, const QMap<QString, int> &modelUsage);
    void setTheme(ThemeMode theme);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_activeRequests = 0;
    int m_totalRequests = 0;
    double m_averageTime = 0.0;
    QMap<QString, int> m_modelUsage;
    ThemeMode m_theme = ThemeMode::CyberBlue;
    
    QTimer *m_updateTimer;
    
    void setupUI();
    void setupAnimations();
    QColor getThemeColor() const;
    QString getThemeGradient() const;
};

/**
 * @brief WebSocket遊戲事件同步器
 */
class GameEventSyncer : public QObject
{
    Q_OBJECT

public:
    explicit GameEventSyncer(QObject *parent = nullptr);
    
    void connectToGameServer(const QString &url);
    void disconnect();
    bool isConnected() const;
    
    void sendAIResponse(const QString &playerId, const QString &response);
    void sendSkillGeneration(const QString &skillId, const QString &description);
    void sendBatchResult(const QStringList &results);

signals:
    void gameEventReceived(const QString &eventType, const QJsonObject &data);
    void playerMessageReceived(const QString &playerId, const QString &message);
    void skillGenerationRequest(const QString &skillId, const QJsonObject &requirements);
    void connectionStatusChanged(bool connected);

private slots:
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(const QString &message);

private:
#ifdef HAVE_WEBSOCKETS
    std::unique_ptr<QWebSocket> m_webSocket;
#endif
    QString m_serverUrl;
    bool m_isConnected = false;
    
    void processGameEvent(const QJsonObject &event);
};

/**
 * @brief 主GUI窗口 - 無邊框科技感界面
 */
class LLMMainWindow : public FramelessWindow
{
    Q_OBJECT

public:
    explicit LLMMainWindow(QWidget *parent = nullptr);
    ~LLMMainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    // LLM管理
    void onSubmitRequest();
    void onSubmitBatchRequests();
    void onCancelAllRequests();
    void onRefreshModels();
    void onExportLogs();
    void onImportTemplates();
    void onExportTemplates();
    
    // 服務器管理
    void onAddServer();
    void onRemoveServer();
    void onTestConnection();
    
    // 主題和設置
    void onThemeChanged();
    void onShowSettings();
    void onShowAbout();
    void onToggleFullscreen();
    void onMinimizeToTray();
    
    // LLM回調
    void onLLMRequestCompleted(const LLMResponse &response);
    void onLLMRequestFailed(const QString &requestId, const QString &error);
    void onLLMStreamUpdate(const QString &requestId, const QString &content);
    void onLLMRequestProgress(const QString &requestId, int progress);
    void onLLMModelsRefreshed(const QList<LLMModelInfo> &models);
    void onLLMServerStatusChanged(const QString &serverName, bool isOnline);
    void onLLMStatisticsUpdated();
    
    // 遊戲事件
    void onGameEventReceived(const QString &eventType, const QJsonObject &data);
    void onPlayerMessageReceived(const QString &playerId, const QString &message);
    void onSkillGenerationRequest(const QString &skillId, const QJsonObject &requirements);
    void onGameConnectionStatusChanged(bool connected);
    
    // 系統托盤
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    // 核心組件
    std::unique_ptr<OllamaLLMManager> m_llmManager;
    std::unique_ptr<PromptTemplateManager> m_templateManager;
    std::unique_ptr<GameEventSyncer> m_gameSync;
    
    // UI組件
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    // 標題欄
    QFrame *m_titleBar;
    QHBoxLayout *m_titleLayout;
    QLabel *m_logoLabel;
    QLabel *m_titleLabel;
    QPushButton *m_minimizeButton;
    QPushButton *m_maximizeButton;
    QPushButton *m_closeButton;
    QPushButton *m_themeButton;
    
    // 主要內容區域
    QSplitter *m_mainSplitter;
    
    // 左側控制面板
    QTabWidget *m_controlTabs;
    QWidget *m_requestTab;
    QWidget *m_batchTab;
    QWidget *m_serverTab;
    QWidget *m_templateTab;
    QWidget *m_settingsTab;
    
    // 請求控制
    QFormLayout *m_requestForm;
    QComboBox *m_modelCombo;
    QComboBox *m_academyCombo;
    QComboBox *m_templateCombo;
    QTextEdit *m_promptEdit;
    QTextEdit *m_systemPromptEdit;
    QDoubleSpinBox *m_temperatureSpin;
    QSpinBox *m_maxTokensSpin;
    QSpinBox *m_topKSpin;
    QDoubleSpinBox *m_topPSpin;
    QPushButton *m_submitButton;
    QPushButton *m_submitBatchButton;
    QPushButton *m_cancelAllButton;
    
    // 批量請求控制
    QVBoxLayout *m_batchLayout;
    QSpinBox *m_batchCountSpin;
    QComboBox *m_batchModelCombo;
    QTextEdit *m_batchPromptEdit;
    QCheckBox *m_autoVariationCheck;
    QPushButton *m_generateBatchButton;
    QListWidget *m_batchPreviewList;
    
    // 服務器管理
    QVBoxLayout *m_serverLayout;
    QListWidget *m_serverList;
    QLineEdit *m_serverHostEdit;
    QSpinBox *m_serverPortSpin;
    QLineEdit *m_serverNameEdit;
    QPushButton *m_addServerButton;
    QPushButton *m_removeServerButton;
    QPushButton *m_testConnectionButton;
    QLabel *m_connectionStatusLabel;
    
    // 右側結果面板
    QTabWidget *m_resultTabs;
    BatchRequestPanel *m_batchPanel;
    QTextEdit *m_logEdit;
    StatsDashboard *m_statsDashboard;
    
    // 底部狀態欄
    QFrame *m_statusBar;
    QHBoxLayout *m_statusLayout;
    QLabel *m_statusLabel;
    QLabel *m_connectionLabel;
    QLabel *m_requestCountLabel;
    QProgressBar *m_globalProgressBar;
    
    // 系統托盤
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    
    // 設置和狀態
    ThemeMode m_currentTheme = ThemeMode::CyberBlue;
    QSettings *m_settings;
    QString m_lastLogExportPath;
    bool m_isInitialized = false;
    
    // 動畫效果
    QPropertyAnimation *m_fadeAnimation;
    QTimer *m_statusUpdateTimer;
    
    // 私有方法
    void setupUI();
    void setupTitleBar();
    void setupControlPanel();
    void setupResultPanel();
    void setupStatusBar();
    void setupTrayIcon();
    void setupConnections();
    void setupAnimations();
    void loadSettings();
    void saveSettings();
    void applyTheme(ThemeMode theme);
    void updateThemeColors();
    void createMenuActions();
    void updateServerList();
    void updateModelCombo();
    void updateStats();
    void addLogMessage(const QString &message, const QString &category = "INFO");
    void exportLogsToFile(const QString &fileName);
    void showNotification(const QString &title, const QString &message);
    QString formatDuration(qint64 milliseconds) const;
    QMap<QString, QString> collectTemplateVariables() const;
    LLMRequestConfig createRequestConfig() const;
    void handleGameSkillGeneration(const QString &skillId, const QJsonObject &requirements);
    void handlePlayerInteraction(const QString &playerId, const QString &message);
    
    // 主題相關
    AcademyTheme getAcademyTheme(const QString &academy) const;
    QString getThemeStyleSheet() const;
    QString getCyberButtonStyle() const;
    QString getGlowEffectStyle(const QColor &color) const;
};

#endif // LLMGUI_H
