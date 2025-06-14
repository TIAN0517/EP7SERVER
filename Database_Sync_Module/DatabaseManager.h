/**
 * @file DatabaseManager.h
 * @brief RANOnline EP7 AI系统 - 数据库管理器头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 * 
 * 功能特色:
 * - SQL Server 2022 完整支持
 * - ODBC/ADO 双重连接选项
 * - 连接池管理
 * - 事务安全保障
 * - 异步数据同步
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

// Windows平台ODBC支持
#ifdef _WIN32
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#endif

// 前置声明
struct AIPlayerData;

/**
 * @struct DatabaseConnection
 * @brief 数据库连接结构
 */
struct DatabaseConnection {
    SQLHENV henv;       ///< 环境句柄
    SQLHDBC hdbc;       ///< 连接句柄
    SQLHSTMT hstmt;     ///< 语句句柄
    bool isConnected;   ///< 连接状态
    std::chrono::steady_clock::time_point lastUsed; ///< 最后使用时间
    std::string connectionId; ///< 连接标识
    
    DatabaseConnection() : henv(SQL_NULL_HENV), hdbc(SQL_NULL_HDBC), 
                          hstmt(SQL_NULL_HSTMT), isConnected(false) {
        lastUsed = std::chrono::steady_clock::now();
    }
};

/**
 * @struct QueryResult
 * @brief 查询结果结构
 */
struct QueryResult {
    bool success;                                    ///< 查询是否成功
    std::string errorMessage;                        ///< 错误信息
    std::vector<std::unordered_map<std::string, std::string>> rows; ///< 结果行
    int affectedRows;                               ///< 影响的行数
    
    QueryResult() : success(false), affectedRows(0) {}
};

/**
 * @struct DatabaseOperation
 * @brief 数据库操作结构
 */
struct DatabaseOperation {
    enum class Type {
        Insert,         ///< 插入操作
        Update,         ///< 更新操作
        Delete,         ///< 删除操作
        Select,         ///< 查询操作
        StoredProc      ///< 存储过程调用
    } type;
    
    std::string sql;                                ///< SQL语句
    std::vector<std::pair<std::string, std::string>> parameters; ///< 参数列表
    int priority;                                   ///< 优先级 (1-10)
    std::chrono::steady_clock::time_point timestamp; ///< 时间戳
    std::function<void(const QueryResult&)> callback; ///< 回调函数
    
    DatabaseOperation(Type t, const std::string& sqlText, int prio = 5) 
        : type(t), sql(sqlText), priority(prio) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * @class DatabaseManager
 * @brief 数据库管理器核心类
 * 
 * 核心功能:
 * 1. SQL Server 2022 连接管理
 * 2. 连接池优化
 * 3. 异步查询处理
 * 4. 事务管理
 * 5. AI数据同步
 * 6. 性能监控
 */
class DatabaseManager {
public:
    /**
     * @brief 构造函数
     */
    DatabaseManager();
    
    /**
     * @brief 析构函数
     */
    ~DatabaseManager();
    
    /**
     * @brief 初始化数据库管理器
     * @param connectionString 连接字符串
     * @param poolSize 连接池大小
     * @return 是否成功初始化
     */
    bool initialize(const std::string& connectionString, int poolSize = 10);
    
    /**
     * @brief 启动数据库管理器
     * @return 是否成功启动
     */
    bool start();
    
    /**
     * @brief 停止数据库管理器
     */
    void stop();
    
    /**
     * @brief 检查是否运行中
     * @return 运行状态
     */
    bool isRunning() const { return m_isRunning.load(); }
    
    // 连接管理接口
    /**
     * @brief 测试数据库连接
     * @return 是否连接成功
     */
    bool testConnection();
    
    /**
     * @brief 获取连接池状态
     * @return 状态信息映射
     */
    std::unordered_map<std::string, int> getConnectionPoolStatus() const;
    
    // 同步查询接口
    /**
     * @brief 执行查询
     * @param sql SQL语句
     * @param parameters 参数列表
     * @return 查询结果
     */
    QueryResult executeQuery(const std::string& sql, 
                           const std::vector<std::pair<std::string, std::string>>& parameters = {});
    
    /**
     * @brief 执行非查询语句
     * @param sql SQL语句
     * @param parameters 参数列表
     * @return 影响的行数，失败返回-1
     */
    int executeNonQuery(const std::string& sql, 
                       const std::vector<std::pair<std::string, std::string>>& parameters = {});
    
