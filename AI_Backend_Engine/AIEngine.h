/**
 * @file AIEngine.h
 * @brief RANOnline EP7 AI系统 - AI推理引擎核心头文件
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 * 
 * 功能特色:
 * - 高性能多线程AI推理
 * - 智能决策算法
 * - 实时状态管理
 * - 内存优化设计
 */

#pragma once

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
#include <string>
#include <random>

// 前置声明
class PlayerSimulator;
class DecisionMaker;
class ThreadPool;
class DatabaseManager;

/**
 * @struct AIPlayerData
 * @brief AI玩家数据结构
 */
struct AIPlayerData {
    std::string id;                     ///< AI唯一标识
    std::string name;                   ///< AI角色名称
    std::string school;                 ///< 学校（聖門/鳳凰/懸岩）
    int level;                          ///< 角色等级
    int serverId;                       ///< 服务器ID (1-4)
    
    // 位置信息
    float posX, posY, posZ;             ///< 3D坐标
    float rotation;                     ///< 旋转角度
    int mapId;                          ///< 地图ID
    
    // 属性参数
    int health;                         ///< 生命值
    int mana;                           ///< 魔法值
    int experience;                     ///< 经验值
    
    // AI特性
    float aggression;                   ///< 攻击性 (0.0-1.0)
    float intelligence;                 ///< 智能度 (0.0-1.0)
    float sociability;                  ///< 社交性 (0.0-1.0)
    
    // 状态信息
    enum class Status {
        Idle,           ///< 空闲
        Moving,         ///< 移动中
        Fighting,       ///< 战斗中
        Questing,       ///< 做任务
        Chatting,       ///< 聊天中
        Offline         ///< 离线
    } status;
    
    std::chrono::steady_clock::time_point lastUpdate;  ///< 最后更新时间
    std::chrono::steady_clock::time_point createdTime; ///< 创建时间
    
    AIPlayerData() : level(1), serverId(1), posX(0), posY(0), posZ(0), 
                     rotation(0), mapId(1), health(100), mana(100), 
                     experience(0), aggression(0.5f), intelligence(0.5f), 
                     sociability(0.5f), status(Status::Idle) {
        lastUpdate = std::chrono::steady_clock::now();
        createdTime = lastUpdate;
    }
};

/**
 * @struct AICommand
 * @brief AI指令结构
 */
struct AICommand {
    enum class Type {
        Move,           ///< 移动指令
        Attack,         ///< 攻击指令
        UseSkill,       ///< 使用技能
        Chat,           ///< 聊天指令
        Quest,          ///< 任务指令
        LevelUp,        ///< 升级指令
        ChangeServer,   ///< 切换服务器
        Shutdown        ///< 关闭指令
    } type;
    
    std::string aiId;                   ///< 目标AI ID
    std::vector<std::string> parameters; ///< 指令参数
    int priority;                       ///< 优先级 (1-10)
    std::chrono::steady_clock::time_point timestamp; ///< 时间戳
    
    AICommand(Type t, const std::string& id, int prio = 5) 
        : type(t), aiId(id), priority(prio) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * @class AIEngine
 * @brief AI推理引擎核心类
 * 
 * 核心功能:
 * 1. 管理所有AI玩家实例
 * 2. 多线程AI推理和决策
 * 3. 指令队列处理
 * 4. 性能监控和优化
 * 5. 数据库同步
 */
class AIEngine {
public:
    /**
     * @brief 构造函数
     */
    AIEngine();
    
    /**
     * @brief 析构函数
     */
    ~AIEngine();
    
    /**
     * @brief 初始化AI引擎
     * @return 是否成功初始化
     */
    bool initialize();
    
    /**
     * @brief 启动AI引擎
     * @return 是否成功启动
     */
    bool start();
    
    /**
     * @brief 停止AI引擎
     */
    void stop();
    
    /**
     * @brief 检查引擎是否运行中
     * @return 运行状态
     */
    bool isRunning() const { return m_isRunning.load(); }
    
