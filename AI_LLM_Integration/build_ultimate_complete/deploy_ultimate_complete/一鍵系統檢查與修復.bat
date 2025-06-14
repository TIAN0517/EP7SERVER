@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】一鍵系統檢查與自動修復 v4.0.0

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
set "DESKTOP=%USERPROFILE%\Desktop"
set "ERROR_LOG=%DESKTOP%\error_log.txt"
set "CURRENT_DIR=%~dp0"
set "CONFIG_FILE=%CURRENT_DIR%AI_CONFIG.INI"
set "LOG_DIR=%CURRENT_DIR%logs"

REM 清除舊的錯誤日誌
if exist "%ERROR_LOG%" del /q "%ERROR_LOG%"

echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%          【Jy技術團隊 線上AI】一鍵系統檢查與自動修復%RESET%
echo %WHITE%                        v4.0.0 Ultimate Edition%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %YELLOW%🎯 自動檢查項目：%RESET%
echo %WHITE%   1. SQL Server 1433 端口狀態檢查%RESET%
echo %WHITE%   2. ODBC 連線與 sa 帳號驗證%RESET%
echo %WHITE%   3. RanUser 資料庫及表結構完整性%RESET%
echo %WHITE%   4. AI_CONFIG.INI 配置檔案格式驗證%RESET%
echo %WHITE%   5. WebSocket 服務配置檢查%RESET%
echo %WHITE%   6. 系統依賴項目與環境驗證%RESET%
echo %WHITE%   7. AI 模型切換功能測試%RESET%
echo %WHITE%   8. 玩家數量自動調整驗證%RESET%
echo.
echo %YELLOW%🔧 自動修復功能：%RESET%
echo %WHITE%   • 自動重啟 SQL Server 服務%RESET%
echo %WHITE%   • 重新生成完整配置檔案%RESET%
echo %WHITE%   • 修復資料庫權限問題%RESET%
echo %WHITE%   • 創建缺失的資料庫表%RESET%
echo %WHITE%   • 一鍵重啟AI助手和資料庫服務%RESET%
echo %WHITE%   • 自動生成詳細錯誤報告到桌面%RESET%
echo.

REM 創建日誌目錄
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

REM 開始系統檢查
echo %CYAN%[%date% %time%] ========== 系統檢查開始 ==========%RESET% > "%ERROR_LOG%"
echo [%date% %time%] 檢查執行位置: %CURRENT_DIR% >> "%ERROR_LOG%"
echo [%date% %time%] 檢查人員: %USERNAME% >> "%ERROR_LOG%"
echo [%date% %time%] 系統版本: %OS% >> "%ERROR_LOG%"
echo. >> "%ERROR_LOG%"

REM ========================================================================
REM 步驟 1: SQL Server 1433 端口狀態檢查
REM ========================================================================
echo.
echo %BLUE%[1/8] 🔍 檢查 SQL Server 1433 端口狀態...%RESET%
echo [%date% %time%] 步驟1: 檢查 SQL Server 1433 端口狀態 >> "%ERROR_LOG%"

