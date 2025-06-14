#include "ODBCConnectionManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QSqlRecord>
#include <QElapsedTimer>
#include <QUuid>

ODBCConnectionManager::ODBCConnectionManager(QObject *parent)
    : QObject(parent)
    , m_healthCheckTimer(new QTimer(this))
    , m_reconnectTimer(new QTimer(this))
{
    // 生成唯一连接ID
    m_connectionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // 连接定时器信号
    connect(m_healthCheckTimer, &QTimer::timeout, this, &ODBCConnectionManager::performPeriodicHealthCheck);
    connect(m_reconnectTimer, &QTimer::timeout, this, &ODBCConnectionManager::reconnect);
    
    logInfo("ODBCConnectionManager initialized with ID: " + m_connectionId);
}

ODBCConnectionManager::~ODBCConnectionManager()
{
    stopHealthCheck();
    disconnectFromDatabase();
    logInfo("ODBCConnectionManager destroyed: " + m_connectionId);
}

void ODBCConnectionManager::setConnectionConfig(const ConnectionConfig &config)
{
    QMutexLocker locker(&m_mutex);
    m_config = config;
    logInfo("Connection configuration updated");
}

ODBCConnectionManager::ConnectionConfig ODBCConnectionManager::getConnectionConfig() const
{
    QMutexLocker locker(&m_mutex);
    return m_config;
}

void ODBCConnectionManager::loadConfigFromFile(const QString &configFile)
{
    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly)) {
        logError("loadConfigFromFile", "Cannot open config file: " + configFile);
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        logError("loadConfigFromFile", "JSON parse error: " + error.errorString());
        return;
    }
    
    QJsonObject obj = doc.object();
    ConnectionConfig config;
    
    // 读取配置
    config.serverAddress = obj["serverAddress"].toString(config.serverAddress);
    config.databaseName = obj["databaseName"].toString(config.databaseName);
    config.username = obj["username"].toString(config.username);
    config.password = obj["password"].toString(config.password);
    config.dsnName = obj["dsnName"].toString(config.dsnName);
    config.driverName = obj["driverName"].toString(config.driverName);
    config.port = obj["port"].toInt(config.port);
    config.connectionTimeout = obj["connectionTimeout"].toInt(config.connectionTimeout);
    config.queryTimeout = obj["queryTimeout"].toInt(config.queryTimeout);
    config.maxRetries = obj["maxRetries"].toInt(config.maxRetries);
    config.retryInterval = obj["retryInterval"].toInt(config.retryInterval);
    config.trustServerCertificate = obj["trustServerCertificate"].toBool(config.trustServerCertificate);
    config.integratedSecurity = obj["integratedSecurity"].toBool(config.integratedSecurity);
    
    setConnectionConfig(config);
    logInfo("Configuration loaded from file: " + configFile);
}

void ODBCConnectionManager::saveConfigToFile(const QString &configFile) const
{
    QMutexLocker locker(&m_mutex);
    
    // 确保目录存在
    QFileInfo info(configFile);
    QDir().mkpath(info.absolutePath());
    
    QJsonObject obj;
    obj["serverAddress"] = m_config.serverAddress;
    obj["databaseName"] = m_config.databaseName;
    obj["username"] = m_config.username;
    obj["password"] = m_config.password; // 注意：实际使用时应加密密码
    obj["dsnName"] = m_config.dsnName;
    obj["driverName"] = m_config.driverName;
    obj["port"] = m_config.port;
    obj["connectionTimeout"] = m_config.connectionTimeout;
    obj["queryTimeout"] = m_config.queryTimeout;
    obj["maxRetries"] = m_config.maxRetries;
    obj["retryInterval"] = m_config.retryInterval;
    obj["trustServerCertificate"] = m_config.trustServerCertificate;
    obj["integratedSecurity"] = m_config.integratedSecurity;
    
    QJsonDocument doc(obj);
    
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        logInfo("Configuration saved to file: " + configFile);
    } else {
        logError("saveConfigToFile", "Cannot write to config file: " + configFile);
    }
}

