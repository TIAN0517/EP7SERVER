@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】增強版智能啟動系統 v4.0.0

REM 設置顏色變量（Windows 10/11 支援）
for /f %%A in ('echo prompt $E ^| cmd') do set "ESC=%%A"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "YELLOW=%ESC%[93m"
set "BLUE=%ESC%[94m"
set "MAGENTA=%ESC%[95m"
set "CYAN=%ESC%[96m"
set "WHITE=%ESC%[97m"
set "RESET=%ESC%[0m"

REM 設置路徑變量
set "CURRENT_DIR=%~dp0"
set "DESKTOP=%USERPROFILE%\Desktop"
set "ERROR_LOG=%DESKTOP%\startup_log.txt"

echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%             【Jy技術團隊 線上AI】增強版智能啟動系統%RESET%
echo %WHITE%                        v4.0.0 Ultimate Edition%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %YELLOW%🎯 增強功能：%RESET%
echo %WHITE%   • SQL Server 1433 端口自動檢查%RESET%
echo %WHITE%   • ODBC 連線與 sa 帳號驗證%RESET%
echo %WHITE%   • RanUser 資料庫完整性檢查%RESET%
echo %WHITE%   • AI 模型自動切換 (Ollama/內建)%RESET%
echo %WHITE%   • 玩家數量智能調整 (基於系統資源)%RESET%
echo %WHITE%   • WebSocket 端口衝突自動解決%RESET%
echo %WHITE%   • VM 環境變數自動配置%RESET%
echo %WHITE%   • 實時管理介面 (重啟/檢查/日誌)%RESET%
echo.

REM 清除舊的錯誤日誌
if exist "%ERROR_LOG%" del /q "%ERROR_LOG%"

echo %CYAN%[%date% %time%] ========== 增強版智能啟動開始 ==========%RESET% > "%ERROR_LOG%"
echo [%date% %time%] 啟動位置: %CURRENT_DIR% >> "%ERROR_LOG%"
echo [%date% %time%] 用戶: %USERNAME% >> "%ERROR_LOG%"

REM ========================================================================
REM 步驟 1/6: SQL Server 完整性檢查
REM ========================================================================
echo.
echo %BLUE%🔍 步驟 1/6：SQL Server 完整性檢查%RESET%
echo %CYAN%████████░░ [80%%] 正在檢查 SQL Server 1433 端口...%RESET%

REM 檢查 SQL Server 1433 端口
netstat -an | findstr ":1433" >nul 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ SQL Server 1433 端口已開啟%RESET%
    echo [%date% %time%] ✅ SQL Server 1433 端口正常 >> "%ERROR_LOG%"
    
    REM 測試 sa 帳號連線
    echo %CYAN%       🔑 正在驗證 sa 帳號連線...%RESET%
    echo SELECT @@VERSION; > "%TEMP%\test_sa.sql"
    sqlcmd -S localhost -U sa -P 123456 -d master -i "%TEMP%\test_sa.sql" >nul 2>&1
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ sa 帳號連線成功%RESET%
        echo [%date% %time%] ✅ sa 帳號驗證成功 >> "%ERROR_LOG%"
        set "SQL_STATUS=OK"
    ) else (
        echo %YELLOW%    ⚠️  sa 帳號連線失敗，將使用備用模式%RESET%
        echo [%date% %time%] ⚠️ sa 帳號連線失敗 >> "%ERROR_LOG%"
        set "SQL_STATUS=BACKUP"
    )
    del /q "%TEMP%\test_sa.sql" 2>nul
) else (
    echo %RED%    ❌ SQL Server 1433 端口未開啟，嘗試啟動服務...%RESET%
    echo [%date% %time%] ❌ SQL Server 1433 端口未開啟 >> "%ERROR_LOG%"
    
    REM 嘗試啟動多種可能的 SQL Server 服務
    echo %YELLOW%       🔄 正在嘗試啟動 SQL Server 服務...%RESET%
    net start MSSQLSERVER 2>>"%ERROR_LOG%"
    net start "SQL Server (MSSQLSERVER)" 2>>"%ERROR_LOG%"
    net start "SQL Server (SQLEXPRESS)" 2>>"%ERROR_LOG%"
    net start SQLEXPRESS 2>>"%ERROR_LOG%"
    
    timeout /t 8 /nobreak >nul
    
    REM 再次檢查端口
    netstat -an | findstr ":1433" >nul 2>&1
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ SQL Server 服務已成功啟動%RESET%
        echo [%date% %time%] ✅ SQL Server 自動修復成功 >> "%ERROR_LOG%"
        set "SQL_STATUS=REPAIRED"
    ) else (
        echo %RED%    ❌ SQL Server 服務啟動失敗，將使用離線模式%RESET%
        echo [%date% %time%] ❌ SQL Server 啟動失敗，使用離線模式 >> "%ERROR_LOG%"
        set "SQL_STATUS=OFFLINE"
    )
)

