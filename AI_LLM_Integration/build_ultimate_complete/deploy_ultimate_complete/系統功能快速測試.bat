@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】系統功能快速測試 v4.0.0

REM 設置顏色變量
for /f %%A in ('echo prompt $E ^| cmd') do set "ESC=%%A"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "YELLOW=%ESC%[93m"
set "BLUE=%ESC%[94m"
set "CYAN=%ESC%[96m"
set "WHITE=%ESC%[97m"
set "RESET=%ESC%[0m"

set "CURRENT_DIR=%~dp0"
set "DESKTOP=%USERPROFILE%\Desktop"
set "TEST_LOG=%DESKTOP%\system_test_log.txt"

echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%          【Jy技術團隊 線上AI】系統功能快速測試%RESET%
echo %WHITE%                        v4.0.0 Ultimate Edition%RESET%
echo %CYAN%========================================================================%RESET%
echo.

echo [%date% %time%] ========== 系統功能測試開始 ========== > "%TEST_LOG%"

echo %YELLOW%🧪 正在測試系統各項功能...%RESET%
echo.

REM ========================================================================
REM 測試1: 檢查腳本檔案完整性
REM ========================================================================
echo %BLUE%[1/6] 🔍 檢查腳本檔案完整性...%RESET%
set script_count=0

if exist "%CURRENT_DIR%一鍵系統檢查與修復.bat" (
    echo %GREEN%    ✅ 一鍵系統檢查與修復.bat%RESET%
    set /a script_count+=1
    echo [%date% %time%] ✅ 一鍵系統檢查與修復.bat 存在 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ 一鍵系統檢查與修復.bat%RESET%
    echo [%date% %time%] ❌ 一鍵系統檢查與修復.bat 缺失 >> "%TEST_LOG%"
)

if exist "%CURRENT_DIR%智能啟動_增強版_SQL支援.bat" (
    echo %GREEN%    ✅ 智能啟動_增強版_SQL支援.bat%RESET%
    set /a script_count+=1
    echo [%date% %time%] ✅ 智能啟動_增強版_SQL支援.bat 存在 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ 智能啟動_增強版_SQL支援.bat%RESET%
    echo [%date% %time%] ❌ 智能啟動_增強版_SQL支援.bat 缺失 >> "%TEST_LOG%"
)

if exist "%CURRENT_DIR%SQL_資料庫表結構檢查.bat" (
    echo %GREEN%    ✅ SQL_資料庫表結構檢查.bat%RESET%
    set /a script_count+=1
    echo [%date% %time%] ✅ SQL_資料庫表結構檢查.bat 存在 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ SQL_資料庫表結構檢查.bat%RESET%
    echo [%date% %time%] ❌ SQL_資料庫表結構檢查.bat 缺失 >> "%TEST_LOG%"
)

if exist "%CURRENT_DIR%系統完整性檢查.bat" (
    echo %GREEN%    ✅ 系統完整性檢查.bat%RESET%
    set /a script_count+=1
    echo [%date% %time%] ✅ 系統完整性檢查.bat 存在 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ 系統完整性檢查.bat%RESET%
    echo [%date% %time%] ❌ 系統完整性檢查.bat 缺失 >> "%TEST_LOG%"
)

echo %CYAN%    腳本完整性: %script_count%/4 個檔案存在%RESET%

REM ========================================================================
REM 測試2: AI_CONFIG.INI 配置檔案測試
REM ========================================================================
echo.
echo %BLUE%[2/6] 📋 測試 AI_CONFIG.INI 配置...%RESET%

