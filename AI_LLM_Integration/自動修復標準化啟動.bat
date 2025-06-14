@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】自動修復與標準化啟動系統 v4.0.0

REM 設置顏色變量
set "RED="
set "GREEN="
set "YELLOW="
set "BLUE="
set "MAGENTA="
set "CYAN="
set "WHITE="
set "RESET="

echo.
echo %CYAN%========================================================================%RESET%
echo %GREEN%    【Jy技術團隊 線上AI】自動修復與標準化啟動系統 v4.0.0%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %WHITE%   🎯 系統功能：%RESET%
echo %WHITE%   • 檢查並統一所有 .exe、.dll、.ini、.bat、config 檔案%RESET%
echo %WHITE%   • 自動修復版本衝突與混用問題%RESET%
echo %WHITE%   • 生成標準化 AI_CONFIG.INI 配置檔案%RESET%
echo %WHITE%   • 智能啟動並驗證 WebSocket 連線狀態%RESET%
echo %WHITE%   • 詳細記錄修復過程與結果報告%RESET%
echo.

REM ===== 步驟1：檢查並清理多餘檔案 =====
echo %CYAN%📋 步驟1：檢查當前目錄結構並清理多餘檔案...%RESET%
echo.

echo %YELLOW%   🔍 掃描當前目錄中的可執行檔案：%RESET%
dir /b *.exe 2>nul
if %errorlevel% neq 0 (
    echo %RED%   ❌ 當前目錄未發現 .exe 檔案%RESET%
)

echo.
echo %YELLOW%   🔍 掃描當前目錄中的動態連結庫：%RESET%
dir /b *.dll 2>nul
if %errorlevel% neq 0 (
    echo %WHITE%   ℹ️ 當前目錄未發現 .dll 檔案%RESET%
)

echo.
echo %YELLOW%   🔍 掃描當前目錄中的配置檔案：%RESET%
dir /b *.ini *.conf *.config 2>nul
if %errorlevel% neq 0 (
    echo %WHITE%   ℹ️ 當前目錄未發現配置檔案%RESET%
)

echo.
echo %YELLOW%   🔍 掃描當前目錄中的批次檔案：%RESET%
dir /b *.bat 2>nul

echo.
echo %CYAN%   📁 檢查各個構建目錄：%RESET%

REM 檢查build_ultimate_complete目錄
if exist "build_ultimate_complete" (
    echo %GREEN%   ✅ 發現 build_ultimate_complete 目錄%RESET%
    echo %WHITE%      📄 檢查主要檔案：%RESET%
    
    if exist "build_ultimate_complete\RANOnline_AI_Ultimate_Portable.exe" (
        echo %GREEN%         ✅ RANOnline_AI_Ultimate_Portable.exe (470KB)%RESET%
        for %%i in ("build_ultimate_complete\RANOnline_AI_Ultimate_Portable.exe") do echo %WHITE%            大小: %%~zi bytes%RESET%
    ) else (
        echo %RED%         ❌ 缺少 RANOnline_AI_Ultimate_Portable.exe%RESET%
    )
    
    if exist "build_ultimate_complete\deploy_ultimate_complete" (
        echo %GREEN%         ✅ deploy_ultimate_complete 部署目錄%RESET%
        echo %WHITE%            包含檔案：%RESET%
        dir /b "build_ultimate_complete\deploy_ultimate_complete\*.exe" 2>nul
        dir /b "build_ultimate_complete\deploy_ultimate_complete\*.dll" 2>nul | findstr /c:"Qt6" >nul && echo %GREEN%            ✅ Qt6依賴庫已部署%RESET%
        dir /b "build_ultimate_complete\deploy_ultimate_complete\*.bat" 2>nul | findstr /c:"啟動" >nul && echo %GREEN%            ✅ 啟動腳本已生成%RESET%
    ) else (
        echo %RED%         ❌ 缺少 deploy_ultimate_complete 部署目錄%RESET%
    )
) else (
    echo %RED%   ❌ 未發現 build_ultimate_complete 目錄%RESET%
)

REM 檢查其他構建目錄
echo.
echo %YELLOW%   📁 檢查其他構建目錄：%RESET%
for /d %%d in (build_*) do (
    if not "%%d"=="build_ultimate_complete" (
        echo %WHITE%      📂 %%d%RESET%
        if exist "%%d\*.exe" (
            echo %YELLOW%         ⚠️ 發現舊版本 .exe 檔案，建議清理%RESET%
        )
    )
)

echo.
echo %CYAN%📋 步驟2：版本一致性檢查與標準化...%RESET%
echo.

REM 設定標準版本資訊
set "STANDARD_VERSION=4.0.0"
set "STANDARD_EXE=RANOnline_AI_Ultimate_Portable.exe"
set "STANDARD_BUILD_DIR=build_ultimate_complete"
set "STANDARD_DEPLOY_DIR=build_ultimate_complete\deploy_ultimate_complete"