REM ========================================================================
REM 步驟 2/6: 配置檔案智能檢查與生成
REM ========================================================================
echo.
echo %BLUE%🔍 步驟 2/6：配置檔案智能檢查%RESET%
echo %CYAN%██████████ [100%%] 正在檢查 AI_CONFIG.INI...%RESET%

if exist "%CURRENT_DIR%AI_CONFIG.INI" (
    echo %GREEN%    ✅ AI_CONFIG.INI 檔案存在%RESET%
    
    REM 檢查是否為 SQL Server 配置
    findstr /C:"Type=SQLServer" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ SQL Server 配置正確%RESET%
        echo [%date% %time%] ✅ SQL Server 配置正確 >> "%ERROR_LOG%"
    ) else (
        echo %YELLOW%    ⚠️  正在更新為 SQL Server 配置...%RESET%
        call :UpdateConfigForSQLServer
        echo %GREEN%    ✅ 配置已更新為 SQL Server%RESET%
        echo [%date% %time%] ✅ 配置已更新為 SQL Server >> "%ERROR_LOG%"
    )
    
    REM 檢查必要區塊
    set config_sections=0
    findstr /C:"[AI]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Database]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Server]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[PlayerAI]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && set /a config_sections+=1
    
    if %config_sections% geq 4 (
        echo %GREEN%    ✅ 配置區塊完整 (%config_sections%/4)%RESET%
        echo [%date% %time%] ✅ 配置區塊完整 >> "%ERROR_LOG%"
    ) else (
        echo %YELLOW%    ⚠️  配置不完整，重新生成...%RESET%
        call :CreateAdvancedConfig
        echo %GREEN%    ✅ 完整配置已重新生成%RESET%
        echo [%date% %time%] ✅ 配置檔案重新生成 >> "%ERROR_LOG%"
    )
) else (
    echo %RED%    ❌ AI_CONFIG.INI 不存在，正在生成...%RESET%
    call :CreateAdvancedConfig
    echo %GREEN%    ✅ 完整配置檔案已生成%RESET%
    echo [%date% %time%] ✅ 配置檔案已生成 >> "%ERROR_LOG%"
)

REM ========================================================================
REM 步驟 3/6: AI 模型檢測與多模型切換
REM ========================================================================
echo.
echo %BLUE%🔍 步驟 3/6：AI 模型檢測與配置%RESET%
echo %CYAN%██████████ [100%%] 正在掃描可用模型...%RESET%

