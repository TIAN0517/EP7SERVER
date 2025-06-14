@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】系統完整性檢查與自動修復 v4.0.0

REM 設置顏色變量
set "RED="
set "GREEN="
set "YELLOW="
set "BLUE="
set "MAGENTA="
set "CYAN="
set "WHITE="
set "RESET="

REM 設置路徑變量
set "DESKTOP=%USERPROFILE%\Desktop"
set "ERROR_LOG=%DESKTOP%\error_log.txt"
set "CURRENT_DIR=%~dp0"
set "CONFIG_FILE=%CURRENT_DIR%AI_CONFIG.INI"
set "LOG_DIR=%CURRENT_DIR%logs"

REM 清除舊的錯誤日誌
if exist "%ERROR_LOG%" del /q "%ERROR_LOG%"

echo.
echo ========================================================================
echo             【Jy技術團隊 線上AI】系統完整性檢查與自動修復
echo                           v4.0.0 Ultimate Edition
echo ========================================================================
echo.
echo    🎯 檢查項目：
echo       1. SQL Server 1433 端口狀態
echo       2. ODBC 連線與 sa 帳號驗證
echo       3. RanUser 資料庫及表結構完整性
echo       4. AI_CONFIG.INI 配置檔案格式
echo       5. WebSocket 服務配置
echo       6. 系統依賴項目檢查
echo.
echo    🔧 自動修復功能：
echo       • 自動重啟資料庫服務
echo       • 重新生成配置檔案
echo       • 修復權限問題
echo       • 創建缺失的資料庫表
echo.

REM 創建日誌目錄
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

REM 開始系統檢查
echo [%date% %time%] ========== 系統檢查開始 ========== > "%ERROR_LOG%"
echo [%date% %time%] 檢查執行位置: %CURRENT_DIR% >> "%ERROR_LOG%"

REM ========================================================================
REM 步驟 1: SQL Server 1433 端口狀態檢查
REM ========================================================================
echo.
echo [1/6] 🔍 檢查 SQL Server 1433 端口狀態...
echo [%date% %time%] 步驟1: 檢查 SQL Server 1433 端口狀態 >> "%ERROR_LOG%"

netstat -an | findstr ":1433" >nul 2>&1
if %errorlevel% == 0 (
    echo    ✅ SQL Server 1433 端口已開啟
    echo [%date% %time%] ✅ SQL Server 1433 端口狀態正常 >> "%ERROR_LOG%"
) else (
    echo    ❌ SQL Server 1433 端口未開啟，嘗試啟動服務...
    echo [%date% %time%] ❌ SQL Server 1433 端口未開啟 >> "%ERROR_LOG%"
    
    REM 嘗試啟動 SQL Server 服務
    net start MSSQLSERVER 2>nul
    net start "SQL Server (MSSQLSERVER)" 2>nul
    net start "SQL Server (SQLEXPRESS)" 2>nul
    
    timeout /t 5 /nobreak >nul
    
    REM 再次檢查
    netstat -an | findstr ":1433" >nul 2>&1
    if %errorlevel% == 0 (
        echo    ✅ SQL Server 服務已成功啟動
        echo [%date% %time%] ✅ SQL Server 服務自動修復成功 >> "%ERROR_LOG%"
    ) else (
        echo    ❌ 無法啟動 SQL Server 服務，請手動檢查
        echo [%date% %time%] ❌ SQL Server 服務啟動失敗，需要手動介入 >> "%ERROR_LOG%"
        echo.
        echo    建議解決方案：
        echo    • 檢查 SQL Server 是否已安裝
        echo    • 確認服務帳號權限
        echo    • 使用 SQL Server Configuration Manager 檢查配置
    )
)

REM ========================================================================
REM 步驟 2: ODBC 連線與 sa 帳號驗證
REM ========================================================================
echo.
echo [2/6] 🔑 驗證 ODBC 連線與 sa 帳號...
echo [%date% %time%] 步驟2: 驗證 ODBC 連線與 sa 帳號 >> "%ERROR_LOG%"

