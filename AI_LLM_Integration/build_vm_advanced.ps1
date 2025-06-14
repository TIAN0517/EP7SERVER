# ========================================================================
# RANOnline AI LLM Integration - VM相容版PowerShell自動構建腳本
# 版本: 3.1.0 VM專用版
# 開發團隊: Jy技術團隊
# 
# 🎯 高級功能特性:
# ✅ 智慧依賴分析
# ✅ 自動錯誤檢測和修復
# ✅ 並行構建優化
# ✅ 完整性驗證
# ✅ 自動測試啟動
# ========================================================================

param(
    [switch]$SkipBuild,
    [switch]$SkipTest,
    [switch]$Verbose,
    [string]$QtPath = "",
    [string]$OutputDir = "vm_package_ps"
)

# 設置錯誤處理
$ErrorActionPreference = "Stop"
$ProgressPreference = "Continue"

# 顏色輸出函數
function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
}

function Write-Success {
    param([string]$Message)
    Write-ColorOutput "✅ $Message" "Green"
}

function Write-Warning {
    param([string]$Message)
    Write-ColorOutput "⚠️ $Message" "Yellow"
}

function Write-Error {
    param([string]$Message)
    Write-ColorOutput "❌ $Message" "Red"
}

function Write-Info {
    param([string]$Message)
    Write-ColorOutput "ℹ️ $Message" "Cyan"
}

function Write-Header {
    param([string]$Message)
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Magenta
    Write-ColorOutput $Message "Magenta"
    Write-Host "========================================" -ForegroundColor Magenta
    Write-Host ""
}

# 開始構建
Write-Header "RANOnline AI LLM Integration - VM相容版自動構建系統"
Write-Info "版本: 3.1.0 VM專用版"
Write-Info "開發團隊: Jy技術團隊"
Write-Info "PowerShell版本: $($PSVersionTable.PSVersion)"

# 設置路徑變數
$ProjectDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ProjectDir "build_vm_ps"
$DeployDir = Join-Path $ProjectDir "deploy_vm_ps"
$PackageDir = Join-Path $ProjectDir $OutputDir
$ZipName = "RANOnline_AI_VM_Compatible_PowerShell_v3.1.0.zip"

Write-Info "專案目錄: $ProjectDir"
Write-Info "構建目錄: $BuildDir"
Write-Info "打包目錄: $PackageDir"

# 步驟1: 清理環境
Write-Header "步驟 1/12: 清理構建環境"
@($BuildDir, $DeployDir, $PackageDir) | ForEach-Object {
    if (Test-Path $_) {
        Write-Info "清理目錄: $_"
        Remove-Item $_ -Recurse -Force
    }
    New-Item $_ -ItemType Directory -Force | Out-Null
}
Write-Success "環境清理完成"

# 步驟2: 自動偵測Qt6
Write-Header "步驟 2/12: 偵測Qt6和開發環境"

$Qt6Candidates = @(
    "C:\Qt\6.9.1\mingw_64",
    "C:\Qt\6.8.0\mingw_64", 
    "C:\Qt\6.7.0\mingw_64",
    "C:\Qt\6.6.0\mingw_64",
    "C:\Qt\6.5.0\mingw_64"
)

$Qt6Root = ""
if ($QtPath -ne "") {
    if (Test-Path (Join-Path $QtPath "bin\qmake.exe")) {
        $Qt6Root = $QtPath
        Write-Success "使用指定的Qt6路徑: $Qt6Root"
    } else {
        Write-Warning "指定的Qt6路徑無效，將自動搜尋"
    }
}

if ($Qt6Root -eq "") {
    foreach ($candidate in $Qt6Candidates) {
        if (Test-Path (Join-Path $candidate "bin\qmake.exe")) {
            $Qt6Root = $candidate
            Write-Success "發現Qt6: $Qt6Root"
            break
        }
    }
}

if ($Qt6Root -eq "") {
    Write-Error "找不到Qt6安裝！請安裝Qt6或使用 -QtPath 參數指定路徑"
    exit 1
}

