@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】完整自動化系統打包工具 v4.0.0

REM 設置顏色變量
for /f %%A in ('echo prompt $E ^| cmd') do set "ESC=%%A"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "YELLOW=%ESC%[93m"
set "BLUE=%ESC%[94m"
set "CYAN=%ESC%[96m"
set "WHITE=%ESC%[97m"
set "RESET=%ESC%[0m"

REM 設置路徑
set "SOURCE_DIR=%~dp0"
set "TIMESTAMP=%date:~0,4%%date:~5,2%%date:~8,2%_%time:~0,2%%time:~3,2%%time:~6,2%"
set "TIMESTAMP=%TIMESTAMP: =0%"
set "PACKAGE_NAME=Jy技術團隊_線上AI_完整自動化系統_v4.0.0_%TIMESTAMP%"
set "DESKTOP=%USERPROFILE%\Desktop"
set "PACKAGE_DIR=%DESKTOP%\%PACKAGE_NAME%"
set "ZIP_FILE=%DESKTOP%\%PACKAGE_NAME%.zip"

echo.
echo %CYAN%========================================================================%RESET%
echo %WHITE%          【Jy技術團隊 線上AI】完整自動化系統打包工具%RESET%
echo %WHITE%                        v4.0.0 Ultimate Edition%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %YELLOW%📦 準備打包以下系統：%RESET%
echo %WHITE%   • 一鍵系統檢查與修復工具 (8步驟完整檢查)%RESET%
echo %WHITE%   • 增強版智能啟動系統 (SQL Server支援)%RESET%
echo %WHITE%   • 資料庫表結構檢查工具%RESET%
echo %WHITE%   • 系統功能快速測試工具%RESET%
echo %WHITE%   • RANOnline AI Ultimate Portable 主程式%RESET%
echo %WHITE%   • 完整的 Qt6 依賴庫%RESET%
echo %WHITE%   • 詳細說明文件和使用指南%RESET%
echo.

REM 檢查源目錄
if not exist "%SOURCE_DIR%RANOnline_AI_Ultimate_Portable.exe" (
    echo %RED%❌ 找不到主程式檔案！%RESET%
    echo %WHITE%請確認您在正確的目錄中執行此腳本%RESET%
    pause
    exit /b 1
)

echo %CYAN%📂 打包資訊：%RESET%
echo %WHITE%   源目錄: %SOURCE_DIR%%RESET%
echo %WHITE%   目標目錄: %PACKAGE_DIR%%RESET%
echo %WHITE%   ZIP檔案: %ZIP_FILE%%RESET%
echo.

REM 確認開始打包
set /p "confirm=%YELLOW%是否開始打包？(Y/N): %RESET%"
if /i not "%confirm%"=="Y" (
    echo %YELLOW%已取消打包作業%RESET%
    pause
    exit /b 0
)

echo.
echo %BLUE%🚀 開始打包作業...%RESET%
echo.

REM ========================================================================
REM 步驟 1: 創建打包目錄
REM ========================================================================
echo %CYAN%[1/7] 📁 創建打包目錄...%RESET%
if exist "%PACKAGE_DIR%" (
    rmdir /s /q "%PACKAGE_DIR%"
)
mkdir "%PACKAGE_DIR%" 2>nul
if exist "%PACKAGE_DIR%" (
    echo %GREEN%    ✅ 打包目錄創建成功%RESET%
) else (
    echo %RED%    ❌ 無法創建打包目錄%RESET%
    pause
    exit /b 1
)

REM ========================================================================
REM 步驟 2: 複製主程式和核心檔案
REM ========================================================================
echo.
echo %CYAN%[2/7] 📋 複製主程式和核心檔案...%RESET%

REM 主程式
copy "%SOURCE_DIR%RANOnline_AI_Ultimate_Portable.exe" "%PACKAGE_DIR%\" >nul 2>&1
if exist "%PACKAGE_DIR%\RANOnline_AI_Ultimate_Portable.exe" (
    echo %GREEN%    ✅ RANOnline_AI_Ultimate_Portable.exe%RESET%
) else (
    echo %RED%    ❌ RANOnline_AI_Ultimate_Portable.exe%RESET%
)