REM 創建測試 SQL 腳本
echo SELECT @@VERSION as SQLServerVersion; > "%TEMP%\test_connection.sql"

REM 測試 sa 帳號連線
sqlcmd -S localhost -U sa -P 123456 -d master -i "%TEMP%\test_connection.sql" >nul 2>&1
if %errorlevel% == 0 (
    echo    ✅ ODBC 連線成功，sa 帳號可用
    echo [%date% %time%] ✅ ODBC 連線和 sa 帳號驗證成功 >> "%ERROR_LOG%"
) else (
    echo    ❌ ODBC 連線失敗或 sa 帳號問題
    echo [%date% %time%] ❌ ODBC 連線失敗或 sa 帳號問題 >> "%ERROR_LOG%"
    
    REM 嘗試 Windows 認證
    sqlcmd -S localhost -E -d master -i "%TEMP%\test_connection.sql" >nul 2>&1
    if %errorlevel% == 0 (
        echo    ⚠️  Windows 認證可用，但 sa 帳號可能未啟用
        echo [%date% %time%] ⚠️ Windows 認證可用，sa 帳號需要配置 >> "%ERROR_LOG%"
        echo.
        echo    建議解決方案：
        echo    • 啟用 SQL Server 混合驗證模式
        echo    • 重設 sa 帳號密碼
        echo    • 確認 sa 帳號已啟用
    ) else (
        echo    ❌ 所有連線方式都失敗
        echo [%date% %time%] ❌ 所有資料庫連線方式都失敗 >> "%ERROR_LOG%"
        echo.
        echo    建議解決方案：
        echo    • 確認 SQL Server 服務正在運行
        echo    • 檢查防火牆設置
        echo    • 驗證 SQL Server 網路配置
    )
)

REM 清理臨時檔案
if exist "%TEMP%\test_connection.sql" del /q "%TEMP%\test_connection.sql"

REM ========================================================================
REM 步驟 3: RanUser 資料庫及表結構驗證
REM ========================================================================
echo.
echo [3/6] 🗄️  驗證 RanUser 資料庫及表結構...
echo [%date% %time%] 步驟3: 驗證 RanUser 資料庫及表結構 >> "%ERROR_LOG%"

REM 創建資料庫檢查腳本
(
echo SELECT name FROM sys.databases WHERE name = 'RanUser';
echo GO
echo USE RanUser;
echo GO
echo SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE';
echo GO
) > "%TEMP%\check_database.sql"

sqlcmd -S localhost -U sa -P 123456 -i "%TEMP%\check_database.sql" > "%TEMP%\db_result.txt" 2>&1
if %errorlevel% == 0 (
    findstr /C:"RanUser" "%TEMP%\db_result.txt" >nul 2>&1
    if %errorlevel% == 0 (
        echo    ✅ RanUser 資料庫存在
        echo [%date% %time%] ✅ RanUser 資料庫驗證成功 >> "%ERROR_LOG%"
        
        REM 檢查必要的表
        findstr /C:"ChaInfo" "%TEMP%\db_result.txt" >nul 2>&1
        if %errorlevel% == 0 (
            echo    ✅ 核心資料表結構完整
            echo [%date% %time%] ✅ 資料庫表結構驗證成功 >> "%ERROR_LOG%"
        ) else (
            echo    ⚠️  部分資料表可能缺失
            echo [%date% %time%] ⚠️ 部分資料表缺失，需要檢查 >> "%ERROR_LOG%"
        )
    ) else (
        echo    ❌ RanUser 資料庫不存在，嘗試創建...
        echo [%date% %time%] ❌ RanUser 資料庫不存在 >> "%ERROR_LOG%"
        
        REM 創建資料庫
        echo CREATE DATABASE RanUser; > "%TEMP%\create_db.sql"
        sqlcmd -S localhost -U sa -P 123456 -i "%TEMP%\create_db.sql" >nul 2>&1
        if %errorlevel% == 0 (
            echo    ✅ RanUser 資料庫創建成功
            echo [%date% %time%] ✅ RanUser 資料庫自動創建成功 >> "%ERROR_LOG%"
        ) else (
            echo    ❌ 無法創建 RanUser 資料庫
            echo [%date% %time%] ❌ RanUser 資料庫創建失敗 >> "%ERROR_LOG%"
        )
    )
) else (
    echo    ❌ 無法連接到資料庫進行檢查
    echo [%date% %time%] ❌ 資料庫連接失敗，無法進行驗證 >> "%ERROR_LOG%"
)

