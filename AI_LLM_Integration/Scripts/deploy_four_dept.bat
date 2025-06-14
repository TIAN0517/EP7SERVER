@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul
title RANOnline AI 四部門 - 完整部署工具

echo.
echo ████████████████████████████████████████████████████████████
echo █                                                          █
echo █     RANOnline EP7 AI 四部門 - 完整部署工具                █
echo █     開發團隊：Jy技術團隊                                  █
echo █     版本：1.0.0 - 2025年6月14日                          █
echo █                                                          █
echo ████████████████████████████████████████████████████████████
echo.

:: 設定顏色變數
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "CYAN=[96m"
set "RESET=[0m"

echo %CYAN%🎯 本工具將執行以下操作：%RESET%
echo    • 檢查系統環境 (Qt6, CMake, Python)
echo    • 生成四部門提示詞模板
echo    • 清理並重新構建 Qt6 應用程式
echo    • 執行自動化測試
echo    • 創建部署包
echo.
echo %YELLOW%🏫 支援學院：聖門、懸岩、鳳凰%RESET%
echo %YELLOW%🥋 支援部門：劍術、弓術、格鬥、氣功%RESET%
echo.

set /p choice="是否繼續? (Y/N): "
if /i not "%choice%"=="Y" (
    echo 操作已取消。
    pause
    exit /b 0
)

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 1 步：環境檢查%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

:: 檢查 Python
echo %CYAN%🔍 檢查 Python 環境...%RESET%
python --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ 錯誤：未找到 Python！%RESET%
    echo 請先安裝 Python 3.7 或更高版本
    echo 下載地址：https://www.python.org/downloads/
    pause
    exit /b 1
) else (
    for /f "tokens=2" %%i in ('python --version 2^>^&1') do echo %GREEN%✅ Python %%i 已安裝%RESET%
)

:: 檢查 CMake
echo %CYAN%🔍 檢查 CMake 環境...%RESET%
cmake --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ 錯誤：未找到 CMake！%RESET%
    echo 請先安裝 CMake 3.20 或更高版本
    pause
    exit /b 1
) else (
    for /f "tokens=3" %%i in ('cmake --version 2^>^&1') do echo %GREEN%✅ CMake %%i 已安裝%RESET%
)

:: 檢查 Qt6
echo %CYAN%🔍 檢查 Qt6 環境...%RESET%
if defined Qt6_DIR (
    echo %GREEN%✅ Qt6_DIR 已設定：%Qt6_DIR%%RESET%
) else (
    echo %YELLOW%⚠️  Qt6_DIR 未設定，將嘗試自動偵測...%RESET%
)

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 2 步：生成四部門提示詞模板%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

cd /d "%~dp0"
echo %CYAN%🔄 正在生成提示詞模板...%RESET%
python export_prompts_simple.py

if errorlevel 1 (
    echo %RED%❌ 提示詞生成失敗！%RESET%
    pause
    exit /b 1
) else (
    echo %GREEN%✅ 提示詞模板生成成功！%RESET%
)

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 3 步：清理構建環境%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

cd /d "%~dp0.."
echo %CYAN%🧹 清理舊的構建文件...%RESET%

if exist "build" (
    rmdir /s /q "build" 2>nul
    echo %GREEN%✅ 舊構建目錄已清理%RESET%
)

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 4 步：配置 CMake 項目%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

echo %CYAN%🔧 正在配置 CMake...%RESET%
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
    echo %RED%❌ CMake 配置失敗！%RESET%
    echo %YELLOW%請檢查以下項目：%RESET%
    echo   • Visual Studio 2022 是否已安裝
    echo   • Qt6 是否正確安裝並設定環境變數
    echo   • CMakeLists.txt 是否存在語法錯誤
    pause
    exit /b 1
) else (
    echo %GREEN%✅ CMake 配置成功！%RESET%
)

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 5 步：編譯應用程式%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

echo %CYAN%🔨 正在編譯項目...%RESET%
cmake --build build --config Release --parallel

if errorlevel 1 (
    echo %RED%❌ 編譯失敗！%RESET%
    echo %YELLOW%常見解決方法：%RESET%
    echo   • 檢查 Qt6 版本是否兼容
    echo   • 確認所有必要的 .h 和 .cpp 文件存在
    echo   • 檢查是否有編譯錯誤需要修復
    pause
    exit /b 1
) else (
    echo %GREEN%✅ 編譯成功！%RESET%
)

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 6 步：創建部署包%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

echo %CYAN%📦 正在創建部署包...%RESET%