bool ODBCConnectionManager::initialize()
{
    QMutexLocker locker(&m_mutex);
    
    logInfo("Initializing ODBC connection manager...");
    
    // 检查并创建DSN
    if (!checkDSNExists(m_config.dsnName)) {
        logInfo("DSN not found, attempting to create: " + m_config.dsnName);
        if (!createSystemDSN(m_config)) {
            logError("initialize", "Failed to create DSN: " + m_config.dsnName);
            return false;
        }
    }
    
    // 创建数据库连接
    if (!createConnection()) {
        logError("initialize", "Failed to create database connection");
        return false;
    }
    
    m_isInitialized = true;
    logInfo("ODBC connection manager initialized successfully");
    return true;
}

bool ODBCConnectionManager::connectToDatabase()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_database.isOpen()) {
        logInfo("Database already connected");
        return true;
    }
    
    logInfo("Connecting to database...");
    
    // 设置连接参数
    setupConnection();
    
    // 尝试连接
    if (!m_database.open()) {
        QString error = m_database.lastError().text();
        logError("connectToDatabase", "Database connection failed: " + error);
        updateConnectionStatus(false, error);
        return false;
    }
    
    // 验证连接
    if (!performHealthCheck()) {
        logError("connectToDatabase", "Database health check failed");
        m_database.close();
        updateConnectionStatus(false, "Health check failed");
        return false;
    }
    
    updateConnectionStatus(true);
    logInfo("Database connected successfully");
    emit connected();
    
    return true;
}

void ODBCConnectionManager::disconnectFromDatabase()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_database.isOpen()) {
        logInfo("Disconnecting from database...");
        m_database.close();
        updateConnectionStatus(false);
        emit disconnected();
        logInfo("Database disconnected");
    }
}

bool ODBCConnectionManager::isConnected() const
{
    QMutexLocker locker(&m_mutex);
    return m_database.isOpen() && m_status.isConnected;
}

ODBCConnectionManager::ConnectionStatus ODBCConnectionManager::getConnectionStatus() const
{
    QMutexLocker locker(&m_mutex);
    return m_status;
}

bool ODBCConnectionManager::checkDSNExists(const QString &dsnName) const
{
#ifdef Q_OS_WIN
    HKEY hKey;
    QString keyPath = "SOFTWARE\\ODBC\\ODBC.INI\\" + dsnName;
    
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                               keyPath.toLocal8Bit().constData(), 
                               0, KEY_READ, &hKey);
    
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        logInfo("DSN found: " + dsnName);
        return true;
    }
    
    logInfo("DSN not found: " + dsnName);
    return false;
#else
    // Linux/Mac实现
    return false;
#endif
}

bool ODBCConnectionManager::createSystemDSN(const ConnectionConfig &config)
{
#ifdef Q_OS_WIN
    logInfo("Creating system DSN: " + config.dsnName);
    
    QString attributes = QString(
        "DSN=%1\0"
        "Description=RANOnline AI System Database Connection\0"
        "Server=%2,%3\0"
        "Database=%4\0"
        "Trusted_Connection=%5\0"
        "TrustServerCertificate=%6\0"
        "Encrypt=Optional\0"
        "MARS_Connection=Yes\0"
        "APP=RANOnline AI System\0"
        "WSID=%7\0\0"
    ).arg(config.dsnName)
     .arg(config.serverAddress)
     .arg(config.port)
     .arg(config.databaseName)
     .arg(config.integratedSecurity ? "Yes" : "No")
     .arg(config.trustServerCertificate ? "Yes" : "No")
     .arg(QHostInfo::localHostName());
    
    QByteArray attributesData = attributes.toLocal8Bit();
    
    BOOL result = SQLConfigDataSource(
        NULL,                                    // 父窗口句柄
        ODBC_ADD_SYS_DSN,                       // 添加系统DSN
        config.driverName.toLocal8Bit().constData(), // 驱动名称
        attributesData.constData()               // DSN属性
    );
    
    if (result) {
        logInfo("System DSN created successfully: " + config.dsnName);
        return true;
    } else {
        DWORD errorCode;
        WORD errorMsg = 1;
        char errorBuffer[SQL_MAX_MESSAGE_LENGTH];
        WORD bufferLength;
        WORD messageLength;
        
        if (SQLInstallerError(errorMsg, &errorCode, errorBuffer, 
                            sizeof(errorBuffer), &messageLength) == SQL_SUCCESS) {
            QString error = QString::fromLocal8Bit(errorBuffer, messageLength);
            logError("createSystemDSN", QString("Failed to create DSN (Code: %1): %2")
                     .arg(errorCode).arg(error));
        } else {
            logError("createSystemDSN", "Failed to create DSN (unknown error)");
        }
        return false;
    }
#else
    Q_UNUSED(config)
    logError("createSystemDSN", "DSN creation not supported on this platform");
    return false;
#endif
}

