# ================================================
# 🗄️ RANOnline AI System - ODBC DSN PowerShell 配置脚本
# 适用于SQL Server 2022 + Windows Server/Client
# 开发团队: Jy技术团队
# 创建日期: 2025年6月14日
# ================================================

param(
    [string]$ServerAddress = "192.168.1.100",
    [string]$DatabaseName = "RanOnlineAI", 
    [string]$Username = "sa",
    [string]$Password = "Ss520520@@",
    [string]$DSNName = "RanOnlineAIDB",
    [int]$Port = 1433,
    [switch]$Help,
    [switch]$TestOnly,
    [switch]$Remove
)

# 颜色函数
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$ForegroundColor = "White"
    )
    Write-Host $Message -ForegroundColor $ForegroundColor
}

function Write-Success { param([string]$Message) Write-ColorOutput "✅ $Message" "Green" }
function Write-Error { param([string]$Message) Write-ColorOutput "❌ $Message" "Red" }
function Write-Warning { param([string]$Message) Write-ColorOutput "⚠️ $Message" "Yellow" }
function Write-Info { param([string]$Message) Write-ColorOutput "ℹ️ $Message" "Cyan" }

# 显示帮助信息
if ($Help) {
    Write-Host @"

RANOnline AI System - ODBC DSN PowerShell 配置工具

用法: .\setup_odbc_dsn.ps1 [参数]

参数:
  -ServerAddress <IP>     SQL Server 服务器地址 (默认: 192.168.1.100)
  -DatabaseName <名称>    数据库名称 (默认: RanOnlineAI)
  -Username <用户名>      SQL Server 用户名 (默认: sa)
  -Password <密码>        SQL Server 密码 (默认: Ss520520@@)
  -DSNName <DSN名称>      ODBC DSN 名称 (默认: RanOnlineAIDB)
  -Port <端口>           SQL Server 端口 (默认: 1433)
  -TestOnly              仅测试连接，不创建DSN
  -Remove                移除指定的DSN
  -Help                  显示此帮助信息

示例:
  .\setup_odbc_dsn.ps1 -ServerAddress "192.168.1.200" -DatabaseName "MyDB"
  .\setup_odbc_dsn.ps1 -TestOnly
  .\setup_odbc_dsn.ps1 -Remove -DSNName "RanOnlineAIDB"

"@ -ForegroundColor Yellow
    exit 0
}

Write-Info "================================================"
Write-Info "🗄️ RANOnline AI System - ODBC DSN 配置工具"
Write-Info "================================================"

# 检查管理员权限
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Error "需要管理员权限运行此脚本"
    Write-Warning "请右键选择'以管理员身份运行 PowerShell'"
    Read-Host "按任意键退出"
    exit 1
}

Write-Success "管理员权限确认"

# 显示配置信息
Write-Info "📋 配置信息:"
Write-Host "   DSN名称: $DSNName" -ForegroundColor White
Write-Host "   服务器地址: $ServerAddress`:$Port" -ForegroundColor White
Write-Host "   数据库名称: $DatabaseName" -ForegroundColor White
Write-Host "   用户名: $Username" -ForegroundColor White
Write-Host ""

# 如果是移除操作
if ($Remove) {
    Write-Info "🗑️ 移除ODBC DSN: $DSNName"
    
    try {
        Remove-ItemProperty -Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources" -Name $DSNName -ErrorAction SilentlyContinue
        Remove-Item -Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\$DSNName" -Recurse -Force -ErrorAction SilentlyContinue
        Write-Success "DSN已成功移除: $DSNName"
    }
    catch {
        Write-Warning "移除DSN时出现错误: $($_.Exception.Message)"
    }
    
    Read-Host "按任意键退出"
    exit 0
}

# 检查可用的ODBC驱动程序
Write-Info "🔍 检查ODBC驱动程序..."

