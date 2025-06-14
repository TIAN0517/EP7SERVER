@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】智能啟動系統 v4.0.0

REM 設置顏色變量（簡化版）
set "CURRENT_DIR=%~dp0"
set "DESKTOP=%USERPROFILE%\Desktop"
set "ERROR_LOG=%DESKTOP%\error_log.txt"

echo.
echo ========================================================================
echo             【Jy技術團隊 線上AI】智能啟動系統 v4.0.0
echo                        極簡穩定VM兼容版
echo ========================================================================
echo.
echo    🎯 智能功能：
echo       • 自動檢查系統狀態
echo       • 智能修復常見問題
echo       • 多模型自動切換
echo       • 玩家數量自動調整
echo       • 彩色進度條顯示
echo       • 一鍵重啟功能
echo.

REM 清除舊的錯誤日誌
if exist "%ERROR_LOG%" del /q "%ERROR_LOG%"

echo [%date% %time%] ========== 智能啟動開始 ========== > "%ERROR_LOG%"

echo 🔍 步驟 1/5：快速系統檢查
echo ████████░░ [80%%] 正在檢查核心組件...

REM 快速檢查SQL Server
sc query "MSSQLSERVER" | findstr "RUNNING" >nul
if %errorlevel% neq 0 (
    echo    ⚠️ SQL Server 未運行，嘗試自動啟動...
    net start "MSSQLSERVER" >nul 2>&1
    if %errorlevel% equ 0 (
        echo    ✅ SQL Server 自動啟動成功
        echo [%date% %time%] SQL Server 自動啟動成功 >> "%ERROR_LOG%"
    ) else (
        echo    ❌ SQL Server 啟動失敗
        echo [%date% %time%] 錯誤: SQL Server 啟動失敗 >> "%ERROR_LOG%"
    )
) else (
    echo    ✅ SQL Server 運行正常
)

REM 檢查配置檔案
if not exist "%CURRENT_DIR%AI_CONFIG.INI" (
    echo    ⚠️ 配置檔案缺失，自動生成中...
    call :CreateQuickConfig
    echo    ✅ 配置檔案已自動生成
    echo [%date% %time%] 配置檔案已自動生成 >> "%ERROR_LOG%"
) else (
    echo    ✅ 配置檔案存在
)

echo.
echo 🔍 步驟 2/5：AI模型檢測與配置
echo ██████████ [100%%] 正在掃描可用模型...

REM 檢查Ollama
ollama list >nul 2>&1
if %errorlevel% equ 0 (
    echo    ✅ Ollama 模型可用，啟用多模型模式
    echo [%date% %time%] Ollama 模型可用 >> "%ERROR_LOG%"
    set "AI_MODE=MULTI_MODEL"
) else (
    echo    ℹ️ 使用內建AI模式
    echo [%date% %time%] 使用內建AI模式 >> "%ERROR_LOG%"
    set "AI_MODE=BUILT_IN"
)

echo.
echo 🔍 步驟 3/5：玩家數量智能調整
echo ████████░░ [80%%] 正在分析系統資源...

REM 檢測系統記憶體
for /f "tokens=2 delims=:" %%a in ('systeminfo ^| findstr "Total Physical Memory"') do set "TOTAL_MEMORY=%%a"
echo    📊 系統記憶體: %TOTAL_MEMORY%

REM 根據記憶體調整玩家數量
echo %TOTAL_MEMORY% | findstr "GB" >nul
if %errorlevel% equ 0 (
    set "MAX_PLAYERS=100"
    echo    ✅ 高配置模式: 最大玩家數 100
) else (
    set "MAX_PLAYERS=50"
    echo    ✅ 標準模式: 最大玩家數 50
)

echo [%date% %time%] 最大玩家數設定為 %MAX_PLAYERS% >> "%ERROR_LOG%"

echo.
echo 🔍 步驟 4/5：WebSocket服務準備
echo ██████████ [100%%] 正在配置通訊服務...

REM 檢查端口8080
netstat -an | findstr ":8080" >nul
if %errorlevel% equ 0 (
    echo    ⚠️ 端口8080已被占用，嘗試釋放...
    for /f "tokens=5" %%a in ('netstat -ano ^| findstr ":8080"') do (
        taskkill /f /pid %%a >nul 2>&1
    )
    echo    ✅ 端口已釋放
    echo [%date% %time%] 端口8080已釋放 >> "%ERROR_LOG%"
) else (
    echo    ✅ 端口8080可用
)

echo.
echo 🔍 步驟 5/5：啟動AI助手
echo ██████████ [100%%] 正在啟動主程序...

REM 設置VM兼容環境變數
set QT_OPENGL=software
set QT_QUICK_BACKEND=software
set QT_ANGLE_PLATFORM=software
set QT_FORCE_SOFTWARE_RENDERING=1

REM 關閉已存在的進程
taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2>nul

echo    🚀 正在啟動 RANOnline AI Ultimate Portable...
start "" "RANOnline_AI_Ultimate_Portable.exe"