REM 配置檔案
copy "%SOURCE_DIR%AI_CONFIG.INI" "%PACKAGE_DIR%\" >nul 2>&1
if exist "%PACKAGE_DIR%\AI_CONFIG.INI" (
    echo %GREEN%    ✅ AI_CONFIG.INI%RESET%
) else (
    echo %RED%    ❌ AI_CONFIG.INI%RESET%
)

REM ========================================================================
REM 步驟 3: 複製自動化系統腳本
REM ========================================================================
echo.
echo %CYAN%[3/7] 🔧 複製自動化系統腳本...%RESET%

set "scripts=一鍵系統檢查與修復.bat 智能啟動_增強版_SQL支援.bat SQL_資料庫表結構檢查.bat 系統功能快速測試.bat 系統完整性檢查.bat 智能啟動_增強版.bat 一鍵啟動_Ultimate_Portable.bat 一鍵關閉.bat WebSocket驗證.bat VM_環境驗證測試.bat"

for %%s in (%scripts%) do (
    if exist "%SOURCE_DIR%%%s" (
        copy "%SOURCE_DIR%%%s" "%PACKAGE_DIR%\" >nul 2>&1
        if exist "%PACKAGE_DIR%\%%s" (
            echo %GREEN%    ✅ %%s%RESET%
        ) else (
            echo %RED%    ❌ %%s%RESET%
        )
    ) else (
        echo %YELLOW%    ⚠️  %%s (檔案不存在)%RESET%
    )
)

REM ========================================================================
REM 步驟 4: 複製 Qt6 依賴庫
REM ========================================================================
echo.
echo %CYAN%[4/7] 📚 複製 Qt6 依賴庫...%RESET%

set "qt_libs=Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll Qt6Svg.dll"
for %%q in (%qt_libs%) do (
    if exist "%SOURCE_DIR%%%q" (
        copy "%SOURCE_DIR%%%q" "%PACKAGE_DIR%\" >nul 2>&1
        if exist "%PACKAGE_DIR%\%%q" (
            echo %GREEN%    ✅ %%q%RESET%
        ) else (
            echo %RED%    ❌ %%q%RESET%
        )
    ) else (
        echo %YELLOW%    ⚠️  %%q (檔案不存在)%RESET%
    )
)

REM 其他重要依賴
set "other_libs=D3Dcompiler_47.dll opengl32sw.dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll"
for %%o in (%other_libs%) do (
    if exist "%SOURCE_DIR%%%o" (
        copy "%SOURCE_DIR%%%o" "%PACKAGE_DIR%\" >nul 2>&1
        if exist "%PACKAGE_DIR%\%%o" (
            echo %GREEN%    ✅ %%o%RESET%
        ) else (
            echo %RED%    ❌ %%o%RESET%
        )
    )
)

REM ========================================================================
REM 步驟 5: 複製目錄結構
REM ========================================================================
echo.
echo %CYAN%[5/7] 📂 複製目錄結構...%RESET%

REM 創建 logs 目錄
mkdir "%PACKAGE_DIR%\logs" 2>nul
echo %GREEN%    ✅ logs\ 目錄已創建%RESET%

REM 複製 platforms 目錄
if exist "%SOURCE_DIR%platforms" (
    xcopy "%SOURCE_DIR%platforms" "%PACKAGE_DIR%\platforms\" /E /I /Q >nul 2>&1
    if exist "%PACKAGE_DIR%\platforms" (
        echo %GREEN%    ✅ platforms\ 目錄%RESET%
    ) else (
        echo %RED%    ❌ platforms\ 目錄%RESET%
    )
)

REM 複製其他重要目錄
set "directories=generic iconengines imageformats styles tls translations networkinformation"
for %%d in (%directories%) do (
    if exist "%SOURCE_DIR%%%d" (
        xcopy "%SOURCE_DIR%%%d" "%PACKAGE_DIR%\%%d\" /E /I /Q >nul 2>&1
        if exist "%PACKAGE_DIR%\%%d" (
            echo %GREEN%    ✅ %%d\ 目錄%RESET%
        ) else (
            echo %YELLOW%    ⚠️  %%d\ 目錄複製失敗%RESET%
        )
    )
)