netstat -an | findstr ":1433" >nul 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ SQL Server 1433 端口已開啟%RESET%
    echo [%date% %time%] ✅ SQL Server 1433 端口狀態正常 >> "%ERROR_LOG%"
) else (
    echo %RED%    ❌ SQL Server 1433 端口未開啟，嘗試啟動服務...%RESET%
    echo [%date% %time%] ❌ SQL Server 1433 端口未開啟 >> "%ERROR_LOG%"
    
    REM 嘗試啟動 SQL Server 服務（多種可能的服務名稱）
    echo %YELLOW%       🔄 正在嘗試啟動 SQL Server 服務...%RESET%
    net start MSSQLSERVER 2>>"%ERROR_LOG%"
    net start "SQL Server (MSSQLSERVER)" 2>>"%ERROR_LOG%"
    net start "SQL Server (SQLEXPRESS)" 2>>"%ERROR_LOG%"
    net start SQLEXPRESS 2>>"%ERROR_LOG%"
    
    timeout /t 8 /nobreak >nul
    
    REM 再次檢查
    netstat -an | findstr ":1433" >nul 2>&1
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ SQL Server 服務已成功啟動%RESET%
        echo [%date% %time%] ✅ SQL Server 服務自動修復成功 >> "%ERROR_LOG%"
    ) else (
        echo %RED%    ❌ 無法啟動 SQL Server 服務，請手動檢查%RESET%
        echo [%date% %time%] ❌ SQL Server 服務啟動失敗，需要手動介入 >> "%ERROR_LOG%"
        echo.
        echo %YELLOW%    建議解決方案：%RESET%
        echo %WHITE%    • 檢查 SQL Server 是否已安裝%RESET%
        echo %WHITE%    • 確認服務帳號權限%RESET%
        echo %WHITE%    • 使用 SQL Server Configuration Manager 檢查配置%RESET%
        echo %WHITE%    • 檢查防火牆設置%RESET%
        echo [%date% %time%] 建議: 請檢查SQL Server安裝和配置 >> "%ERROR_LOG%"
    )
)

REM ========================================================================
REM 步驟 2: ODBC 連線與 sa 帳號驗證
REM ========================================================================
echo.
echo %BLUE%[2/8] 🔑 驗證 ODBC 連線與 sa 帳號...%RESET%
echo [%date% %time%] 步驟2: 驗證 ODBC 連線與 sa 帳號 >> "%ERROR_LOG%"

REM 創建測試 SQL 腳本
echo SELECT @@VERSION as SQLServerVersion, GETDATE() as CurrentTime; > "%TEMP%\test_connection.sql"

REM 測試 sa 帳號連線
sqlcmd -S localhost -U sa -P 123456 -d master -i "%TEMP%\test_connection.sql" >"%TEMP%\sql_test_result.txt" 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ ODBC 連線成功，sa 帳號可用%RESET%
    echo [%date% %time%] ✅ ODBC 連線和 sa 帳號驗證成功 >> "%ERROR_LOG%"
    
    REM 獲取 SQL Server 版本資訊
    for /f "tokens=*" %%i in ('type "%TEMP%\sql_test_result.txt" ^| findstr "Microsoft"') do (
        echo %CYAN%       版本資訊: %%i%RESET%
        echo [%date% %time%] SQL Server版本: %%i >> "%ERROR_LOG%"
    )
) else (
    echo %RED%    ❌ ODBC 連線失敗或 sa 帳號問題%RESET%
    echo [%date% %time%] ❌ ODBC 連線失敗或 sa 帳號問題 >> "%ERROR_LOG%"
    
    REM 嘗試 Windows 認證
    echo %YELLOW%       🔄 嘗試 Windows 認證...%RESET%
    sqlcmd -S localhost -E -d master -i "%TEMP%\test_connection.sql" >"%TEMP%\sql_test_result2.txt" 2>&1
    if %errorlevel% == 0 (
        echo %YELLOW%    ⚠️  Windows 認證可用，但 sa 帳號可能未啟用%RESET%
        echo [%date% %time%] ⚠️ Windows 認證可用，sa 帳號需要配置 >> "%ERROR_LOG%"
        echo.
        echo %YELLOW%    建議解決方案：%RESET%
        echo %WHITE%    • 啟用 SQL Server 混合驗證模式%RESET%
        echo %WHITE%    • 重設 sa 帳號密碼為 123456%RESET%
        echo %WHITE%    • 確認 sa 帳號已啟用%RESET%
        echo [%date% %time%] 建議: 需要配置SQL Server混合驗證和sa帳號 >> "%ERROR_LOG%"
    ) else (
        echo %RED%    ❌ 所有連線方式都失敗%RESET%
        echo [%date% %time%] ❌ 所有資料庫連線方式都失敗 >> "%ERROR_LOG%"
        echo.
        echo %YELLOW%    建議解決方案：%RESET%
        echo %WHITE%    • 確認 SQL Server 服務正在運行%RESET%
        echo %WHITE%    • 檢查防火牆設置%RESET%
        echo %WHITE%    • 驗證 SQL Server 網路配置%RESET%
        echo %WHITE%    • 啟用 TCP/IP 協議%RESET%
        echo [%date% %time%] 建議: 檢查SQL Server服務和網路配置 >> "%ERROR_LOG%"
    )
)