REM 檢查 Ollama
where ollama >nul 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ Ollama 已安裝，啟用多模型模式%RESET%
    echo [%date% %time%] ✅ Ollama 多模型模式可用 >> "%ERROR_LOG%"
    
    REM 獲取已安裝模型列表
    ollama list 2>nul | findstr /V "NAME" > "%TEMP%\models.txt"
    if exist "%TEMP%\models.txt" (
        for /f %%i in ('type "%TEMP%\models.txt" ^| find /c /v ""') do set model_count=%%i
        if !model_count! gtr 0 (
            echo %CYAN%    📚 發現 !model_count! 個可用模型%RESET%
            echo [%date% %time%] 發現!model_count!個Ollama模型 >> "%ERROR_LOG%"
            set "AI_MODE=MULTI_MODEL"
        ) else (
            echo %YELLOW%    ⚠️  Ollama 已安裝但無可用模型%RESET%
            echo [%date% %time%] Ollama無可用模型 >> "%ERROR_LOG%"
            set "AI_MODE=BUILT_IN"
        )
    ) else (
        set "AI_MODE=BUILT_IN"
    )
    del /q "%TEMP%\models.txt" 2>nul
) else (
    echo %CYAN%    ℹ️  使用內建AI模型%RESET%
    echo [%date% %time%] 使用內建AI模型 >> "%ERROR_LOG%"
    set "AI_MODE=BUILT_IN"
)

REM ========================================================================
REM 步驟 4/6: 系統資源檢測與玩家數量智能調整
REM ========================================================================
echo.
echo %BLUE%🔍 步驟 4/6：系統資源分析與玩家數量調整%RESET%
echo %CYAN%████████░░ [80%%] 正在分析系統資源...%RESET%

REM 獲取系統記憶體資訊
for /f "tokens=2 delims==" %%i in ('wmic computersystem get TotalPhysicalMemory /value') do set total_memory=%%i
set /a memory_gb=%total_memory:~0,-9%

REM 獲取CPU核心數
for /f "tokens=2 delims==" %%i in ('wmic cpu get NumberOfCores /value') do set cpu_cores=%%i

echo %CYAN%    📊 系統資源檢測結果：%RESET%
echo %WHITE%       • 記憶體: %memory_gb%GB%RESET%
echo %WHITE%       • CPU核心: %cpu_cores% 核心%RESET%

REM 根據系統資源智能調整玩家數量
if %memory_gb% geq 16 (
    set "MAX_PLAYERS=100"
    set "PERFORMANCE_MODE=HIGH"
    echo %GREEN%    ✅ 高效能模式: 最大玩家數 100%RESET%
) else if %memory_gb% geq 8 (
    set "MAX_PLAYERS=50"
    set "PERFORMANCE_MODE=STANDARD"
    echo %GREEN%    ✅ 標準模式: 最大玩家數 50%RESET%
) else if %memory_gb% geq 4 (
    set "MAX_PLAYERS=30"
    set "PERFORMANCE_MODE=BALANCED"
    echo %YELLOW%    ⚠️  平衡模式: 最大玩家數 30%RESET%
) else (
    set "MAX_PLAYERS=20"
    set "PERFORMANCE_MODE=LOW"
    echo %RED%    ⚠️  節能模式: 最大玩家數 20%RESET%
)

echo [%date% %time%] 系統資源: %memory_gb%GB RAM, %cpu_cores% CPU, 玩家數: %MAX_PLAYERS% >> "%ERROR_LOG%"

REM ========================================================================
REM 步驟 5/6: WebSocket 端口檢查與衝突解決
REM ========================================================================
echo.
echo %BLUE%🔍 步驟 5/6：WebSocket 服務配置%RESET%
echo %CYAN%██████████ [100%%] 正在配置通訊服務...%RESET%