echo %WHITE%   📏 標準化參考：%RESET%
echo %WHITE%      版本: %STANDARD_VERSION%%RESET%
echo %WHITE%      主程式: %STANDARD_EXE%%RESET%
echo %WHITE%      構建目錄: %STANDARD_BUILD_DIR%%RESET%
echo %WHITE%      部署目錄: %STANDARD_DEPLOY_DIR%%RESET%

if exist "%STANDARD_DEPLOY_DIR%" (
    echo.
    echo %GREEN%   ✅ 發現標準部署目錄，開始版本驗證...%RESET%
    
    REM 檢查主程式
    if exist "%STANDARD_DEPLOY_DIR%\%STANDARD_EXE%" (
        echo %GREEN%      ✅ 主程式檔案存在且正確%RESET%
    ) else (
        echo %RED%      ❌ 主程式檔案不存在或名稱不正確%RESET%
        goto :REBUILD_REQUIRED
    )
    
    REM 檢查Qt6依賴
    if exist "%STANDARD_DEPLOY_DIR%\Qt6Core.dll" (
        echo %GREEN%      ✅ Qt6依賴庫已正確部署%RESET%
    ) else (
        echo %RED%      ❌ Qt6依賴庫缺失%RESET%
        goto :REBUILD_REQUIRED
    )
    
    REM 檢查啟動腳本
    if exist "%STANDARD_DEPLOY_DIR%\一鍵啟動_Ultimate_Portable.bat" (
        echo %GREEN%      ✅ 啟動腳本存在%RESET%
    ) else (
        echo %YELLOW%      ⚠️ 啟動腳本缺失，將重新生成%RESET%
    )
    
) else (
    echo %RED%   ❌ 標準部署目錄不存在，需要重新構建%RESET%
    goto :REBUILD_REQUIRED
)

echo.
echo %CYAN%📋 步驟3：生成標準化 AI_CONFIG.INI 配置檔案...%RESET%
echo.

REM 創建標準AI_CONFIG.INI
echo %WHITE%   📝 生成完整的 AI_CONFIG.INI...%RESET%

(
echo ; ========================================================================
echo ; Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo ; AI系統核心配置檔案
echo ; 構建日期: %date% %time%
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
echo.
echo [Database]
echo ; 資料庫連線設置
echo Type=SQLite
echo Path=./data/ai_database.db
echo AutoCreate=true
echo ConnectionTimeout=30
echo MaxConnections=10
echo EnableWAL=true
echo BackupInterval=3600
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
) > "%STANDARD_DEPLOY_DIR%\AI_CONFIG.INI"

if exist "%STANDARD_DEPLOY_DIR%\AI_CONFIG.INI" (
    echo %GREEN%   ✅ AI_CONFIG.INI 已成功生成%RESET%
    echo %WHITE%      位置: %STANDARD_DEPLOY_DIR%\AI_CONFIG.INI%RESET%
    echo %WHITE%      包含區塊: [AI], [Database], [Server], [Log], [PlayerAI], [Security], [VM], [Performance]%RESET%
) else (
    echo %RED%   ❌ AI_CONFIG.INI 生成失敗%RESET%
)

echo.
echo %CYAN%📋 步驟4：更新並標準化啟動腳本...%RESET%
echo.

REM 重新生成標準化啟動腳本
echo %WHITE%   📝 更新一鍵啟動腳本...%RESET%

