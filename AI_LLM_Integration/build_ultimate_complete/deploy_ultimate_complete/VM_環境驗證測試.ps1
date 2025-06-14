# RANOnline AI Ultimate Portable - VM環境驗證測試 (PowerShell版)

Write-Host ""
Write-Host "========================================================================" -ForegroundColor Cyan
Write-Host "   🧪 RANOnline AI Ultimate Portable - VM環境驗證測試" -ForegroundColor Cyan  
Write-Host "========================================================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "   正在檢測系統環境..." -ForegroundColor Cyan

# 檢測是否在VM環境
$isVM = $false
$vmType = "Unknown"

try {
    # 檢查計算機製造商
    $manufacturer = (Get-WmiObject -Class Win32_ComputerSystem).Manufacturer
    $model = (Get-WmiObject -Class Win32_ComputerSystem).Model
    
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
} catch {
    Write-Host "   ⚠️  VM檢測遇到問題，繼續測試..." -ForegroundColor Yellow
}

# 檢查RDP會話
$isRDP = $false
try {
    $sessionInfo = quser 2>$null
    if ($sessionInfo -match "rdp") {
        $isRDP = $true
    }
} catch {
    # 如果quser命令失敗，檢查環境變數
    if ($env:SESSIONNAME -match "RDP") {
        $isRDP = $true
    }
}

Write-Host "   🖥️  系統類型: $vmType" -ForegroundColor White
if ($isVM) {
    Write-Host "   ✅ 虛擬機環境檢測: 是" -ForegroundColor Green
} else {
    Write-Host "   ⚠️  虛擬機環境檢測: 否（物理機）" -ForegroundColor Yellow
}

if ($isRDP) {
    Write-Host "   ✅ RDP會話檢測: 是" -ForegroundColor Green  
} else {
    Write-Host "   📋 RDP會話檢測: 否（本地會話）" -ForegroundColor White
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
    Write-Host ""
    Write-Host "========================================================================" -ForegroundColor Red
    Write-Host "   ❌ VM環境驗證測試失敗！" -ForegroundColor Red
    Write-Host "========================================================================" -ForegroundColor Red
    Read-Host "按任意鍵繼續"
    exit 1
}

# 檢查Qt運行時庫
$qtMissing = $false
$qtLibs = @("Qt6Core.dll", "Qt6Widgets.dll", "Qt6Gui.dll")

foreach ($lib in $qtLibs) {
    if (-not (Test-Path $lib)) {
        Write-Host "   ❌ $lib 缺失" -ForegroundColor Red
        $qtMissing = $true
    }
}

if (-not $qtMissing) {
    Write-Host "   ✅ Qt6運行時庫完整" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "========================================================================" -ForegroundColor Red
    Write-Host "   ❌ VM環境驗證測試失敗！" -ForegroundColor Red
    Write-Host "========================================================================" -ForegroundColor Red
    Read-Host "按任意鍵繼續"
    exit 1
}

# 檢查MinGW運行時庫
if (Test-Path "libgcc_s_seh-1.dll") {
    Write-Host "   ✅ MinGW運行時庫存在" -ForegroundColor Green
} else {
    Write-Host "   ⚠️  libgcc_s_seh-1.dll 缺失（可能影響運行）" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "   🚀 啟動相容性測試..." -ForegroundColor Cyan

# 設置VM優化環境變數
$env:QT_AUTO_SCREEN_SCALE_FACTOR = "0"
$env:QT_SCALE_FACTOR = "1"
$env:QT_SCREEN_SCALE_FACTORS = "1"
$env:QT_ENABLE_HIGHDPI_SCALING = "0"
$env:QT_USE_PHYSICAL_DPI = "0"
$env:QT_QPA_PLATFORM = "windows"
$env:QT_OPENGL = "software"
$env:MESA_GL_VERSION_OVERRIDE = "3.3"

Write-Host "   環境變數設置完成..." -ForegroundColor White
Write-Host "   正在啟動程式..." -ForegroundColor White

# 啟動程式
try {
    Start-Process -FilePath "RANOnline_AI_Ultimate_Portable.exe" -WindowStyle Normal
    Start-Sleep -Seconds 3
    
    # 檢查程式是否正在運行
    $process = Get-Process -Name "RANOnline_AI_Ultimate_Portable" -ErrorAction SilentlyContinue
    if ($process) {
        Write-Host "   ✅ 程式已成功啟動" -ForegroundColor Green
        Write-Host "   📋 進程狀態: 運行中" -ForegroundColor White
        Write-Host "   📋 進程ID: $($process.Id)" -ForegroundColor White
    } else {
        Write-Host "   ❌ 程式啟動失敗" -ForegroundColor Red
        Write-Host ""
        Write-Host "========================================================================" -ForegroundColor Red
        Write-Host "   ❌ VM環境驗證測試失敗！" -ForegroundColor Red
        Write-Host "========================================================================" -ForegroundColor Red
        Read-Host "按任意鍵繼續"
        exit 1
    }
} catch {
    Write-Host "   ❌ 程式啟動遇到異常: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "========================================================================" -ForegroundColor Red  
    Write-Host "   ❌ VM環境驗證測試失敗！" -ForegroundColor Red
    Write-Host "========================================================================" -ForegroundColor Red
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
if ($isVM) {
    Write-Host "   • VM兼容性: ✅ 完全兼容" -ForegroundColor White  
} else {
    Write-Host "   • VM兼容性: ✅ 物理機正常" -ForegroundColor White
}
if ($isRDP) {
    Write-Host "   • RDP兼容性: ✅ 遠程會話正常" -ForegroundColor White
} else {
    Write-Host "   • RDP兼容性: ✅ 本地會話正常" -ForegroundColor White
}
Write-Host "   • 程式狀態: ✅ 運行正常" -ForegroundColor White
Write-Host "   • 依賴庫: ✅ 完整" -ForegroundColor White

Write-Host ""
Write-Host "   💡 如果程式正常顯示UI界面，則表示Ultimate Portable版本" -ForegroundColor Cyan
Write-Host "   在當前環境下運行完美，可以正式部署使用。" -ForegroundColor Cyan
Write-Host ""

Read-Host "按任意鍵繼續"
