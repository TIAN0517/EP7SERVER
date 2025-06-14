@echo off
REM ========================================================================
REM RANOnline AI LLM Integration - 極簡VM兼容版 終極構建腳本
REM 版本: 4.0.0 Ultimate Simple Edition
REM 開發團隊: Jy技術團隊 線上AI
REM ========================================================================

echo.
echo ========================================================================
echo  Jy技術團隊 線上AI - 極簡VM兼容版 終極構建
echo  版本: 4.0.0 Ultimate Simple Edition
echo ========================================================================
echo.

REM 設定基本變數
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build_simple"
set "DEPLOY_DIR=%PROJECT_DIR%deploy_simple"

echo [1/5] 環境準備...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
mkdir "%BUILD_DIR%"
mkdir "%DEPLOY_DIR%"

echo [2/5] 尋找Qt6...
set "QT6_ROOT="
for %%d in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
) do (
    if exist "%%d\bin\qmake.exe" (
        set "QT6_ROOT=%%d"
        echo   ✅ 找到Qt6: %%d
        goto qt_found
    )
)

echo ❌ 錯誤: 找不到Qt6
pause
exit /b 1

:qt_found
REM 設定環境
set "PATH=%QT6_ROOT%\bin;%QT6_ROOT%\..\..\..\Tools\mingw1310_64\bin;%PATH%"
set "Qt6_DIR=%QT6_ROOT%\lib\cmake\Qt6"

echo [3/5] 創建簡化CMakeLists.txt...
(
echo cmake_minimum_required(VERSION 3.16^)
echo project(RANOnline_AI_Simple VERSION 4.0.0 LANGUAGES CXX^)
echo.
echo # C++標準
echo set(CMAKE_CXX_STANDARD 17^)
echo set(CMAKE_CXX_STANDARD_REQUIRED ON^)
echo.
echo # Qt設定
echo set(CMAKE_AUTOMOC ON^)
echo set(CMAKE_AUTORCC ON^)
echo set(CMAKE_AUTOUIC ON^)
echo.
echo # 尋找Qt6
echo find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui^)
echo.
echo # MinGW特定設定
echo if(MINGW^)
echo     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17"^)
echo     set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG"^)
echo endif(^)
echo.
echo # 創建可執行文件
echo add_executable(RANOnline_AI_Simple WIN32 main_simple_ai.cpp^)
echo.
echo # 鏈接Qt庫
echo target_link_libraries(RANOnline_AI_Simple
echo     Qt6::Core
echo     Qt6::Widgets  
echo     Qt6::Gui
echo ^)
echo.
echo # Windows特定鏈接庫
echo if(WIN32^)
echo     target_link_libraries(RANOnline_AI_Simple user32 gdi32 shell32^)
echo endif(^)
echo.
echo # 編譯定義
echo target_compile_definitions(RANOnline_AI_Simple PRIVATE
echo     QT_DISABLE_DEPRECATED_BEFORE=0x060000
echo     UNICODE
echo     _UNICODE
echo     VM_COMPATIBLE_BUILD=1
echo     FORCE_SOFTWARE_RENDERING=1
echo ^)
) > "%BUILD_DIR%\CMakeLists.txt"

echo [4/5] 編譯...
cd /d "%BUILD_DIR%"

REM 複製源文件
copy /y "%PROJECT_DIR%main_simple_ai.cpp" "%BUILD_DIR%\"

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DQt6_DIR="%Qt6_DIR%" "%BUILD_DIR%"
if errorlevel 1 (
    echo ❌ CMake配置失敗
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
    echo ❌ 編譯失敗
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

echo [5/5] 部署...
cd /d "%PROJECT_DIR%"

REM 複製主程序
if exist "%BUILD_DIR%\RANOnline_AI_Simple.exe" (
    copy /y "%BUILD_DIR%\RANOnline_AI_Simple.exe" "%DEPLOY_DIR%\RANOnline_AI_Ultimate_Portable.exe"
    echo   ✅ 主程序已複製
) else (
    echo ❌ 找不到編譯後的執行文件
    pause
    exit /b 1
)

REM 複製Qt核心庫
echo   📦 正在複製Qt依賴庫...
for %%f in (Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll) do (
    if exist "%QT6_ROOT%\bin\%%f" (
        copy /y "%QT6_ROOT%\bin\%%f" "%DEPLOY_DIR%\"
        echo     ✅ %%f
    ) else (
        echo     ❌ 找不到 %%f
    )
)

REM 複製MinGW運行庫
echo   📦 正在複製MinGW運行庫...
set "MINGW_BIN=%QT6_ROOT%\..\..\..\Tools\mingw1310_64\bin"
for %%f in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%MINGW_BIN%\%%f" (
        copy /y "%MINGW_BIN%\%%f" "%DEPLOY_DIR%\"
        echo     ✅ %%f
    ) else (
        echo     ⚠️  找不到 %%f
    )
)

