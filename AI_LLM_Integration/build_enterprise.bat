@echo off
REM ========================================================================
REM RANOnline AI LLM Integration - 企業級構建腳本 (批次檔版本)
REM 版本: 3.0.0 企業版
REM 開發團隊: Jy技術團隊
REM 支援: Windows 10/11 + Qt6 + MinGW64
REM 
REM 🎯 BossJy 企業級標準 - 零配置構建系統
REM ========================================================================

echo.
echo 🎯 RANOnline AI LLM Integration - 企業級構建系統啟動
echo ========================================================================

REM 設置代碼頁為UTF-8
chcp 65001 >nul

REM 檢查CMakeLists.txt
if not exist "CMakeLists.txt" (
    echo ❌ 未找到 CMakeLists.txt 文件
    pause
    exit /b 1
)

echo ✅ 找到專案配置文件

REM 自動偵測Qt6路徑
set QT_ROOT=
for %%p in (
    "C:\Qt\6.9.1\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
    "C:\Qt\6.5.0\mingw_64"
    "%QTDIR%"
    "%QT_DIR%"
) do (
    if exist "%%~p\lib\cmake\Qt6" (
        set QT_ROOT=%%~p
        echo ✅ 自動偵測到 Qt6: %%~p
        goto :qt_found
    )
)

echo ❌ 未找到 Qt6 安裝！
echo    請安裝 Qt6 或設置 QTDIR 環境變數
pause
exit /b 1

:qt_found

REM 自動偵測MinGW
set MINGW_ROOT=
for %%p in (
    "C:\Qt\Tools\mingw1310_64\bin"
    "%QT_ROOT%\bin"
    "C:\Qt\Tools\mingw1120_64\bin"
    "C:\Qt\Tools\mingw1130_64\bin"
    "C:\mingw64\bin"
    "C:\msys64\mingw64\bin"
) do (
    if exist "%%~p\gcc.exe" (
        set MINGW_ROOT=%%~p
        echo ✅ 自動偵測到 MinGW: %%~p
        goto :mingw_found
    )
)

echo ❌ 未找到 MinGW 編譯器！
echo    請安裝 Qt6 with MinGW 或單獨安裝 MinGW64
pause
exit /b 1

:mingw_found

REM 設置環境變數
set PATH=%QT_ROOT%\bin;%MINGW_ROOT%;%PATH%
set CMAKE_PREFIX_PATH=%QT_ROOT%

REM 檢查構建工具
echo.
echo 🔧 檢查構建工具...

cmake --version >nul 2>&1
if errorlevel 1 (
    echo ❌ CMake 未安裝或未在 PATH 中
    pause
    exit /b 1
)
echo ✅ CMake 可用

gcc --version >nul 2>&1
if errorlevel 1 (
    echo ❌ GCC 編譯器不可用
    pause
    exit /b 1
)
echo ✅ GCC 可用

REM 創建構建目錄
echo.
echo 🏗️ 準備構建目錄...

if exist "build" rmdir /s /q "build"
mkdir "build"
echo ✅ 構建目錄已創建

REM CMake 配置
echo.
echo ⚙️ CMake 配置階段...
cd build

cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH=%QT_ROOT% ^
    -DQT_ROOT=%QT_ROOT% ^
    -DBUILD_PORTABLE=ON ^
    -DDEPLOY_QT=ON

if errorlevel 1 (
    echo ❌ CMake 配置失敗
    cd ..
    pause
    exit /b 1
)
echo ✅ CMake 配置完成

REM 編譯
echo.
echo 🔨 編譯階段...

cmake --build . --config Release --parallel

if errorlevel 1 (
    echo ❌ 編譯失敗
    cd ..
    pause
    exit /b 1
)
echo ✅ 編譯完成

cd ..

REM 檢查可執行文件
if not exist "build\ai_llm_integration.exe" (
    echo ❌ 可執行文件未生成
    pause
    exit /b 1
)
echo ✅ 可執行文件已生成

REM 創建部署目錄
echo.
echo 📦 部署階段...

if exist "deploy" rmdir /s /q "deploy"
mkdir "deploy"

copy "build\ai_llm_integration.exe" "deploy\"
echo ✅ 主程式已複製

REM 使用 windeployqt 部署
set WINDEPLOYQT=%QT_ROOT%\bin\windeployqt.exe
if exist "%WINDEPLOYQT%" (
    echo 🔧 使用 windeployqt 部署 Qt 依賴...
    cd deploy
    "%WINDEPLOYQT%" --compiler-runtime --force ai_llm_integration.exe
    cd ..
    echo ✅ Qt 依賴部署完成
) else (
    echo ⚠️ windeployqt 不可用，手動複製關鍵 DLL...
    
    REM 手動複製Qt DLL
    for %%d in (Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll) do (
        if exist "%QT_ROOT%\bin\%%d" (
            copy "%QT_ROOT%\bin\%%d" "deploy\"
            echo ✅ 已複製: %%d
        )
    )
    
    REM 複製MinGW運行時
    for %%d in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
        if exist "%MINGW_ROOT%\%%d" (
            copy "%MINGW_ROOT%\%%d" "deploy\"
            echo ✅ 已複製: %%d
        )
    )
)

REM 創建便攜版本
echo.
echo 📦 創建便攜版本...

if exist "portable" rmdir /s /q "portable"
mkdir "portable"

xcopy "deploy\*" "portable\" /e /i /h /y >nul
echo ✅ 便攜版本文件已複製

REM 創建說明文件
echo # RANOnline AI LLM Integration - 便攜版本 > "portable\README.txt"
echo. >> "portable\README.txt"
echo ## 使用說明 >> "portable\README.txt"
echo 1. 雙擊 ai_llm_integration.exe 啟動應用程式 >> "portable\README.txt"
echo 2. 首次啟動可能需要幾秒鐘初始化 >> "portable\README.txt"
echo 3. 確保系統已安裝 Microsoft Visual C++ Redistributable >> "portable\README.txt"
echo. >> "portable\README.txt"
echo ## 技術支援 >> "portable\README.txt"
echo - 開發團隊: Jy技術團隊 >> "portable\README.txt"
echo - 版本: 3.0.0 企業版 >> "portable\README.txt"

echo ✅ 說明文件已創建

REM 完成報告
echo.
echo ========================================================================
echo 🎉 企業級構建流程已完成！
echo.
echo 構建結果:
echo   - 構建目錄: build\
echo   - 部署目錄: deploy\
echo   - 便攜目錄: portable\
echo.
echo 下一步操作:
echo 1. 測試運行: cd deploy ^&^& ai_llm_integration.exe
echo   2. 分發便攜版本: 打包 portable\ 目錄
echo.
echo ✅ BossJy企業級標準構建完成！ 🚀
echo ========================================================================

pause