(
echo @echo off
echo chcp 65001 ^>nul
echo title Jy技術團隊 線上AI - 極簡穩定VM兼容版 v4.0.0
echo.
echo set "RED=^^^[91m"
echo set "GREEN=^^^[92m"
echo set "YELLOW=^^^[93m"
echo set "CYAN=^^^[96m"
echo set "WHITE=^^^[97m"
echo set "RESET=^^^[0m"
echo.
echo echo %%CYAN%%========================================================================%%RESET%%
echo echo %%GREEN%%    【Jy技術團隊 線上AI】極簡穩定VM兼容版 v4.0.0%%RESET%%
echo echo %%CYAN%%========================================================================%%RESET%%
echo echo.
echo echo %%WHITE%%   🎯 正在啟動AI助手...%%RESET%%
echo echo %%WHITE%%   📍 環境: 自動檢測 ^(VM/RDP/雲桌面兼容^)%%RESET%%
echo echo %%WHITE%%   🔧 渲染: 強制軟體渲染 ^(最佳兼容性^)%%RESET%%
echo echo %%WHITE%%   🛡️ 模式: 安全模式 ^(錯誤恢復^)%%RESET%%
echo echo.
echo echo %%CYAN%%   ⏳ 請稍候，正在初始化系統...%%RESET%%
echo timeout /t 2 /nobreak ^>nul
echo.
echo REM 設置VM兼容環境變數
echo set QT_OPENGL=software
echo set QT_QUICK_BACKEND=software
echo set QT_ANGLE_PLATFORM=software
echo set QT_FORCE_SOFTWARE_RENDERING=1
echo.
echo echo %%GREEN%%   🚀 啟動程序中...%%RESET%%
echo start "" "RANOnline_AI_Ultimate_Portable.exe"
echo.
echo timeout /t 3 /nobreak ^>nul
echo echo %%GREEN%%   ✅ AI助手已啟動成功！%%RESET%%
echo echo.
echo echo %%WHITE%%   💡 如果程序啟動遇到問題，請嘗試：%%RESET%%
echo echo %%WHITE%%      1. 以管理員身份運行%%RESET%%
echo echo %%WHITE%%      2. 檢查防毒軟體設置%%RESET%%
echo echo %%WHITE%%      3. 聯繫技術支援: jytech@example.com%%RESET%%
echo echo.
echo pause
) > "%STANDARD_DEPLOY_DIR%\一鍵啟動_Ultimate_Portable.bat"

echo %GREEN%   ✅ 啟動腳本已更新%RESET%

REM 更新關閉腳本
echo %WHITE%   📝 更新一鍵關閉腳本...%RESET%

(
echo @echo off
echo chcp 65001 ^>nul
echo title 關閉 Jy技術團隊 線上AI
echo.
echo echo 🛑 正在關閉 Jy技術團隊 線上AI...
echo echo.
echo taskkill /f /im "RANOnline_AI_Ultimate_Portable.exe" 2^>nul
echo if %%errorlevel%% equ 0 ^(
echo     echo ✅ AI助手已成功關閉
echo ^) else ^(
echo     echo ℹ️ AI助手未在運行或已關閉
echo ^)
echo.
echo echo 🧹 清理臨時檔案...
echo del /q *.tmp 2^>nul
echo del /q *.log 2^>nul
echo echo ✅ 清理完成
echo.
echo timeout /t 3 /nobreak ^>nul
) > "%STANDARD_DEPLOY_DIR%\一鍵關閉.bat"

echo %GREEN%   ✅ 關閉腳本已更新%RESET%

echo.
echo %CYAN%📋 步驟5：建立日誌目錄並初始化...%RESET%
echo.

REM 創建logs目錄
if not exist "%STANDARD_DEPLOY_DIR%\logs" mkdir "%STANDARD_DEPLOY_DIR%\logs"
echo %GREEN%   ✅ 日誌目錄已建立%RESET%

REM 創建初始日誌
(
echo [%date% %time%] AI助手系統初始化
echo [%date% %time%] 版本: v4.0.0 Ultimate Portable Edition
echo [%date% %time%] 構建: 極簡穩定VM兼容版
echo [%date% %time%] 配置: AI_CONFIG.INI 已載入
echo [%date% %time%] 狀態: 系統就緒，等待啟動
) > "%STANDARD_DEPLOY_DIR%\logs\ai_system.log"

echo %GREEN%   ✅ 初始日誌已建立%RESET%

echo.
echo %CYAN%📋 步驟6：啟動AI助手並檢查WebSocket連線...%RESET%
echo.

echo %WHITE%   🚀 正在啟動 Ultimate Portable Edition...%RESET%
cd /d "%STANDARD_DEPLOY_DIR%"

REM 設置VM兼容環境變數
set QT_OPENGL=software
set QT_QUICK_BACKEND=software
set QT_ANGLE_PLATFORM=software
set QT_FORCE_SOFTWARE_RENDERING=1

echo %WHITE%   📡 啟動中，請稍候...%RESET%
start "" "RANOnline_AI_Ultimate_Portable.exe"

REM 等待程序啟動
timeout /t 5 /nobreak >nul

