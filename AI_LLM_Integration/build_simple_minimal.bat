@echo off
chcp 65001 >nul
title RANOnline AI Simple - 極簡版構建腳本

echo ========================================================================
echo RANOnline AI Simple - 極簡版構建腳本
echo 版本: 1.0.0 Simple Edition
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================================================
echo.

:: 設置顏色
color 0A

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
    echo 請安裝Qt6到以下路徑之一:
    for %%p in (%QT6_PATHS%) do echo   - %%p
    pause
    exit /b 1
)

:: 檢查MinGW路徑
set "MINGW_FOUND=false"
set "MINGW_PATHS=C:\Qt\Tools\mingw1310_64;C:\Qt\Tools\mingw1120_64;C:\Qt\Tools\mingw1110_64;C:\Qt\Tools\mingw1100_64;C:\Qt\Tools\mingw900_64"

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

:: 檢查源文件
if not exist "main_simple_minimal.cpp" (
    echo [X] 錯誤: 找不到 main_simple_minimal.cpp
    if exist "main_simple_ai.cpp" (
        echo [√] 找到 main_simple_ai.cpp，使用此文件
    ) else (
        echo [X] 也找不到 main_simple_ai.cpp
        pause
        exit /b 1
    )
)

:: 創建構建目錄
if exist "build_simple" rmdir /s /q build_simple
mkdir build_simple
cd build_simple

echo [進行中] 正在配置CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT6_PATH%" ^
    -DQt6_DIR="%QT6_PATH%\lib\cmake\Qt6"

if %ERRORLEVEL% neq 0 (
    echo [X] CMake配置失敗
    pause
    exit /b 1
)

echo [進行中] 正在編譯...
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo [X] 編譯失敗
    pause
    exit /b 1
)

echo.
echo [√] 編譯成功！
echo [進行中] 準備部署...

:: 創建部署目錄
if exist "deploy" rmdir /s /q deploy
mkdir deploy

:: 複製可執行文件
copy "ai_llm_integration_vm_simple.exe" "deploy\" >nul
if not exist "deploy\ai_llm_integration_vm_simple.exe" (
    echo [X] 錯誤: 找不到可執行文件
    pause
    exit /b 1
)

:: 部署Qt依賴
cd deploy
"%QT6_PATH%\bin\windeployqt.exe" ai_llm_integration_vm_simple.exe

:: 複製額外的DLL
copy "%QT6_PATH%\bin\Qt6Core.dll" . >nul 2>&1
copy "%QT6_PATH%\bin\Qt6Gui.dll" . >nul 2>&1
copy "%QT6_PATH%\bin\Qt6Widgets.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libstdc++-6.dll" . >nul 2>&1
copy "%MINGW_PATH%\bin\libwinpthread-1.dll" . >nul 2>&1

cd ..

echo [√] 部署完成！
echo.
echo ========================================================================
echo 構建成功完成！
echo ========================================================================
echo 可執行文件位置: %cd%\deploy\ai_llm_integration_vm_simple.exe
echo.
echo 測試運行...
echo.

:: 測試運行
cd deploy
start ai_llm_integration_vm_simple.exe

echo [√] 程序已啟動！如果窗口正常顯示，則構建成功。
echo.
pause
