/**
 * @file main.cpp
 * @brief RANOnline EP7 AI系统 - 前端主程序入口
 * @author Jy技术团队
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtCore/QLoggingCategory>
#include <iostream>
#include <memory>

#include "MainWindow.h"
#include "NetworkManager.h"

// 全局日志配置
Q_LOGGING_CATEGORY(aiSystem, "ai.system")
Q_LOGGING_CATEGORY(aiNetwork, "ai.network")
Q_LOGGING_CATEGORY(aiDatabase, "ai.database")

/**
 * @brief 初始化应用程序设置
 * @param app 应用程序实例
 */
void initializeApplication(QApplication& app)
{
    // 设置应用程序信息
    app.setApplicationName("RANOnline AI System Ultimate");
    app.setApplicationVersion("2.0.0");
    app.setApplicationDisplayName("RANOnline EP7 AI Simulation System - Ultimate C++ Edition");
    app.setOrganizationName("Jy技术团队");
    app.setOrganizationDomain("jytech.team");
    
    // 设置应用程序图标
    app.setWindowIcon(QIcon(":/icons/app_icon.ico"));
    
    // 启用高DPI支持
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // 设置字体
    QFont font("Microsoft YaHei UI", 9);
    app.setFont(font);
}

/**
 * @brief 初始化日志系统
 */
void initializeLogging()
{
    // 创建日志目录
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(logDir);
    
    // 设置日志过滤规则
    QLoggingCategory::setFilterRules(
        "ai.system.debug=true\n"
        "ai.network.debug=true\n"
        "ai.database.debug=true\n"
        "qt.network.ssl.debug=false"
    );
    
    qCInfo(aiSystem) << "RANOnline AI System Ultimate Starting...";
    qCInfo(aiSystem) << "Log directory:" << logDir;
}

/**
 * @brief 检查系统要求
 * @return 是否满足系统要求
 */
bool checkSystemRequirements()
{
    // 检查操作系统版本
#ifdef _WIN32
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        // Windows 10 及以上版本
        if (osvi.dwMajorVersion < 10) {
            qCCritical(aiSystem) << "Unsupported Windows version. Windows 10 or later required.";
            return false;
        }
    }
#endif
    
    // 检查内存
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus)) {
        DWORDLONG totalMemoryGB = memStatus.ullTotalPhys / (1024 * 1024 * 1024);
        if (totalMemoryGB < 4) {
            qCWarning(aiSystem) << "Low memory detected:" << totalMemoryGB << "GB. 4GB or more recommended.";
        } else {
            qCInfo(aiSystem) << "System memory:" << totalMemoryGB << "GB";
        }
    }
    
    // 检查磁盘空间
    QString appDir = QCoreApplication::applicationDirPath();
    QStorageInfo storage(appDir);
    if (storage.isValid()) {
        qint64 availableGB = storage.bytesAvailable() / (1024 * 1024 * 1024);
        if (availableGB < 2) {
            qCWarning(aiSystem) << "Low disk space:" << availableGB << "GB available. 2GB or more recommended.";
        }
    }
    
    qCInfo(aiSystem) << "System requirements check passed.";
    return true;
}

/**
 * @brief 应用程序异常处理器
 * @param type 异常类型
 * @param context 上下文信息
 * @param message 异常消息
 */
