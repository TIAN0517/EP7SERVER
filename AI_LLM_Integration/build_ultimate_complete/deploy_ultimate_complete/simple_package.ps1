# PowerShell 打包工具
Write-Host "开始打包系统..." -ForegroundColor Green

$SOURCE_DIR = $PSScriptRoot
$TIMESTAMP = Get-Date -Format "yyyyMMdd_HHmmss"
$PACKAGE_NAME = "Jy_AI_System_$TIMESTAMP"
$DESKTOP = [Environment]::GetFolderPath("Desktop")
$PACKAGE_DIR = Join-Path $DESKTOP $PACKAGE_NAME

# 创建目录
if (Test-Path $PACKAGE_DIR) { Remove-Item $PACKAGE_DIR -Recurse -Force }
New-Item -ItemType Directory -Path $PACKAGE_DIR | Out-Null

# 复制所有 .bat 脚本文件
Write-Host "复制自动化脚本文件..." -ForegroundColor Cyan
$batFiles = Get-ChildItem -Filter "*.bat" | Where-Object { $_.Name -ne "📦_完整系統打包工具.bat" }
foreach ($batFile in $batFiles) {
    Copy-Item $batFile.FullName $PACKAGE_DIR
    Write-Host "  ✓ $($batFile.Name)" -ForegroundColor Green
}

# 复制主要文件
$files = @(
    "RANOnline_AI_Ultimate_Portable.exe",
    "AI_CONFIG.INI",
    "README_完整自動化系統.md",
    "README_繁體中文.md"
)

foreach ($file in $files) {
    if (Test-Path $file) {
        Copy-Item $file $PACKAGE_DIR
        Write-Host "复制: $file" -ForegroundColor Yellow
    }
}

# 复制 Qt6 库文件
$qtFiles = @(
    "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "Qt6Network.dll", "Qt6Svg.dll",
    "D3Dcompiler_47.dll", "opengl32sw.dll", "libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll"
)

foreach ($file in $qtFiles) {
    if (Test-Path $file) {
        Copy-Item $file $PACKAGE_DIR
        Write-Host "复制 Qt6: $file" -ForegroundColor Cyan
    }
}

# 复制目录
$dirs = @("platforms", "styles", "imageformats", "iconengines", "generic", "networkinformation", "tls", "translations")
foreach ($dir in $dirs) {
    if (Test-Path $dir) {
        Copy-Item $dir $PACKAGE_DIR -Recurse
        Write-Host "复制目录: $dir" -ForegroundColor Magenta
    }
}

# 创建 logs 目录
New-Item -ItemType Directory -Path (Join-Path $PACKAGE_DIR "logs") -Force | Out-Null

# 创建快速启动菜单
$startMenu = @'
@echo off
chcp 65001 >nul
title Jy AI System Quick Start

echo ========================================
echo    Jy AI System Quick Start Menu
echo ========================================
echo.
echo [1] System Check and Repair
echo [2] Smart Startup (SQL Support)
echo [3] Quick Function Test
echo [4] Database Structure Check
echo [5] View Documentation
echo [6] Shutdown Services
echo [0] Exit
echo.
set /p choice=Select option [0-6]: 

if "%choice%"=="1" start "" "一鍵系統檢查與修復.bat"
if "%choice%"=="2" start "" "智能啟動_增強版_SQL支援.bat"
if "%choice%"=="3" start "" "系統功能快速測試.bat"
if "%choice%"=="4" start "" "SQL_資料庫表結構檢查.bat"
if "%choice%"=="5" start "" "README_完整自動化系統.md"
if "%choice%"=="6" start "" "一鍵關閉.bat"
if "%choice%"=="0" exit

echo.
pause
'@

$startMenu | Out-File -FilePath (Join-Path $PACKAGE_DIR "Quick_Start.bat") -Encoding UTF8

# 创建 ZIP 文件
$zipFile = Join-Path $DESKTOP "$PACKAGE_NAME.zip"
if (Test-Path $zipFile) { Remove-Item $zipFile -Force }

try {
    Compress-Archive -Path "$PACKAGE_DIR\*" -DestinationPath $zipFile -CompressionLevel Optimal
    Write-Host "ZIP 文件创建成功: $zipFile" -ForegroundColor Green
} catch {
    Write-Host "ZIP 创建失败: $($_.Exception.Message)" -ForegroundColor Red
}

# 统计信息
$fileCount = (Get-ChildItem $PACKAGE_DIR -Recurse -File).Count
$totalSize = [math]::Round((Get-ChildItem $PACKAGE_DIR -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB, 2)

Write-Host ""
Write-Host "打包完成!" -ForegroundColor Green
Write-Host "文件数量: $fileCount" -ForegroundColor White
Write-Host "总大小: $totalSize MB" -ForegroundColor White
Write-Host "打包位置: $PACKAGE_DIR" -ForegroundColor White
Write-Host "ZIP 文件: $zipFile" -ForegroundColor White

Read-Host "Press Enter to open desktop folder"
Start-Process "explorer.exe" -ArgumentList $DESKTOP