REM ========================================================================
REM 步驟 6: 複製說明文件
REM ========================================================================
echo.
echo %CYAN%[6/7] 📖 複製說明文件...%RESET%

set "docs=README_完整自動化系統.md README_繁體中文.md 自動修復標準化完成報告.md"
for %%d in (%docs%) do (
    if exist "%SOURCE_DIR%%%d" (
        copy "%SOURCE_DIR%%%d" "%PACKAGE_DIR%\" >nul 2>&1
        if exist "%PACKAGE_DIR%\%%d" (
            echo %GREEN%    ✅ %%d%RESET%
        ) else (
            echo %RED%    ❌ %%d%RESET%
        )
    )
)

REM ========================================================================
REM 步驟 7: 創建快速啟動腳本和打包說明
REM ========================================================================
echo.
echo %CYAN%[7/7] 📝 創建快速啟動腳本和打包說明...%RESET%

REM 創建快速啟動腳本
(
echo @echo off
echo chcp 65001 ^>nul
echo title 【Jy技術團隊 線上AI】快速啟動選單 v4.0.0
echo.
echo echo ========================================================================
echo echo             【Jy技術團隊 線上AI】快速啟動選單
echo echo                        v4.0.0 Ultimate Edition
echo echo ========================================================================
echo echo.
echo echo    🚀 請選擇要執行的功能：
echo echo.
echo echo       [1] 智能啟動系統 ^(推薦^)
echo echo       [2] 完整系統檢查與修復
echo echo       [3] 資料庫表結構檢查
echo echo       [4] 系統功能快速測試
echo echo       [5] 傳統啟動方式
echo echo       [6] 系統說明文件
echo echo       [0] 退出
echo echo.
echo set /p "choice=請輸入選項 (0-6): "
echo.
echo if "%%choice%%"=="1" call "智能啟動_增強版_SQL支援.bat"
echo if "%%choice%%"=="2" call "一鍵系統檢查與修復.bat"
echo if "%%choice%%"=="3" call "SQL_資料庫表結構檢查.bat"
echo if "%%choice%%"=="4" call "系統功能快速測試.bat"
echo if "%%choice%%"=="5" call "一鍵啟動_Ultimate_Portable.bat"
echo if "%%choice%%"=="6" start notepad "README_完整自動化系統.md"
echo if "%%choice%%"=="0" exit /b 0
echo.
echo pause
) > "%PACKAGE_DIR%\🚀_快速啟動選單.bat"

if exist "%PACKAGE_DIR%\🚀_快速啟動選單.bat" (
    echo %GREEN%    ✅ 🚀_快速啟動選單.bat%RESET%
) else (
    echo %RED%    ❌ 快速啟動選單創建失敗%RESET%
)

REM 創建打包說明
(
echo # Jy技術團隊 線上AI - 完整自動化系統
echo.
echo ## 📦 打包信息
echo - **打包時間**: %date% %time%
echo - **版本**: v4.0.0 Ultimate Edition
echo - **打包者**: %USERNAME%
echo - **系統**: %OS%
echo.
echo ## 🚀 快速開始
echo 1. 解壓縮此打包檔案到任意目錄
echo 2. 雙擊執行 `🚀_快速啟動選單.bat`
echo 3. 選擇 `[1] 智能啟動系統` ^(推薦^)
echo 4. 系統將自動進行完整檢查和啟動
echo.
echo ## 📋 包含功能
echo - ✅ SQL Server 1433 端口檢查
echo - ✅ ODBC 連線與 sa 帳號驗證
echo - ✅ RanUser 資料庫完整性檢查
echo - ✅ AI_CONFIG.INI 配置檔案驗證
echo - ✅ WebSocket 服務配置
echo - ✅ 多模型檢測與自動切換
echo - ✅ 智能玩家數量調整
echo - ✅ VM 環境自動優化
echo - ✅ 一鍵修復功能
echo.
echo ## 📞 技術支援
echo - **開發團隊**: Jy技術團隊
echo - **郵件**: jytech@example.com
echo - **版本**: v4.0.0 Ultimate Edition
echo.
echo ## 🔧 系統要求
echo - Windows 10/11 ^(x64^)
echo - SQL Server ^(建議安裝^)
echo - 最少 4GB RAM ^(建議 8GB+^)
echo - 網路端口 1433, 8080 可用
echo.
echo 詳細說明請參閱 `README_完整自動化系統.md`
) > "%PACKAGE_DIR%\📖_打包說明.md"