    /**
     * @brief 执行存储过程
     * @param procName 存储过程名称
     * @param parameters 参数列表
     * @return 查询结果
     */
    QueryResult executeStoredProcedure(const std::string& procName,
                                      const std::vector<std::pair<std::string, std::string>>& parameters = {});
    
    // 异步查询接口
    /**
     * @brief 异步执行查询
     * @param operation 数据库操作
     * @return 是否成功加入队列
     */
    bool executeAsync(const DatabaseOperation& operation);
    
    /**
     * @brief 批量异步执行
     * @param operations 操作列表
     * @return 成功加入队列的操作数量
     */
    int executeBatchAsync(const std::vector<DatabaseOperation>& operations);
    
    // 事务管理接口
    /**
     * @brief 开始事务
     * @return 事务ID
     */
    std::string beginTransaction();
    
    /**
     * @brief 提交事务
     * @param transactionId 事务ID
     * @return 是否成功提交
     */
    bool commitTransaction(const std::string& transactionId);
    
    /**
     * @brief 回滚事务
     * @param transactionId 事务ID
     * @return 是否成功回滚
     */
    bool rollbackTransaction(const std::string& transactionId);
    
    // AI数据同步接口
    /**
     * @brief 同步AI玩家数据到数据库
     * @param aiData AI数据
     * @return 是否成功同步
     */
    bool syncAIPlayer(const AIPlayerData& aiData);
    
    /**
     * @brief 批量同步AI玩家数据
     * @param aiDataList AI数据列表
     * @return 成功同步的数量
     */
    int syncBatchAIPlayers(const std::vector<AIPlayerData>& aiDataList);
    
    /**
     * @brief 从数据库加载AI玩家数据
     * @param serverId 服务器ID，-1表示所有服务器
     * @return AI数据列表
     */
    std::vector<AIPlayerData> loadAIPlayers(int serverId = -1);
    
    /**
     * @brief 删除AI玩家数据
     * @param aiId AI ID
     * @return 是否成功删除
     */
    bool deleteAIPlayer(const std::string& aiId);
    
    /**
     * @brief 更新服务器负载统计
     * @param serverId 服务器ID
     * @param cpuUsage CPU使用率
     * @param memoryUsage 内存使用率
     * @param activeConnections 活跃连接数
     * @return 是否成功更新
     */
    bool updateServerLoadStats(int serverId, float cpuUsage, float memoryUsage, int activeConnections);
    
    // 性能监控接口
    /**
     * @brief 获取数据库性能统计
     * @return 性能数据映射
     */
    std::unordered_map<std::string, double> getPerformanceStats() const;
    
    /**
     * @brief 获取查询历史
     * @param limit 限制数量
     * @return 查询历史列表
     */
    std::vector<std::string> getQueryHistory(int limit = 100) const;
    
    /**
     * @brief 清理性能统计
     */
    void clearPerformanceStats();

private:
    /**
     * @brief 创建数据库连接
     * @return 连接指针，失败返回nullptr
     */
    std::shared_ptr<DatabaseConnection> createConnection();
    
    /**
     * @brief 从连接池获取连接
     * @return 连接指针，失败返回nullptr
     */
    std::shared_ptr<DatabaseConnection> getConnection();
    
    /**
     * @brief 返回连接到连接池
     * @param connection 连接指针
     */
    void returnConnection(std::shared_ptr<DatabaseConnection> connection);
    
    /**
     * @brief 关闭连接
     * @param connection 连接指针
     */
    void closeConnection(std::shared_ptr<DatabaseConnection> connection);
    
    /**
     * @brief 异步处理循环
     */
    void asyncProcessingLoop();
    
    /**
     * @brief 连接池维护循环
     */
    void connectionPoolMaintenanceLoop();
    
    /**
     * @brief 性能监控循环
     */
    void performanceMonitorLoop();
    
    /**
     * @brief 处理数据库操作
     * @param operation 数据库操作
     */
    void processOperation(const DatabaseOperation& operation);
    
    /**
     * @brief 检查连接有效性
     * @param connection 连接指针
     * @return 是否有效
     */
    bool isConnectionValid(std::shared_ptr<DatabaseConnection> connection);
    
    /**
     * @brief 准备SQL语句
     * @param connection 连接指针
     * @param sql SQL语句
     * @param parameters 参数列表
     * @return 是否成功准备
     */
    bool prepareStatement(std::shared_ptr<DatabaseConnection> connection,
                         const std::string& sql,
                         const std::vector<std::pair<std::string, std::string>>& parameters);
    
