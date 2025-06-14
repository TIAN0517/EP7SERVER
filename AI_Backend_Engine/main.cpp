/**
 * @file main.cpp
 * @brief RANOnline EP7 AI系统 - 后端推理引擎主程序
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <signal.h>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

#include "AIEngine.h"
#include "SocketServer.h"
#include "DatabaseManager.h"

// 全局变量
std::atomic<bool> g_shouldExit(false);
std::unique_ptr<AIEngine> g_aiEngine;
std::unique_ptr<SocketServer> g_socketServer;
std::unique_ptr<DatabaseManager> g_databaseManager;

/**
 * @brief 信号处理函数
 * @param signal 信号值
 */
void signalHandler(int signal)
{
    switch (signal) {
    case SIGINT:
    case SIGTERM:
        std::cout << "\n🛑 收到停止信号，正在安全关闭系统..." << std::endl;
        g_shouldExit.store(true);
        break;
    default:
        break;
    }
}

/**
 * @brief 显示启动横幅
 */
void showStartupBanner()
{
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║    ██████╗  █████╗ ███╗   ██╗ ██████╗ ███╗   ██╗██╗     ██╗  ║
    ║    ██╔══██╗██╔══██╗████╗  ██║██╔═══██╗████╗  ██║██║     ██║  ║
    ║    ██████╔╝███████║██╔██╗ ██║██║   ██║██╔██╗ ██║██║     ██║  ║
    ║    ██╔══██╗██╔══██║██║╚██╗██║██║   ██║██║╚██╗██║██║     ██║  ║
    ║    ██║  ██║██║  ██║██║ ╚████║╚██████╔╝██║ ╚████║███████╗██║  ║
    ║    ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚═╝  ║
    ║                                                               ║
    ║              EP7 AI Simulation System                         ║
    ║             Ultimate C++ Backend Engine                       ║
    ║                                                               ║
    ║                  🚀 版本: 2.0.0                               ║
    ║                  👥 开发团队: Jy技术团队                       ║
    ║                  📅 发布日期: 2025年6月14日                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

/**
 * @brief 显示系统状态
 */
void showSystemStatus()
{
    if (!g_aiEngine || !g_socketServer || !g_databaseManager) {
        std::cout << "❌ 系统组件未初始化" << std::endl;
        return;
    }
    
    std::cout << "\n📊 系统状态报告:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    // AI引擎状态
    std::cout << "🤖 AI引擎状态: " << (g_aiEngine->isRunning() ? "✅ 运行中" : "❌ 停止") << std::endl;
    std::cout << "   📈 活跃AI数量: " << g_aiEngine->getActiveAICount() << std::endl;
    std::cout << "   📊 总AI数量: " << g_aiEngine->getTotalAICount() << std::endl;
    
    // 服务器负载
    for (int i = 1; i <= 4; ++i) {
        int load = g_aiEngine->getServerLoad(i);
        std::string loadStatus = load < 50 ? "🟢 正常" : load < 80 ? "🟡 中等" : "🔴 高负载";
        std::cout << "   🖥️ 服务器" << i << "负载: " << load << "% " << loadStatus << std::endl;
    }
    
    // Socket服务器状态
    std::cout << "🌐 Socket服务器: " << (g_socketServer->isRunning() ? "✅ 运行中" : "❌ 停止") << std::endl;
    auto socketStats = g_socketServer->getStatistics();
    std::cout << "   📨 已发送消息: " << socketStats["messagesSent"] << std::endl;
    std::cout << "   📩 已接收消息: " << socketStats["messagesReceived"] << std::endl;
    std::cout << "   🔗 活跃连接: " << socketStats["activeConnections"] << std::endl;
    
    // 数据库状态
    std::cout << "🗄️ 数据库管理器: " << (g_databaseManager->isRunning() ? "✅ 运行中" : "❌ 停止") << std::endl;
    auto dbStats = g_databaseManager->getConnectionPoolStatus();
    std::cout << "   🔗 连接池状态: " << dbStats["activeConnections"] << "/" << dbStats["totalConnections"] << std::endl;
    std::cout << "   📊 待处理操作: " << dbStats["pendingOperations"] << std::endl;
    
    // 性能统计
    auto perfStats = g_aiEngine->getPerformanceStats();
    std::cout << "⚡ 性能指标:" << std::endl;
    std::cout << "   🎯 平均决策时间: " << std::fixed << std::setprecision(2) << perfStats["avgDecisionTime"] << "ms" << std::endl;
    std::cout << "   🔄 每秒处理指令: " << std::fixed << std::setprecision(1) << perfStats["commandsPerSecond"] << std::endl;
    std::cout << "   💾 内存使用: " << std::fixed << std::setprecision(1) << perfStats["memoryUsageMB"] << "MB" << std::endl;
    
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
}

/**
 * @brief 处理控制台命令
 * @param command 命令字符串
 * @return 是否继续运行
 */
bool processCommand(const std::string& command)
{
    if (command == "help" || command == "h") {
        std::cout << "\n📋 可用命令:" << std::endl;
        std::cout << "  help, h         - 显示帮助信息" << std::endl;
        std::cout << "  status, s       - 显示系统状态" << std::endl;
        std::cout << "  create <count>  - 创建指定数量的AI (例: create 10)" << std::endl;
        std::cout << "  list            - 列出所有AI" << std::endl;
        std::cout << "  clear           - 清屏" << std::endl;
        std::cout << "  config          - 显示配置信息" << std::endl;
        std::cout << "  quit, exit, q   - 退出程序" << std::endl;
        
    } else if (command == "status" || command == "s") {
        showSystemStatus();
        
    } else if (command.substr(0, 6) == "create") {
        try {
            int count = 1;
            if (command.length() > 7) {
                count = std::stoi(command.substr(7));
            }
            std::cout << "🚀 正在创建 " << count << " 个AI..." << std::endl;
            
            auto aiIds = g_aiEngine->createBatchAI(count, "聖門", 1, 10);
            std::cout << "✅ 成功创建 " << aiIds.size() << " 个AI" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "❌ 创建AI失败: " << e.what() << std::endl;
        }
        
    } else if (command == "list") {
        auto aiList = g_aiEngine->getAllAI();
        std::cout << "\n📋 AI列表 (总数: " << aiList.size() << "):" << std::endl;
        std::cout << "┌─────────────┬─────────────────┬─────────┬─────────┬─────────────────────────┐" << std::endl;
        std::cout << "│    AI ID    │      名称       │  学校   │  等级   │         状态            │" << std::endl;
        std::cout << "├─────────────┼─────────────────┼─────────┼─────────┼─────────────────────────┤" << std::endl;
        
        for (const auto& ai : aiList) {
            std::string statusStr;
            switch (ai->status) {
                case AIPlayerData::Status::Idle: statusStr = "🟢 空闲"; break;
                case AIPlayerData::Status::Moving: statusStr = "🟡 移动中"; break;
                case AIPlayerData::Status::Fighting: statusStr = "🔴 战斗中"; break;
                case AIPlayerData::Status::Questing: statusStr = "🟣 做任务"; break;
                case AIPlayerData::Status::Chatting: statusStr = "🔵 聊天中"; break;
                case AIPlayerData::Status::Offline: statusStr = "⚫ 离线"; break;
            }
            
            printf("│ %-11s │ %-15s │ %-7s │ %-7d │ %-23s │\n",
                   ai->id.substr(0, 11).c_str(),
                   ai->name.substr(0, 15).c_str(),
                   ai->school.c_str(),
                   ai->level,
                   statusStr.c_str());
        }
        std::cout << "└─────────────┴─────────────────┴─────────┴─────────┴─────────────────────────┘" << std::endl;
        
    } else if (command == "clear") {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        showStartupBanner();
        
    } else if (command == "config") {
        std::cout << "\n⚙️ 系统配置:" << std::endl;
        std::cout << "  🔧 线程数: " << AIEngineConfig::getThreadCount() << std::endl;
        std::cout << "  ⏱️ 更新频率: " << AIEngineConfig::getUpdateFrequency() << "ms" << std::endl;
        std::cout << "  🤖 最大AI数量: " << AIEngineConfig::getMaxAICount() << std::endl;
        std::cout << "  ⚖️ 自动负载均衡: " << (AIEngineConfig::getAutoLoadBalancing() ? "启用" : "禁用") << std::endl;
        std::cout << "  🗄️ 数据库连接: " << AIEngineConfig::getDatabaseConnectionString() << std::endl;
        
    } else if (command == "quit" || command == "exit" || command == "q") {
        return false;
        
    } else if (!command.empty()) {
        std::cout << "❌ 未知命令: '" << command << "'. 输入 'help' 查看可用命令." << std::endl;
    }
    
    return true;
}

/**
 * @brief 交互式控制台循环
 */
void interactiveConsole()
{
    std::string command;
    
    std::cout << "\n🎮 交互式控制台已启动。输入 'help' 查看命令列表。" << std::endl;
    
    while (!g_shouldExit.load()) {
        std::cout << "\nRANOnline-AI> ";
        std::getline(std::cin, command);
        
        // 去除前后空格
        command.erase(0, command.find_first_not_of(" \t"));
        command.erase(command.find_last_not_of(" \t") + 1);
        
        if (!processCommand(command)) {
            g_shouldExit.store(true);
            break;
        }
    }
}

/**
 * @brief 初始化系统组件
 * @return 是否成功初始化
 */
bool initializeSystem()
{
    std::cout << "🔧 正在初始化系统组件..." << std::endl;
    
    try {
        // 加载配置
        std::cout << "📋 加载配置文件..." << std::endl;
        if (!AIEngineConfig::loadFromFile("config/ai_engine_config.json")) {
            std::cout << "⚠️ 无法加载配置文件，使用默认配置" << std::endl;
        }
        
        // 初始化数据库管理器
        std::cout << "🗄️ 初始化数据库管理器..." << std::endl;
        g_databaseManager = std::make_unique<DatabaseManager>();
        if (!g_databaseManager->initialize(AIEngineConfig::getDatabaseConnectionString())) {
            std::cout << "❌ 数据库管理器初始化失败" << std::endl;
            return false;
        }
        
        // 初始化AI引擎
        std::cout << "🤖 初始化AI推理引擎..." << std::endl;
        g_aiEngine = std::make_unique<AIEngine>();
        if (!g_aiEngine->initialize()) {
            std::cout << "❌ AI引擎初始化失败" << std::endl;
            return false;
        }
        
        // 初始化Socket服务器
        std::cout << "🌐 初始化Socket服务器..." << std::endl;
        g_socketServer = std::make_unique<SocketServer>();
        if (!g_socketServer->initialize(8080)) {
            std::cout << "❌ Socket服务器初始化失败" << std::endl;
            return false;
        }
        
        std::cout << "✅ 系统组件初始化完成" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 初始化过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 启动系统服务
 * @return 是否成功启动
 */
bool startServices()
{
    std::cout << "🚀 正在启动系统服务..." << std::endl;
    
    try {
        // 启动数据库管理器
        if (!g_databaseManager->start()) {
            std::cout << "❌ 数据库管理器启动失败" << std::endl;
            return false;
        }
        std::cout << "✅ 数据库管理器已启动" << std::endl;
        
        // 启动AI引擎
        if (!g_aiEngine->start()) {
            std::cout << "❌ AI引擎启动失败" << std::endl;
            return false;
        }
        std::cout << "✅ AI推理引擎已启动" << std::endl;
        
        // 启动Socket服务器
        if (!g_socketServer->start()) {
            std::cout << "❌ Socket服务器启动失败" << std::endl;
            return false;
        }
        std::cout << "✅ Socket服务器已启动" << std::endl;
        
        std::cout << "🎉 所有服务启动成功！" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 启动过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 停止系统服务
 */
void stopServices()
{
    std::cout << "🛑 正在停止系统服务..." << std::endl;
    
    try {
        if (g_socketServer) {
            g_socketServer->stop();
            std::cout << "✅ Socket服务器已停止" << std::endl;
        }
        
        if (g_aiEngine) {
            g_aiEngine->stop();
            std::cout << "✅ AI推理引擎已停止" << std::endl;
        }
        
        if (g_databaseManager) {
            g_databaseManager->stop();
            std::cout << "✅ 数据库管理器已停止" << std::endl;
        }
        
        std::cout << "🎯 所有服务已安全停止" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 停止过程中发生异常: " << e.what() << std::endl;
    }
}

/**
 * @brief 主函数
 * @param argc 参数数量
 * @param argv 参数数组
 * @return 程序退出代码
 */
int main(int argc, char* argv[])
{
    // 设置控制台编码为UTF-8
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    
    // 注册信号处理器
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        // 显示启动横幅
        showStartupBanner();
        
        // 检查命令行参数
        bool daemonMode = false;
        bool verboseMode = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--daemon" || arg == "-d") {
                daemonMode = true;
            } else if (arg == "--verbose" || arg == "-v") {
                verboseMode = true;
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "用法: " << argv[0] << " [选项]" << std::endl;
                std::cout << "选项:" << std::endl;
                std::cout << "  -d, --daemon    以守护进程模式运行" << std::endl;
                std::cout << "  -v, --verbose   详细输出模式" << std::endl;
                std::cout << "  -h, --help      显示帮助信息" << std::endl;
                return 0;
            }
        }
        
        if (verboseMode) {
            std::cout << "🔍 详细输出模式已启用" << std::endl;
        }
        
        // 初始化系统
        if (!initializeSystem()) {
            std::cout << "❌ 系统初始化失败，程序退出" << std::endl;
            return -1;
        }
        
        // 启动服务
        if (!startServices()) {
            std::cout << "❌ 服务启动失败，程序退出" << std::endl;
            stopServices();
            return -1;
        }
        
        // 显示初始状态
        showSystemStatus();
        
        if (daemonMode) {
            std::cout << "🤖 以守护进程模式运行，按 Ctrl+C 停止" << std::endl;
            
            // 守护进程模式：等待停止信号
            while (!g_shouldExit.load()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } else {
            // 交互模式：启动控制台
            interactiveConsole();
        }
        
        // 停止服务
        stopServices();
        
        std::cout << "👋 RANOnline AI系统已安全退出。感谢使用！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 程序运行过程中发生未处理的异常: " << e.what() << std::endl;
        stopServices();
        return -1;
    } catch (...) {
        std::cout << "❌ 程序运行过程中发生未知异常" << std::endl;
        stopServices();
        return -1;
    }
}