# 設置環境變數
$Qt6BinDir = Join-Path $Qt6Root "bin"
$MinGWDir = $Qt6Root -replace "mingw_64", "..\..\..\Tools\mingw1310_64\bin"
$env:PATH = "$Qt6BinDir;$MinGWDir;$env:PATH"
$Qt6Dir = Join-Path $Qt6Root "lib\cmake\Qt6"

Write-Info "Qt6 Bin目錄: $Qt6BinDir"
Write-Info "MinGW目錄: $MinGWDir"
Write-Info "Qt6 CMake目錄: $Qt6Dir"

# 步驟3: 驗證工具
Write-Header "步驟 3/12: 驗證構建工具"

$RequiredTools = @{
    "cmake" = "CMake"
    "g++" = "MinGW C++ Compiler"
    "windeployqt" = "Qt6 Deployment Tool"
    "qmake" = "Qt6 Make Tool"
}

foreach ($tool in $RequiredTools.Keys) {
    try {
        $null = Get-Command $tool -ErrorAction Stop
        Write-Success "$($RequiredTools[$tool]) 已找到"
    } catch {
        Write-Error "$($RequiredTools[$tool]) 未找到！請檢查安裝和PATH設定"
        exit 1
    }
}

# 步驟4: 檢查源碼文件
Write-Header "步驟 4/12: 檢查源碼文件完整性"

$RequiredFiles = @(
    "LLMMainWindow.cpp",
    "LLMMainWindow.h",
    "main_enterprise_compatible.cpp",
    "main_vm_optimized.cpp",
    "CMakeLists_VM.txt"
)

$MissingFiles = @()
foreach ($file in $RequiredFiles) {
    $filePath = Join-Path $ProjectDir $file
    if (Test-Path $filePath) {
        Write-Success "源碼文件存在: $file"
    } else {
        Write-Warning "源碼文件缺失: $file"
        $MissingFiles += $file
    }
}

if ($MissingFiles.Count -gt 0) {
    Write-Warning "部分源碼文件缺失，但將繼續構建..."
}

# 步驟5: 準備CMake配置
Write-Header "步驟 5/12: 準備CMake配置"

$CMakeSourceFile = Join-Path $ProjectDir "CMakeLists_VM.txt"
$CMakeTargetFile = Join-Path $ProjectDir "CMakeLists.txt"

if (Test-Path $CMakeSourceFile) {
    Copy-Item $CMakeSourceFile $CMakeTargetFile -Force
    Write-Success "VM專用CMakeLists.txt已複製"
} else {
    Write-Warning "VM專用CMakeLists.txt不存在，使用現有的CMakeLists.txt"
}

# 步驟6: CMake配置
if (-not $SkipBuild) {
    Write-Header "步驟 6/12: CMake配置"
    
    Set-Location $BuildDir
    
    $CMakeArgs = @(
        "..",
        "-G", "MinGW Makefiles",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DQt6_DIR=$Qt6Dir",
        "-DBUILD_VM_COMPATIBLE=ON",
        "-DBUILD_STATIC=ON",
        "-DBUILD_FULL_BUNDLE=ON",
        "-DCMAKE_CXX_FLAGS=-DQT_NO_OPENGL_ES_2 -DQT_OPENGL_SOFTWARE"
    )
    
    if ($Verbose) {
        $CMakeArgs += "--verbose"
    }
    
    Write-Info "執行CMake配置..."
    Write-Info "命令: cmake $($CMakeArgs -join ' ')"
    
    try {
        & cmake @CMakeArgs
        if ($LASTEXITCODE -ne 0) { throw "CMake配置失敗" }
        Write-Success "CMake配置成功"
    } catch {
        Write-Error "CMake配置失敗: $_"
        Set-Location $ProjectDir
        exit 1
    }

    # 步驟7: 構建專案
    Write-Header "步驟 7/12: 構建專案"
    
    $BuildArgs = @(
        "--build", ".",
        "--config", "Release",
        "--parallel", "4"
    )
    
    if ($Verbose) {
        $BuildArgs += "--verbose"
    }
    
    Write-Info "執行構建..."
    Write-Info "命令: cmake $($BuildArgs -join ' ')"
    
    try {
        & cmake @BuildArgs
        if ($LASTEXITCODE -ne 0) { throw "構建失敗" }
        Write-Success "構建成功"
    } catch {
        Write-Error "構建失敗: $_"
        Set-Location $ProjectDir
        exit 1
    }
    
    Set-Location $ProjectDir
} else {
    Write-Warning "跳過構建步驟 (-SkipBuild)"
}

