# 【Jy技術團隊 線上AI】完整自動化系統打包工具 v4.0.0 PowerShell版本

# 設置控制台編碼
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "========================================================================" -ForegroundColor Cyan
Write-Host "          【Jy技術團隊 線上AI】完整自動化系統打包工具" -ForegroundColor White
Write-Host "                        v4.0.0 Ultimate Edition" -ForegroundColor White
Write-Host "========================================================================" -ForegroundColor Cyan
Write-Host ""

# 設置路徑
$SOURCE_DIR = $PSScriptRoot
$TIMESTAMP = Get-Date -Format "yyyyMMdd_HHmmss"
$PACKAGE_NAME = "Jy技術團隊_線上AI_完整自動化系統_v4.0.0_$TIMESTAMP"
$DESKTOP = [Environment]::GetFolderPath("Desktop")
$PACKAGE_DIR = Join-Path $DESKTOP $PACKAGE_NAME
$ZIP_FILE = Join-Path $DESKTOP "$PACKAGE_NAME.zip"

Write-Host "📦 準備打包以下系統：" -ForegroundColor Yellow
Write-Host "   • 一鍵系統檢查與修復工具 (8步驟完整檢查)" -ForegroundColor White
Write-Host "   • 增強版智能啟動系統 (SQL Server支援)" -ForegroundColor White
Write-Host "   • 資料庫表結構檢查工具" -ForegroundColor White
Write-Host "   • 系統功能快速測試工具" -ForegroundColor White
Write-Host "   • RANOnline AI Ultimate Portable 主程式" -ForegroundColor White
Write-Host "   • 完整的 Qt6 依賴庫" -ForegroundColor White
Write-Host "   • 詳細說明文件和使用指南" -ForegroundColor White
Write-Host ""

# 檢查主程式是否存在
$MAIN_EXE = Join-Path $SOURCE_DIR "RANOnline_AI_Ultimate_Portable.exe"
if (-not (Test-Path $MAIN_EXE)) {
    Write-Host "❌ 找不到主程式檔案！" -ForegroundColor Red
    Write-Host "請確認您在正確的目錄中執行此腳本" -ForegroundColor White
    Read-Host "按任意鍵退出"
    exit 1
}

Write-Host "📂 打包資訊：" -ForegroundColor Cyan
Write-Host "   源目錄: $SOURCE_DIR" -ForegroundColor White
Write-Host "   目標目錄: $PACKAGE_DIR" -ForegroundColor White
Write-Host "   ZIP檔案: $ZIP_FILE" -ForegroundColor White
Write-Host ""

# 開始打包流程
Write-Host "🚀 開始 7 步驟自動化打包流程..." -ForegroundColor Green
Write-Host ""

# 步驟 1: 創建目標目錄
Write-Host "【步驟 1/7】創建打包目錄..." -ForegroundColor Cyan
if (Test-Path $PACKAGE_DIR) {
    Remove-Item $PACKAGE_DIR -Recurse -Force
}
New-Item -ItemType Directory -Path $PACKAGE_DIR | Out-Null
Write-Host "✅ 目錄創建成功: $PACKAGE_DIR" -ForegroundColor Green
Write-Host ""

# 步驟 2: 複製主程式
Write-Host "【步驟 2/7】複製主程式檔案..." -ForegroundColor Cyan
Copy-Item "$SOURCE_DIR\RANOnline_AI_Ultimate_Portable.exe" $PACKAGE_DIR
Write-Host "✅ 主程式複製完成" -ForegroundColor Green
Write-Host ""

# 步驟 3: 複製自動化腳本
Write-Host "【步驟 3/7】複製自動化系統腳本..." -ForegroundColor Cyan
$SCRIPTS = @(
    "一鍵系統檢查與修復.bat",
    "智能啟動_增強版_SQL支援.bat",
    "SQL_資料庫表結構檢查.bat",
    "系統功能快速測試.bat",
    "系統完整性檢查.bat",
    "一鍵啟動_Ultimate_Portable.bat",
    "一鍵關閉.bat"
)

foreach ($script in $SCRIPTS) {
    $sourcePath = Join-Path $SOURCE_DIR $script
    if (Test-Path $sourcePath) {
        Copy-Item $sourcePath $PACKAGE_DIR
        Write-Host "  ✓ $script" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ 找不到: $script" -ForegroundColor Yellow
    }
}
Write-Host ""

# 步驟 4: 複製 Qt6 依賴庫
Write-Host "【步驟 4/7】複製 Qt6 依賴庫和平台插件..." -ForegroundColor Cyan
$QT_FILES = @(
    "Qt6Core.dll",
    "Qt6Gui.dll", 
    "Qt6Widgets.dll",
    "Qt6Network.dll",
    "Qt6Svg.dll",
    "D3Dcompiler_47.dll",
    "opengl32sw.dll",
    "libgcc_s_seh-1.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll"
)

foreach ($file in $QT_FILES) {
    $sourcePath = Join-Path $SOURCE_DIR $file
    if (Test-Path $sourcePath) {
        Copy-Item $sourcePath $PACKAGE_DIR
        Write-Host "  ✓ $file" -ForegroundColor Green
    }
}