void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString category = context.category ? context.category : "default";
    QString function = context.function ? context.function : "unknown";
    QString file = context.file ? QFileInfo(context.file).baseName() : "unknown";
    
    QString formattedMessage = QString("[%1] [%2] [%3:%4] %5")
                              .arg(timestamp)
                              .arg(category)
                              .arg(file)
                              .arg(context.line)
                              .arg(message);
    
    // 输出到控制台
    switch (type) {
    case QtDebugMsg:
        std::cout << "DEBUG: " << formattedMessage.toStdString() << std::endl;
        break;
    case QtInfoMsg:
        std::cout << "INFO:  " << formattedMessage.toStdString() << std::endl;
        break;
    case QtWarningMsg:
        std::cout << "WARN:  " << formattedMessage.toStdString() << std::endl;
        break;
    case QtCriticalMsg:
        std::cerr << "ERROR: " << formattedMessage.toStdString() << std::endl;
        break;
    case QtFatalMsg:
        std::cerr << "FATAL: " << formattedMessage.toStdString() << std::endl;
        break;
    }
    
    // 写入日志文件
    static QFile logFile;
    if (!logFile.isOpen()) {
        QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) 
                         + "/logs/ai_frontend_" 
                         + QDateTime::currentDateTime().toString("yyyyMMdd") 
                         + ".log";
        QDir().mkpath(QFileInfo(logPath).absolutePath());
        logFile.setFileName(logPath);
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    
    if (logFile.isOpen()) {
        QTextStream stream(&logFile);
        stream << formattedMessage << Qt::endl;
        stream.flush();
    }
}

/**
 * @brief 主函数
 * @param argc 参数数量
 * @param argv 参数数组
 * @return 程序退出代码
 */
int main(int argc, char *argv[])
{
    // 创建应用程序实例
    QApplication app(argc, argv);
    
    // 安装消息处理器
    qInstallMessageHandler(messageHandler);
    
    try {
        // 初始化应用程序
        initializeApplication(app);
        initializeLogging();
        
        qCInfo(aiSystem) << "=== RANOnline EP7 AI Simulation System ===";
        qCInfo(aiSystem) << "Ultimate C++ Edition v2.0.0";
        qCInfo(aiSystem) << "Developed by Jy技术团队";
        qCInfo(aiSystem) << "Built on" << __DATE__ << __TIME__;
        qCInfo(aiSystem) << "Qt Version:" << QT_VERSION_STR;
        qCInfo(aiSystem) << "==========================================";
        
        // 检查系统要求
        if (!checkSystemRequirements()) {
            qCCritical(aiSystem) << "System requirements not met. Exiting.";
            return -1;
        }
        
        // 检查命令行参数
        QStringList arguments = app.arguments();
        bool debugMode = arguments.contains("--debug") || arguments.contains("-d");
        bool verboseMode = arguments.contains("--verbose") || arguments.contains("-v");
        
        if (debugMode) {
            qCInfo(aiSystem) << "Debug mode enabled";
            QLoggingCategory::setFilterRules("*.debug=true");
        }
        
        if (verboseMode) {
            qCInfo(aiSystem) << "Verbose mode enabled";
        }
        
        // 创建主窗口
        qCInfo(aiSystem) << "Creating main window...";
        std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>();
        
        // 显示启动画面（可选）
        QPixmap splashPixmap(":/images/splash.png");
        if (!splashPixmap.isNull()) {
            QSplashScreen splash(splashPixmap);
            splash.show();
            splash.showMessage("正在启动 RANOnline AI System...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
            app.processEvents();
            
            // 模拟加载过程
            for (int i = 0; i <= 100; i += 10) {
                splash.showMessage(QString("正在加载... %1%").arg(i), Qt::AlignBottom | Qt::AlignCenter, Qt::white);
                app.processEvents();
                QThread::msleep(100);
            }
            
            splash.finish(mainWindow.get());
        }
        
        // 显示主窗口
        qCInfo(aiSystem) << "Showing main window...";
        mainWindow->show();
        
        // 运行应用程序事件循环
        qCInfo(aiSystem) << "Starting application event loop...";
        int result = app.exec();
        
        qCInfo(aiSystem) << "Application shutting down with exit code:" << result;
        return result;
        
    } catch (const std::exception& e) {
        qCCritical(aiSystem) << "Unhandled exception:" << e.what();
        return -1;
    } catch (...) {
        qCCritical(aiSystem) << "Unknown exception occurred";
        return -1;
    }
}
