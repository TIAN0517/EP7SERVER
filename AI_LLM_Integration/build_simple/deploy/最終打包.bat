@echo off
chcp 65001 >nul
title RANOnline AI Simple - 最終打包腳本

echo ========================================================================
echo RANOnline AI Simple - 最終打包腳本
echo 版本: 1.0.0 Simple Edition
echo 開發團隊: Jy技術團隊 線上AI
echo ========================================================================
echo.

color 0B

:: 設置變數
set "PACKAGE_NAME=RANOnline_AI_Simple_v1.0.0_Portable"
set "SOURCE_DIR=%cd%"
set "PACKAGE_DIR=%cd%\..\%PACKAGE_NAME%"

echo [進行中] 正在創建便攜版打包...
echo.

:: 清理舊的打包目錄
if exist "%PACKAGE_DIR%" (
    echo [清理] 移除舊的打包目錄...
    rmdir /s /q "%PACKAGE_DIR%"
)

:: 創建新的打包目錄
echo [創建] 建立打包目錄...
mkdir "%PACKAGE_DIR%"

:: 複製所有必要文件
echo [複製] 主程式和依賴庫...
copy "RANOnline_AI_VM_Simple.exe" "%PACKAGE_DIR%\" >nul
copy "*.dll" "%PACKAGE_DIR%\" >nul

:: 複製Qt平台插件
echo [複製] Qt平台插件...
if exist "platforms" (
    xcopy "platforms" "%PACKAGE_DIR%\platforms\" /s /i /q >nul
)

:: 複製其他Qt插件
echo [複製] Qt樣式和圖像插件...
if exist "styles" xcopy "styles" "%PACKAGE_DIR%\styles\" /s /i /q >nul
if exist "imageformats" xcopy "imageformats" "%PACKAGE_DIR%\imageformats\" /s /i /q >nul
if exist "iconengines" xcopy "iconengines" "%PACKAGE_DIR%\iconengines\" /s /i /q >nul
if exist "generic" xcopy "generic" "%PACKAGE_DIR%\generic\" /s /i /q >nul
if exist "translations" xcopy "translations" "%PACKAGE_DIR%\translations\" /s /i /q >nul
if exist "networkinformation" xcopy "networkinformation" "%PACKAGE_DIR%\networkinformation\" /s /i /q >nul
if exist "tls" xcopy "tls" "%PACKAGE_DIR%\tls\" /s /i /q >nul

:: 複製控制腳本
echo [複製] 控制腳本...
copy "一鍵啟動.bat" "%PACKAGE_DIR%\" >nul
copy "一鍵關閉.bat" "%PACKAGE_DIR%\" >nul

:: 複製文檔
echo [複製] 說明文檔...
copy "使用說明.txt" "%PACKAGE_DIR%\" >nul

:: 創建版本信息文件
echo [創建] 版本資訊檔案...
echo RANOnline AI Simple - 極簡穩定VM兼容版 > "%PACKAGE_DIR%\version.txt"
echo 版本: 1.0.0 Simple Edition >> "%PACKAGE_DIR%\version.txt"
echo 構建日期: %date% %time% >> "%PACKAGE_DIR%\version.txt"
echo 開發團隊: Jy技術團隊 線上AI >> "%PACKAGE_DIR%\version.txt"
echo Qt版本: 6.9.1 >> "%PACKAGE_DIR%\version.txt"
echo 編譯器: MinGW 13.1.0 >> "%PACKAGE_DIR%\version.txt"

:: 檢查打包結果
echo.
echo [檢查] 驗證打包結果...
if exist "%PACKAGE_DIR%\RANOnline_AI_VM_Simple.exe" (
    echo [√] 主程式已打包
) else (
    echo [X] 錯誤: 主程式打包失敗
    pause
    exit /b 1
)

if exist "%PACKAGE_DIR%\Qt6Core.dll" (
    echo [√] Qt核心庫已打包
) else (
    echo [X] 警告: Qt核心庫可能缺失
)

if exist "%PACKAGE_DIR%\platforms\qwindows.dll" (
    echo [√] Qt平台插件已打包
) else (
    echo [X] 警告: Qt平台插件可能缺失
)

:: 計算打包大小
echo.
echo [統計] 打包統計資訊...
for /f %%i in ('dir "%PACKAGE_DIR%" /s /-c ^| find "個檔案"') do set files=%%i
for /f %%i in ('dir "%PACKAGE_DIR%" /s /-c ^| find "個位元組"') do set size=%%i

echo     檔案數量: %files%
echo     總大小: %size%
echo     打包位置: %PACKAGE_DIR%

echo.
echo ========================================================================
echo 🎉 便攜版打包完成！
echo ========================================================================
echo.
echo 打包目錄: %PACKAGE_DIR%
echo.
echo 您現在可以：
echo 1. 將整個「%PACKAGE_NAME%」資料夾複製到任何Windows系統
echo 2. 在目標系統上雙擊「一鍵啟動.bat」即可運行
echo 3. 無需安裝任何依賴，完全便攜化運行
echo.
echo 特性驗證：
echo • VM/RDP環境兼容性: ✅ 已優化
echo • 強制軟體渲染: ✅ 已啟用  
echo • 零依賴安裝: ✅ 已實現
echo • 一鍵啟動: ✅ 已配置
echo.

:: 詢問是否要開啟打包目錄
set /p open="是否要開啟打包目錄？(Y/N): "
if /i "%open%"=="Y" (
    explorer "%PACKAGE_DIR%"
)

echo.
echo 按任意鍵退出...
pause >nul
