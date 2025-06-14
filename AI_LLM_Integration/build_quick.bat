@echo off
REM ========================================================================
REM RANOnline AI LLM Integration - 極簡穩定VM兼容版 快速構建腳本
REM 版本: 4.0.0 Ultimate Portable Edition
REM 開發團隊: Jy技術團隊 線上AI
REM ========================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================================================
echo  Jy技術團隊 線上AI - 極簡穩定VM兼容版 快速構建
echo  版本: 4.0.0 Ultimate Portable Edition
echo ========================================================================
echo.

REM 設定基本變數
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build_quick"
set "DEPLOY_DIR=%PROJECT_DIR%deploy_quick"

echo [1/6] 環境準備...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
mkdir "%BUILD_DIR%"
mkdir "%DEPLOY_DIR%"

echo [2/6] 尋找Qt6...
set "QT6_ROOT="
for %%d in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
) do (
    if exist "%%d\bin\qmake.exe" (
        set "QT6_ROOT=%%d"
        echo   找到Qt6: %%d
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

echo [3/6] 準備源文件...
if exist "%PROJECT_DIR%main_ultimate_portable.cpp" (
    set "MAIN_SOURCE=main_ultimate_portable.cpp"
) else if exist "%PROJECT_DIR%main_vm_simple_complete.cpp" (
    set "MAIN_SOURCE=main_vm_simple_complete.cpp"
) else (
    echo ❌ 錯誤: 找不到源文件
    pause
    exit /b 1
)
echo   使用源文件: %MAIN_SOURCE%

echo [4/6] 創建簡化CMakeLists.txt...
(
echo cmake_minimum_required(VERSION 3.16^)
echo project(RANOnline_AI_Quick VERSION 4.0.0 LANGUAGES CXX^)
echo.
echo set(CMAKE_CXX_STANDARD 17^)
echo set(CMAKE_CXX_STANDARD_REQUIRED ON^)
echo set(CMAKE_AUTOMOC ON^)
echo.
echo find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network^)
echo.
echo add_executable(RANOnline_AI_Quick WIN32 %MAIN_SOURCE%^)
echo.
echo target_link_libraries(RANOnline_AI_Quick
echo     Qt6::Core
echo     Qt6::Widgets  
echo     Qt6::Gui
echo     Qt6::Network
echo ^)
echo.
echo target_compile_definitions(RANOnline_AI_Quick PRIVATE
echo     QT_DISABLE_DEPRECATED_BEFORE=0x060000
echo     UNICODE
echo     _UNICODE
echo     VM_COMPATIBLE_BUILD=1
echo ^)
) > "%BUILD_DIR%\CMakeLists.txt"

echo [5/6] 編譯...
cd /d "%BUILD_DIR%"
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

echo [6/6] 部署...
cd /d "%PROJECT_DIR%"
copy /y "%BUILD_DIR%\RANOnline_AI_Quick.exe" "%DEPLOY_DIR%\RANOnline_AI_Ultimate_Portable.exe"

REM 複製Qt庫
for %%f in (Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll) do (
    if exist "%QT6_ROOT%\bin\%%f" copy /y "%QT6_ROOT%\bin\%%f" "%DEPLOY_DIR%\"
)

REM 複製MinGW庫
for %%f in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%QT6_ROOT%\..\..\..\Tools\mingw1310_64\bin\%%f" copy /y "%QT6_ROOT%\..\..\..\Tools\mingw1310_64\bin\%%f" "%DEPLOY_DIR%\"
)

REM 複製平台插件
if not exist "%DEPLOY_DIR%\platforms" mkdir "%DEPLOY_DIR%\platforms"
if exist "%QT6_ROOT%\plugins\platforms\qwindows.dll" copy /y "%QT6_ROOT%\plugins\platforms\qwindows.dll" "%DEPLOY_DIR%\platforms\"

echo.
echo ✅ 構建完成！
echo 📁 輸出目錄: %DEPLOY_DIR%
echo 🚀 主程序: RANOnline_AI_Ultimate_Portable.exe
echo.

REM 創建啟動腳本
echo @echo off > "%DEPLOY_DIR%\一鍵啟動.bat"
echo echo 正在啟動 Jy技術團隊 線上AI... >> "%DEPLOY_DIR%\一鍵啟動.bat"
echo start RANOnline_AI_Ultimate_Portable.exe >> "%DEPLOY_DIR%\一鍵啟動.bat"

echo @echo off > "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo 正在關閉 Jy技術團隊 線上AI... >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo echo 程序已關閉 >> "%DEPLOY_DIR%\一鍵關閉.bat"
echo pause >> "%DEPLOY_DIR%\一鍵關閉.bat"

REM 創建README
(
echo Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo =======================================================
echo.
echo 🎯 100%% VM兼容的AI助手
echo.
echo 🚀 使用方式:
echo 1. 雙擊 "一鍵啟動.bat" 啟動程序
echo 2. 雙擊 "一鍵關閉.bat" 關閉程序
echo.
echo ✨ 特性:
echo • 完全便攜化，無需安裝
echo • VM/RDP/雲桌面完全兼容
echo • 強制軟體渲染，避免GPU問題
echo • 支持Ollama本地模型
echo.
echo 開發團隊: Jy技術團隊 線上AI
echo 構建時間: %date% %time%
) > "%DEPLOY_DIR%\README.txt"

echo 📖 說明文檔: README.txt
echo.
set /p test_now="是否立即測試程序？ (y/N): "
if /i "!test_now!"=="y" (
    echo 啟動程序進行測試...
    cd "%DEPLOY_DIR%"
    start "" "一鍵啟動.bat"
    cd /d "%PROJECT_DIR%"
)

echo.
echo 按任意鍵退出...
pause >nul
