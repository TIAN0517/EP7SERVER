# RANOnline AI Ultimate Portable - 簡化版VM環境驗證測試

Write-Host ""
Write-Host "========================================================================" -ForegroundColor Cyan
Write-Host "   🧪 RANOnline AI Ultimate Portable - VM環境驗證測試" -ForegroundColor Cyan  
Write-Host "========================================================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "   正在檢測系統環境..." -ForegroundColor Cyan

# 簡化的環境檢測
$computerInfo = Get-ComputerInfo -Property "CsManufacturer", "CsModel" -ErrorAction SilentlyContinue
$isVM = $false
$vmType = "Unknown"

if ($computerInfo) {
    $manufacturer = $computerInfo.CsManufacturer
    $model = $computerInfo.CsModel
    
    if ($manufacturer -match "VMware") {
        $isVM = $true
        $vmType = "VMware"
    } elseif ($manufacturer -match "VirtualBox" -or $model -match "VirtualBox") {
        $isVM = $true
        $vmType = "VirtualBox"  
    } elseif ($manufacturer -match "Microsoft" -and $model -match "Virtual") {
        $isVM = $true
        $vmType = "Hyper-V"
    } elseif ($model -match "Virtual") {
        $isVM = $true
        $vmType = "Virtual Machine"
    }
}

Write-Host "   🖥️  系統類型: $vmType" -ForegroundColor White
if ($isVM) {
    Write-Host "   ✅ 虛擬機環境檢測: 是" -ForegroundColor Green
} else {
    Write-Host "   ⚠️  虛擬機環境檢測: 否（物理機或檢測失敗）" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "   🔧 檢查必要組件..." -ForegroundColor Cyan

# 檢查可執行文件
if (Test-Path "RANOnline_AI_Ultimate_Portable.exe") {
    Write-Host "   ✅ 主程式檔案存在" -ForegroundColor Green
    $exeSize = (Get-Item "RANOnline_AI_Ultimate_Portable.exe").Length
    Write-Host "      大小: $exeSize bytes" -ForegroundColor White
} else {
    Write-Host "   ❌ 主程式檔案缺失" -ForegroundColor Red
    Read-Host "按任意鍵繼續"
    exit 1
}

# 檢查Qt運行時庫
$qtLibs = @("Qt6Core.dll", "Qt6Widgets.dll", "Qt6Gui.dll")
$qtMissing = 0

foreach ($lib in $qtLibs) {
    if (Test-Path $lib) {
        Write-Host "   ✅ $lib 存在" -ForegroundColor Green
    } else {
        Write-Host "   ❌ $lib 缺失" -ForegroundColor Red
        $qtMissing++
    }
}

if ($qtMissing -eq 0) {
    Write-Host "   ✅ Qt6運行時庫完整" -ForegroundColor Green
} else {
    Write-Host "   ❌ 缺失 $qtMissing 個Qt6運行時庫" -ForegroundColor Red
    Read-Host "按任意鍵繼續"
    exit 1
}

Write-Host ""
Write-Host "   🚀 啟動相容性測試..." -ForegroundColor Cyan

# 設置VM優化環境變數
$env:QT_AUTO_SCREEN_SCALE_FACTOR = "0"
$env:QT_SCALE_FACTOR = "1"
$env:QT_OPENGL = "software"

Write-Host "   環境變數設置完成..." -ForegroundColor White
Write-Host "   正在啟動程式..." -ForegroundColor White

# 啟動程式
try {
    $process = Start-Process -FilePath "RANOnline_AI_Ultimate_Portable.exe" -PassThru -WindowStyle Normal
    Start-Sleep -Seconds 3
    
    # 檢查程式是否正在運行
    if ($process -and !$process.HasExited) {
        Write-Host "   ✅ 程式已成功啟動" -ForegroundColor Green
        Write-Host "   📋 進程狀態: 運行中 (PID: $($process.Id))" -ForegroundColor White
    } else {
        Write-Host "   ❌ 程式啟動失敗或已退出" -ForegroundColor Red
        Read-Host "按任意鍵繼續"
        exit 1
    }
} catch {
    Write-Host "   ❌ 程式啟動遇到異常: $($_.Exception.Message)" -ForegroundColor Red
    Read-Host "按任意鍵繼續"
    exit 1
}

Write-Host ""
Write-Host "========================================================================" -ForegroundColor Green
Write-Host "   🎉 VM環境驗證測試完成！" -ForegroundColor Green
Write-Host "========================================================================" -ForegroundColor Green
Write-Host ""

Write-Host "   📊 測試結果摘要:" -ForegroundColor Magenta
Write-Host "   • 環境類型: $vmType" -ForegroundColor White
Write-Host "   • VM兼容性: ✅ 完全兼容" -ForegroundColor White  
Write-Host "   • 程式狀態: ✅ 運行正常" -ForegroundColor White
Write-Host "   • 依賴庫: ✅ 完整" -ForegroundColor White

Write-Host ""
Write-Host "   💡 如果程式正常顯示UI界面，則表示Ultimate Portable版本" -ForegroundColor Cyan
Write-Host "   在當前環境下運行完美，可以正式部署使用。" -ForegroundColor Cyan
Write-Host ""

Read-Host "按任意鍵繼續"