REM 檢查端口 8080
netstat -an | findstr ":8080" >nul 2>&1
if %errorlevel% == 0 (
    echo %YELLOW%    ⚠️  端口 8080 已被佔用，正在處理...%RESET%
    echo [%date% %time%] ⚠️ 端口8080已被佔用 >> "%ERROR_LOG%"
    
    REM 嘗試找出佔用進程並結束
    for /f "tokens=5" %%a in ('netstat -ano ^| findstr ":8080"') do (
        tasklist | findstr "%%a" >nul 2>&1
        if not errorlevel 1 (
            for /f "tokens=1" %%b in ('tasklist ^| findstr "%%a"') do (
                echo %CYAN%       正在結束進程: %%b (PID: %%a)%RESET%
                taskkill /f /pid %%a >nul 2>&1
                echo [%date% %time%] 結束佔用端口8080的進程: %%b >> "%ERROR_LOG%"
            )
        )
    )
    
    timeout /t 2 /nobreak >nul
    
    REM 再次檢查端口
    netstat -an | findstr ":8080" >nul 2>&1
    if %errorlevel% == 0 (
        echo %RED%    ❌ 端口仍被佔用，將使用替代端口 8081%RESET%
        echo [%date% %time%] 端口8080無法釋放，使用8081 >> "%ERROR_LOG%"
        set "WEBSOCKET_PORT=8081"
    ) else (
        echo %GREEN%    ✅ 端口 8080 已釋放%RESET%
        echo [%date% %time%] 端口8080已成功釋放 >> "%ERROR_LOG%"
        set "WEBSOCKET_PORT=8080"
    )
) else (
    echo %GREEN%    ✅ 端口 8080 可用%RESET%
    echo [%date% %time%] 端口8080可用 >> "%ERROR_LOG%"
    set "WEBSOCKET_PORT=8080"
)

REM ========================================================================
REM 步驟 6/6: VM 環境配置與 AI 助手啟動
REM ========================================================================
echo.
echo %BLUE%🔍 步驟 6/6：VM 環境配置與 AI 助手啟動%RESET%
echo %CYAN%██████████ [100%%] 正在配置 VM 兼容環境...%RESET%

REM 設置 VM 兼容環境變數
set QT_OPENGL=software
set QT_QUICK_BACKEND=software
set QT_ANGLE_PLATFORM=software
set QT_FORCE_SOFTWARE_RENDERING=1
set QT_SCALE_FACTOR=1.0
set QT_AUTO_SCREEN_SCALE_FACTOR=0

echo %GREEN%    ✅ VM 環境變數已配置%RESET%
echo [%date% %time%] VM 環境變數已配置 >> "%ERROR_LOG%"

REM 關閉已存在的進程
taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2>nul

echo %CYAN%    🚀 正在啟動 RANOnline AI Ultimate Portable...%RESET%

REM 檢查主程式是否存在
if exist "%CURRENT_DIR%RANOnline_AI_Ultimate_Portable.exe" (
    start "" "%CURRENT_DIR%RANOnline_AI_Ultimate_Portable.exe"
    
    REM 等待程序啟動
    timeout /t 5 /nobreak >nul
    
    REM 檢查啟動狀態
    tasklist /fi "imagename eq RANOnline_AI_Ultimate_Portable.exe" | findstr "RANOnline_AI_Ultimate_Portable.exe" >nul
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ AI 助手啟動成功！%RESET%
        echo [%date% %time%] ✅ AI助手啟動成功 >> "%ERROR_LOG%"
        goto :SUCCESS_SCREEN
    ) else (
        echo %RED%    ❌ AI 助手啟動失敗%RESET%
        echo [%date% %time%] ❌ AI助手啟動失敗 >> "%ERROR_LOG%"
        goto :ERROR_SCREEN
    )
) else (
    echo %RED%    ❌ 找不到 RANOnline_AI_Ultimate_Portable.exe%RESET%
    echo [%date% %time%] ❌ 主程式檔案不存在 >> "%ERROR_LOG%"
    goto :ERROR_SCREEN
)

REM ========================================================================
REM 成功啟動畫面與管理介面
REM ========================================================================
:SUCCESS_SCREEN
echo.
echo %CYAN%========================================================================%RESET%
echo %GREEN%                         🎉 啟動成功！%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %WHITE%📊 系統狀態總覽：%RESET%
echo %GREEN%    ✅ SQL Server: %SQL_STATUS%%RESET%
echo %GREEN%    ✅ AI 模型: %AI_MODE%%RESET%
echo %GREEN%    ✅ 最大玩家: %MAX_PLAYERS% (效能模式: %PERFORMANCE_MODE%)%RESET%
echo %GREEN%    ✅ WebSocket: 端口 %WEBSOCKET_PORT%%RESET%
echo %GREEN%    ✅ AI 助手: 運行中%RESET%
echo %GREEN%    ✅ VM 兼容: 已啟用%RESET%
echo.
echo %YELLOW%🎛️  實時管理介面：%RESET%
echo %WHITE%    [R] 重啟 AI 助手%RESET%
echo %WHITE%    [S] 完整系統檢查%RESET%
echo %WHITE%    [D] 資料庫表結構檢查%RESET%
echo %WHITE%    [L] 查看啟動日誌%RESET%
echo %WHITE%    [C] 清理系統暫存%RESET%
echo %WHITE%    [Q] 退出管理介面%RESET%
echo.

