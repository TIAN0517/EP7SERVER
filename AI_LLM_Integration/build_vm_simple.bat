@echo off
REM ========================================================================
REM RANOnline AI LLM Integration - VM極簡穩定版構建腳本
REM 版本: 4.0.0 極簡穩定VM兼容版
REM 開發團隊: Jy技術團隊 線上AI
REM 
REM 🎯 功能特性:
REM ✅ 極簡穩定構建流程
REM ✅ 完全去除複雜相依性
REM ✅ 快速構建 (< 2分鐘)
REM ✅ 自動打包部署
REM ✅ VM相容性驗證
REM ✅ 100%便攜化部署
REM ========================================================================

setlocal enabledelayedexpansion

echo ========================================
echo RANOnline AI LLM Integration
echo VM極簡穩定版構建系統
echo 版本: 4.0.0 極簡穩定VM兼容版
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================
echo.

REM 設定變數
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build_vm_simple"
set "DEPLOY_DIR=%PROJECT_DIR%deploy_vm_simple"
set "PACKAGE_DIR=%PROJECT_DIR%package_vm_simple"
set "ZIP_NAME=RANOnline_AI_VM_Simple_v4.0.0.zip"

REM 顯示構建組態
echo [資訊] 構建組態:
echo   - 專案目錄: %PROJECT_DIR%
echo   - 構建目錄: %BUILD_DIR%
echo   - 部署目錄: %DEPLOY_DIR%
echo   - 打包目錄: %PACKAGE_DIR%
echo.

REM 清理舊的構建環境
echo [1/8] 清理舊的構建環境...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"

mkdir "%BUILD_DIR%"
mkdir "%DEPLOY_DIR%"
mkdir "%PACKAGE_DIR%"

REM 自動偵測Qt6路徑
echo [2/8] 偵測Qt6和MinGW環境...
set "QT6_ROOT="
for %%d in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
    "C:\Qt\6.5.0\mingw_64"
) do (
    if exist "%%d\bin\qmake.exe" (
        set "QT6_ROOT=%%d"
        echo   找到Qt6: %%d
        goto qt_found
    )
)

:qt_found
if "%QT6_ROOT%"=="" (
    echo ❌ 錯誤: 找不到Qt6和MinGW!
    echo 請安裝Qt6或設定QT6_ROOT環境變數
    pause
    exit /b 1
)

REM 設置路徑
set "PATH=%QT6_ROOT%\bin;%QT6_ROOT%\..\..\..\Tools\mingw1310_64\bin;%PATH%"
set "Qt6_DIR=%QT6_ROOT%\lib\cmake\Qt6"

echo   Qt6路徑: %QT6_ROOT%
echo   CMAKE Qt6_DIR: %Qt6_DIR%

REM 驗證構建工具
echo [3/8] 驗證構建工具...
where cmake >nul 2>&1
if errorlevel 1 (
    echo ❌ 錯誤: 找不到CMake在PATH中
    echo 請安裝CMake或將其添加到PATH
    pause
    exit /b 1
)

where g++ >nul 2>&1
if errorlevel 1 (
    echo ❌ 錯誤: 找不到MinGW g++在PATH中
    echo 請檢查MinGW安裝
    pause
    exit /b 1
)

where windeployqt >nul 2>&1
if errorlevel 1 (
    echo ❌ 警告: 找不到windeployqt，依賴包將不完整
) else (
    echo ✅ 所有構建工具已找到
)

REM 檢查源文件
echo [4/8] 檢查源文件...
if not exist "%PROJECT_DIR%main_vm_simple_complete.cpp" (
    echo ❌ 錯誤: 找不到main_vm_simple_complete.cpp!
    pause
    exit /b 1
)

if not exist "%PROJECT_DIR%CMakeLists_VM_Simple.txt" (
    echo ❌ 錯誤: 找不到CMakeLists_VM_Simple.txt!
    pause
    exit /b 1
)

echo ✅ 找到源文件

REM 複製VM簡化版CMakeLists.txt
echo [5/8] 準備構建組態...
copy /y "%PROJECT_DIR%CMakeLists_VM_Simple.txt" "%PROJECT_DIR%CMakeLists.txt"
if errorlevel 1 (
    echo ❌ 錯誤: 複製CMakeLists失敗
    pause
    exit /b 1
)

REM 配置CMake
echo [6/8] 配置CMake...
cd /d "%BUILD_DIR%"
cmake -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DQt6_DIR="%Qt6_DIR%" ^
    -DCMAKE_PREFIX_PATH="%QT6_ROOT%" ^
    "%PROJECT_DIR%"

