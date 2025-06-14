#ifndef ODBCCONNECTIONMANAGER_H
#define ODBCCONNECTIONMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include <memory>
#include <atomic>

#ifdef Q_OS_WIN
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#endif

/**
 * @brief ODBC连接管理器 - 专用于SQL Server 2022连接
 * 
 * 功能特性:
 * - 自动检测和创建ODBC DSN
 * - 连接池管理
 * - 自动重连机制
 * - 健康检查和故障转移
 * - 详细的错误日志记录
 * - 支持x64/x86架构
 * - 兼容VC++/MinGW编译器
 */
class ODBCConnectionManager : public QObject
{
    Q_OBJECT

public:
    struct ConnectionConfig {
        QString serverAddress = "192.168.1.100";   // 数据库服务器地址
        QString databaseName = "RanOnlineAI";      // 数据库名称
        QString username = "sa";                    // 用户名
        QString password = "Ss520520@@";           // 密码
        QString dsnName = "RanOnlineAIDB";         // DSN名称
        QString driverName = "SQL Server Native Client 11.0"; // ODBC驱动名称
        int port = 1433;                           // SQL Server端口
        int connectionTimeout = 30;                // 连接超时秒数
        int queryTimeout = 60;                     // 查询超时秒数
        int maxRetries = 3;                        // 最大重试次数
        int retryInterval = 5000;                  // 重试间隔(毫秒)
        bool trustServerCertificate = true;        // 信任服务器证书
        bool integratedSecurity = false;           // 是否使用Windows身份验证
    };

    struct ConnectionStatus {
        bool isConnected = false;
        QString lastError;
        QDateTime lastConnectTime;
        QDateTime lastErrorTime;
        int retryCount = 0;
        qint64 totalQueries = 0;
        qint64 successfulQueries = 0;
        qint64 failedQueries = 0;
        double averageQueryTime = 0.0;
    };

    explicit ODBCConnectionManager(QObject *parent = nullptr);
    ~ODBCConnectionManager();

    // 配置管理
    void setConnectionConfig(const ConnectionConfig &config);
    ConnectionConfig getConnectionConfig() const;
    void loadConfigFromFile(const QString &configFile = "config/database_config.json");
    void saveConfigToFile(const QString &configFile = "config/database_config.json") const;

    // 连接管理
    bool initialize();
    bool connectToDatabase();
    void disconnectFromDatabase();
    bool isConnected() const;
    ConnectionStatus getConnectionStatus() const;

    // DSN管理
    bool checkDSNExists(const QString &dsnName) const;
    bool createSystemDSN(const ConnectionConfig &config);
    bool removeSystemDSN(const QString &dsnName);
    QStringList getAvailableDrivers() const;

    // 查询操作
    QSqlQuery executeQuery(const QString &queryString);
    QSqlQuery executePreparedQuery(const QString &queryString, const QVariantList &parameters);
    bool executeNonQuery(const QString &queryString);
    QVariant executeScalar(const QString &queryString);

    // 事务管理
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // 健康检查
    void startHealthCheck(int intervalMs = 30000);
    void stopHealthCheck();
    bool performHealthCheck();

    // 实用方法
    QString getConnectionString() const;
    QString getLastError() const;
    void clearErrorHistory();

public slots:
    void reconnect();
    void performPeriodicHealthCheck();

signals:
    void connected();
    void disconnected();
    void connectionFailed(const QString &error);
    void queryExecuted(const QString &query, bool success, double executionTime);
    void healthCheckCompleted(bool isHealthy);
    void errorOccurred(const QString &error, const QString &details);

private slots:
    void onConnectionLost();

private:
    // 内部方法
    bool createConnection();
    void setupConnection();
    void logError(const QString &operation, const QString &error);
    void logInfo(const QString &message);
    void updateConnectionStatus(bool connected, const QString &error = QString());
    QString formatConnectionString(const ConnectionConfig &config) const;
    bool testConnection(const QString &connectionString);
    void recordQueryMetrics(bool success, double executionTime);

    // 数据成员
    mutable QMutex m_mutex;
    QSqlDatabase m_database;
    QTimer *m_healthCheckTimer = nullptr;
    QTimer *m_reconnectTimer = nullptr;
    
    ConnectionConfig m_config;
    ConnectionStatus m_status;
    
    QString m_connectionId;
    std::atomic<bool> m_isInitialized{false};
    std::atomic<bool> m_shouldReconnect{true};
    
    // 性能统计
    QDateTime m_lastQueryTime;
    double m_totalQueryTime = 0.0;
    
    // 错误历史
    QStringList m_errorHistory;
    static const int MAX_ERROR_HISTORY = 100;
};

/**
 * @brief ODBC连接工厂 - 单例模式
 */
class ODBCConnectionFactory
{
public:
    static ODBCConnectionFactory& instance();
    
    std::shared_ptr<ODBCConnectionManager> createConnection(const QString &name = "default");
    std::shared_ptr<ODBCConnectionManager> getConnection(const QString &name = "default");
    void removeConnection(const QString &name);
    QStringList getConnectionNames() const;
    
    // 批量操作
    bool createSystemDSN(const ODBCConnectionManager::ConnectionConfig &config);
    bool removeSystemDSN(const QString &dsnName);
    QStringList getSystemDSNList() const;

private:
    ODBCConnectionFactory() = default;
    ~ODBCConnectionFactory() = default;
    
    QMutex m_mutex;
    QMap<QString, std::shared_ptr<ODBCConnectionManager>> m_connections;
};

// 便利宏定义
#define ODBC_CONN ODBCConnectionFactory::instance()
#define DEFAULT_DB_CONN ODBC_CONN.getConnection("default")

#endif // ODBCCONNECTIONMANAGER_H