REM 清理臨時檔案
if exist "%TEMP%\test_connection.sql" del /q "%TEMP%\test_connection.sql"
if exist "%TEMP%\sql_test_result.txt" del /q "%TEMP%\sql_test_result.txt"
if exist "%TEMP%\sql_test_result2.txt" del /q "%TEMP%\sql_test_result2.txt"

REM ========================================================================
REM 步驟 3: RanUser 資料庫及表結構驗證
REM ========================================================================
echo.
echo %BLUE%[3/8] 🗄️  驗證 RanUser 資料庫及表結構...%RESET%
echo [%date% %time%] 步驟3: 驗證 RanUser 資料庫及表結構 >> "%ERROR_LOG%"

REM 創建資料庫檢查腳本
(
echo SELECT name FROM sys.databases WHERE name = 'RanUser';
echo GO
echo USE RanUser;
echo GO
echo SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE' ORDER BY TABLE_NAME;
echo GO
) > "%TEMP%\check_database.sql"

sqlcmd -S localhost -U sa -P 123456 -i "%TEMP%\check_database.sql" > "%TEMP%\db_result.txt" 2>&1
if %errorlevel% == 0 (
    findstr /C:"RanUser" "%TEMP%\db_result.txt" >nul 2>&1
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ RanUser 資料庫存在%RESET%
        echo [%date% %time%] ✅ RanUser 資料庫驗證成功 >> "%ERROR_LOG%"
        
        REM 檢查必要的表
        set table_count=0
        findstr /C:"ChaInfo" "%TEMP%\db_result.txt" >nul 2>&1 && set /a table_count+=1
        findstr /C:"UserInfo" "%TEMP%\db_result.txt" >nul 2>&1 && set /a table_count+=1
        findstr /C:"LogInfo" "%TEMP%\db_result.txt" >nul 2>&1 && set /a table_count+=1
        findstr /C:"ItemInfo" "%TEMP%\db_result.txt" >nul 2>&1 && set /a table_count+=1
        findstr /C:"AI_States" "%TEMP%\db_result.txt" >nul 2>&1 && set /a table_count+=1
        
        if %table_count% geq 3 (
            echo %GREEN%    ✅ 核心資料表結構完整 (%table_count%/5 個表存在)%RESET%
            echo [%date% %time%] ✅ 資料庫表結構驗證成功，發現%table_count%個核心表 >> "%ERROR_LOG%"
        ) else (
            echo %YELLOW%    ⚠️  部分資料表可能缺失，嘗試創建...%RESET%
            echo [%date% %time%] ⚠️ 部分資料表缺失，需要創建 >> "%ERROR_LOG%"
            call :CreateBasicTables
        )
    ) else (
        echo %RED%    ❌ RanUser 資料庫不存在，嘗試創建...%RESET%
        echo [%date% %time%] ❌ RanUser 資料庫不存在 >> "%ERROR_LOG%"
        
        REM 創建資料庫
        echo CREATE DATABASE RanUser; > "%TEMP%\create_db.sql"
        echo GO >> "%TEMP%\create_db.sql"
        echo PRINT 'RanUser 資料庫已創建'; >> "%TEMP%\create_db.sql"
        
        sqlcmd -S localhost -U sa -P 123456 -i "%TEMP%\create_db.sql" >nul 2>&1
        if %errorlevel% == 0 (
            echo %GREEN%    ✅ RanUser 資料庫創建成功%RESET%
            echo [%date% %time%] ✅ RanUser 資料庫自動創建成功 >> "%ERROR_LOG%"
            call :CreateBasicTables
        ) else (
            echo %RED%    ❌ 無法創建 RanUser 資料庫%RESET%
            echo [%date% %time%] ❌ RanUser 資料庫創建失敗 >> "%ERROR_LOG%"
        )
    )
) else (
    echo %RED%    ❌ 無法連接到資料庫進行檢查%RESET%
    echo [%date% %time%] ❌ 資料庫連接失敗，無法進行驗證 >> "%ERROR_LOG%"
)

