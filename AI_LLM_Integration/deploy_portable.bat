@echo off
setlocal enabledelayedexpansion

echo 🚀 RAN Online AI管控系統 - 便攜式部署腳本
echo ===========================================
echo.

:: 檢查構建目錄
if not exist "Build\ai_llm_integration.exe" (
    echo ❌ 錯誤: 未找到可執行文件，請先編譯項目
    echo    運行: cmake --build Build --config Release
    pause
    exit /b 1
)

:: 創建部署目錄
set DEPLOY_DIR=Deploy_Portable
if exist "%DEPLOY_DIR%" (
    echo 🧹 清理舊的部署目錄...
    rmdir /s /q "%DEPLOY_DIR%"
)
mkdir "%DEPLOY_DIR%"

echo 📦 複製主程式...
copy "Build\ai_llm_integration.exe" "%DEPLOY_DIR%\" >nul

:: 查找Qt安裝路徑
set QT_DIR=
for /f "tokens=*" %%i in ('where qmake 2^>nul') do (
    set QT_BIN=%%i
    for %%j in ("!QT_BIN!") do set QT_DIR=%%~dpj
    goto :found_qt
)

echo ⚠️  警告: 未找到Qt安裝路徑，嘗試常見位置...
if exist "C:\Qt\6.9.1\mingw_64\bin\windeployqt.exe" (
    set QT_DIR=C:\Qt\6.9.1\mingw_64\bin\
    goto :found_qt
)
if exist "C:\Qt\6.8.0\mingw_64\bin\windeployqt.exe" (
    set QT_DIR=C:\Qt\6.8.0\mingw_64\bin\
    goto :found_qt
)

echo ❌ 錯誤: 未找到windeployqt工具
echo    請確保Qt已正確安裝並在PATH中
pause
exit /b 1

:found_qt
echo ✅ 找到Qt工具: %QT_DIR%

:: 使用windeployqt部署Qt依賴
echo 📚 部署Qt庫文件...
"%QT_DIR%windeployqt.exe" --release --no-translations --no-system-d3d-compiler --no-opengl-sw --no-quick-import "%DEPLOY_DIR%\ai_llm_integration.exe"

:: 複製額外的運行時庫
echo 🔧 複製運行時庫...
set MINGW_DIR=%QT_DIR%

:: 查找MinGW DLL
if exist "%MINGW_DIR%libgcc_s_seh-1.dll" (
    copy "%MINGW_DIR%libgcc_s_seh-1.dll" "%DEPLOY_DIR%\" >nul
)
if exist "%MINGW_DIR%libstdc++-6.dll" (
    copy "%MINGW_DIR%libstdc++-6.dll" "%DEPLOY_DIR%\" >nul
)
if exist "%MINGW_DIR%libwinpthread-1.dll" (
    copy "%MINGW_DIR%libwinpthread-1.dll" "%DEPLOY_DIR%\" >nul
)

:: 創建啟動腳本
echo 📝 創建啟動腳本...
(
echo @echo off
echo cd /d %%~dp0
echo echo 🤖 啟動RAN Online AI管控系統...
echo start ai_llm_integration.exe
) > "%DEPLOY_DIR%\啟動AI管控系統.bat"

:: 創建說明文件
echo 📋 創建說明文件...
(
echo RAN Online AI管控系統 - 便攜版
echo ================================
echo.
echo 🎯 這是一個便攜式版本，包含所有必要的依賴文件
echo.
echo 📁 文件說明：
echo   - ai_llm_integration.exe    : 主程式
echo   - 啟動AI管控系統.bat         : 啟動腳本
echo   - Qt6*.dll                  : Qt庫文件
echo   - platforms/               : Qt平台插件
echo   - styles/                  : Qt樣式插件
echo.
echo 🚀 使用方法：
echo   1. 解壓所有文件到任意目錄
echo   2. 雙擊「啟動AI管控系統.bat」或直接運行ai_llm_integration.exe
echo   3. 無需安裝，隨處可用
echo.
echo 💡 系統要求：
echo   - Windows 10/11 x64
echo   - 建議記憶體: 4GB+
echo   - 網路連接（用於AI模型通信）
echo.
echo 🏫 學院系統：
echo   - 聖門學院：威嚴神聖風格AI
echo   - 懸岩學院：水墨詩意風格AI  
echo   - 鳳凰學院：情感豐富風格AI
echo.
echo 📞 技術支持：Jy技術團隊
echo ================================
) > "%DEPLOY_DIR%\使用說明.txt"

:: 計算包大小
echo.
echo 📊 部署統計信息：
for /f %%i in ('dir "%DEPLOY_DIR%" /s /-c ^| find "個檔案"') do echo    文件數量: %%i
for /f "tokens=3" %%i in ('dir "%DEPLOY_DIR%" /s /-c ^| find "位元組"') do (
    set /a size_mb=%%i/1024/1024
    echo    總大小: !size_mb! MB
)

echo.
echo ✅ 便攜式部署完成！
echo 📁 部署目錄: %DEPLOY_DIR%
echo 🚀 可以將整個目錄複製到其他電腦使用
echo.

:: 詢問是否測試運行
set /p test_run="是否要測試運行？(y/n): "
if /i "%test_run%"=="y" (
    echo 🧪 啟動測試...
    cd "%DEPLOY_DIR%"
    start ai_llm_integration.exe
    cd ..
)

echo.
echo 🎉 部署腳本執行完畢！
pause
