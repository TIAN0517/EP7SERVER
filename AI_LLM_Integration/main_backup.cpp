#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include <iostream>

#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#include "LLMMainWindow.h"

/**
 * @brief 設置控制台輸出編碼（僅在Debug模式下啟用）
 */
void setupConsoleOutput()
{
#ifdef Q_OS_WIN
#ifdef _DEBUG
    // 僅在Debug模式下分配控制台
    if (AllocConsole()) {
        // 重定向標準輸出到控制台
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        
        // 設置控制台標題
        SetConsoleTitleA("RANOnline AI LLM Integration - Debug Console");
        
        // 設置控制台文字模式為UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        
        // 同步C++流與C流
        std::ios::sync_with_stdio(true);
    }
#endif
#endif
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Qt6 默認使用UTF-8，不需要設置編碼
    
    // 設置控制台編碼（僅Debug模式）
    setupConsoleOutput();
    
    // 設置應用程式資訊
    app.setApplicationName("蘭線上 AI 大型語言模型整合系統");
    app.setApplicationDisplayName("RANOnline AI LLM Integration");
    app.setApplicationVersion("3.0.0");
    app.setOrganizationName("Jy技術團隊");
    app.setOrganizationDomain("ranonline.ai");
    
    // 設置應用程式圖標
    app.setWindowIcon(QIcon(":/icons/jy1.ico"));
    
    // 載入繁體中文翻譯
    QTranslator translator;
    if (translator.load(":/translations/zh_TW.qm")) {
        app.installTranslator(&translator);
    }
    
    // 創建主窗口
    LLMMainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}