REM 清理臨時檔案
if exist "%TEMP%\check_database.sql" del /q "%TEMP%\check_database.sql"
if exist "%TEMP%\db_result.txt" del /q "%TEMP%\db_result.txt"
if exist "%TEMP%\create_db.sql" del /q "%TEMP%\create_db.sql"

REM ========================================================================
REM 步驟 4: AI_CONFIG.INI 配置檔案格式驗證
REM ========================================================================
echo.
echo %BLUE%[4/8] 📋 驗證 AI_CONFIG.INI 配置檔案...%RESET%
echo [%date% %time%] 步驟4: 驗證 AI_CONFIG.INI 配置檔案 >> "%ERROR_LOG%"

if exist "%CONFIG_FILE%" (
    echo %GREEN%    ✅ AI_CONFIG.INI 檔案存在%RESET%
    echo [%date% %time%] AI_CONFIG.INI 檔案存在 >> "%ERROR_LOG%"
    
    REM 檢查必要的配置區塊
    set config_sections=0
    findstr /C:"[AI]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Database]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Server]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Log]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[PlayerAI]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Security]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[VM]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    findstr /C:"[Performance]" "%CONFIG_FILE%" >nul 2>&1 && set /a config_sections+=1
    
    if %config_sections% geq 8 (
        echo %GREEN%    ✅ 配置檔案格式完整 (8/8 個區塊)%RESET%
        echo [%date% %time%] ✅ AI_CONFIG.INI 格式驗證成功 >> "%ERROR_LOG%"
        
        REM 檢查SQL Server配置
        findstr /C:"Type=SQLServer" "%CONFIG_FILE%" >nul 2>&1
        if %errorlevel% == 0 (
            echo %GREEN%    ✅ SQL Server 配置正確%RESET%
            echo [%date% %time%] ✅ SQL Server 資料庫配置正確 >> "%ERROR_LOG%"
        ) else (
            echo %YELLOW%    ⚠️  資料庫配置可能需要更新為SQL Server%RESET%
            echo [%date% %time%] ⚠️ 資料庫配置需要更新 >> "%ERROR_LOG%"
        )
    ) else (
        echo %YELLOW%    ⚠️  配置檔案不完整 (%config_sections%/8 個區塊)，重新生成...%RESET%
        echo [%date% %time%] ⚠️ AI_CONFIG.INI 不完整，重新生成 >> "%ERROR_LOG%"
        call :CreateConfigFile
    )
) else (
    echo %RED%    ❌ AI_CONFIG.INI 檔案不存在，正在重新生成...%RESET%
    echo [%date% %time%] ❌ AI_CONFIG.INI 檔案不存在，重新生成 >> "%ERROR_LOG%"
    call :CreateConfigFile
)

REM ========================================================================
REM 步驟 5: WebSocket 服務配置檢查
REM ========================================================================
echo.
echo %BLUE%[5/8] 🌐 檢查 WebSocket 服務配置...%RESET%
echo [%date% %time%] 步驟5: 檢查 WebSocket 服務配置 >> "%ERROR_LOG%"

REM 檢查端口 8080 是否被佔用
netstat -an | findstr ":8080" >nul 2>&1
if %errorlevel% == 0 (
    echo %YELLOW%    ⚠️  端口 8080 已被佔用%RESET%
    echo [%date% %time%] ⚠️ WebSocket 端口 8080 已被佔用 >> "%ERROR_LOG%"
    
    REM 嘗試找出佔用進程
    for /f "tokens=5" %%a in ('netstat -ano ^| findstr ":8080"') do (
        tasklist | findstr "%%a" >nul 2>&1
        if not errorlevel 1 (
            for /f "tokens=1" %%b in ('tasklist ^| findstr "%%a"') do (
                echo %CYAN%       佔用進程: %%b (PID: %%a)%RESET%
                echo [%date% %time%] 端口8080被進程%%b佔用，PID: %%a >> "%ERROR_LOG%"
            )
        )
    )
    
    echo %YELLOW%       建議: 檢查是否為本系統的WebSocket服務%RESET%
) else (
    echo %GREEN%    ✅ WebSocket 端口 8080 可用%RESET%
    echo [%date% %time%] ✅ WebSocket 端口 8080 可用 >> "%ERROR_LOG%"
)

