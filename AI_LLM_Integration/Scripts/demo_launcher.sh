#!/bin/bash
# RANOnline LLM Integration - One-Click Demo Launch Script
# 一鍵演示啟動腳本，快速展示系統功能

set -e

# 顏色定義
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}🚀 RANOnline LLM Integration - Demo Launcher${NC}"
echo -e "${CYAN}================================================${NC}"

# 檢查 Ollama 服務
echo -e "${YELLOW}🔍 Checking Ollama service...${NC}"
if ! curl -s --connect-timeout 3 http://localhost:11434/api/tags > /dev/null 2>&1; then
    echo -e "${YELLOW}⚠️  Ollama service not running. Starting Ollama...${NC}"
    
    # 嘗試啟動 Ollama 服務
    if command -v ollama &> /dev/null; then
        ollama serve &
        OLLAMA_PID=$!
        echo -e "${BLUE}📋 Ollama PID: ${OLLAMA_PID}${NC}"
        
        # 等待服務啟動
        echo -e "${YELLOW}⏳ Waiting for Ollama to start...${NC}"
        for i in {1..30}; do
            if curl -s --connect-timeout 1 http://localhost:11434/api/tags > /dev/null 2>&1; then
                echo -e "${GREEN}✅ Ollama service started successfully${NC}"
                break
            fi
            sleep 1
        done
    else
        echo -e "${RED}❌ Ollama not found. Please install Ollama first.${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}✅ Ollama service is running${NC}"
fi

# 檢查必要模型
echo -e "${YELLOW}🤖 Checking available models...${NC}"
MODELS=$(curl -s http://localhost:11434/api/tags | jq -r '.models[].name' 2>/dev/null || echo "")

if [ -z "$MODELS" ]; then
    echo -e "${YELLOW}⚠️  No models found. Downloading demo models...${NC}"
    
    # 下載小型演示模型
    echo -e "${BLUE}📥 Downloading phi3:mini (2.3GB)...${NC}"
    ollama pull phi3:mini
    
    echo -e "${BLUE}📥 Downloading gemma:2b (1.4GB)...${NC}"
    ollama pull gemma:2b
    
    echo -e "${GREEN}✅ Demo models downloaded${NC}"
else
    echo -e "${GREEN}✅ Available models:${NC}"
    echo "$MODELS" | while read -r model; do
        if [ -n "$model" ]; then
            echo -e "${BLUE}   - ${model}${NC}"
        fi
    done
fi

# 檢查應用程式
APP_PATH="./build/RANOnline_LLM_Integration"
if [ ! -f "$APP_PATH" ]; then
    echo -e "${YELLOW}🔨 Application not found. Building...${NC}"
    
    # 快速建置
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_LLM_TESTS=OFF
    cmake --build . --parallel
    cd ..
    
    if [ ! -f "$APP_PATH" ]; then
        echo -e "${RED}❌ Build failed. Please check the build process.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✅ Application built successfully${NC}"
fi

# 創建演示配置
echo -e "${YELLOW}⚙️  Creating demo configuration...${NC}"

# 創建演示用的簡化配置
mkdir -p Config
cat > Config/demo_servers.json << EOF
{
  "servers": [
    {
      "id": "demo_ollama",
      "name": "Demo Ollama Server",
      "host": "localhost",
      "port": 11434,
      "enabled": true,
      "max_connections": 3,
      "timeout_ms": 30000,
      "models": ["phi3:mini", "gemma:2b"],
      "load_balancer": {
        "strategy": "round_robin",
        "weight": 100
      }
    }
  ]
}
EOF

# 創建演示模板
cat > Config/demo_templates.json << EOF
{
  "templates": {
    "demo_skill": {
      "name": "Demo Skill Generation",
      "template": "Create a {academy} skill for level {level} character. Make it balanced and fun!",
      "preferred_models": ["phi3:mini"],
      "parameters": {
        "academy": {
          "type": "select", 
          "options": ["Warrior", "Mage", "Archer"],
          "default": "Warrior"
        },
        "level": {
          "type": "number",
          "min": 1,
          "max": 100,
          "default": 10
        }
      }
    }
  }
}
EOF

echo -e "${GREEN}✅ Demo configuration created${NC}"

# 啟動應用程式
echo -e "${CYAN}================================================${NC}"
echo -e "${GREEN}🎉 Launching RANOnline LLM Integration Demo${NC}"
echo -e "${CYAN}================================================${NC}"

echo -e "${BLUE}🎮 Demo Features:${NC}"
echo -e "${BLUE}  • Multiple theme switching${NC}"
echo -e "${BLUE}  • Real-time AI request processing${NC}"
echo -e "${BLUE}  • Academy-based color theming${NC}"
echo -e "${BLUE}  • Progress monitoring${NC}"
echo -e "${BLUE}  • Batch request processing${NC}"
echo -e "${BLUE}  • Performance dashboard${NC}"

echo -e "${YELLOW}💡 Demo Tips:${NC}"
echo -e "${YELLOW}  1. Try different themes from the theme selector${NC}"
echo -e "${YELLOW}  2. Create requests with different academies${NC}"
echo -e "${YELLOW}  3. Test batch processing with multiple prompts${NC}"
echo -e "${YELLOW}  4. Monitor performance in the stats panel${NC}"
echo -e "${YELLOW}  5. Check the server management tab${NC}"

echo -e "${CYAN}================================================${NC}"

# 設定環境變數
export RANONLINE_DEMO_MODE=1
export RANONLINE_CONFIG_PATH="./Config"

# 啟動應用程式
"$APP_PATH" --demo --config-path="./Config"

# 清理（如果需要）
cleanup() {
    echo -e "\n${YELLOW}🧹 Cleaning up...${NC}"
    if [ ! -z "$OLLAMA_PID" ]; then
        echo -e "${BLUE}🛑 Stopping Ollama service (PID: $OLLAMA_PID)${NC}"
        kill $OLLAMA_PID 2>/dev/null || true
    fi
    echo -e "${GREEN}✅ Demo session ended${NC}"
}

trap cleanup EXIT