bool ODBCConnectionManager::removeSystemDSN(const QString &dsnName)
{
#ifdef Q_OS_WIN
    logInfo("Removing system DSN: " + dsnName);
    
    QString attributes = QString("DSN=%1\0\0").arg(dsnName);
    QByteArray attributesData = attributes.toLocal8Bit();
    
    BOOL result = SQLConfigDataSource(
        NULL,
        ODBC_REMOVE_SYS_DSN,
        "SQL Server Native Client 11.0",
        attributesData.constData()
    );
    
    if (result) {
        logInfo("System DSN removed successfully: " + dsnName);
        return true;
    } else {
        logError("removeSystemDSN", "Failed to remove DSN: " + dsnName);
        return false;
    }
#else
    Q_UNUSED(dsnName)
    logError("removeSystemDSN", "DSN removal not supported on this platform");
    return false;
#endif
}

QStringList ODBCConnectionManager::getAvailableDrivers() const
{
    QStringList drivers;
    
#ifdef Q_OS_WIN
    CHAR driverName[256];
    CHAR driverAttributes[256];
    WORD nameLength;
    WORD attributesLength;
    WORD direction = SQL_FETCH_FIRST;
    
    SQLHENV hEnv;
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        
        while (SQLDrivers(hEnv, direction, (SQLCHAR*)driverName, sizeof(driverName), &nameLength,
                         (SQLCHAR*)driverAttributes, sizeof(driverAttributes), &attributesLength) == SQL_SUCCESS) {
            
            drivers << QString::fromLocal8Bit(driverName);
            direction = SQL_FETCH_NEXT;
        }
        
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
#endif
    
    logInfo(QString("Found %1 ODBC drivers").arg(drivers.count()));
    return drivers;
}

QSqlQuery ODBCConnectionManager::executeQuery(const QString &queryString)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_database.isOpen()) {
        logError("executeQuery", "Database not connected");
        return QSqlQuery();
    }
    
    QElapsedTimer timer;
    timer.start();
    
    QSqlQuery query(m_database);
    query.setForwardOnly(true); // 优化性能
    
    bool success = query.exec(queryString);
    double executionTime = timer.elapsed();
    
    recordQueryMetrics(success, executionTime);
    
    if (success) {
        logInfo(QString("Query executed successfully in %1ms: %2")
                .arg(executionTime, 0, 'f', 2)
                .arg(queryString.left(100))); // 只记录前100字符
    } else {
        QString error = query.lastError().text();
        logError("executeQuery", QString("Query failed: %1 | Error: %2")
                 .arg(queryString.left(100)).arg(error));
    }
    
    emit queryExecuted(queryString, success, executionTime);
    return query;
}