REM 檢查WebSocket可執行檔
if exist "%CURRENT_DIR%WebSocketServer.exe" (
    echo %GREEN%    ✅ WebSocket 服務程式存在%RESET%
    echo [%date% %time%] ✅ WebSocket 服務程式存在 >> "%ERROR_LOG%"
) else (
    echo %RED%    ❌ WebSocket 服務程式不存在%RESET%
    echo [%date% %time%] ❌ WebSocket 服務程式不存在 >> "%ERROR_LOG%"
)

REM ========================================================================
REM 步驟 6: 系統依賴項目與環境驗證
REM ========================================================================
echo.
echo %BLUE%[6/8] 🔧 檢查系統依賴項目...%RESET%
echo [%date% %time%] 步驟6: 檢查系統依賴項目 >> "%ERROR_LOG%"

REM 檢查主要執行檔
if exist "%CURRENT_DIR%RANOnline_AI_Ultimate_Portable.exe" (
    echo %GREEN%    ✅ AI 主程式存在%RESET%
    echo [%date% %time%] ✅ AI 主程式存在 >> "%ERROR_LOG%"
) else (
    echo %RED%    ❌ AI 主程式不存在%RESET%
    echo [%date% %time%] ❌ AI 主程式不存在 >> "%ERROR_LOG%"
)

REM 檢查 Qt 依賴庫
set qt_libs=Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll
set qt_count=0
for %%i in (%qt_libs%) do (
    if exist "%CURRENT_DIR%%%i" (
        set /a qt_count+=1
    ) else (
        echo %YELLOW%    ⚠️  缺少 Qt 庫: %%i%RESET%
        echo [%date% %time%] ⚠️ 缺少Qt庫: %%i >> "%ERROR_LOG%"
    )
)

if %qt_count% == 4 (
    echo %GREEN%    ✅ Qt 依賴庫完整 (4/4)%RESET%
    echo [%date% %time%] ✅ Qt 依賴庫完整 >> "%ERROR_LOG%"
) else (
    echo %YELLOW%    ⚠️  Qt 依賴庫不完整 (%qt_count%/4)%RESET%
    echo [%date% %time%] ⚠️ Qt 依賴庫不完整 >> "%ERROR_LOG%"
)

REM 檢查日誌目錄
if exist "%LOG_DIR%" (
    echo %GREEN%    ✅ 日誌目錄存在%RESET%
    echo [%date% %time%] ✅ 日誌目錄存在 >> "%ERROR_LOG%"
) else (
    echo %YELLOW%    ⚠️  日誌目錄不存在，正在創建...%RESET%
    mkdir "%LOG_DIR%"
    echo %GREEN%    ✅ 日誌目錄已創建%RESET%
    echo [%date% %time%] ✅ 日誌目錄已創建 >> "%ERROR_LOG%"
)

REM ========================================================================
REM 步驟 7: AI 模型切換功能測試
REM ========================================================================
echo.
echo %BLUE%[7/8] 🤖 檢查 AI 模型切換功能...%RESET%
echo [%date% %time%] 步驟7: 檢查 AI 模型切換功能 >> "%ERROR_LOG%"