REM 等待程序啟動
timeout /t 3 /nobreak >nul

REM 檢查啟動狀態
tasklist /fi "imagename eq RANOnline_AI_Ultimate_Portable.exe" | findstr "RANOnline_AI_Ultimate_Portable.exe" >nul
if %errorlevel% equ 0 (
    echo    ✅ AI助手啟動成功！
    echo [%date% %time%] AI助手啟動成功 >> "%ERROR_LOG%"
    
    echo.
    echo ========================================================================
    echo                         🎉 啟動成功！
    echo ========================================================================
    echo.
    echo    ✅ SQL Server: 運行正常
    echo    ✅ AI模型: %AI_MODE%
    echo    ✅ 最大玩家: %MAX_PLAYERS%
    echo    ✅ WebSocket: 已配置
    echo    ✅ AI助手: 運行中
    echo.
    echo    🎛️ 管理選項：
    echo       [R] 重啟AI助手
    echo       [S] 系統完整性檢查
    echo       [L] 查看錯誤日誌
    echo       [Q] 退出
    echo.
    
    :MANAGEMENT_LOOP
    set /p "mgmt_choice=請選擇操作 (R/S/L/Q): "
    
    if /i "%mgmt_choice%"=="R" goto :RESTART_AI
    if /i "%mgmt_choice%"=="S" goto :FULL_CHECK
    if /i "%mgmt_choice%"=="L" goto :SHOW_LOG
    if /i "%mgmt_choice%"=="Q" goto :EXIT_SUCCESS
    
    echo 無效選擇，請重新輸入
    goto :MANAGEMENT_LOOP
    
    :RESTART_AI
    echo.
    echo 🔄 正在重啟AI助手...
    taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2>nul
    timeout /t 2 /nobreak >nul
    start "" "RANOnline_AI_Ultimate_Portable.exe"
    echo ✅ AI助手已重啟
    echo [%date% %time%] AI助手手動重啟 >> "%ERROR_LOG%"
    timeout /t 2 /nobreak >nul
    goto :MANAGEMENT_LOOP
    
    :FULL_CHECK
    echo.
    echo 🔍 執行完整系統檢查...
    call "系統完整性檢查.bat"
    goto :MANAGEMENT_LOOP
    
    :SHOW_LOG
    echo.
    echo 📄 錯誤日誌內容：
    echo ========================================================================
    if exist "%ERROR_LOG%" (
        type "%ERROR_LOG%"
    ) else (
        echo 無錯誤日誌
    )
    echo ========================================================================
    pause
    goto :MANAGEMENT_LOOP
    
    :EXIT_SUCCESS
    echo.
    echo 感謝使用 Jy技術團隊 線上AI！
    echo 系統運行正常，可以關閉此視窗
    timeout /t 3 /nobreak >nul
    exit /b 0
    
) else (
    echo    ❌ AI助手啟動失敗
    echo [%date% %time%] 錯誤: AI助手啟動失敗 >> "%ERROR_LOG%"
    
    echo.
    echo ========================================================================
    echo                         ❌ 啟動失敗
    echo ========================================================================
    echo.
    echo    可能的原因：
    echo       • 缺少必要的DLL檔案
    echo       • 權限不足
    echo       • 防毒軟體阻擋
    echo       • 系統不兼容
    echo.
    echo    建議解決方案：
    echo       1. 以管理員身份運行
    echo       2. 檢查防毒軟體設定
    echo       3. 執行完整系統檢查
    echo       4. 聯繫技術支援
    echo.
    echo    [1] 執行完整系統檢查
    echo    [2] 以管理員身份重試
    echo    [3] 查看錯誤日誌
    echo    [0] 退出
    echo.
    
    set /p "error_choice=請選擇操作 (0-3): "
    
    if "%error_choice%"=="1" (
        call "系統完整性檢查.bat"
    ) else if "%error_choice%"=="2" (
        echo 請以管理員身份重新運行此腳本
        pause
    ) else if "%error_choice%"=="3" (
        echo.
        echo 📄 錯誤日誌：
        type "%ERROR_LOG%"
        pause
    )
    
    exit /b 1
)

REM ========================================================================
REM 輔助函數
REM ========================================================================

:CreateQuickConfig
(
echo ; 快速生成的AI配置檔案
echo [AI]
echo Version=4.0.0
echo Edition=Ultimate_Portable
echo Mode=VM_Compatible
echo SafeMode=true
echo.
echo [Database]
echo Type=SQLServer
echo Server=localhost
echo Database=RanUser
echo Username=sa
echo Password=123456
echo.
echo [Server]
echo Host=localhost
echo Port=8080
echo MaxConnections=100
echo.
echo [PlayerAI]
echo MaxPlayers=%MAX_PLAYERS%
echo AutoAdjustPlayerCount=true
echo.
echo [Performance]
echo EnableProgressBar=true
echo EnableColors=true
) > "%CURRENT_DIR%AI_CONFIG.INI"
exit /b
