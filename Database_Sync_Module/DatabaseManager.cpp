/**
 * @file DatabaseManager.cpp
 * @brief RANOnline EP7 AI系统 - 数据库管理器实现文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "DatabaseManager.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtConcurrent/QtConcurrent>

/**
 * @brief 构造函数
 */
DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
    , m_connected(false)
    , m_poolSize(10)
    , m_connectionTimeout(30000)
    , m_queryTimeout(10000)
    , m_syncTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_threadPool(new QThreadPool(this))
{
    // 配置定时器
    m_syncTimer->setInterval(5000);     // 5秒同步一次
    m_heartbeatTimer->setInterval(60000); // 60秒心跳一次
    
    // 配置线程池
    m_threadPool->setMaxThreadCount(qMax(2, QThread::idealThreadCount()));
    
    // 连接信号槽
    connect(m_syncTimer, &QTimer::timeout, this, &DatabaseManager::performSync);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &DatabaseManager::sendHeartbeat);
    
    qDebug() << "DatabaseManager: 初始化完成";
}

/**
 * @brief 析构函数
 */
DatabaseManager::~DatabaseManager()
{
    shutdown();
}

/**
 * @brief 初始化数据库管理器
 */
bool DatabaseManager::initialize(const DatabaseConfig& config)
{
    qDebug() << "DatabaseManager: 开始初始化...";
    
    m_config = config;
    
    // 验证配置
    if (!validateConfiguration()) {
        qCritical() << "DatabaseManager: 配置验证失败";
        return false;
    }
    
    // 创建连接池
    if (!createConnectionPool()) {
        qCritical() << "DatabaseManager: 连接池创建失败";
        return false;
    }
    
    // 初始化数据库结构
    if (!initializeDatabaseSchema()) {
        qCritical() << "DatabaseManager: 数据库结构初始化失败";
        return false;
    }
    
    m_initialized = true;
    m_connected = true;
    
    // 启动定时器
    m_syncTimer->start();
    m_heartbeatTimer->start();
    
    emit connectionStateChanged(true);
    
    qDebug() << "DatabaseManager: 初始化成功";
    return true;
}

/**
 * @brief 关闭数据库管理器
 */
void DatabaseManager::shutdown()
{
    qDebug() << "DatabaseManager: 开始关闭...";
    
    // 停止定时器
    m_syncTimer->stop();
    m_heartbeatTimer->stop();
    
    // 等待所有查询完成
    m_threadPool->waitForDone(10000);
    
    // 关闭连接池
    closeConnectionPool();
    
    m_initialized = false;
    m_connected = false;
    
    emit connectionStateChanged(false);
    
    qDebug() << "DatabaseManager: 关闭完成";
}

/**
 * @brief 同步AI数据到数据库
 */
bool DatabaseManager::syncAiData(const QString& aiId, const AIPlayerData& playerData)
{
    if (!m_connected) {
        qWarning() << "DatabaseManager: 未连接到数据库";
        return false;
    }
    
    QString sql = R"(
        MERGE RAN_AI_Players AS target
        USING (SELECT ? AS ai_id, ? AS name, ? AS school, ? AS level, 
                      ? AS server_id, ? AS aggression, ? AS intelligence, 
                      ? AS social, ? AS anti_lag, GETDATE() AS last_update) AS source
        ON target.ai_id = source.ai_id
        WHEN MATCHED THEN
            UPDATE SET name = source.name, school = source.school, level = source.level,
                      server_id = source.server_id, aggression = source.aggression,
                      intelligence = source.intelligence, social = source.social,
                      anti_lag = source.anti_lag, last_update = source.last_update
        WHEN NOT MATCHED THEN
            INSERT (ai_id, name, school, level, server_id, aggression, intelligence, 
                   social, anti_lag, created_time, last_update)
            VALUES (source.ai_id, source.name, source.school, source.level,
                   source.server_id, source.aggression, source.intelligence,
                   source.social, source.anti_lag, GETDATE(), source.last_update);
    )";
    
    return executeQuery(sql, {
        aiId,
        playerData.name,
        static_cast<int>(playerData.school),
        playerData.level,
        playerData.serverId,
        playerData.aggression,
        playerData.intelligence,
        playerData.social,
        playerData.antiLag
    });
}