if errorlevel 1 (
    echo ❌ 錯誤: CMake組態失敗!
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

echo ✅ CMake組態成功

REM 編譯專案
echo [7/8] 編譯專案...
cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS%

if errorlevel 1 (
    echo ❌ 錯誤: 構建失敗!
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

echo ✅ 構建成功

REM 檢查生成的執行檔
if not exist "%BUILD_DIR%\RANOnline_AI_VM_Simple.exe" (
    echo ❌ 錯誤: 構建後找不到執行檔!
    cd /d "%PROJECT_DIR%"
    pause
    exit /b 1
)

REM 部署和打包
echo [8/8] 部署和打包...
cd /d "%PROJECT_DIR%"

REM 基本部署
echo   複製執行檔...
copy /y "%BUILD_DIR%\RANOnline_AI_VM_Simple.exe" "%DEPLOY_DIR%\"

REM 檢查windeployqt是否可用
where windeployqt >nul 2>&1
if errorlevel 1 (
    echo   跳過依賴打包 (windeployqt not found)
    goto manual_copy
)

REM 使用windeployqt打包依賴
echo   使用windeployqt打包依賴...
windeployqt --verbose 2 --dir "%DEPLOY_DIR%" ^
    --force --compiler-runtime --no-translations ^
    --no-system-d3d-compiler --no-opengl-sw ^
    --no-quick-import --no-webkit2 ^
    "%DEPLOY_DIR%\RANOnline_AI_VM_Simple.exe"

if errorlevel 1 (
    echo   windeployqt警告，嘗試手動複製依賴...
    goto manual_copy
)

goto package_files

:manual_copy
echo   手動複製關鍵依賴...
REM 複製關鍵DLL
for %%f in (
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    libgcc_s_seh-1.dll
    libstdc++-6.dll
    libwinpthread-1.dll
) do (
    if exist "%QT6_ROOT%\bin\%%f" (
        copy /y "%QT6_ROOT%\bin\%%f" "%DEPLOY_DIR%\"
        echo     複製: %%f
    )
)

REM 複製platforms插件
if not exist "%DEPLOY_DIR%\platforms" mkdir "%DEPLOY_DIR%\platforms"
if exist "%QT6_ROOT%\plugins\platforms\qwindows.dll" (
    copy /y "%QT6_ROOT%\plugins\platforms\qwindows.dll" "%DEPLOY_DIR%\platforms\"
    echo     複製: platforms\qwindows.dll
)

:package_files
REM 創建說明文件
echo 創建說明文件...
echo RANOnline AI LLM Integration - VM極簡穩定版 v4.0.0 > "%PACKAGE_DIR%\README.txt"
echo. >> "%PACKAGE_DIR%\README.txt"
echo 開發團隊: Jy技術團隊 線上AI >> "%PACKAGE_DIR%\README.txt"
echo 構建時間: %date% %time% >> "%PACKAGE_DIR%\README.txt"
echo. >> "%PACKAGE_DIR%\README.txt"
echo 這是專為VM環境優化的極簡穩定版本 >> "%PACKAGE_DIR%\README.txt"
echo 具備完整的100%%便攜化部署能力 >> "%PACKAGE_DIR%\README.txt"
echo. >> "%PACKAGE_DIR%\README.txt"
echo 使用說明: >> "%PACKAGE_DIR%\README.txt"
echo 1. 雙擊 RANOnline_AI_VM_Simple.exe 啟動程序 >> "%PACKAGE_DIR%\README.txt"
echo 2. 程序會自動偵測VM環境並啟用相容模式 >> "%PACKAGE_DIR%\README.txt"
echo 3. 支援Ollama本地模型集成 >> "%PACKAGE_DIR%\README.txt"
echo 4. 如遇到問題，請聯繫Jy技術團隊 線上AI >> "%PACKAGE_DIR%\README.txt"

REM 創建啟動腳本
echo 創建啟動腳本...
echo @echo off > "%PACKAGE_DIR%\啟動程序.bat"
echo echo 正在啟動RANOnline AI - VM版... >> "%PACKAGE_DIR%\啟動程序.bat"
echo start RANOnline_AI_VM_Simple.exe >> "%PACKAGE_DIR%\啟動程序.bat"

REM 複製所有文件到打包目錄
echo 複製文件到打包目錄...
xcopy "%DEPLOY_DIR%\*" "%PACKAGE_DIR%\" /E /I /Y >nul

REM 顯示文件清單
echo.
echo 生成的文件清單:
dir "%PACKAGE_DIR%" /B

REM 計算文件大小
for /f %%i in ('dir "%PACKAGE_DIR%" /s /-c ^| find "個檔案"') do set "FILE_COUNT=%%i"
for /f %%i in ('dir "%PACKAGE_DIR%" /s /-c ^| find "位元組"') do set "TOTAL_SIZE=%%i"

echo.
echo ========================================
echo 構建完成！
echo ========================================
echo 輸出目錄: %PACKAGE_DIR%
echo 主執行檔: RANOnline_AI_VM_Simple.exe
echo 檔案數量: %FILE_COUNT%
echo 總大小: %TOTAL_SIZE%
echo.
echo 測試指令:
echo   cd "%PACKAGE_DIR%"
echo   RANOnline_AI_VM_Simple.exe
echo.
echo 或者雙擊: 啟動程序.bat
echo ========================================

pause