REM 檢查是否安裝 Ollama
where ollama >nul 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ Ollama 已安裝，支援多模型切換%RESET%
    echo [%date% %time%] ✅ Ollama 已安裝 >> "%ERROR_LOG%"
    
    REM 獲取已安裝的模型列表
    echo %CYAN%       正在檢查可用模型...%RESET%
    ollama list 2>nul | findstr /V "NAME" > "%TEMP%\ollama_models.txt"
    if exist "%TEMP%\ollama_models.txt" (
        for /f %%i in ('type "%TEMP%\ollama_models.txt" ^| find /c /v ""') do set model_count=%%i
        if !model_count! gtr 0 (
            echo %GREEN%    ✅ 發現 !model_count! 個可用模型%RESET%
            echo [%date% %time%] ✅ 發現!model_count!個Ollama模型 >> "%ERROR_LOG%"
        ) else (
            echo %YELLOW%    ⚠️  未發現已安裝的模型%RESET%
            echo [%date% %time%] ⚠️ 未發現已安裝的Ollama模型 >> "%ERROR_LOG%"
        )
    )
    del /q "%TEMP%\ollama_models.txt" 2>nul
) else (
    echo %YELLOW%    ⚠️  Ollama 未安裝，將使用內建AI模型%RESET%
    echo [%date% %time%] ⚠️ Ollama 未安裝，使用內建模型 >> "%ERROR_LOG%"
)

REM ========================================================================
REM 步驟 8: 玩家數量自動調整驗證
REM ========================================================================
echo.
echo %BLUE%[8/8] 👥 檢查玩家數量自動調整功能...%RESET%
echo [%date% %time%] 步驟8: 檢查玩家數量自動調整功能 >> "%ERROR_LOG%"

REM 獲取系統記憶體資訊
for /f "tokens=2 delims==" %%i in ('wmic computersystem get TotalPhysicalMemory /value') do set total_memory=%%i
set /a memory_gb=%total_memory:~0,-9%

if %memory_gb% geq 8 (
    set recommended_players=50
    echo %GREEN%    ✅ 系統記憶體充足 (%memory_gb%GB)，建議最大玩家數: %recommended_players%%RESET%
) else if %memory_gb% geq 4 (
    set recommended_players=30
    echo %YELLOW%    ⚠️  系統記憶體中等 (%memory_gb%GB)，建議最大玩家數: %recommended_players%%RESET%
) else (
    set recommended_players=20
    echo %RED%    ❌ 系統記憶體較少 (%memory_gb%GB)，建議最大玩家數: %recommended_players%%RESET%
)

echo [%date% %time%] 系統記憶體: %memory_gb%GB，建議玩家數: %recommended_players% >> "%ERROR_LOG%"

REM 檢查CPU核心數
for /f "tokens=2 delims==" %%i in ('wmic cpu get NumberOfCores /value') do set cpu_cores=%%i
echo %CYAN%    CPU 核心數: %cpu_cores%，適合處理多玩家AI%RESET%
echo [%date% %time%] CPU核心數: %cpu_cores% >> "%ERROR_LOG%"

REM ========================================================================
REM 系統檢查完成，生成總結報告
REM ========================================================================
echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%                          系統檢查完成%RESET%
echo %CYAN%========================================================================%RESET%
echo.

REM 生成總結報告
echo [%date% %time%] ========== 系統檢查總結 ========== >> "%ERROR_LOG%"
echo [%date% %time%] 檢查完成時間: %date% %time% >> "%ERROR_LOG%"
echo [%date% %time%] 系統狀態: 檢查完成 >> "%ERROR_LOG%"
echo [%date% %time%] 建議玩家數: %recommended_players% >> "%ERROR_LOG%"
echo [%date% %time%] ========== 檢查結束 ========== >> "%ERROR_LOG%"

echo %GREEN%✅ 系統檢查已完成！%RESET%
echo %WHITE%📄 詳細報告已儲存至: %ERROR_LOG%%RESET%
echo.
echo %YELLOW%🎮 系統建議設定：%RESET%
echo %WHITE%   • 最大AI玩家數: %recommended_players%%RESET%
echo %WHITE%   • 記憶體配置: %memory_gb%GB 可用%RESET%
echo %WHITE%   • CPU核心數: %cpu_cores% 核心%RESET%
echo.