if exist "%PACKAGE_DIR%\📖_打包說明.md" (
    echo %GREEN%    ✅ 📖_打包說明.md%RESET%
) else (
    echo %RED%    ❌ 打包說明創建失敗%RESET%
)

REM ========================================================================
REM 完成打包並創建 ZIP 檔案
REM ========================================================================
echo.
echo %BLUE%📦 正在創建 ZIP 壓縮檔案...%RESET%

REM 使用 PowerShell 創建 ZIP 檔案
powershell -Command "Compress-Archive -Path '%PACKAGE_DIR%\*' -DestinationPath '%ZIP_FILE%' -Force" 2>nul

if exist "%ZIP_FILE%" (
    echo %GREEN%✅ ZIP 檔案創建成功！%RESET%
    
    REM 獲取檔案大小
    for %%A in ("%ZIP_FILE%") do set "file_size=%%~zA"
    set /a file_size_mb=%file_size% / 1048576
    
    echo %CYAN%📊 打包完成統計：%RESET%
    echo %WHITE%   ZIP檔案: %ZIP_FILE%%RESET%
    echo %WHITE%   檔案大小: %file_size_mb% MB%RESET%
    
    REM 統計檔案數量
    for /f %%i in ('dir "%PACKAGE_DIR%" /s /b ^| find /c /v ""') do set "total_files=%%i"
    echo %WHITE%   包含檔案: %total_files% 個%RESET%
    
) else (
    echo %RED%❌ ZIP 檔案創建失敗%RESET%
    echo %YELLOW%請檢查是否有足夠的磁碟空間和權限%RESET%
)

REM ========================================================================
REM 打包完成
REM ========================================================================
echo.
echo %CYAN%========================================================================%RESET%
echo %GREEN%                      🎉 打包完成！%RESET%
echo %CYAN%========================================================================%RESET%
echo.
echo %YELLOW%📦 打包結果：%RESET%
if exist "%ZIP_FILE%" (
    echo %GREEN%   ✅ ZIP檔案: %ZIP_FILE%%RESET%
    echo %GREEN%   ✅ 解壓目錄: %PACKAGE_DIR%%RESET%
) else (
    echo %RED%   ❌ ZIP檔案創建失敗%RESET%
    echo %GREEN%   ✅ 檔案目錄: %PACKAGE_DIR%%RESET%
)
echo.
echo %YELLOW%🚀 使用方法：%RESET%
echo %WHITE%   1. 將 ZIP 檔案分享給使用者%RESET%
echo %WHITE%   2. 解壓縮到任意目錄%RESET%
echo %WHITE%   3. 執行 🚀_快速啟動選單.bat%RESET%
echo %WHITE%   4. 選擇智能啟動系統%RESET%
echo.
echo %YELLOW%📖 說明文件：%RESET%
echo %WHITE%   • 📖_打包說明.md - 快速開始指南%RESET%
echo %WHITE%   • README_完整自動化系統.md - 完整功能說明%RESET%
echo.

REM 詢問是否開啟目標目錄
set /p "open_dir=%YELLOW%是否開啟打包目錄？(Y/N): %RESET%"
if /i "%open_dir%"=="Y" (
    explorer "%DESKTOP%"
)

REM 詢問是否刪除解壓目錄
set /p "cleanup=%YELLOW%是否清理臨時解壓目錄？(Y/N): %RESET%"
if /i "%cleanup%"=="Y" (
    if exist "%PACKAGE_DIR%" (
        rmdir /s /q "%PACKAGE_DIR%"
        echo %GREEN%✅ 臨時目錄已清理%RESET%
    )
)

echo.
echo %GREEN%感謝使用 Jy技術團隊 線上AI 打包工具！%RESET%
echo %WHITE%如有問題請聯繫技術支援: jytech@example.com%RESET%
timeout /t 5 /nobreak >nul
exit /b 0
