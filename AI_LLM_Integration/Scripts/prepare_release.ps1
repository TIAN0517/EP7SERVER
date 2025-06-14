# RANOnline LLM Integration 簡化數位簽名腳本
# 作者: Jy技術團隊

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "RANOnline LLM Integration 數位簽名工具" -ForegroundColor Green
Write-Host "開發團隊: Jy技術團隊" -ForegroundColor Yellow
Write-Host "================================================================" -ForegroundColor Cyan

$TargetFile = "build_mingw\ai_llm_integration.exe"
$CertFile = "Certificates\JyTechTeam_Trusted.pfx"

if (-not (Test-Path $TargetFile)) {
    Write-Host "❌ 目標文件不存在: $TargetFile" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $CertFile)) {
    Write-Host "❌ 證書文件不存在: $CertFile" -ForegroundColor Red
    exit 1
}

Write-Host "🔐 目標文件: $TargetFile" -ForegroundColor Yellow
Write-Host "📜 證書文件: $CertFile" -ForegroundColor Yellow

# 獲取文件信息
$fileInfo = Get-Item $TargetFile
Write-Host "📊 文件大小: $([math]::Round($fileInfo.Length/1MB, 2)) MB" -ForegroundColor Blue
Write-Host "📅 編譯時間: $($fileInfo.LastWriteTime)" -ForegroundColor Blue

Write-Host "✅ 文件已準備完成，可進行數位簽名" -ForegroundColor Green
Write-Host "🎉 RANOnline LLM Integration 準備就緒！" -ForegroundColor Green

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "準備完成 - Jy技術團隊" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Cyan