/**
 * @brief 更新AI状态
 */
bool DatabaseManager::updateAiStatus(const QString& aiId, AIStatus status)
{
    if (!m_connected) {
        qWarning() << "DatabaseManager: 未连接到数据库";
        return false;
    }
    
    QString sql = R"(
        UPDATE RAN_AI_Players 
        SET status = ?, last_update = GETDATE()
        WHERE ai_id = ?
    )";
    
    return executeQuery(sql, {
        static_cast<int>(status),
        aiId
    });
}

/**
 * @brief 删除AI数据
 */
bool DatabaseManager::deleteAiData(const QString& aiId)
{
    if (!m_connected) {
        qWarning() << "DatabaseManager: 未连接到数据库";
        return false;
    }
    
    QString sql = "DELETE FROM RAN_AI_Players WHERE ai_id = ?";
    
    return executeQuery(sql, { aiId });
}

/**
 * @brief 获取AI列表
 */
QVector<AIPlayerData> DatabaseManager::getAiList(int serverId)
{
    QVector<AIPlayerData> result;
    
    if (!m_connected) {
        qWarning() << "DatabaseManager: 未连接到数据库";
        return result;
    }
    
    QString sql = R"(
        SELECT ai_id, name, school, level, server_id, aggression, 
               intelligence, social, anti_lag, status
        FROM RAN_AI_Players
    )";
    
    QStringList params;
    if (serverId > 0) {
        sql += " WHERE server_id = ?";
        params << QString::number(serverId);
    }
    
    sql += " ORDER BY created_time DESC";
    
    auto connection = getConnection();
    if (!connection) {
        return result;
    }
    
    QSqlQuery query(*connection);
    query.prepare(sql);
    
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    
    if (query.exec()) {
        while (query.next()) {
            AIPlayerData playerData;
            playerData.name = query.value("name").toString();
            playerData.school = static_cast<School>(query.value("school").toInt());
            playerData.level = query.value("level").toInt();
            playerData.serverId = query.value("server_id").toInt();
            playerData.aggression = query.value("aggression").toInt();
            playerData.intelligence = query.value("intelligence").toInt();
            playerData.social = query.value("social").toInt();
            playerData.antiLag = query.value("anti_lag").toBool();
            
            result.append(playerData);
        }
    } else {
        qCritical() << "DatabaseManager: 查询AI列表失败:" << query.lastError().text();
    }
    
    releaseConnection(connection);
    return result;
}

/**
 * @brief 获取服务器统计信息
 */
QMap<int, ServerStats> DatabaseManager::getServerStats()
{
    QMap<int, ServerStats> result;
    
    if (!m_connected) {
        qWarning() << "DatabaseManager: 未连接到数据库";
        return result;
    }
    
    QString sql = R"(
        SELECT server_id, 
               COUNT(*) as total_count,
               COUNT(CASE WHEN status = 1 THEN 1 END) as online_count,
               AVG(CAST(level AS FLOAT)) as avg_level
        FROM RAN_AI_Players
        GROUP BY server_id
    )";
    
    auto connection = getConnection();
    if (!connection) {
        return result;
    }
    
    QSqlQuery query(*connection);
    if (query.exec(sql)) {
        while (query.next()) {
            int serverId = query.value("server_id").toInt();
            
            ServerStats stats;
            stats.totalCount = query.value("total_count").toInt();
            stats.onlineCount = query.value("online_count").toInt();
            stats.averageLevel = query.value("avg_level").toDouble();
            stats.loadPercentage = stats.totalCount > 0 ? 
                (static_cast<double>(stats.onlineCount) / stats.totalCount * 100) : 0.0;
            
            result[serverId] = stats;
        }
    } else {
        qCritical() << "DatabaseManager: 查询服务器统计失败:" << query.lastError().text();
    }
    
    releaseConnection(connection);
    return result;
}