:MANAGEMENT_LOOP
set /p "mgmt_choice=%YELLOW%請選擇管理操作 (R/S/D/L/C/Q): %RESET%"

if /i "%mgmt_choice%"=="R" goto :RESTART_AI
if /i "%mgmt_choice%"=="S" goto :FULL_CHECK
if /i "%mgmt_choice%"=="D" goto :DATABASE_CHECK
if /i "%mgmt_choice%"=="L" goto :SHOW_LOG
if /i "%mgmt_choice%"=="C" goto :CLEANUP_TEMP
if /i "%mgmt_choice%"=="Q" goto :EXIT_SUCCESS

echo %RED%❌ 無效選項，請重新輸入%RESET%
goto :MANAGEMENT_LOOP

:RESTART_AI
echo.
echo %CYAN%🔄 正在重啟 AI 助手...%RESET%
taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2>nul
timeout /t 3 /nobreak >nul
start "" "%CURRENT_DIR%RANOnline_AI_Ultimate_Portable.exe"
echo %GREEN%✅ AI 助手已重啟%RESET%
echo [%date% %time%] AI助手手動重啟 >> "%ERROR_LOG%"
timeout /t 2 /nobreak >nul
goto :MANAGEMENT_LOOP

:FULL_CHECK
echo.
echo %CYAN%🔍 執行完整系統檢查...%RESET%
if exist "%CURRENT_DIR%一鍵系統檢查與修復.bat" (
    call "%CURRENT_DIR%一鍵系統檢查與修復.bat"
) else if exist "%CURRENT_DIR%系統完整性檢查.bat" (
    call "%CURRENT_DIR%系統完整性檢查.bat"
) else (
    echo %RED%❌ 找不到系統檢查工具%RESET%
)
goto :MANAGEMENT_LOOP

:DATABASE_CHECK
echo.
echo %CYAN%🗄️  執行資料庫表結構檢查...%RESET%
if exist "%CURRENT_DIR%SQL_資料庫表結構檢查.bat" (
    call "%CURRENT_DIR%SQL_資料庫表結構檢查.bat"
) else (
    echo %RED%❌ 找不到資料庫檢查工具%RESET%
)
goto :MANAGEMENT_LOOP

:SHOW_LOG
echo.
echo %CYAN%📄 啟動日誌內容：%RESET%
echo %CYAN%========================================================================%RESET%
if exist "%ERROR_LOG%" (
    type "%ERROR_LOG%"
) else (
    echo %YELLOW%無啟動日誌%RESET%
)
echo %CYAN%========================================================================%RESET%
pause
goto :MANAGEMENT_LOOP

:CLEANUP_TEMP
echo.
echo %CYAN%🧹 清理系統暫存檔案...%RESET%
del /q "%TEMP%\*.sql" 2>nul
del /q "%TEMP%\*.txt" 2>nul
del /q "%TEMP%\*.tmp" 2>nul
echo %GREEN%✅ 暫存檔案清理完成%RESET%
echo [%date% %time%] 暫存檔案清理完成 >> "%ERROR_LOG%"
goto :MANAGEMENT_LOOP

:EXIT_SUCCESS
echo.
echo %GREEN%感謝使用 Jy技術團隊 線上AI 增強版智能啟動系統！%RESET%
echo %WHITE%系統運行正常，可以關閉此視窗%RESET%
timeout /t 3 /nobreak >nul
exit /b 0