    // AI管理接口
    /**
     * @brief 创建AI玩家
     * @param school 学校类型
     * @param level 初始等级
     * @param serverId 服务器ID
     * @return AI玩家ID，失败返回空字符串
     */
    std::string createAI(const std::string& school, int level, int serverId = 1);
    
    /**
     * @brief 批量创建AI玩家
     * @param count 数量
     * @param school 学校类型
     * @param minLevel 最小等级
     * @param maxLevel 最大等级
     * @return 创建的AI ID列表
     */
    std::vector<std::string> createBatchAI(int count, const std::string& school, 
                                            int minLevel, int maxLevel);
    
    /**
     * @brief 删除AI玩家
     * @param aiId AI ID
     * @return 是否成功删除
     */
    bool removeAI(const std::string& aiId);
    
    /**
     * @brief 获取AI玩家数据
     * @param aiId AI ID
     * @return AI数据指针，失败返回nullptr
     */
    std::shared_ptr<AIPlayerData> getAI(const std::string& aiId);
    
    /**
     * @brief 获取所有AI列表
     * @return AI数据向量
     */
    std::vector<std::shared_ptr<AIPlayerData>> getAllAI();
    
    /**
     * @brief 获取指定服务器的AI列表
     * @param serverId 服务器ID
     * @return AI数据向量
     */
    std::vector<std::shared_ptr<AIPlayerData>> getAIByServer(int serverId);
    
    // 指令处理接口
    /**
     * @brief 发送指令给AI
     * @param command AI指令
     * @return 是否成功加入队列
     */
    bool sendCommand(const AICommand& command);
    
    /**
     * @brief 批量发送指令
     * @param commands 指令列表
     * @return 成功发送的指令数量
     */
    int sendBatchCommands(const std::vector<AICommand>& commands);
    
    // 状态查询接口
    /**
     * @brief 获取活跃AI数量
     * @return AI数量
     */
    int getActiveAICount() const;
    
    /**
     * @brief 获取总AI数量
     * @return AI数量
     */
    int getTotalAICount() const;
    
    /**
     * @brief 获取指定服务器负载
     * @param serverId 服务器ID
     * @return 负载百分比 (0-100)
     */
    int getServerLoad(int serverId) const;
    
    /**
     * @brief 获取引擎性能统计
     * @return 性能数据映射
     */
    std::unordered_map<std::string, double> getPerformanceStats() const;
    
    // 配置接口
    /**
     * @brief 设置AI更新频率
     * @param frequency 频率（毫秒）
     */
    void setUpdateFrequency(int frequency);
    
    /**
     * @brief 设置最大AI数量
     * @param maxCount 最大数量
     */
    void setMaxAICount(int maxCount);
    
    /**
     * @brief 启用/禁用自动负载均衡
     * @param enabled 是否启用
     */
    void setAutoLoadBalancing(bool enabled);

private:
    /**
     * @brief 主更新循环
     */
    void updateLoop();
    
    /**
     * @brief 指令处理循环
     */
    void commandProcessingLoop();
    
    /**
     * @brief 数据库同步循环
     */
    void databaseSyncLoop();
    
    /**
     * @brief 性能监控循环
     */
    void performanceMonitorLoop();
    
    /**
     * @brief 更新单个AI
     * @param ai AI数据指针
     */
    void updateAI(std::shared_ptr<AIPlayerData> ai);
    
    /**
     * @brief 处理单个指令
     * @param command 指令引用
     */
    void processCommand(const AICommand& command);
    
    /**
     * @brief 执行AI决策
     * @param ai AI数据指针
     */
    void makeDecision(std::shared_ptr<AIPlayerData> ai);
    
    /**
     * @brief 执行负载均衡
     */
    void performLoadBalancing();
    
    /**
     * @brief 生成随机AI名称
     * @param school 学校类型
     * @return AI名称
     */
    std::string generateRandomName(const std::string& school);
    