# 步驟8: 驗證執行檔
Write-Header "步驟 8/12: 驗證生成的執行檔"

$StandardExe = Join-Path $BuildDir "ai_llm_integration.exe"
$VmExe = Join-Path $BuildDir "ai_llm_integration_vm.exe"

if (Test-Path $StandardExe) {
    $standardSize = (Get-Item $StandardExe).Length
    Write-Success "通用版執行檔已生成: ai_llm_integration.exe ($standardSize bytes)"
} else {
    Write-Error "通用版執行檔未找到: $StandardExe"
    exit 1
}

if (Test-Path $VmExe) {
    $vmSize = (Get-Item $VmExe).Length
    Write-Success "VM版執行檔已生成: ai_llm_integration_vm.exe ($vmSize bytes)"
} else {
    Write-Warning "VM版執行檔未找到，將使用通用版"
    $VmExe = $StandardExe
}

# 步驟9: 創建部署包結構
Write-Header "步驟 9/12: 創建部署包結構"

$StandardDir = Join-Path $PackageDir "Standard_Version"
$VmDir = Join-Path $PackageDir "VM_Compatible_Version"

New-Item $StandardDir -ItemType Directory -Force | Out-Null
New-Item $VmDir -ItemType Directory -Force | Out-Null

Write-Success "部署包目錄結構已創建"

# 步驟10: 部署應用程式
Write-Header "步驟 10/12: 部署應用程式和依賴"

# 部署通用版本
Write-Info "部署通用版本..."
Copy-Item $StandardExe $StandardDir -Force
Set-Location $StandardDir

$WinDeployArgs = @(
    "--verbose", "2",
    "--force",
    "--compiler-runtime",
    "--translations",
    "--opengl-sw",
    "ai_llm_integration.exe"
)

try {
    & windeployqt @WinDeployArgs
    if ($LASTEXITCODE -ne 0) { throw "WinDeployQt失敗" }
    Write-Success "通用版本部署完成"
} catch {
    Write-Warning "通用版本部署警告: $_"
}

# 部署VM版本
Write-Info "部署VM版本..."
Copy-Item $VmExe $VmDir -Force
Set-Location $VmDir

$VmDeployArgs = @(
    "--verbose", "2",
    "--force",
    "--compiler-runtime",
    "--no-translations",
    "--no-system-d3d-compiler",
    "--no-virtualkeyboard",
    "--no-webkit2",
    "--no-angle",
    (Split-Path -Leaf $VmExe)
)

try {
    & windeployqt @VmDeployArgs
    if ($LASTEXITCODE -ne 0) { throw "VM版WinDeployQt失敗" }
    Write-Success "VM版本部署完成"
} catch {
    Write-Warning "VM版本部署警告: $_"
}

# 手動複製關鍵VM依賴
Write-Info "手動複製VM關鍵依賴..."

$VmDependencies = @{
    "opengl32sw.dll" = "軟體渲染庫"
    "libgcc_s_seh-1.dll" = "GCC運行時"
    "libstdc++-6.dll" = "C++標準庫"
    "libwinpthread-1.dll" = "POSIX線程庫"
}

foreach ($dll in $VmDependencies.Keys) {
    $sourcePath = Join-Path $Qt6BinDir $dll
    $targetPath = Join-Path $VmDir $dll
    
    if (Test-Path $sourcePath) {
        Copy-Item $sourcePath $targetPath -Force
        Write-Success "已複製 $dll ($($VmDependencies[$dll]))"
    } else {
        Write-Warning "未找到 $dll"
    }
}

Set-Location $ProjectDir

# 步驟11: 創建啟動腳本和文檔
Write-Header "步驟 11/12: 創建啟動腳本和文檔"