$availableDrivers = @()
$driverPaths = @(
    "HKLM:\SOFTWARE\ODBC\ODBCINST.INI\ODBC Driver 18 for SQL Server",
    "HKLM:\SOFTWARE\ODBC\ODBCINST.INI\ODBC Driver 17 for SQL Server", 
    "HKLM:\SOFTWARE\ODBC\ODBCINST.INI\SQL Server Native Client 11.0",
    "HKLM:\SOFTWARE\ODBC\ODBCINST.INI\SQL Server"
)

$driverNames = @(
    "ODBC Driver 18 for SQL Server",
    "ODBC Driver 17 for SQL Server",
    "SQL Server Native Client 11.0", 
    "SQL Server"
)

$selectedDriver = $null
for ($i = 0; $i -lt $driverPaths.Length; $i++) {
    if (Test-Path $driverPaths[$i]) {
        $selectedDriver = $driverNames[$i]
        Write-Success "找到驱动程序: $selectedDriver"
        break
    }
}

if (-not $selectedDriver) {
    Write-Error "未找到任何SQL Server ODBC驱动程序"
    Write-Warning "请安装Microsoft ODBC Driver for SQL Server"
    Write-Warning "下载地址: https://docs.microsoft.com/sql/connect/odbc/download-odbc-driver-for-sql-server"
    Read-Host "按任意键退出"
    exit 1
}

# 构建连接字符串
$connectionString = "DRIVER={$selectedDriver};SERVER=$ServerAddress,$Port;DATABASE=$DatabaseName;UID=$Username;PWD=$Password;TrustServerCertificate=Yes;Encrypt=Optional"