    /**
     * @brief 生成唯一AI ID
     * @return AI ID
     */
    std::string generateUniqueId();

private:
    // 核心组件
    std::unique_ptr<ThreadPool> m_threadPool;          ///< 线程池
    std::unique_ptr<DecisionMaker> m_decisionMaker;    ///< 决策制定器
    std::unique_ptr<DatabaseManager> m_databaseManager; ///< 数据库管理器
    
    // AI数据存储
    std::unordered_map<std::string, std::shared_ptr<AIPlayerData>> m_aiPlayers;
    mutable std::shared_mutex m_aiPlayersMutex;        ///< AI数据读写锁
    
    // 指令队列
    std::priority_queue<AICommand, std::vector<AICommand>, 
                       std::function<bool(const AICommand&, const AICommand&)>> m_commandQueue;
    std::mutex m_commandQueueMutex;                    ///< 指令队列互斥锁
    std::condition_variable m_commandCondition;        ///< 指令条件变量
    
    // 线程管理
    std::vector<std::thread> m_workerThreads;         ///< 工作线程组
    std::atomic<bool> m_isRunning;                    ///< 运行状态标志
    std::atomic<bool> m_shouldStop;                   ///< 停止标志
    
    // 配置参数
    std::atomic<int> m_updateFrequency;               ///< 更新频率（毫秒）
    std::atomic<int> m_maxAICount;                    ///< 最大AI数量
    std::atomic<bool> m_autoLoadBalancing;            ///< 自动负载均衡
    
    // 性能统计
    mutable std::mutex m_statsMutex;                  ///< 统计数据互斥锁
    std::unordered_map<std::string, double> m_performanceStats; ///< 性能统计
    std::chrono::steady_clock::time_point m_lastStatsUpdate;    ///< 上次统计更新时间
    
    // 随机数生成
    mutable std::random_device m_randomDevice;        ///< 随机设备
    mutable std::mt19937 m_randomGenerator;           ///< 随机数生成器
    
    // 常量定义
    static constexpr int DEFAULT_UPDATE_FREQUENCY = 100;  ///< 默认更新频率（毫秒）
    static constexpr int DEFAULT_MAX_AI_COUNT = 1000;     ///< 默认最大AI数量
    static constexpr int DEFAULT_THREAD_COUNT = 8;        ///< 默认线程数量
    static constexpr int COMMAND_QUEUE_MAX_SIZE = 10000;  ///< 指令队列最大大小
};

/**
 * @class AIEngineConfig
 * @brief AI引擎配置类
 */
class AIEngineConfig {
public:
    /**
     * @brief 从配置文件加载
     * @param configPath 配置文件路径
     * @return 是否成功加载
     */
    static bool loadFromFile(const std::string& configPath);
    
    /**
     * @brief 保存到配置文件
     * @param configPath 配置文件路径
     * @return 是否成功保存
     */
    static bool saveToFile(const std::string& configPath);
    
    // 配置参数访问器
    static int getThreadCount() { return s_threadCount; }
    static int getUpdateFrequency() { return s_updateFrequency; }
    static int getMaxAICount() { return s_maxAICount; }
    static bool getAutoLoadBalancing() { return s_autoLoadBalancing; }
    static std::string getDatabaseConnectionString() { return s_dbConnectionString; }
    
    // 配置参数设置器
    static void setThreadCount(int count) { s_threadCount = count; }
    static void setUpdateFrequency(int freq) { s_updateFrequency = freq; }
    static void setMaxAICount(int count) { s_maxAICount = count; }
    static void setAutoLoadBalancing(bool enabled) { s_autoLoadBalancing = enabled; }
    static void setDatabaseConnectionString(const std::string& connStr) { s_dbConnectionString = connStr; }

private:
    static int s_threadCount;
    static int s_updateFrequency;
    static int s_maxAICount;
    static bool s_autoLoadBalancing;
    static std::string s_dbConnectionString;
};

#endif // AIENGINE_H