    /**
     * @brief 执行准备好的语句
     * @param connection 连接指针
     * @return 查询结果
     */
    QueryResult executeStatement(std::shared_ptr<DatabaseConnection> connection);
    
    /**
     * @brief 处理ODBC错误
     * @param handleType 句柄类型
     * @param handle 句柄
     * @return 错误信息
     */
    std::string getODBCError(SQLSMALLINT handleType, SQLHANDLE handle);

private:
    // 连接配置
    std::string m_connectionString;                    ///< 连接字符串
    int m_poolSize;                                    ///< 连接池大小
    
    // 连接池管理
    std::queue<std::shared_ptr<DatabaseConnection>> m_connectionPool;
    std::mutex m_poolMutex;                           ///< 连接池互斥锁
    std::condition_variable m_poolCondition;          ///< 连接池条件变量
    std::atomic<int> m_activeConnections;             ///< 活跃连接数
    std::atomic<int> m_totalConnections;              ///< 总连接数
    
    // 操作队列
    std::priority_queue<DatabaseOperation, std::vector<DatabaseOperation>,
                       std::function<bool(const DatabaseOperation&, const DatabaseOperation&)>> m_operationQueue;
    std::mutex m_queueMutex;                          ///< 操作队列互斥锁
    std::condition_variable m_queueCondition;         ///< 队列条件变量
    
    // 线程管理
    std::vector<std::thread> m_workerThreads;         ///< 工作线程组
    std::atomic<bool> m_isRunning;                    ///< 运行状态标志
    std::atomic<bool> m_shouldStop;                   ///< 停止标志
    
    // 事务管理
    std::unordered_map<std::string, std::shared_ptr<DatabaseConnection>> m_transactions;
    std::mutex m_transactionMutex;                    ///< 事务互斥锁
    
    // 性能统计
    mutable std::mutex m_statsMutex;                  ///< 统计互斥锁
    std::unordered_map<std::string, double> m_performanceStats; ///< 性能统计
    std::vector<std::string> m_queryHistory;          ///< 查询历史
    std::chrono::steady_clock::time_point m_lastStatsUpdate; ///< 上次统计更新时间
    
    // 常量定义
    static constexpr int DEFAULT_POOL_SIZE = 10;             ///< 默认连接池大小
    static constexpr int MAX_POOL_SIZE = 50;                 ///< 最大连接池大小
    static constexpr int CONNECTION_TIMEOUT = 30;            ///< 连接超时时间（秒）
    static constexpr int QUERY_TIMEOUT = 60;                 ///< 查询超时时间（秒）
    static constexpr int MAINTENANCE_INTERVAL = 300;         ///< 维护间隔（秒）
    static constexpr int MAX_QUERY_HISTORY = 1000;           ///< 最大查询历史数量
};

/**
 * @class SqlServerConnector
 * @brief SQL Server专用连接器
 */
class SqlServerConnector {
public:
    /**
     * @brief 构建SQL Server连接字符串
     * @param server 服务器地址
     * @param database 数据库名称
     * @param username 用户名（可选，使用Windows身份验证时为空）
     * @param password 密码（可选）
     * @param useWindowsAuth 是否使用Windows身份验证
     * @return 连接字符串
     */
    static std::string buildConnectionString(const std::string& server,
                                            const std::string& database,
                                            const std::string& username = "",
                                            const std::string& password = "",
                                            bool useWindowsAuth = true);
    
    /**
     * @brief 测试SQL Server连接
     * @param connectionString 连接字符串
     * @return 测试结果信息
     */
    static std::pair<bool, std::string> testConnection(const std::string& connectionString);
    
    /**
     * @brief 获取SQL Server版本信息
     * @param connectionString 连接字符串
     * @return 版本信息
     */
    static std::string getServerVersion(const std::string& connectionString);
    
    /**
     * @brief 检查数据库是否存在
     * @param connectionString 连接字符串
     * @param databaseName 数据库名称
     * @return 是否存在
     */
    static bool isDatabaseExists(const std::string& connectionString, const std::string& databaseName);
    
    /**
     * @brief 创建数据库（如果不存在）
     * @param connectionString 连接字符串
     * @param databaseName 数据库名称
     * @return 是否成功创建
     */
    static bool createDatabaseIfNotExists(const std::string& connectionString, const std::string& databaseName);
};

#endif // DATABASEMANAGER_H