/**
 * @brief 记录AI操作日志
 */
bool DatabaseManager::logAiOperation(const QString& aiId, const QString& operation, 
                                    const QString& details)
{
    if (!m_connected) {
        return false;
    }
    
    QString sql = R"(
        INSERT INTO RAN_AI_Logs (ai_id, operation, details, timestamp)
        VALUES (?, ?, ?, GETDATE())
    )";
    
    return executeQuery(sql, { aiId, operation, details });
}

/**
 * @brief 清理过期数据
 */
bool DatabaseManager::cleanupExpiredData(int daysToKeep)
{
    if (!m_connected) {
        return false;
    }
    
    // 清理过期日志
    QString logSql = R"(
        DELETE FROM RAN_AI_Logs 
        WHERE timestamp < DATEADD(day, -?, GETDATE())
    )";
    
    // 清理长时间离线的AI
    QString aiSql = R"(
        DELETE FROM RAN_AI_Players 
        WHERE status = 0 AND last_update < DATEADD(day, -?, GETDATE())
    )";
    
    bool logCleanup = executeQuery(logSql, { daysToKeep });
    bool aiCleanup = executeQuery(aiSql, { daysToKeep * 2 }); // AI数据保留更长时间
    
    return logCleanup && aiCleanup;
}

/**
 * @brief 批量同步AI数据
 */
bool DatabaseManager::batchSyncAiData(const QMap<QString, AIPlayerData>& aiDataMap)
{
    if (!m_connected || aiDataMap.isEmpty()) {
        return false;
    }
    
    auto connection = getConnection();
    if (!connection) {
        return false;
    }
    
    QSqlDatabase db = *connection;
    
    // 开始事务
    if (!db.transaction()) {
        qCritical() << "DatabaseManager: 开始事务失败";
        releaseConnection(connection);
        return false;
    }
    
    bool success = true;
    QString sql = R"(
        MERGE RAN_AI_Players AS target
        USING (SELECT ? AS ai_id, ? AS name, ? AS school, ? AS level, 
                      ? AS server_id, ? AS aggression, ? AS intelligence, 
                      ? AS social, ? AS anti_lag, GETDATE() AS last_update) AS source
        ON target.ai_id = source.ai_id
        WHEN MATCHED THEN
            UPDATE SET name = source.name, school = source.school, level = source.level,
                      server_id = source.server_id, aggression = source.aggression,
                      intelligence = source.intelligence, social = source.social,
                      anti_lag = source.anti_lag, last_update = source.last_update
        WHEN NOT MATCHED THEN
            INSERT (ai_id, name, school, level, server_id, aggression, intelligence, 
                   social, anti_lag, created_time, last_update)
            VALUES (source.ai_id, source.name, source.school, source.level,
                   source.server_id, source.aggression, source.intelligence,
                   source.social, source.anti_lag, GETDATE(), source.last_update);
    )";
    
    QSqlQuery query(db);
    query.prepare(sql);
    
    for (auto it = aiDataMap.constBegin(); it != aiDataMap.constEnd(); ++it) {
        const QString& aiId = it.key();
        const AIPlayerData& playerData = it.value();
        
        query.bindValue(0, aiId);
        query.bindValue(1, playerData.name);
        query.bindValue(2, static_cast<int>(playerData.school));
        query.bindValue(3, playerData.level);
        query.bindValue(4, playerData.serverId);
        query.bindValue(5, playerData.aggression);
        query.bindValue(6, playerData.intelligence);
        query.bindValue(7, playerData.social);
        query.bindValue(8, playerData.antiLag);
        
        if (!query.exec()) {
            qCritical() << "DatabaseManager: 批量同步AI数据失败:" << query.lastError().text();
            success = false;
            break;
        }
    }
    
    // 提交或回滚事务
    if (success) {
        if (!db.commit()) {
            qCritical() << "DatabaseManager: 提交事务失败";
            success = false;
        }
    } else {
        db.rollback();
    }
    
    releaseConnection(connection);
    
    if (success) {
        emit dataSynced(aiDataMap.size());
    }
    
    return success;
}

