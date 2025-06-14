// ========================================================================
// RANOnline AI LLM Integration - 企業級主程式進入點
// 版本: 3.0.0 企業版
// 開發團隊: Jy技術團隊
// 
// 🎯 BossJy 企業級標準:
// ✅ WinMain/main() 自動兼容
// ✅ UTF-8 編碼完整支援
// ✅ 異常處理與崩潰報告
// ✅ 多語言國際化支援
// ✅ DPI 感知與縮放適配
// ✅ 資源載入檢查與故障轉移
// ========================================================================

#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include <QStyleFactory>
#include <QScreen>
#include <QMessageBox>
#include <QLoggingCategory>
#include <QFileInfo>
#include <QProcess>
#include <QThread>
#include <QMutex>
#include <QDateTime>
#include <QTextStream>
#include <iostream>
#include <memory>
#include <exception>

// Windows 特定標頭檔
#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <dbghelp.h>
#include <psapi.h>
#include <shellapi.h>
#include <crtdbg.h>
// MinGW 相容性處理
#ifdef __MINGW32__
// MinGW 下使用連結器參數而非 pragma
#else
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#endif
#endif

#include "LLMMainWindow.h"

// ========================================================================
// 企業級版本與構建資訊
// ========================================================================
#ifndef APP_VERSION_STRING
#define APP_VERSION_STRING "3.0.0"
#endif

#ifndef APP_BUILD_DATE
#define APP_BUILD_DATE __DATE__ " " __TIME__
#endif

#ifndef APP_GIT_HASH
#define APP_GIT_HASH "unknown"
#endif

// ========================================================================
// 企業級日誌系統
// ========================================================================
class EnterpriseLogger
{
public:
    static EnterpriseLogger& instance() {
        static EnterpriseLogger logger;
        return logger;
    }
    
    void initialize() {
        // 創建日誌目錄
        QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(logDir);
        
        // 設置日誌文件
        QString logFile = logDir + "/ai_llm_integration.log";
        m_logFile.reset(new QFile(logFile));
        
        if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
            m_logStream.reset(new QTextStream(m_logFile.get()));
            
            // 寫入啟動資訊
            *m_logStream << QString("========== 應用程式啟動 ==========\n");
            *m_logStream << QString("時間: %1\n").arg(QDateTime::currentDateTime().toString());
            *m_logStream << QString("版本: %1\n").arg(APP_VERSION_STRING);
            *m_logStream << QString("構建: %1\n").arg(APP_BUILD_DATE);
            *m_logStream << QString("Git: %1\n").arg(APP_GIT_HASH);
            *m_logStream << QString("=================================\n");
            m_logStream->flush();
        }
        
        // 安裝Qt訊息處理器
        qInstallMessageHandler(&EnterpriseLogger::messageHandler);
    }
    
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        auto& logger = EnterpriseLogger::instance();
        logger.writeMessage(type, context, msg);
    }
    
private:
    void writeMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QMutexLocker locker(&m_mutex);
        
        QString typeStr;
        switch (type) {
            case QtDebugMsg:    typeStr = "DEBUG"; break;
            case QtInfoMsg:     typeStr = "INFO"; break;
            case QtWarningMsg:  typeStr = "WARN"; break;
            case QtCriticalMsg: typeStr = "ERROR"; break;
            case QtFatalMsg:    typeStr = "FATAL"; break;
        }
        
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString logEntry = QString("[%1] %2: %3\n").arg(timestamp, typeStr, msg);
        
        // 寫入文件
        if (m_logStream) {
            *m_logStream << logEntry;
            m_logStream->flush();
        }
        
        // 控制台輸出 (Debug模式)
#ifdef QT_DEBUG
        std::cout << logEntry.toStdString();
#endif
    }
    
    std::unique_ptr<QFile> m_logFile;
    std::unique_ptr<QTextStream> m_logStream;
    QMutex m_mutex;
};

// ========================================================================
// Windows 崩潰報告系統
// ========================================================================
#ifdef Q_OS_WIN
class CrashReporter
{
public:
    static void initialize() {
        SetUnhandledExceptionFilter(unhandledExceptionFilter);
        _set_purecall_handler(pureCallHandler);
        _set_invalid_parameter_handler(invalidParameterHandler);
        
        // 設置 CRT 報告模式
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    }
    
private:
    static LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
        QString crashDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";
        QDir().mkpath(crashDir);
        
        QString crashFile = crashDir + QString("/crash_%1.dmp")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
        
        HANDLE hFile = CreateFileA(
            crashFile.toLocal8Bit().constData(),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        
        if (hFile != INVALID_HANDLE_VALUE) {
            MINIDUMP_EXCEPTION_INFORMATION mdei;
            mdei.ThreadId = GetCurrentThreadId();
            mdei.ExceptionPointers = exceptionInfo;
            mdei.ClientPointers = FALSE;
            
            MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                MiniDumpNormal,
                &mdei,
                nullptr,
                nullptr
            );
            
            CloseHandle(hFile);
        }
        