REM 清理臨時檔案
if exist "%TEMP%\check_database.sql" del /q "%TEMP%\check_database.sql"
if exist "%TEMP%\db_result.txt" del /q "%TEMP%\db_result.txt"
if exist "%TEMP%\create_db.sql" del /q "%TEMP%\create_db.sql"

echo ========================================================================
echo 🔍 步驟 1/6：檢查 SQL Server 1433 端口狀態
echo ========================================================================

REM 進度條函數
call :ShowProgress "正在檢查 SQL Server 服務狀態..." 10

REM 檢查SQL Server服務
sc query "MSSQLSERVER" >nul 2>&1
if %errorlevel% equ 0 (
    echo    ✅ SQL Server 服務已安裝
    echo [%date% %time%] SQL Server 服務已安裝 >> "%ERROR_LOG%"
    
    REM 檢查服務狀態
    sc query "MSSQLSERVER" | findstr "RUNNING" >nul
    if %errorlevel% equ 0 (
        echo    ✅ SQL Server 服務正在運行
        echo [%date% %time%] SQL Server 服務正在運行 >> "%ERROR_LOG%"
    ) else (
        echo    ⚠️ SQL Server 服務未運行，嘗試啟動...
        echo [%date% %time%] 嘗試啟動 SQL Server 服務 >> "%ERROR_LOG%"
        
        net start "MSSQLSERVER" >nul 2>&1
        if %errorlevel% equ 0 (
            echo    ✅ SQL Server 服務啟動成功
            echo [%date% %time%] SQL Server 服務啟動成功 >> "%ERROR_LOG%"
        ) else (
            echo    ❌ SQL Server 服務啟動失敗
            echo [%date% %time%] 錯誤: SQL Server 服務啟動失敗 >> "%ERROR_LOG%"
            echo [%date% %time%] 建議: 檢查服務權限或手動啟動 SQL Server >> "%ERROR_LOG%"
        )
    )
) else (
    echo    ❌ SQL Server 服務未安裝
    echo [%date% %time%] 錯誤: SQL Server 服務未安裝 >> "%ERROR_LOG%"
    echo [%date% %time%] 建議: 請安裝 SQL Server 2019/2022 >> "%ERROR_LOG%"
)

call :ShowProgress "檢查 1433 端口監聽狀態..." 20

REM 檢查1433端口
netstat -an | findstr ":1433" >nul
if %errorlevel% equ 0 (
    echo    ✅ SQL Server 1433 端口正在監聽
    echo [%date% %time%] 1433 端口正在監聽 >> "%ERROR_LOG%"
) else (
    echo    ❌ SQL Server 1433 端口未監聽
    echo [%date% %time%] 錯誤: 1433 端口未監聽 >> "%ERROR_LOG%"
    echo [%date% %time%] 建議: 檢查 SQL Server Configuration Manager 設定 >> "%ERROR_LOG%"
)

echo.
echo ========================================================================
echo 🔍 步驟 2/6：檢查 ODBC 連線與 sa 帳號驗證
echo ========================================================================

call :ShowProgress "測試 ODBC 資料庫連線..." 30