/**
 * @brief 获取数据库连接
 */
std::shared_ptr<QSqlDatabase> DatabaseManager::getConnection()
{
    QMutexLocker locker(&m_poolMutex);
    
    if (!m_connectionPool.isEmpty()) {
        return m_connectionPool.dequeue();
    }
    
    // 如果池为空，创建新连接
    auto connection = createNewConnection();
    if (connection && connection->isOpen()) {
        return connection;
    }
    
    return nullptr;
}

/**
 * @brief 释放数据库连接
 */
void DatabaseManager::releaseConnection(std::shared_ptr<QSqlDatabase> connection)
{
    if (!connection) {
        return;
    }
    
    QMutexLocker locker(&m_poolMutex);
    
    // 检查连接是否有效
    if (connection->isOpen()) {
        m_connectionPool.enqueue(connection);
    }
}

/**
 * @brief 执行查询
 */
bool DatabaseManager::executeQuery(const QString& sql, const QVariantList& params)
{
    auto connection = getConnection();
    if (!connection) {
        return false;
    }
    
    QSqlQuery query(*connection);
    query.prepare(sql);
    
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    
    bool success = query.exec();
    if (!success) {
        qCritical() << "DatabaseManager: 查询执行失败:" << query.lastError().text();
        qCritical() << "SQL:" << sql;
    }
    
    releaseConnection(connection);
    return success;
}

/**
 * @brief 验证配置
 */
bool DatabaseManager::validateConfiguration()
{
    if (m_config.serverName.isEmpty()) {
        qCritical() << "DatabaseManager: 服务器名称不能为空";
        return false;
    }
    
    if (m_config.databaseName.isEmpty()) {
        qCritical() << "DatabaseManager: 数据库名称不能为空";
        return false;
    }
    
    if (m_config.poolSize <= 0 || m_config.poolSize > 50) {
        qWarning() << "DatabaseManager: 连接池大小无效，使用默认值";
        m_config.poolSize = 10;
    }
    
    m_poolSize = m_config.poolSize;
    m_connectionTimeout = m_config.connectionTimeout;
    m_queryTimeout = m_config.queryTimeout;
    
    return true;
}

/**
 * @brief 创建连接池
 */
bool DatabaseManager::createConnectionPool()
{
    qDebug() << "DatabaseManager: 创建连接池，大小:" << m_poolSize;
    
    for (int i = 0; i < m_poolSize; ++i) {
        auto connection = createNewConnection();
        if (!connection || !connection->isOpen()) {
            qCritical() << "DatabaseManager: 创建连接失败，索引:" << i;
            return false;
        }
        
        m_connectionPool.enqueue(connection);
    }
    
    qDebug() << "DatabaseManager: 连接池创建成功";
    return true;
}

/**
 * @brief 创建新连接
 */
std::shared_ptr<QSqlDatabase> DatabaseManager::createNewConnection()
{
    QString connectionName = QString("RAN_AI_DB_%1").arg(QDateTime::currentMSecsSinceEpoch());
    
    auto db = std::make_shared<QSqlDatabase>(QSqlDatabase::addDatabase("QODBC", connectionName));
    
    // 构建连接字符串
    QString connectionString = QString(
        "DRIVER={ODBC Driver 17 for SQL Server};"
        "SERVER=%1;"
        "DATABASE=%2;"
        "Trusted_Connection=%3;"
    ).arg(m_config.serverName)
     .arg(m_config.databaseName)
     .arg(m_config.trustedConnection ? "yes" : "no");
    
    if (!m_config.trustedConnection) {
        connectionString += QString("UID=%1;PWD=%2;").arg(m_config.username, m_config.password);
    }
    
    // 添加连接选项
    connectionString += "Connection Timeout=30;Login Timeout=10;";
    
    db->setDatabaseName(connectionString);
    
    if (!db->open()) {
        qCritical() << "DatabaseManager: 数据库连接失败:" << db->lastError().text();
        QSqlDatabase::removeDatabase(connectionName);
        return nullptr;
    }
    
    qDebug() << "DatabaseManager: 数据库连接成功:" << connectionName;
    return db;
}