REM 複製平台插件
echo   📦 正在複製平台插件...
if not exist "%DEPLOY_DIR%\platforms" mkdir "%DEPLOY_DIR%\platforms"
if exist "%QT6_ROOT%\plugins\platforms\qwindows.dll" (
    copy /y "%QT6_ROOT%\plugins\platforms\qwindows.dll" "%DEPLOY_DIR%\platforms\"
    echo     ✅ platforms\qwindows.dll
) else (
    echo     ❌ 找不到 qwindows.dll
)

REM 創建啟動和關閉腳本
echo   📝 創建控制腳本...

echo @echo off > "%DEPLOY_DIR%\一鍵啟動.bat"
echo title Jy技術團隊 線上AI - 極簡VM兼容版 >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo. >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo ======================================== >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo  Jy技術團隊 線上AI 正在啟動... >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo  極簡穩定VM兼容版 v4.0.0 >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo ======================================== >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo. >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo 🚀 正在啟動AI助手，請稍候... >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo timeout /t 2 /nobreak ^>nul >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo start "" "RANOnline_AI_Ultimate_Portable.exe" >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo ✅ AI助手已啟動 >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo. >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo 💡 提示: 可直接關閉此視窗 >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo timeout /t 3 /nobreak ^>nul >> "%DEPLOY_DIR%\一鍵啟動.bat"

echo @echo off > "%DEPLOY_DIR%\一鍵關閉.bat"
echo title 一鍵關閉 - Jy技術團隊 線上AI >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo. >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo ======================================== >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo  正在關閉 Jy技術團隊 線上AI >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo  極簡穩定VM兼容版 v4.0.0 >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo ======================================== >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo. >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo 🛑 正在關閉所有相關進程... >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo if errorlevel 1 ^( >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo     echo ✅ 沒有找到運行中的AI助手進程 >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo ^) else ^( >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo     echo ✅ AI助手已成功關閉 >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo ^) >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo. >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo 👋 感謝使用 Jy技術團隊 線上AI >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo pause >> "%DEPLOY_DIR%\一鍵關閉.bat"

REM 創建README檔案
echo   📖 創建說明文檔...
(
echo Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo =======================================================
echo.
echo 🎯 專為VM和雲桌面環境設計的AI助手
echo.
echo 🚀 快速使用:
echo 1. 雙擊 "一鍵啟動.bat" 啟動程序
echo 2. 雙擊 "一鍵關閉.bat" 關閉程序
echo 3. 或直接執行 "RANOnline_AI_Ultimate_Portable.exe"
echo.
echo ✨ 核心特性:
echo • 100%% 便攜化，無需安裝任何依賴
echo • VM/RDP/雲桌面完全兼容
echo • 強制軟體渲染，避免GPU相關問題
echo • 智能環境檢測和自適應
echo • 系統原生UI，極致穩定
echo • 完整錯誤處理和恢復機制
echo.
echo 🛠️ 故障排除:
echo • 如程序無法啟動，請嘗試以管理員身份運行
echo • 在VM環境中會自動啟用安全模式
echo • 支持Windows 10/11 和 Windows Server
echo • 如缺少DLL，請檢查platforms資料夾
echo.
echo 📞 技術支援:
echo 開發團隊: Jy技術團隊 線上AI
echo 郵箱: jytech@example.com
echo 構建時間: %date% %time%
echo 構建版本: v4.0.0 Ultimate Simple Edition
echo.
echo 🎮 使用指令:
echo • 輸入 "時間" 查看當前時間
echo • 輸入 "系統" 查看系統資訊  
echo • 輸入 "幫助" 查看所有可用指令
echo • 或直接與AI助手對話
) > "%DEPLOY_DIR%\README.txt"

REM 檢查部署結果
echo.
echo ========================================================================
echo ✅ 構建完成！
echo ========================================================================
echo.
echo 📁 輸出目錄: %DEPLOY_DIR%
echo 🚀 主程序: RANOnline_AI_Ultimate_Portable.exe  
echo 🎮 一鍵啟動: 一鍵啟動.bat
echo 🛑 一鍵關閉: 一鍵關閉.bat
echo 📖 說明文檔: README.txt
echo.

REM 統計檔案
echo 📊 部署統計:
for /f %%i in ('dir "%DEPLOY_DIR%" /s /-c ^| find "個檔案"') do echo   檔案數量: %%i
for /f %%i in ('dir "%DEPLOY_DIR%" /s /-c ^| find "位元組" ^| find /v "個檔案"') do echo   總大小: %%i

echo.
echo 🎯 VM兼容特性:
echo   ✅ 強制軟體渲染
echo   ✅ 智能環境檢測  
echo   ✅ 100%便攜化部署
echo   ✅ 完整錯誤處理
echo.

REM 詢問是否立即測試
set /p test_now="是否立即測試程序？ (y/N): "
if /i "!test_now!"=="y" (
    echo 🚀 正在啟動程序進行測試...
    cd "%DEPLOY_DIR%"
    start "" "一鍵啟動.bat"
    cd /d "%PROJECT_DIR%"
    echo.
    echo 💡 程序已啟動，請在新視窗中測試功能
) else (
    echo 👍 您可以稍後手動測試程序
)

echo.
echo 構建完成時間: %date% %time%
echo 感謝使用 Jy技術團隊 線上AI 構建系統！
echo.
echo 按任意鍵退出...
pause >nul
