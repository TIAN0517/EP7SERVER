@echo off
REM ================================================
REM 🗄️ RANOnline AI System - ODBC DSN 自动注册脚本
REM 适用于SQL Server 2022 + Windows Server/Client
REM 开发团队: Jy技术团队
REM 创建日期: 2025年6月14日
REM ================================================

setlocal enabledelayedexpansion

REM 颜色定义
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "CYAN=[96m"
set "RESET=[0m"

echo.
echo %CYAN%================================================%RESET%
echo %CYAN%🗄️ RANOnline AI System - ODBC DSN 配置工具%RESET%
echo %CYAN%================================================%RESET%
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo %RED%❌ 错误: 需要管理员权限运行此脚本%RESET%
    echo %YELLOW%请右键选择"以管理员身份运行"%RESET%
    pause
    exit /b 1
)

echo %GREEN%✅ 管理员权限确认%RESET%

REM 配置参数 - 可根据实际环境修改
set "DSN_NAME=RanOnlineAIDB"
set "SERVER_ADDRESS=192.168.1.100"
set "SERVER_PORT=1433"
set "DATABASE_NAME=RanOnlineAI"
set "USERNAME=sa"
set "PASSWORD=Ss520520@@"
set "DRIVER_NAME=SQL Server Native Client 11.0"
set "DESCRIPTION=RANOnline AI System Database Connection"

REM 解析命令行参数
:parse_args
if "%1"=="--server" (
    set "SERVER_ADDRESS=%2"
    shift
    shift
    goto parse_args
)
if "%1"=="--database" (
    set "DATABASE_NAME=%2"
    shift
    shift
    goto parse_args
)
if "%1"=="--user" (
    set "USERNAME=%2"
    shift
    shift
    goto parse_args
)
if "%1"=="--password" (
    set "PASSWORD=%2"
    shift
    shift
    goto parse_args
)
if "%1"=="--dsn" (
    set "DSN_NAME=%2"
    shift
    shift
    goto parse_args
)
if "%1"=="--help" (
    echo.
    echo 用法: %0 [选项]
    echo.
    echo 选项:
    echo   --server ^<IP^>       SQL Server 服务器地址 (默认: 192.168.1.100)
    echo   --database ^<名称^>   数据库名称 (默认: RanOnlineAI)
    echo   --user ^<用户名^>     SQL Server 用户名 (默认: sa)
    echo   --password ^<密码^>   SQL Server 密码 (默认: Ss520520@@)
    echo   --dsn ^<DSN名称^>     ODBC DSN 名称 (默认: RanOnlineAIDB)
    echo   --help             显示此帮助信息
    echo.
    echo 示例:
    echo   %0 --server 192.168.1.200 --database MyDB --user admin --password MyPass123
    echo.
    pause
    exit /b 0
)
if "%1" neq "" (
    shift
    goto parse_args
)

echo %BLUE%📋 配置信息:%RESET%
echo    DSN 名称: %DSN_NAME%
echo    服务器地址: %SERVER_ADDRESS%:%SERVER_PORT%
echo    数据库名称: %DATABASE_NAME%
echo    用户名: %USERNAME%
echo    驱动程序: %DRIVER_NAME%
echo.

REM 检查ODBC驱动程序
echo %BLUE%🔍 检查ODBC驱动程序...%RESET%

