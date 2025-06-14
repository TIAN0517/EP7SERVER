@echo off
setlocal enabledelayedexpansion

echo 🔨 RAN Online AI管控系統 - 便攜式構建腳本
echo =============================================
echo.

:: 檢查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ❌ 錯誤: 未找到CMake，請先安裝CMake
    pause
    exit /b 1
)

:: 檢查編譯器
where mingw32-make >nul 2>&1
if errorlevel 1 (
    echo ❌ 錯誤: 未找到MinGW編譯器
    echo    請確保MinGW已安裝並在PATH中
    pause
    exit /b 1
)

:: 清理並創建構建目錄
if exist "Build" (
    echo 🧹 清理舊的構建目錄...
    rmdir /s /q "Build"
)
mkdir Build
cd Build

echo 🔧 配置CMake項目（便攜式模式）...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_PORTABLE=ON -DDEPLOY_QT=ON

if errorlevel 1 (
    echo ❌ CMake配置失敗
    cd ..
    pause
    exit /b 1
)

echo 🔨 編譯項目...
mingw32-make -j4

if errorlevel 1 (
    echo ❌ 編譯失敗
    cd ..
    pause
    exit /b 1
)

echo ✅ 編譯成功！

:: 檢查可執行文件
if exist "ai_llm_integration.exe" (
    echo 🎯 可執行文件已生成: ai_llm_integration.exe
    
    :: 獲取文件大小
    for %%i in (ai_llm_integration.exe) do (
        set /a size_kb=%%~zi/1024
        echo    文件大小: !size_kb! KB
    )
) else (
    echo ❌ 錯誤: 未找到可執行文件
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo 🚀 構建完成！接下來可以運行便攜式部署：
echo    deploy_portable.bat
echo.

:: 詢問是否立即部署
set /p deploy_now="是否立即創建便攜式包？(y/n): "
if /i "%deploy_now%"=="y" (
    echo.
    echo 🚀 啟動便攜式部署...
    call deploy_portable.bat
)

echo.
echo 🎉 構建腳本執行完畢！
pause
