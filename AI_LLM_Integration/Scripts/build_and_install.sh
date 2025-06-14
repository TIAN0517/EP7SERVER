#!/bin/bash
# AI LLM Integration - Build and Installation Script
# RANOnline EP7 AI Simulation System
# 開發團隊: Jy技術團隊

set -e

# 顏色定義
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 配置變數
BUILD_TYPE=${1:-Release}
INSTALL_PREFIX=${2:-$HOME/RANOnline_LLM}
BUILD_TESTS=${3:-OFF}
NUM_CORES=$(nproc 2>/dev/null || echo 4)

echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}🤖 RANOnline LLM Integration - Build Script${NC}"
echo -e "${CYAN}================================================${NC}"
echo -e "${BLUE}📋 Build Type: ${BUILD_TYPE}${NC}"
echo -e "${BLUE}📁 Install Prefix: ${INSTALL_PREFIX}${NC}"
echo -e "${BLUE}🧪 Build Tests: ${BUILD_TESTS}${NC}"
echo -e "${BLUE}🔧 CPU Cores: ${NUM_CORES}${NC}"
echo -e "${CYAN}================================================${NC}"

# 檢查依賴
echo -e "${YELLOW}🔍 Checking dependencies...${NC}"

# 檢查CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}❌ CMake not found. Please install CMake 3.20 or higher.${NC}"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | grep -oP 'cmake version \K[0-9.]+')
echo -e "${GREEN}✅ CMake found: ${CMAKE_VERSION}${NC}"

# 檢查Qt6
if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo -e "${RED}❌ Qt6 not found. Please install Qt6 development packages.${NC}"
    exit 1
fi

if command -v qmake6 &> /dev/null; then
    QT_VERSION=$(qmake6 -query QT_VERSION)
else
    QT_VERSION=$(qmake -query QT_VERSION)
fi
echo -e "${GREEN}✅ Qt found: ${QT_VERSION}${NC}"

# 檢查編譯器
if command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | head -n1 | grep -oP 'g\+\+ \K[0-9.]+')
    echo -e "${GREEN}✅ GCC found: ${GCC_VERSION}${NC}"
elif command -v clang++ &> /dev/null; then
    CLANG_VERSION=$(clang++ --version | head -n1 | grep -oP 'clang version \K[0-9.]+')
    echo -e "${GREEN}✅ Clang found: ${CLANG_VERSION}${NC}"
else
    echo -e "${RED}❌ No C++ compiler found. Please install GCC or Clang.${NC}"
    exit 1
fi

# 檢查Ollama服務
echo -e "${YELLOW}🔍 Checking Ollama service...${NC}"
if curl -s --connect-timeout 3 http://localhost:11434/api/tags > /dev/null 2>&1; then
    echo -e "${GREEN}✅ Ollama service is running${NC}"
    
    # 獲取可用模型
    MODELS=$(curl -s http://localhost:11434/api/tags | jq -r '.models[].name' 2>/dev/null || echo "Unable to fetch models")
    echo -e "${BLUE}📋 Available models:${NC}"
    echo "$MODELS" | while read -r model; do
        if [ -n "$model" ]; then
            echo -e "${BLUE}   - ${model}${NC}"
        fi
    done
else
    echo -e "${YELLOW}⚠️  Ollama service not detected. Please start Ollama service.${NC}"
fi

# 創建構建目錄
echo -e "${YELLOW}📁 Creating build directory...${NC}"
mkdir -p build
cd build

# 配置CMake
echo -e "${YELLOW}🔧 Configuring CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
    -DBUILD_LLM_TESTS=${BUILD_TESTS} \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ CMake configuration failed!${NC}"
    exit 1
fi

echo -e "${GREEN}✅ CMake configuration completed${NC}"

# 構建項目
echo -e "${YELLOW}🔨 Building project...${NC}"
cmake --build . --config ${BUILD_TYPE} -j ${NUM_CORES}

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Build completed successfully${NC}"

# 運行測試（如果啟用）
if [ "${BUILD_TESTS}" = "ON" ]; then
    echo -e "${YELLOW}🧪 Running tests...${NC}"
    ctest --output-on-failure -C ${BUILD_TYPE}
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Some tests failed!${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✅ All tests passed${NC}"
fi

# 安裝
echo -e "${YELLOW}📦 Installing...${NC}"
cmake --install . --config ${BUILD_TYPE}

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Installation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Installation completed${NC}"

# 創建快捷方式
echo -e "${YELLOW}🔗 Creating desktop entry...${NC}"
DESKTOP_FILE="$HOME/.local/share/applications/ranonline-llm-integration.desktop"
mkdir -p "$(dirname "$DESKTOP_FILE")"

cat > "$DESKTOP_FILE" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=RANOnline LLM Integration
Comment=AI LLM Distribution System for RANOnline
Exec=${INSTALL_PREFIX}/bin/RANOnline_LLM_Integration
Icon=${INSTALL_PREFIX}/share/ai_llm_integration/resources/icons/ai_brain.png
Terminal=false
StartupNotify=true
Categories=Game;Development;
Keywords=AI;LLM;RANOnline;Game;
EOF

chmod +x "$DESKTOP_FILE"
echo -e "${GREEN}✅ Desktop entry created${NC}"

# 配置systemd服務（可選）
if command -v systemctl &> /dev/null; then
    echo -e "${YELLOW}🔧 Creating systemd service...${NC}"
    
    SERVICE_FILE="$HOME/.config/systemd/user/ranonline-llm.service"
    mkdir -p "$(dirname "$SERVICE_FILE")"
    
    cat > "$SERVICE_FILE" << EOF
[Unit]
Description=RANOnline LLM Integration Service
After=network.target

[Service]
Type=simple
ExecStart=${INSTALL_PREFIX}/bin/RANOnline_LLM_Integration --service
Restart=always
RestartSec=10
Environment=QT_QPA_PLATFORM=minimal

[Install]
WantedBy=default.target
EOF

    systemctl --user daemon-reload
    echo -e "${GREEN}✅ Systemd service created${NC}"
    echo -e "${BLUE}💡 To enable auto-start: systemctl --user enable ranonline-llm${NC}"
fi

# 完成信息
echo -e "${CYAN}================================================${NC}"
echo -e "${GREEN}🎉 Installation Complete!${NC}"
echo -e "${CYAN}================================================${NC}"
echo -e "${BLUE}📁 Installation directory: ${INSTALL_PREFIX}${NC}"
echo -e "${BLUE}🚀 Executable: ${INSTALL_PREFIX}/bin/RANOnline_LLM_Integration${NC}"
echo -e "${BLUE}📋 Configuration: ${INSTALL_PREFIX}/share/ai_llm_integration/config/${NC}"
echo -e "${BLUE}🎨 Resources: ${INSTALL_PREFIX}/share/ai_llm_integration/resources/${NC}"

echo -e "${YELLOW}📝 Next Steps:${NC}"
echo -e "${BLUE}1. Configure your Ollama servers in the settings${NC}"
echo -e "${BLUE}2. Customize prompt templates as needed${NC}"
echo -e "${BLUE}3. Set up WebSocket connection for game integration${NC}"
echo -e "${BLUE}4. Test the application with your LLM models${NC}"

echo -e "${CYAN}================================================${NC}"
echo -e "${GREEN}✨ Ready to use RANOnline LLM Integration!${NC}"
echo -e "${CYAN}================================================${NC}">
