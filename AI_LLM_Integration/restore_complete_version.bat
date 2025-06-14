@echo off
chcp 65001 >nul
title 恢復完整版本並構建 - Jy技術團隊

echo ========================================================================
echo 恢復完整版本並構建 RANOnline AI LLM Integration
echo 版本: 3.0.0 Complete Edition
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================================================
echo.

color 0A

echo [進行中] 恢復完整版本的source文件...

:: 更新CMakeLists.txt使用完整版本
echo [進行中] 配置CMake使用完整版本...

:: 檢查完整版本文件是否存在
if not exist "main.cpp" (
    echo [X] 錯誤: 找不到 main.cpp
    pause
    exit /b 1
)

if not exist "LLMMainWindow.cpp" (
    echo [X] 錯誤: 找不到 LLMMainWindow.cpp
    pause
    exit /b 1
)

if not exist "LLMMainWindow.h" (
    echo [X] 錯誤: 找不到 LLMMainWindow.h
    pause
    exit /b 1
)

echo [√] 完整版本文件檢查通過

:: 檢查Qt6路徑
set "QT6_FOUND=false"
set "QT6_PATHS=C:\Qt\6.9.1\mingw_64;C:\Qt\6.8.0\mingw_64;C:\Qt\6.7.0\mingw_64;C:\Qt\6.6.0\mingw_64;C:\Qt\6.5.0\mingw_64"

for %%p in (%QT6_PATHS%) do (
    if exist "%%p\bin\qmake.exe" (
        set "QT6_PATH=%%p"
        set "QT6_FOUND=true"
        echo [√] 找到Qt6安裝路徑: %%p
        goto :qt6_found
    )
)

:qt6_found
if "%QT6_FOUND%"=="false" (
    echo [X] 錯誤: 未找到Qt6安裝
    pause
    exit /b 1
)

:: 檢查MinGW路徑
set "MINGW_FOUND=false"
set "MINGW_PATHS=C:\Qt\Tools\mingw1310_64;C:\Qt\Tools\mingw1120_64;C:\Qt\Tools\mingw1110_64;C:\Qt\Tools\mingw1100_64"

for %%p in (%MINGW_PATHS%) do (
    if exist "%%p\bin\gcc.exe" (
        set "MINGW_PATH=%%p"
        set "MINGW_FOUND=true"
        echo [√] 找到MinGW路徑: %%p
        goto :mingw_found
    )
)

:mingw_found
if "%MINGW_FOUND%"=="false" (
    echo [X] 錯誤: 未找到MinGW安裝
    pause
    exit /b 1
)

:: 設置路徑
set "PATH=%QT6_PATH%\bin;%MINGW_PATH%\bin;%PATH%"
set "CMAKE_PREFIX_PATH=%QT6_PATH%"

echo.
echo [√] 環境配置完成
echo     Qt6路徑: %QT6_PATH%
echo     MinGW路徑: %MINGW_PATH%
echo.

:: 創建構建目錄
if exist "build_complete" rmdir /s /q build_complete
mkdir build_complete
cd build_complete

echo [進行中] 正在配置CMake（完整版本）...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT6_PATH%" ^
    -DQt6_DIR="%QT6_PATH%\lib\cmake\Qt6" ^
    -DMAIN_SOURCE=main.cpp

if %ERRORLEVEL% neq 0 (
    echo [X] CMake配置失敗
    pause
    exit /b 1
)

echo [進行中] 正在編譯完整版本...
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo [X] 編譯失敗
    pause
    exit /b 1
)

echo.
echo [√] 編譯成功！
echo [進行中] 準備完整版本部署...

:: 創建部署目錄
if exist "deploy_complete" rmdir /s /q deploy_complete
mkdir deploy_complete

:: 複製可執行文件
copy "*.exe" "deploy_complete\" >nul
cd deploy_complete

:: 部署Qt依賴
"%QT6_PATH%\bin\windeployqt.exe" --qmldir "%QT6_PATH%\qml" *.exe

:: 複製額外的DLL
copy "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libstdc++-6.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libwinpthread-1.dll" . >nul 2>&1

cd ..

echo [√] 完整版本部署完成！
echo.
echo ========================================================================
echo 完整版本構建成功完成！
echo ========================================================================
echo 可執行文件位置: %cd%\deploy_complete\
echo.
echo 測試運行完整版本...
echo.

:: 測試運行
cd deploy_complete
start *.exe

echo [√] 完整版本程序已啟動！
echo.
echo 如果窗口正常顯示完整的AI界面（包含聊天、AI管理等功能），
echo 則完整版本恢復成功。
echo.
pause