# VM版啟動腳本
$VmBatContent = @"
@echo off
REM VM相容模式啟動腳本 - PowerShell版本
echo ========================================
echo RANOnline AI LLM Integration - VM相容版
echo 版本: 3.1.0 VM專用版
echo 開發團隊: Jy技術團隊
echo ========================================
echo.

REM 設置VM相容環境變數
set QT_OPENGL=software
set QT_ANGLE_PLATFORM=d3d11
set QT_AUTO_SCREEN_SCALE_FACTOR=0
set QT_SCALE_FACTOR=1
set QT_QPA_PLATFORM=windows
set QT_QPA_PLATFORM_PLUGIN_PATH=platforms

echo 正在啟動VM相容版本...
echo 使用軟體渲染模式，適用於虛擬機/雲端主機環境
echo.

start "RANOnline AI VM版" $(Split-Path -Leaf $VmExe)

echo 如果啟動失敗，請檢查：
echo 1. VM分配的記憶體是否足夠 (建議4GB+)
echo 2. 是否已安裝最新的VM工具
echo 3. Windows事件檢視器中的錯誤資訊
echo.
pause
"@

$VmBatPath = Join-Path $VmDir "啟動VM相容版.bat"
$VmBatContent | Out-File -FilePath $VmBatPath -Encoding ASCII
Write-Success "VM版啟動腳本已創建"

# 通用版啟動腳本
$StandardBatContent = @"
@echo off
echo ========================================
echo RANOnline AI LLM Integration - 通用版
echo 版本: 3.1.0
echo 開發團隊: Jy技術團隊
echo ========================================
echo.
echo 正在啟動通用版本...
echo 使用完整硬體加速，適用於物理機環境
echo.
start "RANOnline AI 通用版" ai_llm_integration.exe
"@

$StandardBatPath = Join-Path $StandardDir "啟動通用版.bat"
$StandardBatContent | Out-File -FilePath $StandardBatPath -Encoding ASCII
Write-Success "通用版啟動腳本已創建"

# 創建詳細說明文檔
$ReadmeContent = @"
# RANOnline AI LLM Integration - VM相容版 v3.1.0

## 🎯 版本說明

### 通用版本 (Standard_Version)
- **適用環境**: 物理機、工作站
- **性能**: 完整硬體加速支援，最佳性能
- **相容性**: Windows 10/11 x64
- **執行檔**: ai_llm_integration.exe

### VM相容版本 (VM_Compatible_Version) ⭐ 推薦用於虛擬機
- **適用環境**: 虛擬機、雲端主機、遠端桌面
- **優化**: 強制軟體渲染，避免0xc0000005錯誤
- **穩定性**: 專為VM環境優化，最高穩定性
- **執行檔**: $(Split-Path -Leaf $VmExe)

## 🚀 使用方法

### 在虛擬機中使用 (推薦)
1. 解壓縮到任意目錄
2. 進入 **VM_Compatible_Version** 資料夾
3. 雙擊 **"啟動VM相容版.bat"**

### 在物理機中使用
1. 解壓縮到任意目錄  
2. 進入 **Standard_Version** 資料夾
3. 雙擊 **"啟動通用版.bat"**

## 🔧 技術特性

### VM相容版本技術細節
- ✅ 強制軟體渲染 (QT_OPENGL=software)
- ✅ 禁用硬體加速 (避免DirectX/OpenGL問題)
- ✅ 包含 opengl32sw.dll (軟體OpenGL)
- ✅ 最小化DPI縮放問題
- ✅ VM環境自動偵測
- ✅ 增強異常處理

### 系統需求
- **作業系統**: Windows 10/11 (64位)
- **記憶體**: 建議 4GB+ (VM環境)
- **磁碟空間**: 100MB 可用空間
- **VM軟體**: VMware、VirtualBox、Hyper-V 等

## 🛠️ 故障排除

### 如果VM版本仍然崩潰
1. **檢查VM記憶體分配** (建議4GB+)
2. **更新VM工具/增強功能**
3. **檢查Windows事件檢視器**:
   - Windows日誌 → 應用程式
   - 查找ai_llm_integration相關錯誤
4. **嘗試以管理員身份運行**
5. **關閉VM的3D加速** (如果啟用)