# 测试数据库连接
function Test-DatabaseConnection {
    param([string]$ConnectionString)
    
    Write-Info "🔧 测试数据库连接..."
    
    try {
        $connection = New-Object System.Data.Odbc.OdbcConnection($ConnectionString)
        $connection.Open()
        
        $command = $connection.CreateCommand()
        $command.CommandText = "SELECT @@VERSION"
        $version = $command.ExecuteScalar()
        
        $connection.Close()
        
        Write-Success "数据库连接测试成功"
        Write-Host "   SQL Server版本: $($version.ToString().Split("`n")[0])" -ForegroundColor Gray
        return $true
    }
    catch {
        Write-Error "数据库连接测试失败"
        Write-Host "   错误信息: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

# 如果仅测试连接
if ($TestOnly) {
    $testResult = Test-DatabaseConnection -ConnectionString $connectionString
    if ($testResult) {
        Write-Success "连接测试完成 - 连接正常"
    } else {
        Write-Error "连接测试完成 - 连接失败"
        Write-Info "💡 故障排除建议:"
        Write-Host "   1. 检查SQL Server服务是否运行" -ForegroundColor Yellow
        Write-Host "   2. 确认服务器地址和端口正确" -ForegroundColor Yellow  
        Write-Host "   3. 验证用户名密码" -ForegroundColor Yellow
        Write-Host "   4. 检查防火墙设置" -ForegroundColor Yellow
        Write-Host "   5. 确认SQL Server配置允许远程连接" -ForegroundColor Yellow
    }
    Read-Host "按任意键退出"
    exit 0
}

# 检查现有DSN
Write-Info "🔍 检查现有DSN..."
$dsnExists = Test-Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\$DSNName"

if ($dsnExists) {
    Write-Warning "DSN已存在: $DSNName"
    $overwrite = Read-Host "是否覆盖现有DSN? (Y/N)"
    if ($overwrite -ne "Y" -and $overwrite -ne "y") {
        Write-Warning "操作已取消"
        Read-Host "按任意键退出"
        exit 0
    }
    
    Write-Info "🗑️ 删除现有DSN..."
    try {
        Remove-ItemProperty -Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources" -Name $DSNName -ErrorAction SilentlyContinue
        Remove-Item -Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\$DSNName" -Recurse -Force -ErrorAction SilentlyContinue
    }
    catch {
        Write-Warning "删除现有DSN时出现错误: $($_.Exception.Message)"
    }
}

# 测试连接
$connectionTest = Test-DatabaseConnection -ConnectionString $connectionString
if (-not $connectionTest) {
    Write-Error "数据库连接测试失败"
    $continue = Read-Host "是否继续创建DSN(尽管连接测试失败)? (Y/N)"
    if ($continue -ne "Y" -and $continue -ne "y") {
        Write-Warning "操作已取消"
        Read-Host "按任意键退出"
        exit 1
    }
}

# 创建DSN注册表项
Write-Info "📝 创建ODBC DSN注册表项..."

try {
    # 创建DSN主键
    $dsnPath = "HKLM:\SOFTWARE\ODBC\ODBC.INI\$DSNName"
    New-Item -Path $dsnPath -Force | Out-Null
    
    # 设置DSN属性
    $dsnProperties = @{
        "Driver" = "C:\Windows\System32\sqlncli11.dll"
        "Description" = "RANOnline AI System Database Connection"
        "Server" = "$ServerAddress,$Port"
        "Database" = $DatabaseName
        "Trusted_Connection" = "No"
        "TrustServerCertificate" = "Yes"
        "Encrypt" = "Optional"
        "MARS_Connection" = "Yes"
        "APP" = "RANOnline AI System"
        "WSID" = $env:COMPUTERNAME
    }
    
    foreach ($property in $dsnProperties.GetEnumerator()) {
        New-ItemProperty -Path $dsnPath -Name $property.Key -Value $property.Value -PropertyType String -Force | Out-Null
    }
    
    # 在数据源列表中注册DSN
    $dataSourcesPath = "HKLM:\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources"
    New-ItemProperty -Path $dataSourcesPath -Name $DSNName -Value $selectedDriver -PropertyType String -Force | Out-Null
    
    Write-Success "DSN注册表项创建成功"
}
catch {
    Write-Error "DSN注册表项创建失败: $($_.Exception.Message)"
    Read-Host "按任意键退出"
    exit 1
}

# 验证DSN创建
Write-Info "🔍 验证DSN创建..."
$dsnTestString = "DSN=$DSNName;UID=$Username;PWD=$Password"

try {
    $testConnection = New-Object System.Data.Odbc.OdbcConnection($dsnTestString)
    $testConnection.Open()
    $testConnection.Close()
    Write-Success "DSN验证成功"
}
catch {
    Write-Warning "DSN验证失败: $($_.Exception.Message)"
}

# 创建配置文件
Write-Info "📄 创建连接配置文件..."

$configDir = "Config"
if (-not (Test-Path $configDir)) {
    New-Item -ItemType Directory -Path $configDir -Force | Out-Null
}

$configFile = Join-Path $configDir "odbc_config.json"
$config = @{
    dsnName = $DSNName
    serverAddress = $ServerAddress
    port = $Port
    databaseName = $DatabaseName
    username = $Username
    password = $Password
    driverName = $selectedDriver
    connectionTimeout = 30
    queryTimeout = 60
    maxRetries = 3
    retryInterval = 5000
    trustServerCertificate = $true
    integratedSecurity = $false
} | ConvertTo-Json -Depth 10

$config | Out-File -FilePath $configFile -Encoding UTF8
Write-Success "配置文件已创建: $configFile"

# 创建卸载脚本
Write-Info "📄 创建卸载脚本..."
$uninstallScript = "Remove-ODBC-DSN.ps1"

$uninstallContent = @"
# RANOnline AI System - ODBC DSN 卸载脚本
param([string]`$DSNName = "$DSNName")

Write-Host "移除ODBC DSN: `$DSNName" -ForegroundColor Yellow

try {
    Remove-ItemProperty -Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources" -Name `$DSNName -ErrorAction SilentlyContinue
    Remove-Item -Path "HKLM:\SOFTWARE\ODBC\ODBC.INI\`$DSNName" -Recurse -Force -ErrorAction SilentlyContinue
    Write-Host "✅ DSN已成功移除" -ForegroundColor Green
}
catch {
    Write-Host "❌ 移除DSN时出现错误: `$(`$_.Exception.Message)" -ForegroundColor Red
}

Read-Host "按任意键退出"
"@

$uninstallContent | Out-File -FilePath $uninstallScript -Encoding UTF8
Write-Success "卸载脚本已创建: $uninstallScript"

# 生成C++测试代码
Write-Info "📄 生成C++测试代码..."
$cppTestFile = "test_odbc_connection.cpp"

$cppTestContent = @"
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // 方法1: 使用DSN连接
    QSqlDatabase db1 = QSqlDatabase::addDatabase("QODBC", "dsn_connection");
    db1.setDatabaseName("DSN=$DSNName;UID=$Username;PWD=$Password");
    
    if (db1.open()) {
        qDebug() << "✅ DSN连接成功!";
        QSqlQuery query1(db1);
        if (query1.exec("SELECT @@VERSION")) {
            if (query1.next()) {
                qDebug() << "SQL Server版本:" << query1.value(0).toString();
            }
        }
        db1.close();
    } else {
        qDebug() << "❌ DSN连接失败:" << db1.lastError().text();
    }

    // 方法2: 使用完整连接字符串
    QSqlDatabase db2 = QSqlDatabase::addDatabase("QODBC", "direct_connection");
    QString connectionString = "DRIVER={$selectedDriver};"
                             "SERVER=$ServerAddress,$Port;"
                             "DATABASE=$DatabaseName;"
                             "UID=$Username;"
                             "PWD=$Password;"
                             "TrustServerCertificate=Yes;"
                             "Encrypt=Optional";
    db2.setDatabaseName(connectionString);
    
    if (db2.open()) {
        qDebug() << "✅ 直接连接成功!";
        QSqlQuery query2(db2);
        if (query2.exec("SELECT GETDATE() AS CurrentTime")) {
            if (query2.next()) {
                qDebug() << "服务器时间:" << query2.value(0).toString();
            }
        }
        db2.close();
    } else {
        qDebug() << "❌ 直接连接失败:" << db2.lastError().text();
    }

    return 0;
}
"@