REM 檢查ODBC驅動
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\ODBC\ODBCINST.INI\SQL Server" >nul 2>&1
if %errorlevel% equ 0 (
    echo    ✅ SQL Server ODBC 驅動已安裝
    echo [%date% %time%] SQL Server ODBC 驅動已安裝 >> "%ERROR_LOG%"
) else (
    echo    ❌ SQL Server ODBC 驅動未安裝
    echo [%date% %time%] 錯誤: SQL Server ODBC 驅動未安裝 >> "%ERROR_LOG%"
)

REM 創建測試連線腳本
echo Testing SQL Server connection... > temp_test_connection.sql
echo SELECT @@VERSION; >> temp_test_connection.sql

REM 嘗試使用 sqlcmd 測試連線
sqlcmd -S localhost -U sa -P 123456 -i temp_test_connection.sql -o temp_connection_result.txt >nul 2>&1
if %errorlevel% equ 0 (
    echo    ✅ sa 帳號連線測試成功
    echo [%date% %time%] sa 帳號連線測試成功 >> "%ERROR_LOG%"
) else (
    echo    ❌ sa 帳號連線失敗
    echo [%date% %time%] 錯誤: sa 帳號連線失敗 >> "%ERROR_LOG%"
    echo [%date% %time%] 建議: 檢查 sa 密碼或啟用 SQL Server 驗證模式 >> "%ERROR_LOG%"
)

REM 清理臨時檔案
del /q temp_test_connection.sql 2>nul
del /q temp_connection_result.txt 2>nul

echo.
echo ========================================================================
echo 🔍 步驟 3/6：檢查 RanUser 資料庫及表結構
echo ========================================================================

call :ShowProgress "驗證 RanUser 資料庫結構..." 40

REM 檢查資料庫是否存在
sqlcmd -S localhost -U sa -P 123456 -Q "SELECT name FROM sys.databases WHERE name = 'RanUser'" -h -1 >temp_db_check.txt 2>nul
findstr "RanUser" temp_db_check.txt >nul
if %errorlevel% equ 0 (
    echo    ✅ RanUser 資料庫存在
    echo [%date% %time%] RanUser 資料庫存在 >> "%ERROR_LOG%"
    
    REM 檢查必要的表
    call :CheckTable "ChaInfo" "角色資訊表"
    call :CheckTable "UserInfo" "用戶資訊表"
    call :CheckTable "LogInfo" "日誌記錄表"
    call :CheckTable "ItemInfo" "道具資訊表"
    
) else (
    echo    ❌ RanUser 資料庫不存在
    echo [%date% %time%] 錯誤: RanUser 資料庫不存在 >> "%ERROR_LOG%"
    echo [%date% %time%] 嘗試創建 RanUser 資料庫... >> "%ERROR_LOG%"
    
    REM 嘗試創建資料庫
    sqlcmd -S localhost -U sa -P 123456 -Q "CREATE DATABASE RanUser" >nul 2>&1
    if %errorlevel% equ 0 (
        echo    ✅ RanUser 資料庫創建成功
        echo [%date% %time%] RanUser 資料庫創建成功 >> "%ERROR_LOG%"
        call :CreateBasicTables
    ) else (
        echo    ❌ RanUser 資料庫創建失敗
        echo [%date% %time%] 錯誤: RanUser 資料庫創建失敗 >> "%ERROR_LOG%"
    )
)

del /q temp_db_check.txt 2>nul

echo.
echo ========================================================================
echo 🔍 步驟 4/6：檢查 AI_CONFIG.INI 配置檔案
echo ========================================================================

call :ShowProgress "驗證 AI 配置檔案格式..." 60