if exist "%CURRENT_DIR%AI_CONFIG.INI" (
    echo %GREEN%    ✅ AI_CONFIG.INI 檔案存在%RESET%
    echo [%date% %time%] ✅ AI_CONFIG.INI 檔案存在 >> "%TEST_LOG%"
    
    REM 檢查必要區塊
    set config_sections=0
    findstr /C:"[AI]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && (
        echo %GREEN%    ✅ [AI] 區塊%RESET%
        set /a config_sections+=1
    )
    findstr /C:"[Database]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && (
        echo %GREEN%    ✅ [Database] 區塊%RESET%
        set /a config_sections+=1
    )
    findstr /C:"[Server]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && (
        echo %GREEN%    ✅ [Server] 區塊%RESET%
        set /a config_sections+=1
    )
    findstr /C:"[PlayerAI]" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1 && (
        echo %GREEN%    ✅ [PlayerAI] 區塊%RESET%
        set /a config_sections+=1
    )
    
    REM 檢查SQL Server配置
    findstr /C:"Type=SQLServer" "%CURRENT_DIR%AI_CONFIG.INI" >nul 2>&1
    if %errorlevel% == 0 (
        echo %GREEN%    ✅ SQL Server 配置正確%RESET%
        echo [%date% %time%] ✅ SQL Server 配置正確 >> "%TEST_LOG%"
    ) else (
        echo %RED%    ❌ SQL Server 配置缺失%RESET%
        echo [%date% %time%] ❌ SQL Server 配置缺失 >> "%TEST_LOG%"
    )
    
    echo %CYAN%    配置完整性: %config_sections%/4 個核心區塊%RESET%
    echo [%date% %time%] 配置完整性: %config_sections%/4 個核心區塊 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ AI_CONFIG.INI 檔案不存在%RESET%
    echo [%date% %time%] ❌ AI_CONFIG.INI 檔案不存在 >> "%TEST_LOG%"
)

REM ========================================================================
REM 測試3: SQL Server 連線測試
REM ========================================================================
echo.
echo %BLUE%[3/6] 🗄️  測試 SQL Server 連線...%RESET%

REM 檢查 1433 端口
netstat -an | findstr ":1433" >nul 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ SQL Server 1433 端口已開啟%RESET%
    echo [%date% %time%] ✅ SQL Server 1433 端口已開啟 >> "%TEST_LOG%"
    
    REM 測試 sa 帳號連線
    echo SELECT 'Connection Test Success' as TestResult; > "%TEMP%\test_connection.sql"
    sqlcmd -S localhost -U sa -P 123456 -d master -i "%TEMP%\test_connection.sql" >"%TEMP%\sql_result.txt" 2>&1
    if %errorlevel% == 0 (
        findstr /C:"Connection Test Success" "%TEMP%\sql_result.txt" >nul 2>&1
        if %errorlevel% == 0 (
            echo %GREEN%    ✅ sa 帳號連線測試成功%RESET%
            echo [%date% %time%] ✅ sa 帳號連線測試成功 >> "%TEST_LOG%"
        ) else (
            echo %YELLOW%    ⚠️  sa 帳號連線異常%RESET%
            echo [%date% %time%] ⚠️ sa 帳號連線異常 >> "%TEST_LOG%"
        )
    ) else (
        echo %RED%    ❌ sa 帳號連線失敗%RESET%
        echo [%date% %time%] ❌ sa 帳號連線失敗 >> "%TEST_LOG%"
    )
    
    del /q "%TEMP%\test_connection.sql" "%TEMP%\sql_result.txt" 2>nul
) else (
    echo %RED%    ❌ SQL Server 1433 端口未開啟%RESET%
    echo [%date% %time%] ❌ SQL Server 1433 端口未開啟 >> "%TEST_LOG%"
)

REM ========================================================================
REM 測試4: WebSocket 端口測試
REM ========================================================================
echo.
echo %BLUE%[4/6] 🌐 測試 WebSocket 端口...%RESET%

REM 檢查端口 8080
netstat -an | findstr ":8080" >nul 2>&1
if %errorlevel% == 0 (
    echo %YELLOW%    ⚠️  端口 8080 已被佔用%RESET%
    echo [%date% %time%] ⚠️ 端口 8080 已被佔用 >> "%TEST_LOG%"
    
    REM 找出佔用進程
    for /f "tokens=5" %%a in ('netstat -ano ^| findstr ":8080"') do (
        for /f "tokens=1" %%b in ('tasklist ^| findstr "%%a"') do (
            echo %CYAN%    進程: %%b (PID: %%a)%RESET%
            echo [%date% %time%] 端口8080被進程%%b佔用 >> "%TEST_LOG%"
        )
    )
) else (
    echo %GREEN%    ✅ 端口 8080 可用%RESET%
    echo [%date% %time%] ✅ 端口 8080 可用 >> "%TEST_LOG%"
)