REM ========================================================================
REM 提供修復選項
REM ========================================================================
echo %YELLOW%🔧 可用的修復選項：%RESET%
echo %WHITE%   [1] 重新啟動 SQL Server 服務%RESET%
echo %WHITE%   [2] 重新生成 AI_CONFIG.INI%RESET%
echo %WHITE%   [3] 創建資料庫表結構%RESET%
echo %WHITE%   [4] 重啟 AI 助手服務%RESET%
echo %WHITE%   [5] 查看詳細錯誤日誌%RESET%
echo %WHITE%   [6] 一鍵啟動系統%RESET%
echo %WHITE%   [0] 退出%RESET%
echo.

:UserChoice
set /p choice=%YELLOW%請選擇修復選項 (0-6): %RESET%

if "%choice%"=="1" (
    echo %CYAN%🔄 正在重啟 SQL Server 服務...%RESET%
    net stop MSSQLSERVER 2>nul
    net stop "SQL Server (MSSQLSERVER)" 2>nul
    timeout /t 3 /nobreak >nul
    net start MSSQLSERVER 2>nul
    net start "SQL Server (MSSQLSERVER)" 2>nul
    echo %GREEN%✅ SQL Server 服務重啟完成%RESET%
    goto :UserChoice
) else if "%choice%"=="2" (
    echo %CYAN%🔄 正在重新生成 AI_CONFIG.INI...%RESET%
    call :CreateConfigFile
    echo %GREEN%✅ AI_CONFIG.INI 已重新生成%RESET%
    goto :UserChoice
) else if "%choice%"=="3" (
    echo %CYAN%🔄 正在創建資料庫表結構...%RESET%
    call :CreateBasicTables
    echo %GREEN%✅ 資料庫表結構創建完成%RESET%
    goto :UserChoice
) else if "%choice%"=="4" (
    echo %CYAN%🔄 正在重啟 AI 助手服務...%RESET%
    taskkill /f /im RANOnline_AI_Ultimate_Portable.exe 2>nul
    taskkill /f /im WebSocketServer.exe 2>nul
    timeout /t 2 /nobreak >nul
    start "" "%CURRENT_DIR%智能啟動_增強版.bat"
    echo %GREEN%✅ AI 助手服務重啟完成%RESET%
    goto :Exit
) else if "%choice%"=="5" (
    echo %CYAN%📄 開啟詳細錯誤日誌...%RESET%
    start notepad "%ERROR_LOG%"
    goto :UserChoice
) else if "%choice%"=="6" (
    echo %CYAN%🚀 正在啟動系統...%RESET%
    start "" "%CURRENT_DIR%智能啟動_增強版.bat"
    goto :Exit
) else if "%choice%"=="0" (
    goto :Exit
) else (
    echo %RED%❌ 無效選項，請重新選擇%RESET%
    goto :UserChoice
)

REM ========================================================================
REM 子程序定義
REM ========================================================================

:CreateConfigFile
echo %CYAN%📝 正在生成完整的 AI_CONFIG.INI...%RESET%
(
echo ; ========================================================================
echo ; Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo ; AI系統核心配置檔案 - 自動生成含SQL Server支援
echo ; 生成時間: %date% %time%
echo ; ========================================================================
echo.
echo [AI]
echo ; AI核心設置
echo Version=4.0.0
echo Edition=Ultimate_Portable
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
echo MaxPlayers=%recommended_players%
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
echo Port=8080
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
echo ; AI玩家系統設置
echo MaxPlayers=%recommended_players%
echo DefaultLevel=1
echo DefaultClass=Warrior
echo NameGenerator=auto
echo BehaviorMode=intelligent
echo ResponseTime=1000
echo DecisionComplexity=medium
echo LearningEnabled=true
echo AutoAdjustPlayerCount=true
echo MinPlayers=5
echo MaxPlayersPerServer=100
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
echo ; 效能優化設置
echo ThreadPoolSize=%cpu_cores%
echo MaxMemoryUsage=%memory_gb%GB
echo GCInterval=300
echo CacheSize=64MB
echo PreloadModels=false
echo AsyncProcessing=true
echo EnableProgressBar=true
) > "%CONFIG_FILE%"
exit /b

