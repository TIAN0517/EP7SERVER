/**
 * @file main_odbc_test.cpp
 * @brief RANOnline EP7 AI系統 - ODBC連接測試工具主程序
 * 
 * 功能特性:
 * - 圖形化ODBC連接測試界面
 * - 自動DSN檢測和創建
 * - SQL Server 2022 連接驗證
 * - 性能基準測試
 * - 詳細錯誤診斷
 * - 一鍵式部署驗證
 * 
 * 編譯指令:
 * Windows (VC++):
 *   cl /EHsc main_odbc_test.cpp ODBCTestApplication.cpp ODBCConnectionManager.cpp /I"C:\Qt\6.x\msvc2019_64\include" /link /LIBPATH:"C:\Qt\6.x\msvc2019_64\lib"
 * 
 * Windows (MinGW):
 *   g++ -std=c++20 main_odbc_test.cpp ODBCTestApplication.cpp ODBCConnectionManager.cpp -I"C:\Qt\6.x\mingw_64\include" -L"C:\Qt\6.x\mingw_64\lib" -lQt6Core -lQt6Widgets -lQt6Sql -lodbc32
 * 
 * @author Jy技術團隊
 * @date 2025-06-14
 * @version 2.0.0
 */

#include "ODBCTestApplication.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <iostream>

#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

/**
 * @brief 設置控制台輸出（Windows專用）
 */
void setupConsoleOutput()
{
#ifdef Q_OS_WIN
    // 分配控制台
    if (AllocConsole()) {
        // 重定向標準輸出到控制台
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        
        // 設置控制台標題
        SetConsoleTitleA("RANOnline ODBC Test Tool - Debug Console");
        
        // 設置控制台文字模式為UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
#endif
}

/**
 * @brief 檢查系統環境和依賴
 */
bool checkSystemRequirements()
{
    std::cout << "=== 系統環境檢查 ===" << std::endl;
    
    // 檢查Qt版本
    std::cout << "Qt版本: " << QT_VERSION_STR << std::endl;
    
    // 檢查作業系統
#ifdef Q_OS_WIN
    std::cout << "作業系統: Windows" << std::endl;
    
    // 檢查ODBC驅動
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
        "SOFTWARE\\ODBC\\ODBCINST.INI\\ODBC Drivers", 
        0, KEY_READ, &hKey);
    
    if (result == ERROR_SUCCESS) {
        std::cout << "✅ ODBC註冊表項存在" << std::endl;
        RegCloseKey(hKey);
    } else {
        std::cout << "⚠️ ODBC註冊表項不存在或無法訪問" << std::endl;
    }
    
    // 檢查SQL Server驅動
    result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
        "SOFTWARE\\ODBC\\ODBCINST.INI\\SQL Server", 
        0, KEY_READ, &hKey);
    
    if (result == ERROR_SUCCESS) {
        std::cout << "✅ SQL Server ODBC驅動已安裝" << std::endl;
        RegCloseKey(hKey);
    } else {
        std::cout << "⚠️ SQL Server ODBC驅動未找到" << std::endl;
    }
    
#else
    std::cout << "作業系統: 非Windows系統" << std::endl;
    std::cout << "⚠️ 此工具主要為Windows系統設計" << std::endl;
#endif
    
    std::cout << "========================" << std::endl;
    return true;
}

/**
 * @brief 創建應用程序配置目錄
 */
void createAppDirectories()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    
    if (!dir.exists(appDataPath)) {
        dir.mkpath(appDataPath);
        std::cout << "創建應用程序數據目錄: " << appDataPath.toStdString() << std::endl;
    }
    
    QString logPath = appDataPath + "/logs";
    if (!dir.exists(logPath)) {
        dir.mkpath(logPath);
        std::cout << "創建日誌目錄: " << logPath.toStdString() << std::endl;
    }
}

/**
 * @brief 設置Qt翻譯
 */
void setupTranslations(QApplication &app)
{
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale::system().name(),
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
    
    QTranslator appTranslator;
    if (appTranslator.load("ranonline_" + QLocale::system().name(),
                           QApplication::applicationDirPath() + "/translations")) {
        app.installTranslator(&appTranslator);
    }
}

/**
 * @brief 異常處理函數
 */
void handleException(const std::exception &e)
{
    QString errorMsg = QString("應用程序發生嚴重錯誤: %1").arg(e.what());
    std::cerr << errorMsg.toStdString() << std::endl;
    
    QMessageBox::critical(nullptr, "嚴重錯誤", errorMsg);
}

/**
 * @brief 主函數
 */
int main(int argc, char *argv[])
{
    try {
        // 設置控制台輸出（僅在調試模式）
#ifdef DEBUG
        setupConsoleOutput();
#endif
        
        std::cout << "=== RANOnline EP7 AI - ODBC測試工具 ===" << std::endl;
        std::cout << "版本: 2.0.0" << std::endl;
        std::cout << "開發團隊: Jy技術團隊" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // 檢查系統需求
        if (!checkSystemRequirements()) {
            std::cerr << "系統需求檢查失敗！" << std::endl;
            return 1;
        }
        
        // 創建Qt應用程序
        ODBCTestApplication app(argc, argv);
        
        std::cout << "Qt應用程序已創建" << std::endl;
        
        // 設置翻譯
        setupTranslations(app);
        
        // 創建必要的目錄
        createAppDirectories();
        
        std::cout << "正在啟動ODBC測試工具..." << std::endl;
        
        // 運行應用程序
        int result = app.runApplication();
        
        std::cout << "應用程序退出，代碼: " << result << std::endl;
        
#ifdef DEBUG
        std::cout << "按任意鍵退出..." << std::endl;
        std::cin.get();
#endif
        
        return result;
        
    } catch (const std::exception &e) {
        handleException(e);
        return 1;
    } catch (...) {
        std::cerr << "發生未知異常！" << std::endl;
        QMessageBox::critical(nullptr, "未知錯誤", "應用程序發生未知錯誤！");
        return 1;
    }
}

/**
 * @brief 輸出系統信息（調試用）
 */
void printSystemInfo()
{
    std::cout << "\n=== 系統信息 ===" << std::endl;
    std::cout << "Qt版本: " << qVersion() << std::endl;
    std::cout << "編譯器: " << 
#ifdef _MSC_VER
        "Microsoft Visual C++ " << _MSC_VER
#elif defined(__GNUC__)
        "GCC " << __GNUC__ << "." << __GNUC_MINOR__
#elif defined(__clang__)
        "Clang " << __clang_major__ << "." << __clang_minor__
#else
        "Unknown"
#endif
        << std::endl;
    
    std::cout << "架構: " << 
#ifdef _WIN64
        "x64"
#else
        "x86"
#endif
        << std::endl;
    
    std::cout << "構建類型: " << 
#ifdef DEBUG
        "Debug"
#else
        "Release"
#endif
        << std::endl;
    
    std::cout << "=================" << std::endl;
}