reg query "HKLM\SOFTWARE\ODBC\ODBCINST.INI\%DRIVER_NAME%" >nul 2>&1
if %errorlevel% neq 0 (
    echo %YELLOW%⚠️ 未找到驱动程序: %DRIVER_NAME%%RESET%
    echo %BLUE%📦 尝试检查其他可用的SQL Server驱动程序...%RESET%
    
    REM 检查其他常见驱动程序
    reg query "HKLM\SOFTWARE\ODBC\ODBCINST.INI\ODBC Driver 17 for SQL Server" >nul 2>&1
    if %errorlevel% equ 0 (
        set "DRIVER_NAME=ODBC Driver 17 for SQL Server"
        echo %GREEN%✅ 找到驱动程序: !DRIVER_NAME!%RESET%
    ) else (
        reg query "HKLM\SOFTWARE\ODBC\ODBCINST.INI\ODBC Driver 18 for SQL Server" >nul 2>&1
        if %errorlevel% equ 0 (
            set "DRIVER_NAME=ODBC Driver 18 for SQL Server"
            echo %GREEN%✅ 找到驱动程序: !DRIVER_NAME!%RESET%
        ) else (
            reg query "HKLM\SOFTWARE\ODBC\ODBCINST.INI\SQL Server" >nul 2>&1
            if %errorlevel% equ 0 (
                set "DRIVER_NAME=SQL Server"
                echo %GREEN%✅ 找到驱动程序: !DRIVER_NAME!%RESET%
            ) else (
                echo %RED%❌ 错误: 未找到任何SQL Server ODBC驱动程序%RESET%
                echo %YELLOW%请安装SQL Server Native Client或Microsoft ODBC Driver%RESET%
                echo %YELLOW%下载地址: https://docs.microsoft.com/sql/connect/odbc/download-odbc-driver-for-sql-server%RESET%
                pause
                exit /b 1
            )
        )
    )
) else (
    echo %GREEN%✅ 驱动程序确认: %DRIVER_NAME%%RESET%
)

REM 检查现有DSN
echo %BLUE%🔍 检查现有DSN...%RESET%
reg query "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" >nul 2>&1
if %errorlevel% equ 0 (
    echo %YELLOW%⚠️ DSN已存在: %DSN_NAME%%RESET%
    set /p OVERWRITE=是否覆盖现有DSN? (Y/N): 
    if /i "!OVERWRITE!" neq "Y" (
        echo %YELLOW%操作已取消%RESET%
        pause
        exit /b 0
    )
    echo %BLUE%🗑️ 删除现有DSN...%RESET%
    reg delete "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /f >nul 2>&1
    reg delete "HKLM\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources" /v "%DSN_NAME%" /f >nul 2>&1
)

REM 创建DSN注册表项
echo %BLUE%📝 创建ODBC DSN注册表项...%RESET%

REM 1. 创建DSN主键
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /f >nul 2>&1

REM 2. 设置DSN基本属性
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "Driver" /t REG_SZ /d "C:\Windows\System32\sqlncli11.dll" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "Description" /t REG_SZ /d "%DESCRIPTION%" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "Server" /t REG_SZ /d "%SERVER_ADDRESS%,%SERVER_PORT%" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "Database" /t REG_SZ /d "%DATABASE_NAME%" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "Trusted_Connection" /t REG_SZ /d "No" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "TrustServerCertificate" /t REG_SZ /d "Yes" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "Encrypt" /t REG_SZ /d "Optional" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "MARS_Connection" /t REG_SZ /d "Yes" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "APP" /t REG_SZ /d "RANOnline AI System" /f >nul 2>&1
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /v "WSID" /t REG_SZ /d "%COMPUTERNAME%" /f >nul 2>&1

REM 3. 在数据源列表中注册DSN
reg add "HKLM\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources" /v "%DSN_NAME%" /t REG_SZ /d "%DRIVER_NAME%" /f >nul 2>&1

if %errorlevel% equ 0 (
    echo %GREEN%✅ DSN注册表项创建成功%RESET%
) else (
    echo %RED%❌ DSN注册表项创建失败%RESET%
    pause
    exit /b 1
)

REM 测试数据库连接
echo %BLUE%🔧 测试数据库连接...%RESET%

REM 创建临时VBS脚本进行连接测试
set "TEST_SCRIPT=%TEMP%\test_odbc_connection.vbs"
echo Dim conn > "%TEST_SCRIPT%"
echo Set conn = CreateObject("ADODB.Connection") >> "%TEST_SCRIPT%"
echo On Error Resume Next >> "%TEST_SCRIPT%"
echo conn.Open "DSN=%DSN_NAME%;UID=%USERNAME%;PWD=%PASSWORD%" >> "%TEST_SCRIPT%"
echo If Err.Number = 0 Then >> "%TEST_SCRIPT%"
echo     WScript.Echo "SUCCESS" >> "%TEST_SCRIPT%"
echo     conn.Close >> "%TEST_SCRIPT%"
echo Else >> "%TEST_SCRIPT%"
echo     WScript.Echo "ERROR:" ^& Err.Description >> "%TEST_SCRIPT%"
echo End If >> "%TEST_SCRIPT%"

