@echo off
REM RANOnline LLM Integration - Windows Demo Launcher
REM 一鍵演示啟動腳本，快速展示系統功能

setlocal enabledelayedexpansion

REM 顏色定義
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set BLUE=[94m
set CYAN=[96m
set NC=[0m

echo %CYAN%================================================%NC%
echo %CYAN%🚀 RANOnline LLM Integration - Demo Launcher%NC%
echo %CYAN%================================================%NC%

REM 檢查 Ollama 服務
echo %YELLOW%🔍 Checking Ollama service...%NC%
curl -s --connect-timeout 3 http://localhost:11434/api/tags >nul 2>&1
if errorlevel 1 (
    echo %YELLOW%⚠️  Ollama service not running. Please start Ollama manually:%NC%
    echo %BLUE%   ollama serve%NC%
    echo.
    echo %YELLOW%Press any key when Ollama is running...%NC%
    pause >nul
) else (
    echo %GREEN%✅ Ollama service is running%NC%
)

REM 檢查可用模型
echo %YELLOW%🤖 Checking available models...%NC%
curl -s http://localhost:11434/api/tags > temp_models.json 2>nul
if exist temp_models.json (
    echo %GREEN%✅ Models available%NC%
    del temp_models.json
) else (
    echo %YELLOW%⚠️  No models detected. Please download demo models:%NC%
    echo %BLUE%   ollama pull phi3:mini%NC%
    echo %BLUE%   ollama pull gemma:2b%NC%
    echo.
    echo %YELLOW%Press any key when models are ready...%NC%
    pause >nul
)

REM 檢查應用程式
set APP_PATH=build\Release\RANOnline_LLM_Integration.exe
if not exist "%APP_PATH%" set APP_PATH=build\RANOnline_LLM_Integration.exe

if not exist "%APP_PATH%" (
    echo %YELLOW%🔨 Application not found. Building...%NC%
    
    if not exist build mkdir build
    cd build
    
    cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_LLM_TESTS=OFF
    if errorlevel 1 (
        echo %RED%❌ CMake configuration failed%NC%
        pause
        exit /b 1
    )
    
    cmake --build . --config Release --parallel
    if errorlevel 1 (
        echo %RED%❌ Build failed%NC%
        pause
        exit /b 1
    )
    
    cd ..
    echo %GREEN%✅ Application built successfully%NC%
)

REM 創建演示配置
echo %YELLOW%⚙️  Creating demo configuration...%NC%

if not exist Config mkdir Config

REM 創建演示伺服器配置
echo {> Config\demo_servers.json
echo   "servers": [>> Config\demo_servers.json
echo     {>> Config\demo_servers.json
echo       "id": "demo_ollama",>> Config\demo_servers.json
echo       "name": "Demo Ollama Server",>> Config\demo_servers.json
echo       "host": "localhost",>> Config\demo_servers.json
echo       "port": 11434,>> Config\demo_servers.json
echo       "enabled": true,>> Config\demo_servers.json
echo       "max_connections": 3,>> Config\demo_servers.json
echo       "timeout_ms": 30000,>> Config\demo_servers.json
echo       "models": ["phi3:mini", "gemma:2b"],>> Config\demo_servers.json
echo       "load_balancer": {>> Config\demo_servers.json
echo         "strategy": "round_robin",>> Config\demo_servers.json
echo         "weight": 100>> Config\demo_servers.json
echo       }>> Config\demo_servers.json
echo     }>> Config\demo_servers.json
echo   ]>> Config\demo_servers.json
echo }>> Config\demo_servers.json

REM 創建演示模板配置
echo {> Config\demo_templates.json
echo   "templates": {>> Config\demo_templates.json
echo     "demo_skill": {>> Config\demo_templates.json
echo       "name": "Demo Skill Generation",>> Config\demo_templates.json
echo       "template": "Create a {academy} skill for level {level} character. Make it balanced and fun!",>> Config\demo_templates.json
echo       "preferred_models": ["phi3:mini"],>> Config\demo_templates.json
echo       "parameters": {>> Config\demo_templates.json
echo         "academy": {>> Config\demo_templates.json
echo           "type": "select",>> Config\demo_templates.json
echo           "options": ["Warrior", "Mage", "Archer"],>> Config\demo_templates.json
echo           "default": "Warrior">> Config\demo_templates.json
echo         },>> Config\demo_templates.json
echo         "level": {>> Config\demo_templates.json
echo           "type": "number",>> Config\demo_templates.json
echo           "min": 1,>> Config\demo_templates.json
echo           "max": 100,>> Config\demo_templates.json
echo           "default": 10>> Config\demo_templates.json
echo         }>> Config\demo_templates.json
echo       }>> Config\demo_templates.json
echo     }>> Config\demo_templates.json
echo   }>> Config\demo_templates.json
echo }>> Config\demo_templates.json

echo %GREEN%✅ Demo configuration created%NC%

REM 啟動應用程式
echo.
echo %CYAN%================================================%NC%
echo %GREEN%🎉 Launching RANOnline LLM Integration Demo%NC%
echo %CYAN%================================================%NC%

echo %BLUE%🎮 Demo Features:%NC%
echo %BLUE%  • Multiple theme switching%NC%
echo %BLUE%  • Real-time AI request processing%NC%
echo %BLUE%  • Academy-based color theming%NC%
echo %BLUE%  • Progress monitoring%NC%
echo %BLUE%  • Batch request processing%NC%
echo %BLUE%  • Performance dashboard%NC%

echo.
echo %YELLOW%💡 Demo Tips:%NC%
echo %YELLOW%  1. Try different themes from the theme selector%NC%
echo %YELLOW%  2. Create requests with different academies%NC%
echo %YELLOW%  3. Test batch processing with multiple prompts%NC%
echo %YELLOW%  4. Monitor performance in the stats panel%NC%
echo %YELLOW%  5. Check the server management tab%NC%

echo.
echo %CYAN%================================================%NC%
echo %GREEN%🚀 Starting Application...%NC%
echo %CYAN%================================================%NC%

REM 設定環境變數
set RANONLINE_DEMO_MODE=1
set RANONLINE_CONFIG_PATH=.\Config

REM 啟動應用程式
"%APP_PATH%" --demo --config-path=".\Config"

echo.
echo %GREEN%✅ Demo session ended%NC%
pause
