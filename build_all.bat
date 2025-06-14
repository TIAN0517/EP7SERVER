@echo off
REM ================================================
REM 🚀 RANOnline EP7 AI System - 完整构建脚本
REM C++ Ultimate Edition - 自动化构建和部署
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
echo %CYAN%🚀 RANOnline EP7 AI System - C++ Ultimate Edition%RESET%
echo %CYAN%================================================%RESET%
echo.

REM 检查构建环境
echo %BLUE%📋 检查构建环境...%RESET%

REM 检查CMake
cmake --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo %RED%❌ CMake 未找到，请安装 CMake 3.20 或更高版本%RESET%
    pause
    exit /b 1
)

REM 检查Visual Studio
where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo %YELLOW%⚠️  Visual Studio 编译器未在PATH中，尝试设置环境...%RESET%
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
        if %ERRORLEVEL% neq 0 (
            echo %RED%❌ Visual Studio 未找到，请安装 Visual Studio 2019/2022%RESET%
            pause
            exit /b 1
        )
    )
)

echo %GREEN%✅ 构建环境检查完成%RESET%
echo.

REM 设置构建目录
set BUILD_DIR=%~dp0Build
set INSTALL_DIR=%~dp0Install
set CONFIG=Release

REM 解析命令行参数
:parse_args
if "%1"=="--debug" (
    set CONFIG=Debug
    shift
    goto parse_args
)
if "%1"=="--clean" (
    set CLEAN_BUILD=1
    shift
    goto parse_args
)
if "%1"=="--help" (
    echo.
    echo 用法: %0 [选项]
    echo.
    echo 选项:
    echo   --debug     构建Debug版本（默认Release）
    echo   --clean     清理构建目录后重新构建
    echo   --help      显示此帮助信息
    echo.
    pause
    exit /b 0
)
if "%1" neq "" (
    shift
    goto parse_args
)

echo %BLUE%🔧 构建配置:%RESET%
echo    构建类型: %CONFIG%
echo    构建目录: %BUILD_DIR%
echo    安装目录: %INSTALL_DIR%
echo.

REM 清理构建目录
if defined CLEAN_BUILD (
    echo %YELLOW%🧹 清理构建目录...%RESET%
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    if exist "%INSTALL_DIR%" rmdir /s /q "%INSTALL_DIR%"
)

REM 创建构建目录
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

REM 进入构建目录
cd /d "%BUILD_DIR%"

echo %BLUE%⚙️ 配置CMake项目...%RESET%

REM CMake配置
cmake .. ^
    -G "Visual Studio 17 2022" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=%CONFIG% ^
    -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
    -DBUILD_TESTS=OFF ^
    -DQt6_DIR="C:/Qt/6.5.0/msvc2022_64/lib/cmake/Qt6"

if %ERRORLEVEL% neq 0 (
    echo %RED%❌ CMake配置失败%RESET%
    pause
    exit /b 1
)

echo %GREEN%✅ CMake配置完成%RESET%
echo.

echo %BLUE%🔨 开始编译项目...%RESET%

REM 编译项目
cmake --build . --config %CONFIG% --parallel 8

if %ERRORLEVEL% neq 0 (
    echo %RED%❌ 编译失败%RESET%
    pause
    exit /b 1
)

echo %GREEN%✅ 编译完成%RESET%
echo.

echo %BLUE%📦 安装应用程序...%RESET%

REM 安装
cmake --install . --config %CONFIG%

if %ERRORLEVEL% neq 0 (
    echo %RED%❌ 安装失败%RESET%
    pause
    exit /b 1
)

echo %GREEN%✅ 安装完成%RESET%
echo.

REM 创建启动脚本
echo %BLUE%📄 创建启动脚本...%RESET%

set LAUNCH_SCRIPT=%INSTALL_DIR%\start_ranonline_ai.bat
echo @echo off > "%LAUNCH_SCRIPT%"
echo echo 🚀 启动 RANOnline AI System... >> "%LAUNCH_SCRIPT%"
echo echo. >> "%LAUNCH_SCRIPT%"
echo cd /d "%%~dp0bin" >> "%LAUNCH_SCRIPT%"
echo echo 🤖 启动 AI 后端引擎... >> "%LAUNCH_SCRIPT%"
echo start "AI Backend" ai_backend_engine.exe >> "%LAUNCH_SCRIPT%"
echo timeout /t 3 /nobreak ^>nul >> "%LAUNCH_SCRIPT%"
echo echo 🖥️ 启动 AI 前端管理界面... >> "%LAUNCH_SCRIPT%"
echo start "AI Frontend" RANOnline_AI_Frontend.exe >> "%LAUNCH_SCRIPT%"
echo echo ✅ RANOnline AI System 已启动 >> "%LAUNCH_SCRIPT%"
echo pause >> "%LAUNCH_SCRIPT%"

REM 复制配置文件
echo %BLUE%📁 复制配置文件...%RESET%
if not exist "%INSTALL_DIR%\config" mkdir "%INSTALL_DIR%\config"
xcopy /y /e "%~dp0Config\*" "%INSTALL_DIR%\config\"

REM 复制资源文件
echo %BLUE%🎨 复制资源文件...%RESET%
if not exist "%INSTALL_DIR%\resources" mkdir "%INSTALL_DIR%\resources"
xcopy /y /e "%~dp0Resources\*" "%INSTALL_DIR%\resources\"

REM 生成构建报告
echo %BLUE%📊 生成构建报告...%RESET%
set REPORT_FILE=%INSTALL_DIR%\build_report.txt

echo ================================================ > "%REPORT_FILE%"
echo RANOnline EP7 AI System - 构建报告 >> "%REPORT_FILE%"
echo ================================================ >> "%REPORT_FILE%"
echo 构建时间: %DATE% %TIME% >> "%REPORT_FILE%"
echo 构建类型: %CONFIG% >> "%REPORT_FILE%"
echo 构建目录: %BUILD_DIR% >> "%REPORT_FILE%"
echo 安装目录: %INSTALL_DIR% >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"
echo 🎯 构建的组件: >> "%REPORT_FILE%"
echo   ✅ Communication Protocol Library >> "%REPORT_FILE%"
echo   ✅ Database Sync Module >> "%REPORT_FILE%"
echo   ✅ AI Backend Engine >> "%REPORT_FILE%"
echo   ✅ AI Frontend Manager >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"
echo 📁 文件结构: >> "%REPORT_FILE%"
dir /s "%INSTALL_DIR%" >> "%REPORT_FILE%"

echo.
echo %GREEN%================================================%RESET%
echo %GREEN%🎉 构建成功完成！%RESET%
echo %GREEN%================================================%RESET%
echo.
echo %CYAN%📁 安装目录: %INSTALL_DIR%%RESET%
echo %CYAN%🚀 启动脚本: %LAUNCH_SCRIPT%%RESET%
echo %CYAN%📊 构建报告: %REPORT_FILE%%RESET%
echo.
echo %YELLOW%💡 提示: 运行 start_ranonline_ai.bat 启动应用程序%RESET%
echo.

REM 询问是否立即启动
set /p START_NOW=是否立即启动应用程序？(Y/N): 
if /i "%START_NOW%"=="Y" (
    echo.
    echo %GREEN%🚀 启动 RANOnline AI System...%RESET%
    call "%LAUNCH_SCRIPT%"
)

echo.
echo %GREEN%构建脚本执行完成！%RESET%
pause