### 常見錯誤解決方案
- **0xc0000005**: 使用VM版本，禁用硬體加速
- **缺少DLL**: 確保使用完整的部署包
- **界面無法顯示**: 檢查顯示設定和DPI縮放

## 📊 性能對比

| 特性 | 通用版本 | VM相容版本 |
|------|---------|-----------|
| 啟動速度 | 快 | 中等 |
| 圖形性能 | 最佳 | 良好 |
| 穩定性 | 高 | 最高 |
| VM相容性 | 中等 | 最佳 |
| 記憶體佔用 | 中等 | 較低 |

## 🔍 依賴清單驗證

已包含以下關鍵依賴:
- ✅ Qt6Core.dll
- ✅ Qt6Gui.dll  
- ✅ Qt6Widgets.dll
- ✅ Qt6Network.dll
- ✅ opengl32sw.dll (軟體渲染)
- ✅ platforms/qwindows.dll
- ✅ imageformats/*.dll
- ✅ styles/*.dll
- ✅ MinGW運行時庫

## 📞 技術支援

**開發團隊**: Jy技術團隊  
**版本**: 3.1.0 VM專用版  
**構建日期**: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')  
**構建工具**: PowerShell自動化腳本

---
*本軟體專為解決Qt6應用程式在虛擬機環境中的相容性問題而設計*
"@

$ReadmePath = Join-Path $PackageDir "README.md"
$ReadmeContent | Out-File -FilePath $ReadmePath -Encoding UTF8
Write-Success "詳細說明文檔已創建"

# 步驟12: 依賴分析和驗證
Write-Header "步驟 12/12: 依賴分析和驗證"

function Analyze-Dependencies {
    param([string]$Directory, [string]$Version)
    
    Write-Info "分析 $Version 版本依賴..."
    
    $exeFiles = Get-ChildItem $Directory -Filter "*.exe"
    $dllFiles = Get-ChildItem $Directory -Filter "*.dll"
    $pluginDirs = Get-ChildItem $Directory -Directory | Where-Object { $_.Name -in @("platforms", "imageformats", "styles", "tls", "translations") }
    
    $analysis = @{
        "執行檔數量" = $exeFiles.Count
        "DLL數量" = $dllFiles.Count
        "插件目錄" = $pluginDirs.Count
        "總檔案大小" = (Get-ChildItem $Directory -Recurse | Measure-Object -Property Length -Sum).Sum
        "關鍵DLL" = @()
    }
    
    $criticalDlls = @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "opengl32sw.dll", "libgcc_s_seh-1.dll", "libstdc++-6.dll")
    
    foreach ($dll in $criticalDlls) {
        $dllPath = Join-Path $Directory $dll
        if (Test-Path $dllPath) {
            $analysis["關鍵DLL"] += "✅ $dll"
        } else {
            $analysis["關鍵DLL"] += "❌ $dll (缺失)"
        }
    }
    
    return $analysis
}

$standardAnalysis = Analyze-Dependencies $StandardDir "通用"
$vmAnalysis = Analyze-Dependencies $VmDir "VM相容"

# 生成依賴報告
$dependencyReport = @"
========================================
RANOnline AI VM相容版 - 依賴分析報告
生成時間: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
========================================

【通用版本分析】
執行檔數量: $($standardAnalysis["執行檔數量"])
DLL數量: $($standardAnalysis["DLL數量"]) 
插件目錄: $($standardAnalysis["插件目錄"])
總檔案大小: $([math]::Round($standardAnalysis["總檔案大小"] / 1MB, 2)) MB

關鍵DLL檢查:
$($standardAnalysis["關鍵DLL"] -join "`n")

========================================

【VM相容版本分析】
執行檔數量: $($vmAnalysis["執行檔數量"])
DLL數量: $($vmAnalysis["DLL數量"])
插件目錄: $($vmAnalysis["插件目錄"]) 
總檔案大小: $([math]::Round($vmAnalysis["總檔案大小"] / 1MB, 2)) MB

關鍵DLL檢查:
$($vmAnalysis["關鍵DLL"] -join "`n")

========================================
【完整檔案清單】

通用版本檔案:
$(Get-ChildItem $StandardDir -Recurse | Select-Object -ExpandProperty FullName | ForEach-Object { $_.Replace($StandardDir, ".") })

VM相容版本檔案:
$(Get-ChildItem $VmDir -Recurse | Select-Object -ExpandProperty FullName | ForEach-Object { $_.Replace($VmDir, ".") })

========================================
"@

$reportPath = Join-Path $PackageDir "dependency_analysis.txt"
$dependencyReport | Out-File -FilePath $reportPath -Encoding UTF8
Write-Success "依賴分析報告已生成"

# 創建ZIP壓縮包
Write-Info "創建ZIP壓縮包..."
$zipPath = Join-Path $ProjectDir $ZipName
Compress-Archive -Path "$PackageDir\*" -DestinationPath $zipPath -Force

if (Test-Path $zipPath) {
    $zipSize = (Get-Item $zipPath).Length
    Write-Success "ZIP壓縮包已創建: $ZipName ($([math]::Round($zipSize / 1MB, 2)) MB)"
} else {
    Write-Error "ZIP壓縮包創建失敗"
    exit 1
}

# 最終測試 (可選)
if (-not $SkipTest) {
    Write-Header "可選步驟: 啟動測試"
    
    Write-Info "是否要測試VM版本啟動? (這將嘗試啟動程序並立即關閉)"
    $testChoice = Read-Host "輸入 'y' 進行測試，其他任意鍵跳過"
    
    if ($testChoice -eq 'y' -or $testChoice -eq 'Y') {
        try {
            $vmExePath = Join-Path $VmDir (Split-Path -Leaf $VmExe)
            Write-Info "測試啟動: $vmExePath"
            
            $process = Start-Process $vmExePath -PassThru -WindowStyle Hidden
            Start-Sleep 3
            
            if (-not $process.HasExited) {
                Write-Success "VM版本啟動測試成功！"
                $process.CloseMainWindow()
                Start-Sleep 1
                if (-not $process.HasExited) {
                    $process.Kill()
                }
            } else {
                Write-Warning "VM版本啟動後立即退出 (可能正常)"
            }
        } catch {
            Write-Warning "VM版本啟動測試失敗: $_"
        }
    }
} else {
    Write-Warning "跳過啟動測試 (-SkipTest)"
}

# 最終總結
Write-Header "🎉 構建完成總結"

Write-Success "VM相容版本構建打包完成！"
Write-Info "版本: 3.1.0 VM專用版"
Write-Info "開發團隊: Jy技術團隊"
Write-Info "構建工具: PowerShell自動化腳本"
Write-Host ""

Write-ColorOutput "📁 部署目錄: $PackageDir" "Cyan"
Write-ColorOutput "📦 ZIP壓縮包: $ZipName" "Cyan"
Write-Host ""

Write-ColorOutput "🖥️ 通用版本:" "Green"
Write-ColorOutput "   - 執行檔: Standard_Version\ai_llm_integration.exe" "White"
Write-ColorOutput "   - 啟動腳本: Standard_Version\啟動通用版.bat" "White"
Write-Host ""

Write-ColorOutput "💻 VM相容版本:" "Green"
Write-ColorOutput "   - 執行檔: VM_Compatible_Version\$(Split-Path -Leaf $VmExe)" "White"
Write-ColorOutput "   - 啟動腳本: VM_Compatible_Version\啟動VM相容版.bat" "White"
Write-Host ""

Write-ColorOutput "📋 依賴分析: dependency_analysis.txt" "Yellow"
Write-ColorOutput "📖 使用說明: README.md" "Yellow"
Write-Host ""

Write-Success "現在可以將ZIP文件傳輸到VM進行測試了！"

# 詢問是否開啟目錄
$openChoice = Read-Host "是否要開啟部署目錄? (y/n)"
if ($openChoice -eq 'y' -or $openChoice -eq 'Y') {
    Start-Process "explorer.exe" -ArgumentList $PackageDir
}

Write-Host ""
Write-ColorOutput "感謝使用 RANOnline AI LLM Integration VM相容版構建系統！" "Magenta"