$cppTestContent | Out-File -FilePath $cppTestFile -Encoding UTF8
Write-Success "C++测试代码已生成: $cppTestFile"

# 显示摘要信息
Write-Success "================================================"
Write-Success "🎉 ODBC DSN 配置完成！"
Write-Success "================================================"

Write-Host ""
Write-Info "📋 配置摘要:"
Write-Host "   DSN名称: $DSNName" -ForegroundColor White
Write-Host "   服务器: $ServerAddress`:$Port" -ForegroundColor White
Write-Host "   数据库: $DatabaseName" -ForegroundColor White
Write-Host "   驱动程序: $selectedDriver" -ForegroundColor White
Write-Host "   配置文件: $configFile" -ForegroundColor White
Write-Host "   卸载脚本: $uninstallScript" -ForegroundColor White
Write-Host ""

Write-Info "🔗 连接字符串示例:"
Write-Host "   DSN连接: DSN=$DSNName;UID=$Username;PWD=***" -ForegroundColor Gray
Write-Host "   直接连接: DRIVER={$selectedDriver};SERVER=$ServerAddress,$Port;DATABASE=$DatabaseName;UID=$Username;PWD=***" -ForegroundColor Gray
Write-Host ""

Write-Info "💡 使用说明:"
Write-Host "   1. 在C++/Qt应用程序中使用 QODBC 驱动" -ForegroundColor Yellow
Write-Host "   2. 使用DSN名称或完整连接字符串连接数据库" -ForegroundColor Yellow
Write-Host "   3. 运行 .\$uninstallScript 可卸载DSN" -ForegroundColor Yellow
Write-Host ""

Write-Success "配置脚本执行完成！"
Read-Host "按任意键退出"