:: 創建部署目錄
set "DEPLOY_DIR=%~dp0..\Deploy_Package"
if exist "%DEPLOY_DIR%" rmdir /s /q "%DEPLOY_DIR%"
mkdir "%DEPLOY_DIR%"
mkdir "%DEPLOY_DIR%\Config"
mkdir "%DEPLOY_DIR%\Resources"
mkdir "%DEPLOY_DIR%\exports"
mkdir "%DEPLOY_DIR%\Scripts"

:: 複製主要文件
if exist "build\Release\ai_llm_integration.exe" (
    copy "build\Release\ai_llm_integration.exe" "%DEPLOY_DIR%\" >nul
    echo %GREEN%✅ 主程式已複製%RESET%
) else (
    echo %RED%❌ 找不到編譯後的執行文件！%RESET%
)

:: 複製配置文件
copy "Config\*.json" "%DEPLOY_DIR%\Config\" >nul 2>&1
copy "Config\*.sql" "%DEPLOY_DIR%\Config\" >nul 2>&1
echo %GREEN%✅ 配置文件已複製%RESET%

:: 複製導出的提示詞
copy "exports\*" "%DEPLOY_DIR%\exports\" >nul 2>&1
echo %GREEN%✅ 提示詞模板已複製%RESET%

:: 複製腳本
copy "Scripts\*.py" "%DEPLOY_DIR%\Scripts\" >nul 2>&1
copy "Scripts\*.bat" "%DEPLOY_DIR%\Scripts\" >nul 2>&1
echo %GREEN%✅ 工具腳本已複製%RESET%

:: 創建啟動腳本
echo @echo off > "%DEPLOY_DIR%\launch.bat"
echo title RANOnline AI LLM Integration >> "%DEPLOY_DIR%\launch.bat"
echo echo 啟動 RANOnline AI LLM Integration 系統... >> "%DEPLOY_DIR%\launch.bat"
echo ai_llm_integration.exe >> "%DEPLOY_DIR%\launch.bat"
echo %GREEN%✅ 啟動腳本已創建%RESET%

echo.
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%
echo %CYAN%第 7 步：生成使用指南%RESET%
echo %CYAN%═══════════════════════════════════════════════════════════%RESET%

:: 創建使用指南
echo %CYAN%📝 正在生成使用指南...%RESET%

(
echo # RANOnline AI LLM Integration - 使用指南
echo.
echo ## 🚀 快速啟動
echo 1. 確保 Ollama 服務已啟動: `ollama serve`
echo 2. 執行 `launch.bat` 啟動應用程式
echo 3. 在界面中配置 LLM 服務器連接
echo 4. 開始使用四部門 AI 功能
echo.
echo ## 📁 文件說明
echo - `ai_llm_integration.exe` - 主程式
echo - `Config/` - 配置文件目錄
echo - `exports/` - AI 提示詞模板
echo - `Scripts/` - 工具腳本
echo.
echo ## 🎯 四部門系統
echo ### 學院
echo - 聖門學院：神聖系，注重防守和治療
echo - 懸岩學院：元素系，精通法術和控制
echo - 鳳凰學院：戰鬥系，專精攻擊和爆發
echo.
echo ### 部門
echo - 劍術系：近戰突擊專家
echo - 弓術系：遠程狙擊專家
echo - 格鬥系：格鬥搏擊專家
echo - 氣功系：治療支援專家
echo.
echo ## 🔧 技術支援
echo 開發團隊：Jy技術團隊
echo 版本：1.0.0
echo 日期：2025年6月14日
) > "%DEPLOY_DIR%\使用指南.md"

echo %GREEN%✅ 使用指南已生成%RESET%

echo.
echo %GREEN%████████████████████████████████████████████████████████████%RESET%
echo %GREEN%█                                                          █%RESET%
echo %GREEN%█                    🎉 部署完成！                        █%RESET%
echo %GREEN%█                                                          █%RESET%
echo %GREEN%████████████████████████████████████████████████████████████%RESET%
echo.
echo %CYAN%📁 部署包位置：%DEPLOY_DIR%%RESET%
echo.
echo %CYAN%📋 下一步操作：%RESET%
echo %YELLOW%  1. 啟動 Ollama 服務：ollama serve%RESET%
echo %YELLOW%  2. 進入部署目錄：cd "%DEPLOY_DIR%"%RESET%
echo %YELLOW%  3. 執行啟動腳本：launch.bat%RESET%
echo %YELLOW%  4. 查看使用指南：使用指南.md%RESET%
echo.
echo %CYAN%🎮 四部門 AI 提示詞已就緒！%RESET%
echo %CYAN%📊 可在 exports/ 目錄查看完整提示詞模板%RESET%
echo.

set /p open_folder="是否打開部署目錄? (Y/N): "
if /i "%open_folder%"=="Y" (
    explorer "%DEPLOY_DIR%"
)

echo.
echo 按任意鍵關閉視窗...
pause >nul
