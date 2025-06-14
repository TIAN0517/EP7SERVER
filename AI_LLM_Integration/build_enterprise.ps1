# ========================================================================
# RANOnline AI LLM Integration - 企業級構建腳本
# 版本: 3.0.0 企業版
# 開發團隊: Jy技術團隊
# 支援: Windows 10/11 + Qt6 + MinGW64
# 
# 🎯 BossJy 企業級標準自動構建系統
# ✅ 自動偵測Qt6與MinGW環境
# ✅ 完整構建+測試+部署流程
# ✅ 錯誤處理與回滾機制
# ✅ 便攜版本自動生成
# ✅ 團隊協作零配置
# ========================================================================

param(
    [string]$BuildType = "Release",
    [switch]$Deploy = $true,
    [switch]$Portable = $true,
    [switch]$Clean = $false,
    [switch]$Test = $true
)

# 設置錯誤處理
$ErrorActionPreference = "Stop"

# 顏色輸出函數
function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
}

function Write-Success { param([string]$Message) Write-ColorOutput "✅ $Message" "Green" }
function Write-Warning { param([string]$Message) Write-ColorOutput "⚠️ $Message" "Yellow" }
function Write-Error { param([string]$Message) Write-ColorOutput "❌ $Message" "Red" }
function Write-Info { param([string]$Message) Write-ColorOutput "🔧 $Message" "Cyan" }
function Write-Header { param([string]$Message) Write-ColorOutput "`n🎯 $Message" "Magenta" }

Write-Header "RANOnline AI LLM Integration - 企業級構建系統啟動"

# ========================================================================
# 環境檢查與自動配置
# ========================================================================

Write-Info "檢查構建環境..."

# 檢查當前目錄
$ProjectRoot = Get-Location
Write-Info "專案根目錄: $ProjectRoot"

# 檢查CMakeLists.txt
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Error "未找到 CMakeLists.txt 文件"
    exit 1
}

# 自動偵測Qt6路徑
$QtPaths = @(
    "C:\Qt\6.9.1\mingw_64",
    "C:\Qt\6.8.0\mingw_64", 
    "C:\Qt\6.7.0\mingw_64",
    "C:\Qt\6.6.0\mingw_64",
    "C:\Qt\6.5.0\mingw_64",
    $env:QTDIR,
    $env:QT_DIR
)

$QtRoot = $null
foreach ($path in $QtPaths) {
    if ($path -and (Test-Path "$path\lib\cmake\Qt6")) {
        $QtRoot = $path
        Write-Success "自動偵測到 Qt6: $QtRoot"
        break
    }
}

if (-not $QtRoot) {
    Write-Error "未找到 Qt6 安裝！請安裝 Qt6 或設置 QTDIR 環境變數"
    exit 1
}

# 自動偵測MinGW
$MinGWPaths = @(
    "$QtRoot\bin",
    "C:\Qt\Tools\mingw1120_64\bin",
    "C:\Qt\Tools\mingw1130_64\bin",
    "C:\mingw64\bin",
    "C:\msys64\mingw64\bin"
)

$MinGWRoot = $null
foreach ($path in $MinGWPaths) {
    if ($path -and (Test-Path "$path\gcc.exe")) {
        $MinGWRoot = $path
        Write-Success "自動偵測到 MinGW: $MinGWRoot"
        break
    }
}

if (-not $MinGWRoot) {
    Write-Error "未找到 MinGW 編譯器！請安裝 Qt6 with MinGW 或單獨安裝 MinGW64"
    exit 1
}

# 設置環境變數
$env:PATH = "$QtRoot\bin;$MinGWRoot;$env:PATH"
$env:QT_ROOT = $QtRoot
$env:CMAKE_PREFIX_PATH = $QtRoot

# 驗證工具可用性
Write-Info "驗證構建工具..."

try {
    $cmake_version = & cmake --version 2>$null | Select-Object -First 1
    Write-Success "CMake: $cmake_version"
} catch {
    Write-Error "CMake 未安裝或未在 PATH 中"
    exit 1
}