QSqlQuery ODBCConnectionManager::executePreparedQuery(const QString &queryString, const QVariantList &parameters)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_database.isOpen()) {
        logError("executePreparedQuery", "Database not connected");
        return QSqlQuery();
    }
    
    QElapsedTimer timer;
    timer.start();
    
    QSqlQuery query(m_database);
    query.setForwardOnly(true);
    
    if (!query.prepare(queryString)) {
        QString error = query.lastError().text();
        logError("executePreparedQuery", QString("Query preparation failed: %1 | Error: %2")
                 .arg(queryString.left(100)).arg(error));
        return query;
    }
    
    // 绑定参数
    for (int i = 0; i < parameters.size(); ++i) {
        query.bindValue(i, parameters[i]);
    }
    
    bool success = query.exec();
    double executionTime = timer.elapsed();
    
    recordQueryMetrics(success, executionTime);
    
    if (success) {
        logInfo(QString("Prepared query executed successfully in %1ms: %2")
                .arg(executionTime, 0, 'f', 2)
                .arg(queryString.left(100)));
    } else {
        QString error = query.lastError().text();
        logError("executePreparedQuery", QString("Prepared query failed: %1 | Error: %2")
                 .arg(queryString.left(100)).arg(error));
    }
    
    emit queryExecuted(queryString, success, executionTime);
    return query;
}

bool ODBCConnectionManager::executeNonQuery(const QString &queryString)
{
    QSqlQuery query = executeQuery(queryString);
    return !query.lastError().isValid();
}

QVariant ODBCConnectionManager::executeScalar(const QString &queryString)
{
    QSqlQuery query = executeQuery(queryString);
    if (query.next()) {
        return query.value(0);
    }
    return QVariant();
}

bool ODBCConnectionManager::beginTransaction()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_database.isOpen()) {
        logError("beginTransaction", "Database not connected");
        return false;
    }
    
    bool success = m_database.transaction();
    if (success) {
        logInfo("Transaction started");
    } else {
        logError("beginTransaction", "Failed to start transaction: " + m_database.lastError().text());
    }
    
    return success;
}

bool ODBCConnectionManager::commitTransaction()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_database.isOpen()) {
        logError("commitTransaction", "Database not connected");
        return false;
    }
    
    bool success = m_database.commit();
    if (success) {
        logInfo("Transaction committed");
    } else {
        logError("commitTransaction", "Failed to commit transaction: " + m_database.lastError().text());
    }
    
    return success;
}

bool ODBCConnectionManager::rollbackTransaction()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_database.isOpen()) {
        logError("rollbackTransaction", "Database not connected");
        return false;
    }
    
    bool success = m_database.rollback();
    if (success) {
        logInfo("Transaction rolled back");
    } else {
        logError("rollbackTransaction", "Failed to rollback transaction: " + m_database.lastError().text());
    }
    
    return success;
}

void ODBCConnectionManager::startHealthCheck(int intervalMs)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_healthCheckTimer->isActive()) {
        m_healthCheckTimer->stop();
    }
    
    logInfo(QString("Starting health check with interval: %1ms").arg(intervalMs));
    m_healthCheckTimer->start(intervalMs);
}

void ODBCConnectionManager::stopHealthCheck()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_healthCheckTimer->isActive()) {
        logInfo("Stopping health check");
        m_healthCheckTimer->stop();
    }
}

bool ODBCConnectionManager::performHealthCheck()
{
    if (!m_database.isOpen()) {
        return false;
    }
    
    // 执行简单的健康检查查询
    QSqlQuery query = executeQuery("SELECT 1 AS HealthCheck");
    bool isHealthy = query.next() && query.value(0).toInt() == 1;
    
    if (isHealthy) {
        logInfo("Health check passed");
    } else {
        logError("performHealthCheck", "Health check failed");
    }
    
    return isHealthy;
}

QString ODBCConnectionManager::getConnectionString() const
{
    QMutexLocker locker(&m_mutex);
    return formatConnectionString(m_config);
}

QString ODBCConnectionManager::getLastError() const
{
    QMutexLocker locker(&m_mutex);
    return m_status.lastError;
}

void ODBCConnectionManager::clearErrorHistory()
{
    QMutexLocker locker(&m_mutex);
    m_errorHistory.clear();
    logInfo("Error history cleared");
}