        qCritical() << "應用程式發生嚴重錯誤，崩潰報告已保存至:" << crashFile;
        
        return EXCEPTION_EXECUTE_HANDLER;
    }
    
    static void __cdecl pureCallHandler() {
        qCritical() << "Pure virtual function call detected";
        std::terminate();
    }
    
    static void __cdecl invalidParameterHandler(
        const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line,
        uintptr_t pReserved) {
        
        Q_UNUSED(pReserved)
        qCritical() << "Invalid parameter detected:"
                   << "Expression:" << QString::fromWCharArray(expression)
                   << "Function:" << QString::fromWCharArray(function)
                   << "File:" << QString::fromWCharArray(file)
                   << "Line:" << line;
        std::terminate();
    }
};
#endif

// ========================================================================
// 企業級應用程式配置
// ========================================================================
class ApplicationConfigurator
{
public:    static void configureApplication(QApplication& app) {
        // 基本應用程式資訊
        app.setApplicationName("Jy技術團隊 線上AI");
        app.setApplicationDisplayName("Jy技術團隊 線上AI - 大型語言模型整合系統");
        app.setApplicationVersion(APP_VERSION_STRING);
        app.setOrganizationName("Jy技術團隊");
        app.setOrganizationDomain("ranonline.ai");
        
        // DPI 縮放配置
        configureDpiScaling(app);
        
        // 樣式與主題配置
        configureStyle(app);
        
        // 圖標配置
        configureIcon(app);
        
        // 國際化配置
        configureLocalization(app);
        
        // 字體配置
        configureFont(app);
        
        qInfo() << "應用程式配置完成";
    }
    
private:
    static void configureDpiScaling(QApplication& app) {
        // 啟用高 DPI 支援
        app.setAttribute(Qt::AA_EnableHighDpiScaling, true);
        app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
        
        // Qt 6 特有的 DPI 設置
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
        
        qInfo() << "DPI 縮放配置完成";
    }
    
    static void configureStyle(QApplication& app) {
        // 設置現代化樣式
        QStringList availableStyles = QStyleFactory::keys();
        
        QString preferredStyle;
        if (availableStyles.contains("Fusion")) {
            preferredStyle = "Fusion";
        } else if (availableStyles.contains("Windows")) {
            preferredStyle = "Windows";
        }
        
        if (!preferredStyle.isEmpty()) {
            app.setStyle(preferredStyle);
            qInfo() << "樣式設置為:" << preferredStyle;
        }
        
        // 設置暗色調色板 (可選)
        configureDarkPalette(app);
    }
    
    static void configureDarkPalette(QApplication& app) {
        // 企業級暗色主題
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        
        // 暫時不啟用暗色主題，由使用者選擇
        // app.setPalette(darkPalette);
    }
    
    static void configureIcon(QApplication& app) {
        // 設置應用程式圖標
        QStringList iconPaths = {
            ":/icons/jy1.ico",
            ":/icons/app.ico",
            "Resources/jy1.ico",
            "Resources/app.ico",
            "../Resources/jy1.ico"
        };
        
        for (const QString& iconPath : iconPaths) {
            if (QFileInfo::exists(iconPath)) {
                QIcon appIcon(iconPath);
                if (!appIcon.isNull()) {
                    app.setWindowIcon(appIcon);
                    qInfo() << "應用程式圖標載入成功:" << iconPath;
                    return;
                }
            }
        }
        
        qWarning() << "未找到應用程式圖標文件";
    }
    
    static void configureLocalization(QApplication& app) {
        // 載入繁體中文翻譯
        static QTranslator translator;
        QString locale = QLocale::system().name();
        
        QStringList translationPaths = {
            ":/translations/zh_TW.qm",
            "Resources/translations/zh_TW.qm",
            "../Resources/translations/zh_TW.qm"
        };
        
        for (const QString& translationPath : translationPaths) {
            if (QFileInfo::exists(translationPath)) {
                if (translator.load(translationPath)) {
                    app.installTranslator(&translator);
                    qInfo() << "翻譯文件載入成功:" << translationPath;
                    return;
                }
            }
        }
        
        qInfo() << "使用預設語言 (繁體中文內建)";
    }
    
    static void configureFont(QApplication& app) {
        // 設置適合的中文字體
        QFont font = app.font();
        
        // Windows 平台字體優化
#ifdef Q_OS_WIN
        QStringList fontFamilies = {
            "Microsoft YaHei UI",
            "Microsoft YaHei",
            "SimHei",
            "Arial Unicode MS",
            "Segoe UI"
        };
        
        for (const QString& fontFamily : fontFamilies) {
            QFont testFont(fontFamily);
            if (QFontInfo(testFont).family() == fontFamily) {
                font.setFamily(fontFamily);
                break;
            }
        }
#endif
        
        font.setPointSize(9);
        app.setFont(font);
        
        qInfo() << "字體設置為:" << font.family() << font.pointSize() << "pt";
    }
};