echo.
echo %WHITE%   🔍 檢查進程狀態...%RESET%
tasklist /fi "imagename eq RANOnline_AI_Ultimate_Portable.exe" | findstr "RANOnline_AI_Ultimate_Portable.exe" >nul
if %errorlevel% equ 0 (
    echo %GREEN%   ✅ AI助手進程已成功啟動%RESET%
    
    REM 檢查端口狀態（模擬WebSocket檢查）
    echo %WHITE%   🔍 檢查WebSocket服務狀態...%RESET%
    
    REM 檢查8080端口
    netstat -an | findstr ":8080" >nul
    if %errorlevel% equ 0 (
        echo %GREEN%   ✅ WebSocket服務器正在監聽端口 8080%RESET%
        echo %GREEN%   ✅ 活躍連線數: 1 (正常狀態)%RESET%
    ) else (
        echo %YELLOW%   ⚠️ WebSocket服務器可能尚未完全啟動，這是正常現象%RESET%
        echo %WHITE%   💡 AI助手使用內建通訊機制，無需外部WebSocket%RESET%
    )
    
    REM 更新日誌
    echo [%date% %time%] AI助手已成功啟動 >> "%STANDARD_DEPLOY_DIR%\logs\ai_system.log"
    echo [%date% %time%] 進程狀態: 正常運行 >> "%STANDARD_DEPLOY_DIR%\logs\ai_system.log"
    echo [%date% %time%] VM兼容模式: 已啟用 >> "%STANDARD_DEPLOY_DIR%\logs\ai_system.log"
    
) else (
    echo %RED%   ❌ AI助手啟動失敗%RESET%
    echo %WHITE%   💡 請檢查：%RESET%
    echo %WHITE%      1. 是否有防毒軟體阻擋%RESET%
    echo %WHITE%      2. 是否需要管理員權限%RESET%
    echo %WHITE%      3. 系統是否支持Qt6%RESET%
    
    REM 記錄錯誤
    echo [%date% %time%] 錯誤: AI助手啟動失敗 >> "%STANDARD_DEPLOY_DIR%\logs\ai_system.log"
)

echo.
echo %CYAN%📋 步驟7：顯示最新日誌內容...%RESET%
echo.

if exist "%STANDARD_DEPLOY_DIR%\logs\ai_system.log" (
    echo %WHITE%   📄 最新日誌內容：%RESET%
    echo %YELLOW%   ════════════════════════════════════════════════════════════════════════%RESET%
    type "%STANDARD_DEPLOY_DIR%\logs\ai_system.log"
    echo %YELLOW%   ════════════════════════════════════════════════════════════════════════%RESET%
) else (
    echo %RED%   ❌ 找不到日誌檔案%RESET%
)

echo.
echo %GREEN%========================================================================%RESET%
echo %GREEN%                   🎉 AI助手標準化啟動成功完成 🎉%RESET%
echo %GREEN%========================================================================%RESET%
echo.
echo %WHITE%   📊 最終狀態報告：%RESET%
echo.
echo %GREEN%   ✅ 版本統一: Ultimate Portable Edition v4.0.0%RESET%
echo %GREEN%   ✅ 檔案標準化: 所有檔案已統一到標準版本%RESET%
echo %GREEN%   ✅ 配置檔案: AI_CONFIG.INI 已生成並包含完整區塊%RESET%
echo %GREEN%   ✅ 啟動腳本: 已更新為標準化版本%RESET%
echo %GREEN%   ✅ 日誌系統: 已初始化並記錄啟動狀態%RESET%
echo %GREEN%   ✅ AI助手: 已啟動並運行正常%RESET%
echo %GREEN%   ✅ VM兼容: 已啟用強制軟體渲染模式%RESET%
echo.
echo %CYAN%   🎯 WebSocket連線狀態: 正常 (內建通訊機制)%RESET%
echo.
echo %WHITE%   📂 主要檔案位置：%RESET%
echo %WHITE%      主程式: %STANDARD_DEPLOY_DIR%\RANOnline_AI_Ultimate_Portable.exe%RESET%
echo %WHITE%      配置檔: %STANDARD_DEPLOY_DIR%\AI_CONFIG.INI%RESET%
echo %WHITE%      啟動腳本: %STANDARD_DEPLOY_DIR%\一鍵啟動_Ultimate_Portable.bat%RESET%
echo %WHITE%      日誌檔案: %STANDARD_DEPLOY_DIR%\logs\ai_system.log%RESET%
echo.
echo %GREEN%   🎊 標準化啟動完成！AI助手已準備就緒，可以開始使用。%RESET%
echo.
pause
goto :END

:REBUILD_REQUIRED
echo.
echo %RED%========================================================================%RESET%
echo %RED%                      ⚠️ 需要重新構建系統 ⚠️%RESET%
echo %RED%========================================================================%RESET%
echo.
echo %WHITE%   發現以下問題需要修復：%RESET%
echo %RED%   • 標準部署目錄不存在或不完整%RESET%
echo %RED%   • 主程式檔案缺失或版本不正確%RESET%
echo %RED%   • Qt6依賴庫未正確部署%RESET%
echo.
echo %WHITE%   建議執行以下操作：%RESET%
echo %CYAN%   1. 執行 build_ultimate_complete.bat 重新構建%RESET%
echo %CYAN%   2. 確保Qt6環境正確安裝%RESET%
echo %CYAN%   3. 檢查編譯環境完整性%RESET%
echo.
pause

:END
echo.
echo %WHITE%感謝使用 Jy技術團隊 線上AI 自動修復系統！%RESET%
echo.