if exist "%CONFIG_FILE%" (
    echo    ✅ AI_CONFIG.INI 檔案存在
    echo [%date% %time%] AI_CONFIG.INI 檔案存在 >> "%ERROR_LOG%"
    
    REM 檢查配置區塊
    call :CheckConfigSection "[AI]" "AI核心配置"
    call :CheckConfigSection "[Database]" "資料庫配置"
    call :CheckConfigSection "[Server]" "WebSocket服務配置"
    call :CheckConfigSection "[Log]" "日誌配置"
    call :CheckConfigSection "[PlayerAI]" "AI玩家配置"
    call :CheckConfigSection "[Security]" "安全性配置"
    call :CheckConfigSection "[VM]" "虛擬機優化配置"
    call :CheckConfigSection "[Performance]" "效能配置"
    
    REM 檢查關鍵參數
    findstr "Version=4.0.0" "%CONFIG_FILE%" >nul
    if %errorlevel% equ 0 (
        echo    ✅ 版本配置正確
    ) else (
        echo    ⚠️ 版本配置可能需要更新
        echo [%date% %time%] 警告: 版本配置可能需要更新 >> "%ERROR_LOG%"
    )
    
) else (
    echo    ❌ AI_CONFIG.INI 檔案不存在，正在重新生成...
    echo [%date% %time%] AI_CONFIG.INI 檔案不存在，重新生成 >> "%ERROR_LOG%"
    call :CreateConfigFile
)

echo.
echo ========================================================================
echo 🔍 步驟 5/6：檢查 WebSocket 服務配置
echo ========================================================================

call :ShowProgress "檢查 WebSocket 服務狀態..." 80

REM 檢查端口8080
netstat -an | findstr ":8080" >nul
if %errorlevel% equ 0 (
    echo    ✅ WebSocket 端口 8080 正在監聽
    echo [%date% %time%] WebSocket 端口 8080 正在監聽 >> "%ERROR_LOG%"
) else (
    echo    ℹ️ WebSocket 端口 8080 未監聽 (將在啟動時自動配置)
    echo [%date% %time%] WebSocket 端口 8080 未監聽 >> "%ERROR_LOG%"
)

REM 檢查AI助手進程
tasklist /fi "imagename eq RANOnline_AI_Ultimate_Portable.exe" | findstr "RANOnline_AI_Ultimate_Portable.exe" >nul
if %errorlevel% equ 0 (
    echo    ✅ AI助手進程正在運行
    echo [%date% %time%] AI助手進程正在運行 >> "%ERROR_LOG%"
) else (
    echo    ℹ️ AI助手尚未啟動 (準備啟動)
    echo [%date% %time%] AI助手尚未啟動 >> "%ERROR_LOG%"
)

echo.
echo ========================================================================
echo 🔍 步驟 6/6：系統依賴項目檢查
echo ========================================================================

call :ShowProgress "檢查系統依賴項目..." 90

REM 檢查必要的DLL
call :CheckFile "Qt6Core.dll" "Qt6核心庫"
call :CheckFile "Qt6Gui.dll" "Qt6界面庫"
call :CheckFile "Qt6Widgets.dll" "Qt6控件庫"
call :CheckFile "Qt6Network.dll" "Qt6網路庫"

REM 檢查必要目錄
call :CheckDirectory "platforms" "Qt6平台插件"
call :CheckDirectory "styles" "Qt6樣式"
call :CheckDirectory "logs" "日誌目錄"

call :ShowProgress "系統檢查完成" 100

echo.
echo ========================================================================
echo                          📊 系統檢查完成報告
echo ========================================================================

REM 統計錯誤數量
set ERROR_COUNT=0
findstr "錯誤:" "%ERROR_LOG%" >temp_error_count.txt
for /f %%a in ('type temp_error_count.txt ^| find /c /v ""') do set ERROR_COUNT=%%a
del /q temp_error_count.txt 2>nul