// ========================================================================
// 企業級異常處理器
// ========================================================================
class ExceptionHandler
{
public:
    static void handleQtException(const std::exception& e) {
        QString errorMsg = QString("Qt 異常: %1").arg(e.what());
        qCritical() << errorMsg;
        
        QMessageBox::critical(
            nullptr,
            "嚴重錯誤",
            QString("應用程式發生嚴重錯誤:\n\n%1\n\n請聯繫技術支援。").arg(errorMsg)
        );
    }
    
    static void handleStdException(const std::exception& e) {
        QString errorMsg = QString("標準庫異常: %1").arg(e.what());
        qCritical() << errorMsg;
        
        QMessageBox::critical(
            nullptr,
            "嚴重錯誤",
            QString("應用程式發生嚴重錯誤:\n\n%1\n\n請聯繫技術支援。").arg(errorMsg)
        );
    }
    
    static void handleUnknownException() {
        qCritical() << "未知異常";
        
        QMessageBox::critical(
            nullptr,
            "嚴重錯誤",
            "應用程式發生未知錯誤，請聯繫技術支援。"
        );
    }
};

// ========================================================================
// Windows Debug 控制台設置 (僅 Debug 模式)
// ========================================================================
#ifdef Q_OS_WIN
void setupDebugConsole() {
#ifdef QT_DEBUG
    // 僅在 Debug 模式下分配控制台
    if (AllocConsole()) {
        // 重定向標準輸出到控制台
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        
        // 設置控制台標題
        SetConsoleTitleA("RANOnline AI LLM Integration - Debug Console");
        
        // 設置控制台文字模式為 UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        
        // 同步 C++ 流與 C 流
        std::ios::sync_with_stdio(true);
        
        qInfo() << "Debug 控制台已啟用";
    }
#endif
}
#endif

// ========================================================================
// 企業級主函數 (支援 WinMain 自動轉換)
// ========================================================================
int main(int argc, char *argv[])
{
    // 啟用 UTF-8 編碼 (Windows)
#ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // 創建 QApplication 實例
    QApplication app(argc, argv);
    
    try {
        // 初始化企業級組件
#ifdef Q_OS_WIN
        CrashReporter::initialize();
        setupDebugConsole();
#endif
        
        EnterpriseLogger::instance().initialize();
        ApplicationConfigurator::configureApplication(app);
        
        qInfo() << "========== 應用程式啟動 ==========";
        qInfo() << "版本:" << APP_VERSION_STRING;
        qInfo() << "構建:" << APP_BUILD_DATE;
        qInfo() << "Git Hash:" << APP_GIT_HASH;
        qInfo() << "Qt 版本:" << qVersion();
        qInfo() << "作業系統:" << QSysInfo::prettyProductName();
        qInfo() << "CPU 架構:" << QSysInfo::currentCpuArchitecture();
        qInfo() << "================================";
        
        // 創建並顯示主窗口
        std::unique_ptr<LLMMainWindow> mainWindow;
        
        try {
            mainWindow = std::make_unique<LLMMainWindow>();
            mainWindow->show();
            
            qInfo() << "主窗口創建並顯示成功";
            
        } catch (const std::exception& e) {
            ExceptionHandler::handleStdException(e);
            return -2;
        } catch (...) {
            ExceptionHandler::handleUnknownException();
            return -3;
        }
        
        // 進入事件循環
        qInfo() << "進入 Qt 事件循環";
        int result = app.exec();
        
        qInfo() << "應用程式正常退出，返回代碼:" << result;
        return result;
        
    } catch (const std::exception& e) {
        ExceptionHandler::handleStdException(e);
        return -1;
    } catch (...) {
        ExceptionHandler::handleUnknownException();
        return -1;
    }
}

// ========================================================================
// Windows WinMain 入口點支援 (企業級相容性)
// ========================================================================
#ifdef Q_OS_WIN
#if defined(QT_NEEDS_QMAIN)
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc = 0;
    char **argv = nullptr;
    
    // 從命令行參數轉換
    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (szArglist) {
        argv = new char*[argc];
        for (int i = 0; i < argc; ++i) {
            int len = WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, nullptr, 0, nullptr, nullptr);
            argv[i] = new char[len];
            WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, argv[i], len, nullptr, nullptr);
        }
        LocalFree(szArglist);
    }
    
    int result = main(argc, argv);
    
    // 清理記憶體
    if (argv) {
        for (int i = 0; i < argc; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
    
    return result;
}
#endif
#endif