# 複製 Qt6 插件目錄
$QT_DIRS = @("platforms", "styles", "imageformats", "iconengines", "generic", "networkinformation", "tls", "translations")
foreach ($dir in $QT_DIRS) {
    $sourcePath = Join-Path $SOURCE_DIR $dir
    if (Test-Path $sourcePath) {
        Copy-Item $sourcePath $PACKAGE_DIR -Recurse
        Write-Host "  ✓ $dir/" -ForegroundColor Green
    }
}
Write-Host ""

# 步驟 5: 複製配置文件和文檔
Write-Host "【步驟 5/7】複製配置文件和說明文檔..." -ForegroundColor Cyan
$CONFIG_FILES = @(
    "AI_CONFIG.INI",
    "README_完整自動化系統.md",
    "README_繁體中文.md"
)

foreach ($file in $CONFIG_FILES) {
    $sourcePath = Join-Path $SOURCE_DIR $file
    if (Test-Path $sourcePath) {
        Copy-Item $sourcePath $PACKAGE_DIR
        Write-Host "  ✓ $file" -ForegroundColor Green
    }
}

# 創建 logs 目錄
$logsDir = Join-Path $PACKAGE_DIR "logs"
New-Item -ItemType Directory -Path $logsDir -Force | Out-Null
Write-Host "  ✓ logs/" -ForegroundColor Green
Write-Host ""

# 步驟 6: 創建快速啟動選單
Write-Host "【步驟 6/7】創建快速啟動選單..." -ForegroundColor Cyan
$QUICK_START = Join-Path $PACKAGE_DIR "🚀_快速啟動選單.bat"
$quickStartContent = @'
@echo off
chcp 65001 >nul
title 【Jy技術團隊 線上AI】快速啟動選單 v4.0.0

echo.
echo ========================================================================
echo           【Jy技術團隊 線上AI】快速啟動選單 v4.0.0
echo ========================================================================
echo.
echo 請選擇要執行的功能：
echo.
echo [1] 一鍵系統檢查與修復 (推薦首次使用)
echo [2] 智能啟動 AI 系統 (SQL Server支援)
echo [3] 系統功能快速測試
echo [4] SQL 資料庫表結構檢查
echo [5] 查看完整說明文檔
echo [6] 一鍵關閉所有服務
echo [0] 退出
echo.
set /p choice=請輸入選項 [0-6]: 

if "%choice%"=="1" start "系統檢查" "一鍵系統檢查與修復.bat"
if "%choice%"=="2" start "智能啟動" "智能啟動_增強版_SQL支援.bat"
if "%choice%"=="3" start "功能測試" "系統功能快速測試.bat"
if "%choice%"=="4" start "資料庫檢查" "SQL_資料庫表結構檢查.bat"
if "%choice%"=="5" start "說明文檔" "README_完整自動化系統.md"
if "%choice%"=="6" start "關閉服務" "一鍵關閉.bat"
if "%choice%"=="0" exit

echo.
echo 功能已啟動，您可以關閉此視窗或繼續選擇其他功能
echo.
pause >nul
'@
$quickStartContent | Out-File -FilePath $QUICK_START -Encoding UTF8
Write-Host "✅ 快速啟動選單創建完成" -ForegroundColor Green
Write-Host ""

# 步驟 7: 創建 ZIP 壓縮檔案
Write-Host "【步驟 7/7】創建 ZIP 壓縮檔案..." -ForegroundColor Cyan
if (Test-Path $ZIP_FILE) {
    Remove-Item $ZIP_FILE -Force
}

try {
    Compress-Archive -Path "$PACKAGE_DIR\*" -DestinationPath $ZIP_FILE -CompressionLevel Optimal
    Write-Host "✅ ZIP 檔案創建成功: $ZIP_FILE" -ForegroundColor Green
} catch {
    Write-Host "❌ ZIP 檔案創建失敗: $($_.Exception.Message)" -ForegroundColor Red
}
Write-Host ""

# 統計檔案資訊
Write-Host "📊 打包統計資訊：" -ForegroundColor Cyan
$fileCount = (Get-ChildItem $PACKAGE_DIR -Recurse -File).Count
$totalSize = [math]::Round((Get-ChildItem $PACKAGE_DIR -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB, 2)
$zipSize = if (Test-Path $ZIP_FILE) { [math]::Round((Get-Item $ZIP_FILE).Length / 1MB, 2) } else { 0 }

Write-Host "   檔案數量: $fileCount 個" -ForegroundColor White
Write-Host "   總大小: $totalSize MB" -ForegroundColor White
Write-Host "   ZIP大小: $zipSize MB" -ForegroundColor White
Write-Host ""

# 完成報告
Write-Host "🎉 完整自動化系統打包完成！" -ForegroundColor Green
Write-Host ""
Write-Host "📦 打包檔案位置：" -ForegroundColor Yellow
Write-Host "   目錄: $PACKAGE_DIR" -ForegroundColor White
Write-Host "   ZIP: $ZIP_FILE" -ForegroundColor White
Write-Host ""
Write-Host "🚀 使用方法：" -ForegroundColor Yellow
Write-Host "   1. 解壓 ZIP 檔案到目標電腦" -ForegroundColor White
Write-Host "   2. 執行 '🚀_快速啟動選單.bat'" -ForegroundColor White
Write-Host "   3. 首次使用建議先執行 '一鍵系統檢查與修復'" -ForegroundColor White
Write-Host ""

Read-Host "按任意鍵開啟打包目錄"
Start-Process "explorer.exe" -ArgumentList $DESKTOP