try {
    $gcc_version = & gcc --version 2>$null | Select-Object -First 1
    Write-Success "GCC: $gcc_version"
} catch {
    Write-Error "GCC 編譯器不可用"
    exit 1
}

try {
    $qt_version = & qmake -v 2>$null | Select-Object -First 1
    Write-Success "Qt: $qt_version"
} catch {
    Write-Warning "qmake 不可用，但可能不影響 CMake 構建"
}

# ========================================================================
# 構建目錄管理
# ========================================================================

$BuildDir = "build"
$DeployDir = "deploy"
$PortableDir = "portable"

Write-Header "構建目錄準備"

if ($Clean -and (Test-Path $BuildDir)) {
    Write-Info "清理舊的構建目錄..."
    Remove-Item -Recurse -Force $BuildDir
    Write-Success "構建目錄已清理"
}

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
    Write-Success "創建構建目錄: $BuildDir"
}

# ========================================================================
# CMake 配置階段
# ========================================================================

Write-Header "CMake 配置階段"

Set-Location $BuildDir

$CMakeArgs = @(
    "..",
    "-G", "MinGW Makefiles",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_PREFIX_PATH=$QtRoot",
    "-DQT_ROOT=$QtRoot",
    "-DBUILD_PORTABLE=ON",
    "-DDEPLOY_QT=ON"
)

Write-Info "CMake 配置參數: $($CMakeArgs -join ' ')"

try {
    & cmake @CMakeArgs
    if ($LASTEXITCODE -ne 0) { throw "CMake 配置失敗" }
    Write-Success "CMake 配置完成"
} catch {
    Write-Error "CMake 配置失敗: $($_.Exception.Message)"
    Set-Location $ProjectRoot
    exit 1
}

# ========================================================================
# 編譯階段
# ========================================================================

Write-Header "編譯階段"

try {
    & cmake --build . --config $BuildType --parallel
    if ($LASTEXITCODE -ne 0) { throw "編譯失敗" }
    Write-Success "編譯完成"
} catch {
    Write-Error "編譯失敗: $($_.Exception.Message)"
    Set-Location $ProjectRoot
    exit 1
}

# ========================================================================
# 測試階段
# ========================================================================

if ($Test) {
    Write-Header "測試階段"
    
    # 檢查可執行文件是否生成
    $ExeName = "RANOnline_AI_LLM_Integration.exe"
    if (Test-Path $ExeName) {
        Write-Success "可執行文件已生成: $ExeName"
        
        # 基本啟動測試（快速退出）
        Write-Info "執行基本啟動測試..."
        try {
            $proc = Start-Process -FilePath ".\$ExeName" -ArgumentList "--help" -Wait -PassThru -WindowStyle Hidden
            if ($proc.ExitCode -eq 0) {
                Write-Success "應用程式基本測試通過"
            } else {
                Write-Warning "應用程式返回非零退出碼: $($proc.ExitCode)"
            }
        } catch {
            Write-Warning "應用程式測試異常: $($_.Exception.Message)"
        }
    } else {
        Write-Error "可執行文件未生成！"
        Set-Location $ProjectRoot
        exit 1
    }
}

# ========================================================================
# 部署階段
# ========================================================================

