// ========================================================================
// RANOnline AI LLM Integration - VM相容版簡化主程式
// 版本: 3.1.0 VM專用版 (簡化版)
// 開發團隊: Jy技術團隊
// 
// 🎯 VM/雲端主機優化特性 (最小依賴版本):
// ✅ 強制軟體渲染 (禁用硬體加速)
// ✅ 禁用DirectX/OpenGL硬體依賴
// ✅ 最小化DPI縮放問題
// ✅ 基本異常處理
// ✅ VM環境適配
// ========================================================================

#include <QApplication>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QIcon>
#include <QDir>
#include <QMessageBox>
#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QFont>
#include <iostream>
#include <memory>
#include <exception>

// Windows 特定標頭檔
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "LLMMainWindow.h"

// ========================================================================
// VM相容性設置函數 (簡化版)
// ========================================================================
void setupVMCompatibility() {
    // 1. 強制軟體渲染 - 這是最重要的設置
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
    
    // 2. 禁用硬體加速相關功能
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL, false);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES, false);
    
    // 3. 圖形相關優化
    QCoreApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents, true);
    QCoreApplication::setAttribute(Qt::AA_CompressTabletEvents, true);
    
    // 4. 禁用不必要的功能以提高穩定性
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache, true);
    
    // 5. 設置環境變數強制軟體渲染
    qputenv("QT_OPENGL", "software");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_SCALE_FACTOR", "1");
    qputenv("QT_QPA_PLATFORM", "windows");
}

// ========================================================================
// 軟體渲染設置 (簡化版)
// ========================================================================
void setupSoftwareRendering() {
    // 設置軟體渲染的Surface格式
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(2, 1);  // 使用較低版本確保相容性
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSamples(0);  // 禁用MSAA以提高相容性
    QSurfaceFormat::setDefaultFormat(format);
}

// ========================================================================
// 主程式進入點 (簡化版)
// ========================================================================
int main(int argc, char *argv[])
{
    try {
        // 1. 早期VM相容性設置 (必須在QApplication創建前)
        setupVMCompatibility();
        setupSoftwareRendering();
        
        // 2. 創建應用程式實例
        QApplication app(argc, argv);
        
        // 3. 應用程式基本信息
        app.setApplicationName("Jy技術團隊 線上AI");
        app.setApplicationDisplayName("Jy技術團隊 線上AI - VM相容版");
        app.setApplicationVersion("3.1.0");
        app.setOrganizationName("Jy技術團隊");
        app.setOrganizationDomain("jytech.com");
        
        // 4. 設置應用程式圖標 (簡化版)
        QString iconPath = ":/icons/jy1.ico";
        if (QFile::exists(iconPath)) {
            app.setWindowIcon(QIcon(iconPath));
        } else {
            // 嘗試本地圖標文件
            QStringList iconPaths = {"./jy1.ico", "./Resources/jy1.ico"};
            for (const QString& path : iconPaths) {
                if (QFile::exists(path)) {
                    app.setWindowIcon(QIcon(path));
                    break;
                }
            }
        }
        
        // 5. 設置VM安全的樣式
        app.setStyle("Fusion");  // Fusion樣式在VM中最穩定
        
        // 6. 設置字體 (簡化版)
        QFont defaultFont("Microsoft YaHei UI", 9);
        defaultFont.setHintingPreference(QFont::PreferFullHinting);
        app.setFont(defaultFont);
        
        // 7. 創建並顯示主窗口
        std::unique_ptr<LLMMainWindow> mainWindow = std::make_unique<LLMMainWindow>();
        
        // VM環境特殊設置
        mainWindow->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | 
                                 Qt::WindowMinimizeButtonHint);
        
        // 確保窗口在VM中正確顯示
        mainWindow->show();
        mainWindow->raise();
        mainWindow->activateWindow();
        
        // 8. 進入事件循環
        qDebug() << "VM Compatible Application started successfully";
        return app.exec();
        
    } catch (const std::exception& e) {
        QString errorMsg = QString("應用程式啟動失敗: %1").arg(e.what());
        
#ifdef Q_OS_WIN
        MessageBoxA(nullptr, errorMsg.toLocal8Bit().data(), 
                   "RANOnline AI - 啟動錯誤", MB_OK | MB_ICONERROR);
#endif
        return -1;
    }
}