REM 执行连接测试
for /f "delims=" %%i in ('cscript //nologo "%TEST_SCRIPT%"') do set "TEST_RESULT=%%i"
del "%TEST_SCRIPT%" >nul 2>&1

echo %TEST_RESULT% | findstr "SUCCESS" >nul
if %errorlevel% equ 0 (
    echo %GREEN%✅ 数据库连接测试成功%RESET%
) else (
    echo %RED%❌ 数据库连接测试失败%RESET%
    echo %YELLOW%错误信息: %TEST_RESULT%%RESET%
    echo.
    echo %BLUE%💡 故障排除建议:%RESET%
    echo    1. 检查SQL Server服务是否运行
    echo    2. 确认服务器地址和端口正确
    echo    3. 验证用户名密码
    echo    4. 检查防火墙设置
    echo    5. 确认SQL Server配置允许远程连接
    echo.
    set /p CONTINUE=是否继续(尽管连接测试失败)? (Y/N): 
    if /i "!CONTINUE!" neq "Y" (
        echo %YELLOW%操作已取消%RESET%
        pause
        exit /b 1
    )
)

REM 创建连接配置文件
echo %BLUE%📄 创建连接配置文件...%RESET%

set "CONFIG_DIR=Config"
if not exist "%CONFIG_DIR%" mkdir "%CONFIG_DIR%"

set "CONFIG_FILE=%CONFIG_DIR%\odbc_config.json"
echo { > "%CONFIG_FILE%"
echo   "dsnName": "%DSN_NAME%", >> "%CONFIG_FILE%"
echo   "serverAddress": "%SERVER_ADDRESS%", >> "%CONFIG_FILE%"
echo   "port": %SERVER_PORT%, >> "%CONFIG_FILE%"
echo   "databaseName": "%DATABASE_NAME%", >> "%CONFIG_FILE%"
echo   "username": "%USERNAME%", >> "%CONFIG_FILE%"
echo   "password": "%PASSWORD%", >> "%CONFIG_FILE%"
echo   "driverName": "%DRIVER_NAME%", >> "%CONFIG_FILE%"
echo   "connectionTimeout": 30, >> "%CONFIG_FILE%"
echo   "queryTimeout": 60, >> "%CONFIG_FILE%"
echo   "maxRetries": 3, >> "%CONFIG_FILE%"
echo   "retryInterval": 5000, >> "%CONFIG_FILE%"
echo   "trustServerCertificate": true, >> "%CONFIG_FILE%"
echo   "integratedSecurity": false >> "%CONFIG_FILE%"
echo } >> "%CONFIG_FILE%"

echo %GREEN%✅ 配置文件已创建: %CONFIG_FILE%%RESET%

REM 创建卸载脚本
echo %BLUE%📄 创建卸载脚本...%RESET%
set "UNINSTALL_SCRIPT=uninstall_odbc_dsn.bat"
echo @echo off > "%UNINSTALL_SCRIPT%"
echo REM RANOnline AI System - ODBC DSN 卸载脚本 >> "%UNINSTALL_SCRIPT%"
echo echo 卸载ODBC DSN: %DSN_NAME% >> "%UNINSTALL_SCRIPT%"
echo reg delete "HKLM\SOFTWARE\ODBC\ODBC.INI\%DSN_NAME%" /f ^>nul 2^>^&1 >> "%UNINSTALL_SCRIPT%"
echo reg delete "HKLM\SOFTWARE\ODBC\ODBC.INI\ODBC Data Sources" /v "%DSN_NAME%" /f ^>nul 2^>^&1 >> "%UNINSTALL_SCRIPT%"
echo echo DSN已卸载 >> "%UNINSTALL_SCRIPT%"
echo pause >> "%UNINSTALL_SCRIPT%"

