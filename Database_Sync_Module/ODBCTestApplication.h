#ifndef ODBCTESTAPPLICATION_H
#define ODBCTESTAPPLICATION_H

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QProgressBar>
#include <QGroupBox>
#include <QFormLayout>
#include <QTimer>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>
#include <QCoreApplication>
#include "ODBCConnectionManager.h"

/**
 * @brief ODBC测试应用程序主窗口
 * 
 * 功能特性:
 * - 图形化ODBC连接测试
 * - 实时连接状态监控
 * - 自动DSN检测和创建
 * - SQL查询测试工具
 * - 性能基准测试
 * - 详细错误诊断
 * - 一键式部署验证
 */
class ODBCTestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ODBCTestMainWindow(QWidget *parent = nullptr);
    ~ODBCTestMainWindow();

private slots:
    void onTestConnection();
    void onCreateDSN();
    void onRemoveDSN();
    void onRunBenchmark();
    void onExecuteQuery();
    void onClearLog();
    void onConnectionStatusChanged(bool connected);
    void onUpdateConnectionStatus();

private:
    void setupUI();
    void setupConnections();
    void updateStatus(const QString &message, bool isError = false);
    void logMessage(const QString &message);
    
    // UI组件
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    // 连接配置组
    QGroupBox *m_connectionGroup;
    QFormLayout *m_configLayout;
    QLineEdit *m_serverEdit;
    QLineEdit *m_databaseEdit;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_dsnEdit;
    
    // 控制按钮组
    QGroupBox *m_controlGroup;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_testButton;
    QPushButton *m_createDsnButton;
    QPushButton *m_removeDsnButton;
    QPushButton *m_benchmarkButton;
    
    // SQL测试组
    QGroupBox *m_sqlGroup;
    QVBoxLayout *m_sqlLayout;
    QTextEdit *m_sqlEdit;
    QPushButton *m_executeButton;
    QTextEdit *m_resultEdit;
    
    // 状态和日志组
    QGroupBox *m_statusGroup;
    QVBoxLayout *m_statusLayout;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QTextEdit *m_logEdit;
    QPushButton *m_clearLogButton;
    
    // 连接管理器
    std::unique_ptr<ODBCConnectionManager> m_odbcManager;
    QTimer *m_statusTimer;
    
    // 状态变量
    bool m_isConnected;
    QDateTime m_lastConnectionTime;
};

/**
 * @brief ODBC测试应用程序类
 */
class ODBCTestApplication : public QApplication
{
    Q_OBJECT

public:
    ODBCTestApplication(int &argc, char **argv);
    ~ODBCTestApplication();
    
    int runApplication();

private:
    void showSplashScreen();
    void initializeApplication();
    
    QSplashScreen *m_splash;
    std::unique_ptr<ODBCTestMainWindow> m_mainWindow;
};

/**
 * @brief 后台连接测试线程
 */
class ConnectionTestThread : public QThread
{
    Q_OBJECT

public:
    ConnectionTestThread(const ODBCConnectionManager::ConnectionConfig &config, QObject *parent = nullptr);

signals:
    void testCompleted(bool success, const QString &message);
    void progressUpdate(int percentage);

protected:
    void run() override;

private:
    ODBCConnectionManager::ConnectionConfig m_config;
};

/**
 * @brief 性能基准测试线程
 */
class BenchmarkTestThread : public QThread
{
    Q_OBJECT

public:
    BenchmarkTestThread(const ODBCConnectionManager::ConnectionConfig &config, QObject *parent = nullptr);

signals:
    void benchmarkCompleted(const QString &results);
    void progressUpdate(int percentage);

protected:
    void run() override;

private:
    ODBCConnectionManager::ConnectionConfig m_config;
    QString runConnectivityTest();
    QString runQueryPerformanceTest();
    QString runTransactionTest();
    QString runConnectionPoolTest();
};

#endif // ODBCTESTAPPLICATION_H