REM 檢查端口 8081 (備用)
netstat -an | findstr ":8081" >nul 2>&1
if %errorlevel% == 0 (
    echo %YELLOW%    ⚠️  備用端口 8081 已被佔用%RESET%
    echo [%date% %time%] ⚠️ 備用端口 8081 已被佔用 >> "%TEST_LOG%"
) else (
    echo %GREEN%    ✅ 備用端口 8081 可用%RESET%
    echo [%date% %time%] ✅ 備用端口 8081 可用 >> "%TEST_LOG%"
)

REM ========================================================================
REM 測試5: 系統資源檢測
REM ========================================================================
echo.
echo %BLUE%[5/6] 💻 測試系統資源檢測...%RESET%

REM 檢測記憶體
for /f "tokens=2 delims==" %%i in ('wmic computersystem get TotalPhysicalMemory /value 2^>nul') do set total_memory=%%i
if defined total_memory (
    set /a memory_gb=%total_memory:~0,-9%
    echo %GREEN%    ✅ 系統記憶體檢測: %memory_gb%GB%RESET%
    echo [%date% %time%] ✅ 系統記憶體: %memory_gb%GB >> "%TEST_LOG%"
    
    REM 根據記憶體建議玩家數
    if %memory_gb% geq 16 (
        echo %GREEN%    ✅ 建議最大玩家數: 100 (高效能模式)%RESET%
    ) else if %memory_gb% geq 8 (
        echo %GREEN%    ✅ 建議最大玩家數: 50 (標準模式)%RESET%
    ) else if %memory_gb% geq 4 (
        echo %YELLOW%    ⚠️  建議最大玩家數: 30 (平衡模式)%RESET%
    ) else (
        echo %RED%    ❌ 建議最大玩家數: 20 (節能模式)%RESET%
    )
) else (
    echo %RED%    ❌ 無法檢測系統記憶體%RESET%
    echo [%date% %time%] ❌ 無法檢測系統記憶體 >> "%TEST_LOG%"
)

REM 檢測CPU
for /f "tokens=2 delims==" %%i in ('wmic cpu get NumberOfCores /value 2^>nul') do set cpu_cores=%%i
if defined cpu_cores (
    echo %GREEN%    ✅ CPU 核心檢測: %cpu_cores% 核心%RESET%
    echo [%date% %time%] ✅ CPU 核心: %cpu_cores% 核心 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ 無法檢測CPU核心數%RESET%
    echo [%date% %time%] ❌ 無法檢測CPU核心數 >> "%TEST_LOG%"
)

REM ========================================================================
REM 測試6: AI 模型檢測
REM ========================================================================
echo.
echo %BLUE%[6/6] 🤖 測試 AI 模型檢測...%RESET%

REM 檢查 Ollama
where ollama >nul 2>&1
if %errorlevel% == 0 (
    echo %GREEN%    ✅ Ollama 已安裝%RESET%
    echo [%date% %time%] ✅ Ollama 已安裝 >> "%TEST_LOG%"
    
    REM 檢查可用模型
    ollama list 2>nul | findstr /V "NAME" > "%TEMP%\models.txt"
    if exist "%TEMP%\models.txt" (
        for /f %%i in ('type "%TEMP%\models.txt" ^| find /c /v ""') do set model_count=%%i
        if defined model_count (
            if %model_count% gtr 0 (
                echo %GREEN%    ✅ 發現 %model_count% 個可用模型%RESET%
                echo [%date% %time%] ✅ 發現%model_count%個Ollama模型 >> "%TEST_LOG%"
                set "AI_MODE=MULTI_MODEL"
            ) else (
                echo %YELLOW%    ⚠️  Ollama 已安裝但無可用模型%RESET%
                echo [%date% %time%] ⚠️ Ollama 無可用模型 >> "%TEST_LOG%"
                set "AI_MODE=BUILT_IN"
            )
        )
    )
    del /q "%TEMP%\models.txt" 2>nul
) else (
    echo %CYAN%    ℹ️  Ollama 未安裝，將使用內建AI%RESET%
    echo [%date% %time%] ℹ️ Ollama 未安裝 >> "%TEST_LOG%"
    set "AI_MODE=BUILT_IN"
)