echo %GREEN%✅ 卸载脚本已创建: %UNINSTALL_SCRIPT%%RESET%

REM 显示摘要信息
echo.
echo %GREEN%================================================%RESET%
echo %GREEN%🎉 ODBC DSN 配置完成！%RESET%
echo %GREEN%================================================%RESET%
echo.
echo %CYAN%📋 配置摘要:%RESET%
echo    DSN名称: %DSN_NAME%
echo    服务器: %SERVER_ADDRESS%:%SERVER_PORT%
echo    数据库: %DATABASE_NAME%
echo    驱动程序: %DRIVER_NAME%
echo    配置文件: %CONFIG_FILE%
echo    卸载脚本: %UNINSTALL_SCRIPT%
echo.
echo %CYAN%🔗 连接字符串示例:%RESET%
echo    DSN连接: DSN=%DSN_NAME%;UID=%USERNAME%;PWD=***
echo    直接连接: DRIVER={%DRIVER_NAME%};SERVER=%SERVER_ADDRESS%,%SERVER_PORT%;DATABASE=%DATABASE_NAME%;UID=%USERNAME%;PWD=***
echo.
echo %CYAN%💡 使用说明:%RESET%
echo    1. 在C++/Qt应用程序中使用 QODBC 驱动
echo    2. 使用DSN名称或完整连接字符串连接数据库
echo    3. 运行 %UNINSTALL_SCRIPT% 可卸载DSN
echo.

REM 询问是否测试C++示例
set /p TEST_CPP=是否运行C++连接示例? (Y/N): 
if /i "%TEST_CPP%"=="Y" (
    echo.
    echo %BLUE%🔧 生成C++测试代码...%RESET%
    
    set "CPP_TEST=test_odbc_connection.cpp"
    echo #include ^<QCoreApplication^> > "%CPP_TEST%"
    echo #include ^<QSqlDatabase^> >> "%CPP_TEST%"
    echo #include ^<QSqlQuery^> >> "%CPP_TEST%"
    echo #include ^<QSqlError^> >> "%CPP_TEST%"
    echo #include ^<QDebug^> >> "%CPP_TEST%"
    echo. >> "%CPP_TEST%"
    echo int main(int argc, char *argv[]) >> "%CPP_TEST%"
    echo { >> "%CPP_TEST%"
    echo     QCoreApplication app(argc, argv); >> "%CPP_TEST%"
    echo. >> "%CPP_TEST%"
    echo     QSqlDatabase db = QSqlDatabase::addDatabase("QODBC"); >> "%CPP_TEST%"
    echo     db.setDatabaseName("DSN=%DSN_NAME%;UID=%USERNAME%;PWD=%PASSWORD%"); >> "%CPP_TEST%"
    echo. >> "%CPP_TEST%"
    echo     if (db.open()) { >> "%CPP_TEST%"
    echo         qDebug() ^<^< "Database connected successfully!"; >> "%CPP_TEST%"
    echo         QSqlQuery query("SELECT @@VERSION"); >> "%CPP_TEST%"
    echo         if (query.next()) { >> "%CPP_TEST%"
    echo             qDebug() ^<^< "SQL Server Version:" ^<^< query.value(0).toString(); >> "%CPP_TEST%"
    echo         } >> "%CPP_TEST%"
    echo         db.close(); >> "%CPP_TEST%"
    echo     } else { >> "%CPP_TEST%"
    echo         qDebug() ^<^< "Connection failed:" ^<^< db.lastError().text(); >> "%CPP_TEST%"
    echo     } >> "%CPP_TEST%"
    echo. >> "%CPP_TEST%"
    echo     return 0; >> "%CPP_TEST%"
    echo } >> "%CPP_TEST%"
    
    echo %GREEN%✅ C++测试代码已生成: %CPP_TEST%%RESET%
    echo %YELLOW%使用qmake和make编译此文件来测试连接%RESET%
)

echo.
echo %GREEN%配置脚本执行完成！%RESET%
pause
