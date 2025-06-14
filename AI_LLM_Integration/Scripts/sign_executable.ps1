# RANOnline LLM Integration 數位簽名腳本
# 作者: Jy技術團隊
# 日期: 2025年6月14日

param(
    [string]$TargetFile = "build_mingw\ai_llm_integration.exe",
    [string]$CertFile = "Certificates\JyTechTeam_Trusted.pfx",
    [string]$Password = "JyTechTeam2025",
    [string]$Description = "RANOnline EP7 AI LLM整合系統 - Jy技術團隊開發",
    [string]$DescriptionUrl = "https://github.com/JyTechTeam/RANOnline"
)

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "RANOnline LLM Integration 數位簽名工具" -ForegroundColor Green
Write-Host "開發團隊: Jy技術團隊" -ForegroundColor Yellow
Write-Host "================================================================" -ForegroundColor Cyan

# 檢查文件是否存在
if (-not (Test-Path $TargetFile)) {
    Write-Host "❌ 目標文件不存在: $TargetFile" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $CertFile)) {
    Write-Host "❌ 證書文件不存在: $CertFile" -ForegroundColor Red
    exit 1
}

Write-Host "🔐 正在為以下文件進行數位簽名..." -ForegroundColor Yellow
Write-Host "   目標文件: $TargetFile" -ForegroundColor Gray
Write-Host "   證書文件: $CertFile" -ForegroundColor Gray
Write-Host "   說明: $Description" -ForegroundColor Gray

try {
    # 使用 signtool 進行數位簽名
    $signtoolPath = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\x64\signtool.exe"
    
    if (-not (Test-Path $signtoolPath)) {
        # 嘗試其他常見路徑
        $signtoolPath = Get-ChildItem "C:\Program Files*\Windows Kits\*\bin\*\*\signtool.exe" | Select-Object -First 1 -ExpandProperty FullName
    }
    
    if (-not $signtoolPath) {
        Write-Host "❌ 找不到 signtool.exe，請安裝 Windows SDK" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "🔧 使用 signtool: $signtoolPath" -ForegroundColor Blue
    
    # 執行簽名
    $arguments = @(
        "sign",
        "/f", $CertFile,
        "/p", $Password,
        "/d", $Description,
        "/du", $DescriptionUrl,
        "/t", "http://timestamp.digicert.com",
        "/v",
        $TargetFile
    )
    
    $process = Start-Process -FilePath $signtoolPath -ArgumentList $arguments -Wait -PassThru -NoNewWindow
    
    if ($process.ExitCode -eq 0) {
        Write-Host "✅ 數位簽名成功完成！" -ForegroundColor Green
        Write-Host "📋 簽名信息:" -ForegroundColor Cyan
        
        # 驗證簽名
        & $signtoolPath verify /pa /v $TargetFile
        
        Write-Host "🎉 RANOnline LLM Integration 已成功簽名！" -ForegroundColor Green
    } else {
        Write-Host "❌ 數位簽名失敗，退出碼: $($process.ExitCode)" -ForegroundColor Red
        exit $process.ExitCode
    }
    
} catch {
    Write-Host "❌ 簽名過程中發生錯誤: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "簽名完成 - Jy技術團隊" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Cyan