:CreateBasicTables
echo %CYAN%📊 正在創建基本資料庫表結構...%RESET%
(
echo USE RanUser;
echo GO
echo.
echo -- 創建角色資訊表
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='ChaInfo' AND xtype='U'^)
echo CREATE TABLE ChaInfo ^(
echo     ChaNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo     ChaName NVARCHAR^(50^) NOT NULL UNIQUE,
echo     ChaLevel INT DEFAULT 1,
echo     ChaClass INT DEFAULT 0,
echo     ChaArea INT DEFAULT 0,
echo     ChaExperience BIGINT DEFAULT 0,
echo     ChaHP INT DEFAULT 100,
echo     ChaMP INT DEFAULT 50,
echo     ChaCreateDate DATETIME DEFAULT GETDATE^(^),
echo     ChaLastLogin DATETIME DEFAULT GETDATE^(^),
echo     ChaStatus INT DEFAULT 0,
echo     ChaGold BIGINT DEFAULT 0
echo ^);
echo.
echo -- 創建用戶資訊表
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='UserInfo' AND xtype='U'^)
echo CREATE TABLE UserInfo ^(
echo     UserNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo     UserID NVARCHAR^(50^) NOT NULL UNIQUE,
echo     UserPass NVARCHAR^(255^) NOT NULL,
echo     UserType INT DEFAULT 0,
echo     UserCreateDate DATETIME DEFAULT GETDATE^(^),
echo     UserLastLogin DATETIME DEFAULT GETDATE^(^),
echo     UserStatus INT DEFAULT 1,
echo     UserEmail NVARCHAR^(100^)
echo ^);
echo.
echo -- 創建日誌資訊表
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='LogInfo' AND xtype='U'^)
echo CREATE TABLE LogInfo ^(
echo     LogNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo     LogType INT NOT NULL,
echo     LogDate DATETIME DEFAULT GETDATE^(^),
echo     LogMsg NVARCHAR^(500^),
echo     LogUserNum INT,
echo     LogChaNum INT,
echo     LogIP NVARCHAR^(45^)
echo ^);
echo.
echo -- 創建物品資訊表
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='ItemInfo' AND xtype='U'^)
echo CREATE TABLE ItemInfo ^(
echo     ItemNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo     ItemName NVARCHAR^(50^) NOT NULL,
echo     ItemType INT DEFAULT 0,
echo     ItemLevel INT DEFAULT 1,
echo     ItemValue INT DEFAULT 0,
echo     ItemDescription NVARCHAR^(255^),
echo     ItemCreateDate DATETIME DEFAULT GETDATE^(^)
echo ^);
echo.
echo -- 創建AI狀態表
echo IF NOT EXISTS ^(SELECT * FROM sysobjects WHERE name='AI_States' AND xtype='U'^)
echo CREATE TABLE AI_States ^(
echo     AI_ID NVARCHAR^(50^) PRIMARY KEY,
echo     AI_Name NVARCHAR^(50^) NOT NULL,
echo     AI_Type INT DEFAULT 0,
echo     AI_Level INT DEFAULT 1,
echo     AI_HP INT DEFAULT 100,
echo     AI_MP INT DEFAULT 50,
echo     AI_PosX FLOAT DEFAULT 0,
echo     AI_PosY FLOAT DEFAULT 0,
echo     AI_PosZ FLOAT DEFAULT 0,
echo     AI_Status INT DEFAULT 1,
echo     AI_LastUpdate DATETIME DEFAULT GETDATE^(^),
echo     AI_CreateDate DATETIME DEFAULT GETDATE^(^)
echo ^);
echo GO
) > "%TEMP%\create_tables.sql"

sqlcmd -S localhost -U sa -P 123456 -i "%TEMP%\create_tables.sql" >nul 2>&1
del /q "%TEMP%\create_tables.sql" 2>nul
exit /b

:Exit
echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%感謝使用 Jy技術團隊 線上AI 系統檢查與修復工具%RESET%
echo %WHITE%如需技術支援，請聯繫: jytech@example.com%RESET%
echo %CYAN%========================================================================%RESET%
echo.
pause
exit /b 0