void ODBCConnectionManager::reconnect()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_shouldReconnect) {
        return;
    }
    
    logInfo("Attempting to reconnect...");
    
    // 关闭现有连接
    if (m_database.isOpen()) {
        m_database.close();
    }
    
    // 尝试重新连接
    if (connectToDatabase()) {
        logInfo("Reconnection successful");
        m_reconnectTimer->stop();
        m_status.retryCount = 0;
    } else {
        m_status.retryCount++;
        if (m_status.retryCount < m_config.maxRetries) {
            logInfo(QString("Reconnection failed, retry %1/%2 in %3ms")
                    .arg(m_status.retryCount)
                    .arg(m_config.maxRetries)
                    .arg(m_config.retryInterval));
            m_reconnectTimer->start(m_config.retryInterval);
        } else {
            logError("reconnect", "Maximum retry attempts reached, giving up");
            m_reconnectTimer->stop();
            emit connectionFailed("Maximum retry attempts reached");
        }
    }
}

void ODBCConnectionManager::performPeriodicHealthCheck()
{
    bool isHealthy = performHealthCheck();
    emit healthCheckCompleted(isHealthy);
    
    if (!isHealthy && m_shouldReconnect) {
        onConnectionLost();
    }
}

void ODBCConnectionManager::onConnectionLost()
{
    logError("onConnectionLost", "Database connection lost, attempting to reconnect");
    updateConnectionStatus(false, "Connection lost");
    
    if (m_shouldReconnect && !m_reconnectTimer->isActive()) {
        m_status.retryCount = 0;
        reconnect();
    }
}

bool ODBCConnectionManager::createConnection()
{
    // 移除旧连接
    if (QSqlDatabase::contains(m_connectionId)) {
        QSqlDatabase::removeDatabase(m_connectionId);
    }
    
    // 创建新连接
    m_database = QSqlDatabase::addDatabase("QODBC", m_connectionId);
    
    if (!m_database.isValid()) {
        logError("createConnection", "Failed to create QODBC database connection");
        return false;
    }
    
    logInfo("Database connection created with ID: " + m_connectionId);
    return true;
}

void ODBCConnectionManager::setupConnection()
{
    QString connectionString = formatConnectionString(m_config);
    
    m_database.setDatabaseName(connectionString);
    m_database.setConnectOptions(QString("SQL_ATTR_CONNECTION_TIMEOUT=%1;SQL_ATTR_LOGIN_TIMEOUT=%2")
                                 .arg(m_config.connectionTimeout)
                                 .arg(m_config.connectionTimeout));
    
    logInfo("Connection setup completed");
    logInfo("Connection string: " + connectionString);
}

void ODBCConnectionManager::logError(const QString &operation, const QString &error)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage = QString("[%1] ERROR in %2: %3").arg(timestamp, operation, error);
    
    qCritical() << logMessage;
    
    QMutexLocker locker(&m_mutex);
    m_errorHistory.append(logMessage);
    if (m_errorHistory.size() > MAX_ERROR_HISTORY) {
        m_errorHistory.removeFirst();
    }
    
    emit errorOccurred(operation, error);
}

void ODBCConnectionManager::logInfo(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage = QString("[%1] INFO: %2").arg(timestamp, message);
    
    qDebug() << logMessage;
}

void ODBCConnectionManager::updateConnectionStatus(bool connected, const QString &error)
{
    m_status.isConnected = connected;
    m_status.lastError = error;
    
    if (connected) {
        m_status.lastConnectTime = QDateTime::currentDateTime();
        m_status.retryCount = 0;
    } else {
        m_status.lastErrorTime = QDateTime::currentDateTime();
    }
}