REM 檢查主程式
if exist "%CURRENT_DIR%RANOnline_AI_Ultimate_Portable.exe" (
    echo %GREEN%    ✅ AI 主程式存在%RESET%
    echo [%date% %time%] ✅ AI 主程式存在 >> "%TEST_LOG%"
) else (
    echo %RED%    ❌ AI 主程式不存在%RESET%
    echo [%date% %time%] ❌ AI 主程式不存在 >> "%TEST_LOG%"
)

REM ========================================================================
REM 測試結果總結
REM ========================================================================
echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%                          測試結果總結%RESET%
echo %CYAN%========================================================================%RESET%
echo.

echo [%date% %time%] ========== 系統功能測試總結 ========== >> "%TEST_LOG%"

echo %YELLOW%📊 系統狀態：%RESET%
if %script_count% geq 3 (
    echo %GREEN%    ✅ 腳本完整性: %script_count%/4 個檔案 (良好)%RESET%
) else (
    echo %RED%    ❌ 腳本完整性: %script_count%/4 個檔案 (需要修復)%RESET%
)

if defined memory_gb (
    echo %GREEN%    ✅ 系統記憶體: %memory_gb%GB%RESET%
) else (
    echo %RED%    ❌ 記憶體檢測失敗%RESET%
)

if defined cpu_cores (
    echo %GREEN%    ✅ CPU 核心: %cpu_cores% 核心%RESET%
) else (
    echo %RED%    ❌ CPU 檢測失敗%RESET%
)

echo %GREEN%    ✅ AI 模型: %AI_MODE%%RESET%

echo.
echo %YELLOW%📝 建議操作：%RESET%
echo %WHITE%    [1] 執行完整系統檢查與修復%RESET%
echo %WHITE%    [2] 啟動智能啟動系統%RESET%
echo %WHITE%    [3] 檢查資料庫表結構%RESET%
echo %WHITE%    [4] 查看詳細測試日誌%RESET%
echo %WHITE%    [0] 退出%RESET%
echo.

:CHOICE_LOOP
set /p "choice=%YELLOW%請選擇操作 (0-4): %RESET%"

if "%choice%"=="1" (
    echo.
    echo %CYAN%🔧 啟動完整系統檢查與修復...%RESET%
    if exist "%CURRENT_DIR%一鍵系統檢查與修復.bat" (
        call "%CURRENT_DIR%一鍵系統檢查與修復.bat"
    ) else (
        echo %RED%❌ 找不到檢查工具%RESET%
    )
    goto :END_TEST
) else if "%choice%"=="2" (
    echo.
    echo %CYAN%🚀 啟動智能啟動系統...%RESET%
    if exist "%CURRENT_DIR%智能啟動_增強版_SQL支援.bat" (
        call "%CURRENT_DIR%智能啟動_增強版_SQL支援.bat"
    ) else (
        echo %RED%❌ 找不到啟動工具%RESET%
    )
    goto :END_TEST
) else if "%choice%"=="3" (
    echo.
    echo %CYAN%🗄️  啟動資料庫檢查...%RESET%
    if exist "%CURRENT_DIR%SQL_資料庫表結構檢查.bat" (
        call "%CURRENT_DIR%SQL_資料庫表結構檢查.bat"
    ) else (
        echo %RED%❌ 找不到資料庫檢查工具%RESET%
    )
    goto :END_TEST
) else if "%choice%"=="4" (
    echo.
    echo %CYAN%📄 測試日誌內容：%RESET%
    echo %CYAN%========================================================================%RESET%
    if exist "%TEST_LOG%" (
        type "%TEST_LOG%"
    ) else (
        echo %YELLOW%無測試日誌%RESET%
    )
    echo %CYAN%========================================================================%RESET%
    pause
    goto :CHOICE_LOOP
) else if "%choice%"=="0" (
    goto :END_TEST
) else (
    echo %RED%❌ 無效選項，請重新選擇%RESET%
    goto :CHOICE_LOOP
)

:END_TEST
echo.
echo [%date% %time%] ========== 系統功能測試結束 ========== >> "%TEST_LOG%"
echo %GREEN%✅ 系統功能測試完成！%RESET%
echo %WHITE%📄 詳細測試報告已儲存至: %TEST_LOG%%RESET%
echo.
echo %CYAN%感謝使用 Jy技術團隊 線上AI 系統功能測試工具！%RESET%
timeout /t 3 /nobreak >nul
exit /b 0