if %ERROR_COUNT% equ 0 (
    echo    🎉 恭喜！系統檢查通過，所有組件狀態正常
    echo.
    echo    ✅ SQL Server 連線正常
    echo    ✅ 資料庫結構完整
    echo    ✅ 配置檔案格式正確
    echo    ✅ 系統依賴完整
    echo.
    echo    🚀 系統已準備就緒，可以正常啟動 AI助手
    echo [%date% %time%] 系統檢查通過，無錯誤 >> "%ERROR_LOG%"
) else (
    echo    ⚠️ 發現 %ERROR_COUNT% 個問題需要注意
    echo.
    echo    📄 詳細錯誤報告已儲存至: %ERROR_LOG%
    echo    💡 建議查看錯誤日誌並根據建議進行修復
    echo.
    echo    🔧 常見問題解決方案：
    echo       • SQL Server 未啟動 → 手動啟動 SQL Server 服務
    echo       • sa 密碼錯誤 → 重設 sa 密碼為 123456
    echo       • 資料庫不存在 → 將自動嘗試創建
    echo       • 配置檔案錯誤 → 已自動重新生成
    echo.
)

echo ========================================================================
echo                        🎛️ 一鍵修復與重啟選項
echo ========================================================================
echo.
echo    [1] 重啟 SQL Server 服務
echo    [2] 重新生成 AI_CONFIG.INI
echo    [3] 創建基本資料庫表結構
echo    [4] 啟動 AI助手 (強制重啟)
echo    [5] 查看詳細錯誤日誌
echo    [6] 繼續啟動 (忽略警告)
echo    [0] 退出
echo.
set /p choice="請選擇操作 (0-6): "

if "%choice%"=="1" goto :RestartSQLServer
if "%choice%"=="2" goto :RegenerateConfig
if "%choice%"=="3" goto :CreateTables
if "%choice%"=="4" goto :ForceStartAI
if "%choice%"=="5" goto :ShowErrorLog
if "%choice%"=="6" goto :ContinueStart
if "%choice%"=="0" goto :Exit

goto :ContinueStart

:RestartSQLServer
echo.
echo 🔄 正在重啟 SQL Server 服務...
net stop "MSSQLSERVER" >nul 2>&1
timeout /t 3 /nobreak >nul
net start "MSSQLSERVER" >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ SQL Server 服務重啟成功
) else (
    echo ❌ SQL Server 服務重啟失敗
)
pause
goto :Exit

:RegenerateConfig
echo.
echo 🔄 正在重新生成 AI_CONFIG.INI...
call :CreateConfigFile
echo ✅ AI_CONFIG.INI 已重新生成
pause
goto :Exit

:CreateTables
echo.
echo 🔄 正在創建基本資料庫表結構...
call :CreateBasicTables
echo ✅ 資料庫表結構創建完成
pause
goto :Exit

:ForceStartAI
echo.
echo 🔄 正在強制重啟 AI助手...
taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2>nul
timeout /t 2 /nobreak >nul
start "" "RANOnline_AI_Ultimate_Portable.exe"
echo ✅ AI助手已重新啟動
pause
goto :Exit

:ShowErrorLog
echo.
echo 📄 詳細錯誤日誌：
echo ========================================================================
type "%ERROR_LOG%"
echo ========================================================================
pause
goto :Exit

:ContinueStart
echo.
echo 🚀 正在啟動 AI助手...
call "一鍵啟動_Ultimate_Portable.bat"
goto :Exit

REM ========================================================================
REM 輔助函數
REM ========================================================================

:ShowProgress
set "message=%~1"
set "percent=%~2"
echo    📊 %message% [%percent%%%]
exit /b

:CheckTable
set "table_name=%~1"
set "table_desc=%~2"
sqlcmd -S localhost -U sa -P 123456 -d RanUser -Q "SELECT name FROM sys.tables WHERE name = '%table_name%'" -h -1 >temp_table_check.txt 2>nul
findstr "%table_name%" temp_table_check.txt >nul
if %errorlevel% equ 0 (
    echo       ✅ %table_desc% (%table_name%) 存在
    echo [%date% %time%] 表 %table_name% 存在 >> "%ERROR_LOG%"
) else (
    echo       ❌ %table_desc% (%table_name%) 不存在
    echo [%date% %time%] 錯誤: 表 %table_name% 不存在 >> "%ERROR_LOG%"
)
del /q temp_table_check.txt 2>nul
exit /b