REM ========================================================================
REM 錯誤處理畫面
REM ========================================================================
:ERROR_SCREEN
echo.
echo %CYAN%========================================================================%RESET%
echo %RED%                         ❌ 啟動失敗%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %YELLOW%🔍 可能的原因：%RESET%
echo %WHITE%    • 缺少必要的 DLL 檔案%RESET%
echo %WHITE%    • 系統權限不足%RESET%
echo %WHITE%    • 防毒軟體阻擋%RESET%
echo %WHITE%    • VM 環境不兼容%RESET%
echo %WHITE%    • SQL Server 服務異常%RESET%
echo.
echo %YELLOW%🛠️  建議解決方案：%RESET%
echo %WHITE%    [1] 執行完整系統檢查與修復%RESET%
echo %WHITE%    [2] 以管理員身份重新執行%RESET%
echo %WHITE%    [3] 檢查系統依賴項目%RESET%
echo %WHITE%    [4] 查看詳細錯誤日誌%RESET%
echo %WHITE%    [5] 聯繫技術支援%RESET%
echo %WHITE%    [0] 退出%RESET%
echo.

set /p "error_choice=%YELLOW%請選擇解決方案 (0-5): %RESET%"

if "%error_choice%"=="1" (
    if exist "%CURRENT_DIR%一鍵系統檢查與修復.bat" (
        call "%CURRENT_DIR%一鍵系統檢查與修復.bat"
    ) else (
        call "%CURRENT_DIR%系統完整性檢查.bat"
    )
) else if "%error_choice%"=="2" (
    echo %CYAN%請以管理員身份重新運行此腳本%RESET%
    pause
) else if "%error_choice%"=="3" (
    echo.
    echo %CYAN%🔍 系統依賴項目檢查：%RESET%
    echo %WHITE%正在檢查 Qt 依賴庫...%RESET%
    set qt_libs=Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll
    for %%i in (%qt_libs%) do (
        if exist "%CURRENT_DIR%%%i" (
            echo %GREEN%    ✅ %%i%RESET%
        ) else (
            echo %RED%    ❌ %%i 缺失%RESET%
        )
    )
    pause
) else if "%error_choice%"=="4" (
    echo.
    echo %CYAN%📄 詳細錯誤日誌：%RESET%
    echo %CYAN%========================================================================%RESET%
    type "%ERROR_LOG%"
    echo %CYAN%========================================================================%RESET%
    pause
) else if "%error_choice%"=="5" (
    echo.
    echo %CYAN%📞 技術支援資訊：%RESET%
    echo %WHITE%    • 郵件: jytech@example.com%RESET%
    echo %WHITE%    • 版本: v4.0.0 Ultimate Edition%RESET%
    echo %WHITE%    • 日誌位置: %ERROR_LOG%%RESET%
    pause
)

exit /b 1

REM ========================================================================
REM 輔助函數
REM ========================================================================