/**
 * @brief 关闭连接池
 */
void DatabaseManager::closeConnectionPool()
{
    QMutexLocker locker(&m_poolMutex);
    
    while (!m_connectionPool.isEmpty()) {
        auto connection = m_connectionPool.dequeue();
        if (connection) {
            QString connectionName = connection->connectionName();
            connection->close();
            QSqlDatabase::removeDatabase(connectionName);
        }
    }
    
    qDebug() << "DatabaseManager: 连接池已关闭";
}

/**
 * @brief 初始化数据库结构
 */
bool DatabaseManager::initializeDatabaseSchema()
{
    qDebug() << "DatabaseManager: 初始化数据库结构...";
    
    QStringList createTableSqls = {
        // AI玩家表
        R"(
            IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='RAN_AI_Players' AND xtype='U')
            CREATE TABLE RAN_AI_Players (
                id BIGINT IDENTITY(1,1) PRIMARY KEY,
                ai_id NVARCHAR(50) UNIQUE NOT NULL,
                name NVARCHAR(100) NOT NULL,
                school INT NOT NULL,
                level INT NOT NULL DEFAULT 1,
                server_id INT NOT NULL DEFAULT 1,
                aggression INT NOT NULL DEFAULT 50,
                intelligence INT NOT NULL DEFAULT 50,
                social INT NOT NULL DEFAULT 50,
                anti_lag BIT NOT NULL DEFAULT 0,
                status INT NOT NULL DEFAULT 0,
                created_time DATETIME NOT NULL DEFAULT GETDATE(),
                last_update DATETIME NOT NULL DEFAULT GETDATE(),
                INDEX IX_AI_Players_ServerId (server_id),
                INDEX IX_AI_Players_School (school),
                INDEX IX_AI_Players_Status (status)
            )
        )",
        
        // AI操作日志表
        R"(
            IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='RAN_AI_Logs' AND xtype='U')
            CREATE TABLE RAN_AI_Logs (
                id BIGINT IDENTITY(1,1) PRIMARY KEY,
                ai_id NVARCHAR(50) NOT NULL,
                operation NVARCHAR(100) NOT NULL,
                details NVARCHAR(MAX),
                timestamp DATETIME NOT NULL DEFAULT GETDATE(),
                INDEX IX_AI_Logs_AiId (ai_id),
                INDEX IX_AI_Logs_Timestamp (timestamp)
            )
        )",
        
        // 服务器状态表
        R"(
            IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='RAN_Server_Status' AND xtype='U')
            CREATE TABLE RAN_Server_Status (
                server_id INT PRIMARY KEY,
                server_name NVARCHAR(100) NOT NULL,
                ai_count INT NOT NULL DEFAULT 0,
                online_count INT NOT NULL DEFAULT 0,
                load_percentage FLOAT NOT NULL DEFAULT 0.0,
                last_update DATETIME NOT NULL DEFAULT GETDATE()
            )
        )"
    };
    
    for (const QString& sql : createTableSqls) {
        if (!executeQuery(sql, {})) {
            qCritical() << "DatabaseManager: 创建表失败";
            return false;
        }
    }
    
    qDebug() << "DatabaseManager: 数据库结构初始化完成";
    return true;
}

/**
 * @brief 执行同步
 */
void DatabaseManager::performSync()
{
    if (!m_connected) {
        return;
    }
    
    // 这里可以实现定期同步逻辑
    // 例如：同步统计数据、清理过期数据等
    
    emit syncCompleted();
}

/**
 * @brief 发送心跳
 */
void DatabaseManager::sendHeartbeat()
{
    if (!m_connected) {
        return;
    }
    
    // 简单的心跳查询
    QString sql = "SELECT 1";
    bool success = executeQuery(sql, {});
    
    if (!success) {
        qWarning() << "DatabaseManager: 心跳检测失败，可能已断开连接";
        m_connected = false;
        emit connectionStateChanged(false);
    }
}
