@echo off
REM AI LLM Integration - Windows Build and Installation Script
REM RANOnline EP7 AI Simulation System
REM 開發團隊: Jy技術團隊

setlocal enabledelayedexpansion

REM 配置變數
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set INSTALL_PREFIX=%2
if "%INSTALL_PREFIX%"=="" set INSTALL_PREFIX=%USERPROFILE%\RANOnline_LLM

set BUILD_TESTS=%3
if "%BUILD_TESTS%"=="" set BUILD_TESTS=OFF

REM 顏色定義（Windows 10+）
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set BLUE=[94m
set PURPLE=[95m
set CYAN=[96m
set NC=[0m

echo %CYAN%================================================%NC%
echo %CYAN%🤖 RANOnline LLM Integration - Windows Build Script%NC%
echo %CYAN%================================================%NC%
echo %BLUE%📋 Build Type: %BUILD_TYPE%%NC%
echo %BLUE%📁 Install Prefix: %INSTALL_PREFIX%%NC%
echo %BLUE%🧪 Build Tests: %BUILD_TESTS%%NC%
echo %CYAN%================================================%NC%

REM 檢查依賴
echo %YELLOW%🔍 Checking dependencies...%NC%

REM 檢查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ CMake not found. Please install CMake 3.20 or higher.%NC%
    pause
    exit /b 1
)

for /f "tokens=3" %%i in ('cmake --version 2^>^&1 ^| findstr "cmake version"') do set CMAKE_VERSION=%%i
echo %GREEN%✅ CMake found: %CMAKE_VERSION%%NC%

REM 檢查Visual Studio或MinGW
cl >nul 2>&1
if not errorlevel 1 (
    echo %GREEN%✅ Visual Studio compiler found%NC%
    set COMPILER=MSVC
) else (
    g++ --version >nul 2>&1
    if not errorlevel 1 (
        for /f "tokens=3" %%i in ('g++ --version 2^>^&1 ^| findstr "g++"') do set GCC_VERSION=%%i
        echo %GREEN%✅ MinGW GCC found: %GCC_VERSION%%NC%
        set COMPILER=MinGW
    ) else (
        echo %RED%❌ No C++ compiler found. Please install Visual Studio or MinGW.%NC%
        pause
        exit /b 1
    )
)

REM 檢查Qt6
qmake --version >nul 2>&1
if errorlevel 1 (
    echo %RED%❌ Qt6 not found. Please install Qt6 and add to PATH.%NC%
    pause
    exit /b 1
)

for /f "tokens=4" %%i in ('qmake --version 2^>^&1 ^| findstr "Qt version"') do set QT_VERSION=%%i
echo %GREEN%✅ Qt found: %QT_VERSION%%NC%

REM 檢查Ollama服務
echo %YELLOW%🔍 Checking Ollama service...%NC%
curl -s --connect-timeout 3 http://localhost:11434/api/tags >nul 2>&1
if not errorlevel 1 (
    echo %GREEN%✅ Ollama service is running%NC%
    
    REM 嘗試獲取模型列表
    curl -s http://localhost:11434/api/tags > temp_models.json 2>nul
    if exist temp_models.json (
        echo %BLUE%📋 Ollama service detected%NC%
        del temp_models.json
    )
) else (
    echo %YELLOW%⚠️  Ollama service not detected. Please start Ollama service.%NC%
)

REM 創建構建目錄
echo %YELLOW%📁 Creating build directory...%NC%
if not exist build mkdir build
cd build

REM 配置CMake
echo %YELLOW%🔧 Configuring CMake...%NC%

if "%COMPILER%"=="MSVC" (
    cmake .. -G "Visual Studio 17 2022" -A x64 ^
        -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
        -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
        -DBUILD_LLM_TESTS=%BUILD_TESTS% ^
        -DCMAKE_CXX_STANDARD=20
) else (
    cmake .. -G "MinGW Makefiles" ^
        -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
        -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
        -DBUILD_LLM_TESTS=%BUILD_TESTS% ^
        -DCMAKE_CXX_STANDARD=20
)

if errorlevel 1 (
    echo %RED%❌ CMake configuration failed!%NC%
    pause
    exit /b 1
)

echo %GREEN%✅ CMake configuration completed%NC%

REM 構建項目
echo %YELLOW%🔨 Building project...%NC%
cmake --build . --config %BUILD_TYPE% --parallel

if errorlevel 1 (
    echo %RED%❌ Build failed!%NC%
    pause
    exit /b 1
)

echo %GREEN%✅ Build completed successfully%NC%

REM 運行測試（如果啟用）
if "%BUILD_TESTS%"=="ON" (
    echo %YELLOW%🧪 Running tests...%NC%
    ctest --output-on-failure -C %BUILD_TYPE%
    
    if errorlevel 1 (
        echo %RED%❌ Some tests failed!%NC%
        pause
        exit /b 1
    )
    
    echo %GREEN%✅ All tests passed%NC%
)

REM 安裝
echo %YELLOW%📦 Installing...%NC%
cmake --install . --config %BUILD_TYPE%

if errorlevel 1 (
    echo %RED%❌ Installation failed!%NC%
    pause
    exit /b 1
)

echo %GREEN%✅ Installation completed%NC%

REM 創建快捷方式
echo %YELLOW%🔗 Creating shortcuts...%NC%

REM 桌面快捷方式
set DESKTOP=%USERPROFILE%\Desktop
set SHORTCUT_PATH=%DESKTOP%\RANOnline LLM Integration.lnk
set TARGET_PATH=%INSTALL_PREFIX%\bin\RANOnline_LLM_Integration.exe

powershell -Command "& {$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%SHORTCUT_PATH%'); $Shortcut.TargetPath = '%TARGET_PATH%'; $Shortcut.WorkingDirectory = '%INSTALL_PREFIX%\bin'; $Shortcut.Description = 'RANOnline LLM Integration System'; $Shortcut.Save()}"

if exist "%SHORTCUT_PATH%" (
    echo %GREEN%✅ Desktop shortcut created%NC%
) else (
    echo %YELLOW%⚠️  Failed to create desktop shortcut%NC%
)

REM 開始菜單快捷方式
set STARTMENU=%APPDATA%\Microsoft\Windows\Start Menu\Programs
if not exist "%STARTMENU%\RANOnline" mkdir "%STARTMENU%\RANOnline"
set STARTMENU_SHORTCUT=%STARTMENU%\RANOnline\LLM Integration.lnk

powershell -Command "& {$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%STARTMENU_SHORTCUT%'); $Shortcut.TargetPath = '%TARGET_PATH%'; $Shortcut.WorkingDirectory = '%INSTALL_PREFIX%\bin'; $Shortcut.Description = 'RANOnline LLM Integration System'; $Shortcut.Save()}"

if exist "%STARTMENU_SHORTCUT%" (
    echo %GREEN%✅ Start menu shortcut created%NC%
) else (
    echo %YELLOW%⚠️  Failed to create start menu shortcut%NC%
)

REM 註冊Windows服務（可選）
echo %YELLOW%🔧 Registering Windows service...%NC%
sc query "RANOnlineLLM" >nul 2>&1
if not errorlevel 1 (
    echo %BLUE%📋 Service already exists, skipping registration%NC%
) else (
    sc create "RANOnlineLLM" binPath= "\"%TARGET_PATH%\" --service" start= manual DisplayName= "RANOnline LLM Integration Service" >nul 2>&1
    if not errorlevel 1 (
        echo %GREEN%✅ Windows service registered%NC%
        echo %BLUE%💡 To start service: sc start RANOnlineLLM%NC%
    ) else (
        echo %YELLOW%⚠️  Failed to register Windows service (requires admin privileges)%NC%
    )
)

REM 設置防火牆規則（可選）
echo %YELLOW%🛡️  Setting up firewall rules...%NC%
netsh advfirewall firewall show rule name="RANOnline LLM Integration" >nul 2>&1
if not errorlevel 1 (
    echo %BLUE%📋 Firewall rule already exists%NC%
) else (
    netsh advfirewall firewall add rule name="RANOnline LLM Integration" dir=in action=allow program="%TARGET_PATH%" >nul 2>&1
    if not errorlevel 1 (
        echo %GREEN%✅ Firewall rule added%NC%
    ) else (
        echo %YELLOW%⚠️  Failed to add firewall rule (requires admin privileges)%NC%
    )
)

REM 完成信息
echo.
echo %CYAN%================================================%NC%
echo %GREEN%🎉 Installation Complete!%NC%
echo %CYAN%================================================%NC%
echo %BLUE%📁 Installation directory: %INSTALL_PREFIX%%NC%
echo %BLUE%🚀 Executable: %TARGET_PATH%%NC%
echo %BLUE%📋 Configuration: %INSTALL_PREFIX%\share\ai_llm_integration\config\%NC%
echo %BLUE%🎨 Resources: %INSTALL_PREFIX%\share\ai_llm_integration\resources\%NC%

echo.
echo %YELLOW%📝 Next Steps:%NC%
echo %BLUE%1. Launch the application from desktop shortcut%NC%
echo %BLUE%2. Configure your Ollama servers in the settings%NC%
echo %BLUE%3. Customize prompt templates as needed%NC%
echo %BLUE%4. Set up WebSocket connection for game integration%NC%
echo %BLUE%5. Test the application with your LLM models%NC%

echo.
echo %CYAN%================================================%NC%
echo %GREEN%✨ Ready to use RANOnline LLM Integration!%NC%
echo %CYAN%================================================%NC%

pause