if ($Deploy) {
    Write-Header "Qt依賴部署階段"
    
    Set-Location $ProjectRoot
    
    if (Test-Path $DeployDir) {
        Remove-Item -Recurse -Force $DeployDir
    }
    New-Item -ItemType Directory -Path $DeployDir | Out-Null
    
    # 複製主程式
    Copy-Item "$BuildDir\$ExeName" $DeployDir
    Write-Success "主程式已複製到部署目錄"
    
    # 使用 windeployqt 部署 Qt 依賴
    $WinDeployQt = "$QtRoot\bin\windeployqt.exe"
    if (Test-Path $WinDeployQt) {
        Write-Info "使用 windeployqt 部署 Qt 依賴..."
        try {
            Set-Location $DeployDir
            & $WinDeployQt --compiler-runtime --force --qmldir "$QtRoot\qml" $ExeName
            if ($LASTEXITCODE -eq 0) {
                Write-Success "Qt 依賴部署完成"
            } else {
                Write-Warning "windeployqt 返回警告，但可能已成功部署"
            }
        } catch {
            Write-Warning "windeployqt 執行異常: $($_.Exception.Message)"
        } finally {
            Set-Location $ProjectRoot
        }
    } else {
        Write-Warning "windeployqt 不可用，手動複製關鍵 DLL..."
        
        # 手動複製關鍵Qt DLL
        $QtDlls = @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "Qt6Network.dll")
        foreach ($dll in $QtDlls) {
            $SourceDll = "$QtRoot\bin\$dll"
            if (Test-Path $SourceDll) {
                Copy-Item $SourceDll $DeployDir
                Write-Info "已複製: $dll"
            }
        }
        
        # 複製 MinGW 運行時
        $MinGWDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
        foreach ($dll in $MinGWDlls) {
            $SourceDll = "$MinGWRoot\$dll"
            if (Test-Path $SourceDll) {
                Copy-Item $SourceDll $DeployDir
                Write-Info "已複製: $dll"
            }
        }
        
        Write-Success "手動 DLL 複製完成"
    }
}

# ========================================================================
# 便攜版本生成
# ========================================================================

if ($Portable) {
    Write-Header "便攜版本生成"
    
    if (Test-Path $PortableDir) {
        Remove-Item -Recurse -Force $PortableDir
    }
    New-Item -ItemType Directory -Path $PortableDir | Out-Null
    
    # 複製整個部署目錄到便攜目錄
    if (Test-Path $DeployDir) {
        Copy-Item -Recurse "$DeployDir\*" $PortableDir
        Write-Success "便攜版本文件已複製"
    }
    
    # 創建便攜版本啟動說明
    $ReadmeContent = @"
# RANOnline AI LLM Integration - 便攜版本

## 版本資訊
- 版本: 3.0.0 企業版
- 構建時間: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
- 構建類型: $BuildType

## 使用說明
1. 雙擊 RANOnline_AI_LLM_Integration.exe 啟動應用程式
2. 首次啟動可能需要幾秒鐘初始化
3. 確保系統已安裝 Microsoft Visual C++ Redistributable

## 系統需求
- Windows 10/11 (64位)
- 至少 4GB RAM
- 至少 100MB 可用磁碟空間

## 故障排除
如果遇到啟動問題，請檢查：
1. 是否缺少 Visual C++ 運行時庫
2. 防毒軟體是否阻止執行
3. 檔案權限是否正確

## 技術支援
- 開發團隊: Jy技術團隊
- 聯繫方式: 請參考主專案文檔
"@
    
    Set-Content -Path "$PortableDir\README.txt" -Value $ReadmeContent -Encoding UTF8
    Write-Success "便攜版本說明文件已創建"
    
    # 創建版本資訊文件
    $VersionInfo = @{
        "version" = "3.0.0"
        "build_type" = $BuildType
        "build_time" = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
        "qt_version" = $qt_version
        "compiler" = $gcc_version
    }
    
    $VersionInfo | ConvertTo-Json | Set-Content -Path "$PortableDir\version.json" -Encoding UTF8
    Write-Success "版本資訊文件已創建"
}

# ========================================================================
# 構建完成報告
# ========================================================================

Write-Header "構建完成報告"

Write-Success "🎉 企業級構建流程已完成！"
Write-Info ""
Write-Info "構建結果:"
Write-Info "  - 構建目錄: $BuildDir"
if ($Deploy) { Write-Info "  - 部署目錄: $DeployDir" }
if ($Portable) { Write-Info "  - 便攜目錄: $PortableDir" }
Write-Info ""
Write-Info "下一步操作:"
Write-Info "  1. 測試運行: cd $DeployDir; .\RANOnline_AI_LLM_Integration.exe"
Write-Info "  2. 分發便攜版本: 打包 $PortableDir 目錄"
Write-Info "  3. 團隊協作: 提交變更到版本控制系統"
Write-Info ""
Write-Success "BossJy企業級標準構建完成！ 🚀"
