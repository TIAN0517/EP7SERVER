// ========================================================================
// RANOnline AI LLM Integration - VM相容版主程式進入點
// 版本: 3.1.0 VM專用版
// 開發團隊: Jy技術團隊
// 
// 🎯 VM/雲端主機優化特性:
// ✅ 強制軟體渲染 (禁用硬體加速)
// ✅ 禁用DirectX/OpenGL硬體依賴
// ✅ 最小化DPI縮放問題
// ✅ 增強異常處理與錯誤恢復
// ✅ VM環境自動偵測與適配
// ✅ 軟體渲染後備方案
// ========================================================================

#include <QApplication>
#include <QGuiApplication>
#include <QCoreApplication>
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
#include <QSysInfo>
#include <QPalette>
#include <QFont>
#include <QFontInfo>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <iostream>
#include <memory>
#include <exception>

// Windows 特定標頭檔
#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <winerror.h>
#endif

#include "LLMMainWindow.h"

// ========================================================================
// VM環境偵測函數
// ========================================================================
bool isRunningInVM() {
    bool isVM = false;
    
#ifdef Q_OS_WIN
    // 檢查常見VM標識
    std::vector<std::string> vmIndicators = {
        "VBOX", "VMWARE", "QEMU", "VIRTUAL", "HYPER-V", "XEN"
    };
    
    // 檢查系統製造商
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                      "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", 
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256];
        DWORD bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "SystemManufacturer", nullptr, nullptr, 
                            (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            std::string manufacturer(buffer);
            std::transform(manufacturer.begin(), manufacturer.end(), 
                          manufacturer.begin(), ::toupper);
            
            for (const auto& indicator : vmIndicators) {
                if (manufacturer.find(indicator) != std::string::npos) {
                    isVM = true;
                    break;
                }
            }
        }
        RegCloseKey(hKey);
    }
    
    // 檢查CPU型號 (VM通常有特殊標識)
    if (!isVM) {
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                          "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char buffer[256];
            DWORD bufferSize = sizeof(buffer);
            if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr, 
                                (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
                std::string processor(buffer);
                std::transform(processor.begin(), processor.end(), 
                              processor.begin(), ::toupper);
                
                for (const auto& indicator : vmIndicators) {
                    if (processor.find(indicator) != std::string::npos) {
                        isVM = true;
                        break;
                    }
                }
            }
            RegCloseKey(hKey);
        }
    }
#endif
    
    return isVM;
}

// ========================================================================
// VM相容性設置函數
// ========================================================================
void setupVMCompatibility() {
    // 1. 強制軟體渲染 - 這是最重要的設置
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
    
    // 2. 禁用硬體加速相關功能
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL, false);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES, false);
      // 3. DPI縮放設置 (VM環境通常需要禁用) - Qt6.9兼容
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);
    #endif
    
    // 4. 圖形相關優化
    QCoreApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents, true);
    QCoreApplication::setAttribute(Qt::AA_CompressTabletEvents, true);
    
    // 5. 禁用不必要的功能以提高穩定性
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache, true);
    
    // 6. 設置環境變數強制軟體渲染
    qputenv("QT_OPENGL", "software");
    qputenv("QT_ANGLE_PLATFORM", "d3d11");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_SCALE_FACTOR", "1");
    qputenv("QT_SCREEN_SCALE_FACTORS", "1");
    
    // 7. 強制使用基本平台插件
    qputenv("QT_QPA_PLATFORM", "windows");
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", "platforms");
}

// ========================================================================
// 軟體渲染後備設置
// ========================================================================
void setupSoftwareRenderingFallback() {
    // 設置軟體渲染的Surface格式
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(2, 1);  // 使用較低版本確保相容性
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlphaBufferSize(8);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(0);  // 禁用MSAA以提高相容性
    QSurfaceFormat::setDefaultFormat(format);
}

// ========================================================================
// 主程式進入點
// ========================================================================
int main(int argc, char *argv[])
{
    try {
        // 1. 早期VM相容性設置 (必須在QApplication創建前)
        setupVMCompatibility();
        setupSoftwareRenderingFallback();
        
        // 2. 創建應用程式實例
        QApplication app(argc, argv);
        
        // 3. 應用程式基本信息
        app.setApplicationName("Jy技術團隊 線上AI");
        app.setApplicationDisplayName("Jy技術團隊 線上AI - VM相容版");
        app.setApplicationVersion("3.1.0");
        app.setOrganizationName("Jy技術團隊");
        app.setOrganizationDomain("jytech.com");
        
        // 4. 設置應用程式圖標
        QString iconPath = ":/icons/jy1.ico";
        if (QFile::exists(iconPath)) {
            app.setWindowIcon(QIcon(iconPath));
        }
        
        // 5. 檢查VM環境
        if (isRunningInVM()) {
            qDebug() << "VM Environment detected - Running in compatibility mode";
        }
        
        // 6. 設置樣式 (VM安全)
        app.setStyle("Fusion");  // Fusion樣式在VM中最穩定
        
        // 7. 創建並顯示主窗口
        std::unique_ptr<LLMMainWindow> mainWindow = std::make_unique<LLMMainWindow>();
        mainWindow->show();
        
        // 8. 進入事件循環
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