:CreateAdvancedConfig
echo %CYAN%📝 正在生成增強版 AI_CONFIG.INI...%RESET%
(
echo ; ========================================================================
echo ; Jy技術團隊 線上AI - 增強版配置檔案
echo ; 包含 SQL Server 支援和智能調整功能
echo ; 生成時間: %date% %time%
echo ; ========================================================================
echo.
echo [AI]
echo ; AI核心設置
echo Version=4.0.0
echo Edition=Ultimate_Portable_Enhanced
echo Mode=VM_Compatible
echo SafeMode=true
echo Model=local
echo Provider=built-in
echo OllamaSupport=true
echo MaxTokens=2048
echo Temperature=0.7
echo TopP=0.9
echo PresencePenalty=0.0
echo FrequencyPenalty=0.0
echo AutoModelSwitch=true
echo.
echo [Database]
echo ; 資料庫連線設置 - SQL Server 支援
echo Type=SQLServer
echo Server=localhost
echo Port=1433
echo Database=RanUser
echo Username=sa
echo Password=123456
echo ConnectionTimeout=30
echo MaxConnections=10
echo EnableConnectionPool=true
echo RetryAttempts=3
echo TrustServerCertificate=true
echo Encrypt=false
echo.
echo [Server]
echo ; WebSocket服務器設置
echo Host=localhost
echo Port=%WEBSOCKET_PORT%
echo SecurePort=8443
echo EnableSSL=false
echo MaxConnections=100
echo HeartbeatInterval=30
echo BufferSize=8192
echo EnableCompression=true
echo CorsEnabled=true
echo AllowedOrigins=*
echo AutoRestart=true
echo.
echo [Log]
echo ; 日誌系統設置
echo Level=INFO
echo EnableConsole=true
echo EnableFile=true
echo FilePath=./logs/ai_system.log
echo MaxFileSize=10MB
echo MaxBackupFiles=5
echo RotateDaily=true
echo TimestampFormat=yyyy-MM-dd hh:mm:ss.zzz
echo EnableColors=true
echo.
echo [PlayerAI]
echo ; AI玩家系統設置 - 智能調整
echo MaxPlayers=%MAX_PLAYERS%
echo DefaultLevel=1
echo DefaultClass=Warrior
echo NameGenerator=auto
echo BehaviorMode=intelligent
echo ResponseTime=1000
echo DecisionComplexity=medium
echo LearningEnabled=true
echo AutoAdjustPlayerCount=true
echo MinPlayers=5
echo MaxPlayersPerServer=%MAX_PLAYERS%
echo PerformanceMode=%PERFORMANCE_MODE%
echo.
echo [Security]
echo ; 安全性設置
echo EnableAuthentication=false
echo ApiKeyRequired=false
echo RateLimitEnabled=true
echo MaxRequestsPerMinute=100
echo EnableEncryption=false
echo SessionTimeout=3600
echo AllowedIPs=127.0.0.1,localhost
echo.
echo [VM]
echo ; 虛擬機優化設置
echo ForceSOFTWARE_RENDERING=true
echo DisableHardwareAcceleration=true
echo MemoryOptimization=true
echo ResourceMonitoring=true
echo SafeModeDetection=true
echo RDPCompatibility=true
echo CloudDesktopSupport=true
echo.
echo [Performance]
echo ; 效能優化設置 - 基於系統資源
echo ThreadPoolSize=%cpu_cores%
echo MaxMemoryUsage=%memory_gb%GB
echo GCInterval=300
echo CacheSize=64MB
echo PreloadModels=false
echo AsyncProcessing=true
echo EnableProgressBar=true
echo SystemMemoryGB=%memory_gb%
echo CPUCores=%cpu_cores%
) > "%CURRENT_DIR%AI_CONFIG.INI"
exit /b

:UpdateConfigForSQLServer
echo %CYAN%📝 正在更新配置為 SQL Server...%RESET%
REM 備份原配置
copy "%CURRENT_DIR%AI_CONFIG.INI" "%CURRENT_DIR%AI_CONFIG.INI.backup" >nul 2>&1

REM 替換資料庫配置區塊
powershell -Command "(Get-Content '%CURRENT_DIR%AI_CONFIG.INI') -replace 'Type=SQLite', 'Type=SQLServer' -replace 'Path=.*', 'Server=localhost' | Set-Content '%CURRENT_DIR%AI_CONFIG.INI'"

REM 添加 SQL Server 特定配置
echo. >> "%CURRENT_DIR%AI_CONFIG.INI"
echo Port=1433 >> "%CURRENT_DIR%AI_CONFIG.INI"
echo Database=RanUser >> "%CURRENT_DIR%AI_CONFIG.INI"
echo Username=sa >> "%CURRENT_DIR%AI_CONFIG.INI"
echo Password=123456 >> "%CURRENT_DIR%AI_CONFIG.INI"
echo TrustServerCertificate=true >> "%CURRENT_DIR%AI_CONFIG.INI"
echo Encrypt=false >> "%CURRENT_DIR%AI_CONFIG.INI"
exit /b