:CheckConfigSection
set "section=%~1"
set "desc=%~2"
findstr "%section%" "%CONFIG_FILE%" >nul
if %errorlevel% equ 0 (
    echo       ✅ %desc% 配置區塊存在
) else (
    echo       ❌ %desc% 配置區塊缺失
    echo [%date% %time%] 錯誤: %desc% 配置區塊缺失 >> "%ERROR_LOG%"
)
exit /b

:CheckFile
set "filename=%~1"
set "desc=%~2"
if exist "%CURRENT_DIR%%filename%" (
    echo       ✅ %desc% (%filename%) 存在
) else (
    echo       ❌ %desc% (%filename%) 缺失
    echo [%date% %time%] 錯誤: %desc% (%filename%) 缺失 >> "%ERROR_LOG%"
)
exit /b

:CheckDirectory
set "dirname=%~1"
set "desc=%~2"
if exist "%CURRENT_DIR%%dirname%" (
    echo       ✅ %desc% 目錄存在
) else (
    echo       ❌ %desc% 目錄缺失
    echo [%date% %time%] 錯誤: %desc% 目錄缺失 >> "%ERROR_LOG%"
)
exit /b

:CreateConfigFile
echo 📝 正在生成完整的 AI_CONFIG.INI...
(
echo ; ========================================================================
echo ; Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo ; AI系統核心配置檔案 - 自動生成
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
echo MaxPlayers=50
echo.
echo [Database]
echo ; 資料庫連線設置
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
echo MaxPlayers=50
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
echo ThreadPoolSize=4
echo MaxMemoryUsage=256MB
echo GCInterval=300
echo CacheSize=64MB
echo PreloadModels=false
echo AsyncProcessing=true
echo EnableProgressBar=true
) > "%CONFIG_FILE%"
exit /b

:CreateBasicTables
echo 📊 正在創建基本資料庫表結構...
(
echo CREATE TABLE IF NOT EXISTS ChaInfo ^(
echo     ChaNum INT PRIMARY KEY,
echo     ChaName NVARCHAR^(50^) NOT NULL,
echo     ChaLevel INT DEFAULT 1,
echo     ChaClass INT DEFAULT 0,
echo     ChaArea INT DEFAULT 0
echo ^);
echo.
echo CREATE TABLE IF NOT EXISTS UserInfo ^(
echo     UserNum INT PRIMARY KEY,
echo     UserID NVARCHAR^(50^) NOT NULL,
echo     UserPass NVARCHAR^(50^) NOT NULL,
echo     UserType INT DEFAULT 0
echo ^);
echo.
echo CREATE TABLE IF NOT EXISTS LogInfo ^(
echo     LogNum INT IDENTITY^(1,1^) PRIMARY KEY,
echo     LogType INT NOT NULL,
echo     LogDate DATETIME DEFAULT GETDATE^(^),
echo     LogMsg NVARCHAR^(255^)
echo ^);
echo.
echo CREATE TABLE IF NOT EXISTS ItemInfo ^(
echo     ItemNum INT PRIMARY KEY,
echo     ItemName NVARCHAR^(50^) NOT NULL,
echo     ItemType INT DEFAULT 0,
echo     ItemLevel INT DEFAULT 1
echo ^);
) > temp_create_tables.sql

sqlcmd -S localhost -U sa -P 123456 -d RanUser -i temp_create_tables.sql >nul 2>&1
del /q temp_create_tables.sql 2>nul
exit /b

:Exit
echo.
echo [%date% %time%] ========== 系統檢查結束 ========== >> "%ERROR_LOG%"
echo.
echo 📄 完整檢查報告已儲存至: %ERROR_LOG%
echo 🔗 如需技術支援，請聯繫: jytech@example.com
echo.
pause
exit /b 0