QString ODBCConnectionManager::formatConnectionString(const ConnectionConfig &config) const
{
    QString connectionString;
    
    if (!config.dsnName.isEmpty()) {
        // 使用DSN连接
        connectionString = QString("DSN=%1").arg(config.dsnName);
        
        if (!config.integratedSecurity) {
            connectionString += QString(";UID=%1;PWD=%2")
                              .arg(config.username)
                              .arg(config.password);
        }
    } else {
        // 使用直接连接字符串
        connectionString = QString(
            "DRIVER={%1};"
            "SERVER=%2,%3;"
            "DATABASE=%4;"
            "Trusted_Connection=%5;"
            "TrustServerCertificate=%6;"
            "Encrypt=Optional;"
            "MARS_Connection=Yes;"
            "APP=RANOnline AI System"
        ).arg(config.driverName)
         .arg(config.serverAddress)
         .arg(config.port)
         .arg(config.databaseName)
         .arg(config.integratedSecurity ? "Yes" : "No")
         .arg(config.trustServerCertificate ? "Yes" : "No");
         
        if (!config.integratedSecurity) {
            connectionString += QString(";UID=%1;PWD=%2")
                              .arg(config.username)
                              .arg(config.password);
        }
    }
    
    return connectionString;
}

bool ODBCConnectionManager::testConnection(const QString &connectionString)
{
    QSqlDatabase testDb = QSqlDatabase::addDatabase("QODBC", "test_connection");
    testDb.setDatabaseName(connectionString);
    
    bool success = testDb.open();
    testDb.close();
    QSqlDatabase::removeDatabase("test_connection");
    
    return success;
}

void ODBCConnectionManager::recordQueryMetrics(bool success, double executionTime)
{
    m_status.totalQueries++;
    
    if (success) {
        m_status.successfulQueries++;
    } else {
        m_status.failedQueries++;
    }
    
    m_totalQueryTime += executionTime;
    m_status.averageQueryTime = m_totalQueryTime / m_status.totalQueries;
}

// ============================================================================
// ODBCConnectionFactory Implementation
// ============================================================================

ODBCConnectionFactory& ODBCConnectionFactory::instance()
{
    static ODBCConnectionFactory instance;
    return instance;
}

std::shared_ptr<ODBCConnectionManager> ODBCConnectionFactory::createConnection(const QString &name)
{
    QMutexLocker locker(&m_mutex);
    
    auto connection = std::make_shared<ODBCConnectionManager>();
    m_connections[name] = connection;
    
    return connection;
}

std::shared_ptr<ODBCConnectionManager> ODBCConnectionFactory::getConnection(const QString &name)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_connections.contains(name)) {
        return m_connections[name];
    }
    
    return createConnection(name);
}

void ODBCConnectionFactory::removeConnection(const QString &name)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_connections.contains(name)) {
        m_connections[name]->disconnectFromDatabase();
        m_connections.remove(name);
    }
}

QStringList ODBCConnectionFactory::getConnectionNames() const
{
    QMutexLocker locker(&m_mutex);
    return m_connections.keys();
}

bool ODBCConnectionFactory::createSystemDSN(const ODBCConnectionManager::ConnectionConfig &config)
{
    auto tempConnection = std::make_unique<ODBCConnectionManager>();
    return tempConnection->createSystemDSN(config);
}

bool ODBCConnectionFactory::removeSystemDSN(const QString &dsnName)
{
    auto tempConnection = std::make_unique<ODBCConnectionManager>();
    return tempConnection->removeSystemDSN(dsnName);
}

QStringList ODBCConnectionFactory::getSystemDSNList() const
{
    QStringList dsnList;
    
#ifdef Q_OS_WIN
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources", 
                     0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        DWORD index = 0;
        CHAR dsnName[256];
        DWORD dsnNameSize = sizeof(dsnName);
        
        while (RegEnumValueA(hKey, index, dsnName, &dsnNameSize, 
                           NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            dsnList << QString::fromLocal8Bit(dsnName);
            dsnNameSize = sizeof(dsnName);
            index++;
        }
        
        RegCloseKey(hKey);
    }
#endif
    
    return dsnList;
}

#include "ODBCConnectionManager.moc"
