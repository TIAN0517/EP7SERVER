# RANOnline AI決策核心系統 v4.0.0

🧠 **強化AI決策系統** - 集成多種先進AI決策算法的完整解決方案

## 📋 目錄
- [概述](#概述)
- [核心特性](#核心特性)
- [系統架構](#系統架構)
- [安裝與配置](#安裝與配置)
- [快速開始](#快速開始)
- [詳細API說明](#詳細api說明)
- [配置文件說明](#配置文件說明)
- [性能優化](#性能優化)
- [故障排除](#故障排除)
- [開發指南](#開發指南)

---

## 🎯 概述

RANOnline AI決策核心系統是一個先進的AI決策框架，專為MMO遊戲中的智能NPC和AI玩家設計。系統集成了多種AI決策算法，包括效用函數系統、行為樹、強化學習(Q-Learning)、分層決策等，提供可組合、可擴展的AI決策解決方案。

### 🌟 主要優勢

- **🔄 多策略支援**: 支援6種不同的決策策略，可動態切換
- **📚 學習能力**: 集成Q-Learning強化學習，AI可自主學習和進化
- **🔥 熱更新**: 支援配置文件熱更新，無需重啟即可調整AI行為
- **⚡ 高性能**: 優化的決策算法，支援大量AI實體同時運行
- **🧩 模組化**: 完全模組化設計，易於擴展和自定義
- **📊 監控工具**: 內建性能監控和調試工具

---

## 🚀 核心特性

### 🎛️ 六大決策策略

1. **效用函數系統 (Utility-Based)**
   - 多維度效用計算 (戰鬥、生存、支援)
   - 動態權重調整
   - 支援自定義效用函數

2. **行為樹決策 (Behavior Tree)**
   - 完整的節點系統 (條件、動作、組合器)
   - JSON配置支援
   - 可視化調試

3. **強化學習 (Q-Learning)**
   - 深度Q網路支援
   - 經驗回放機制
   - 自適應探索策略

4. **分層決策 (Hierarchical)**
   - 三層決策架構 (戰略→戰術→操作)
   - 動態目標管理
   - 計劃重新規劃

5. **混合策略 (Hybrid)**
   - 多策略組合決策
   - 投票機制
   - 信心度加權

6. **自定義策略 (Custom)**
   - 開放式介面
   - 插件式架構
   - 完全自定義邏輯

### 🔧 核心組件

```
JyAI 命名空間
├── AIDecisionCore        # 決策核心引擎
├── AIPlayerBrain         # AI玩家大腦
├── AIPlayerManager       # AI玩家管理器
├── UtilitySystem         # 效用函數系統
├── BehaviorTree          # 行為樹系統
├── QLearningAgent        # Q學習代理
├── HierarchicalPlanner   # 分層規劃器
├── EnvironmentPerceptor  # 環境感知器
└── ConfigManager         # 配置管理器
```

---

## 🏗️ 系統架構

```
┌─────────────────────────────────────┐
│           應用層 (Application)        │
├─────────────────────────────────────┤
│         AI玩家管理器                  │
│      (AIPlayerManager)              │
├─────────────────────────────────────┤
│         AI玩家大腦                   │
│       (AIPlayerBrain)               │
├─────────────────────────────────────┤
│         AI決策核心                   │
│      (AIDecisionCore)               │
├─────────────────────────────────────┤
│  決策策略層 (Strategy Layer)          │
│  ┌─────────┬─────────┬─────────┐    │
│  │ Utility │Behavior │Q-Learning│    │
│  │ System  │  Tree   │ Agent   │    │
│  └─────────┴─────────┴─────────┘    │
├─────────────────────────────────────┤
│  支援系統層 (Support Layer)           │
│  ┌─────────┬─────────┬─────────┐    │
│  │ Config  │ Percep- │ Learn-  │    │
│  │Manager  │  tor    │ ing     │    │
│  └─────────┴─────────┴─────────┘    │
└─────────────────────────────────────┘
```

---

## 📦 安裝與配置

### 系統要求

- **編譯器**: C++17 或更新版本
- **Qt Framework**: Qt 5.15+ 或 Qt 6.x
- **記憶體**: 建議 4GB+ (取決於AI實體數量)
- **CPU**: 支援多核心處理器

### 編譯步驟

1. **檢查依賴項**
```bash
# 確保已安裝 Qt 開發環境
qmake --version
```

2. **添加到項目**
```cpp
// 在 CMakeLists.txt 或 .pro 文件中添加
SOURCES += \
    AIDecisionCore.cpp \
    AIPlayerBrain.cpp \
    AIBehaviorSystems.cpp \
    AIDecisionCoreTest.cpp

HEADERS += \
    AIDecisionCore.h \
    AIPlayerBrain.h
```

3. **包含頭文件**
```cpp
#include "AIDecisionCore.h"
#include "AIPlayerBrain.h"
using namespace JyAI;
```

### 配置文件設置

將配置文件複製到項目目錄：
- `ai_decision_config.json` - 主配置文件
- `behavior_tree_config.json` - 行為樹配置
- `qlearning_config.json` - Q學習配置

---

## ⚡ 快速開始

### 基本使用範例

```cpp
#include "AIPlayerBrain.h"
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 1. 創建AI玩家大腦
    auto brain = std::make_unique<JyAI::AIPlayerBrain>("player_001");
    
    // 2. 設置玩家數據
    RANOnline::AI::AIPlayerData playerData;
    playerData.id = "player_001";
    playerData.currentHP = 100;
    playerData.maxHP = 100;
    playerData.level = 25;
    brain->setPlayerData(playerData);
    
    // 3. 選擇決策策略
    brain->setDecisionStrategy(JyAI::DecisionStrategyType::HYBRID);
    
    // 4. 啟動AI
    brain->start();
    
    // 5. 連接信號以接收AI行為
    QObject::connect(brain.get(), &JyAI::AIPlayerBrain::moveRequested,
                     [](const QString &playerId, const QVector3D &position) {
        qDebug() << "Player" << playerId << "wants to move to" << position;
    });
    
    return app.exec();
}
```

### 管理多個AI玩家

```cpp
#include "AIPlayerBrain.h"

void setupMultipleAIPlayers()
{
    // 創建AI玩家管理器
    JyAI::AIPlayerManager manager;
    
    // 添加多個AI玩家
    for (int i = 1; i <= 10; ++i) {
        QString playerId = QString("ai_player_%1").arg(i);
        
        RANOnline::AI::AIPlayerData playerData;
        playerData.id = playerId;
        playerData.currentHP = 100;
        playerData.maxHP = 100;
        playerData.level = 20 + (i % 20);
        
        manager.addPlayer(playerId, playerData);
    }
    
    // 設置所有AI使用混合策略
    manager.setAllPlayersStrategy(JyAI::DecisionStrategyType::HYBRID);
    
    // 啟動所有AI
    manager.start();
    
    // 獲取管理器統計
    auto stats = manager.getManagerStats();
    qDebug() << "Managing" << stats.totalPlayers << "AI players";
}
```

---

## 📚 詳細API說明

### AIDecisionCore 類

核心決策引擎，負責執行AI決策邏輯。

#### 主要方法

```cpp
class AIDecisionCore {
public:
    // 構造和析構
    AIDecisionCore();
    ~AIDecisionCore();
    
    // 決策方法
    AIAction makeDecision(const PerceptionData &perception);
    void setStrategy(DecisionStrategyType strategy);
    DecisionStrategyType getCurrentStrategy() const;
    
    // 學習方法
    void learn(const PerceptionData &perception, 
               const AIAction &action, 
               float reward);
    
    // 配置管理
    bool loadConfiguration(const QString &configPath);
    bool reloadConfiguration();
    
    // 模型管理
    bool saveModel(const QString &modelPath);
    bool loadModel(const QString &modelPath);
    
    // 統計和調試
    AIStatistics getStatistics() const;
    void enableDebug(bool enabled);
};
```

#### 決策策略類型

```cpp
enum class DecisionStrategyType {
    UTILITY_BASED,  // 效用函數決策
    BEHAVIOR_TREE,  // 行為樹決策
    Q_LEARNING,     // Q學習決策
    HIERARCHICAL,   // 分層決策
    HYBRID,         // 混合策略
    CUSTOM          // 自定義策略
};
```

### AIPlayerBrain 類

AI玩家大腦，集成AIDecisionCore並提供完整的AI玩家實現。

#### 主要功能

```cpp
class AIPlayerBrain : public QObject {
    Q_OBJECT
    
public:
    // 構造和控制
    explicit AIPlayerBrain(const QString &playerId, QObject *parent = nullptr);
    
    void start();       // 啟動AI
    void stop();        // 停止AI
    void pause();       // 暫停AI
    void resume();      // 恢復AI
    
    // 配置方法
    void setPlayerData(const RANOnline::AI::AIPlayerData &playerData);
    void setDecisionStrategy(DecisionStrategyType strategy);
    void setUpdateFrequency(int milliseconds);
    void enableDebug(bool enabled);
    
    // 監控方法
    QString getDebugInfo() const;
    PerformanceStats getPerformanceStats() const;
    QList<ActionRecord> getActionHistory(int limit = 50) const;

signals:
    // 遊戲動作信號
    void moveRequested(const QString &playerId, const QVector3D &position);
    void attackRequested(const QString &playerId, const QString &targetId);
    void skillUseRequested(const QString &playerId, const QString &skillId, const QString &targetId);
    void itemUseRequested(const QString &playerId, const QString &itemId);
    void interactionRequested(const QString &playerId, const QString &objectId);
    
    // AI狀態信號
    void actionExecuted(const QString &playerId, const AIAction &action);
    void errorOccurred(const QString &playerId, const QString &error);
    void teamSupportNeeded(const QString &playerId, const QString &allyId);
};
```

### AIPlayerManager 類

AI玩家管理器，負責管理多個AI玩家實例。

```cpp
class AIPlayerManager : public QObject {
    Q_OBJECT
    
public:
    // 玩家管理
    bool addPlayer(const QString &playerId, const RANOnline::AI::AIPlayerData &playerData);
    bool removePlayer(const QString &playerId);
    AIPlayerBrain* getPlayer(const QString &playerId);
    QStringList getPlayerIds() const;
    void clearAllPlayers();
    
    // 批量控制
    void startAllPlayers();
    void stopAllPlayers();
    void setAllPlayersStrategy(DecisionStrategyType strategy);
    void setAllPlayersUpdateFrequency(int milliseconds);
    
    // 管理器控制
    void start();
    void stop();
    void setMaxPlayers(int maxPlayers);
    
    // 統計監控
    ManagerStats getManagerStats() const;
    QString getDetailedStats() const;
};
```

---

## ⚙️ 配置文件說明

### 主配置文件 (ai_decision_config.json)

```json
{
  "aiDecisionConfig": {
    "globalSettings": {
      "enableDebugMode": false,
      "enableLearning": true,
      "maxDecisionHistorySize": 1000,
      "logLevel": "INFO"
    },
    
    "strategySettings": {
      "defaultStrategy": "HYBRID",
      "allowStrategyChange": true,
      "adaptiveStrategySelection": true
    },
    
    "utilitySystem": {
      "combatWeight": 0.4,
      "survivalWeight": 0.4,
      "supportWeight": 0.2,
      "utilityThreshold": 0.5
    }
  }
}
```

### 行為樹配置 (behavior_tree_config.json)

定義AI的行為邏輯樹，包含條件節點、動作節點和組合器節點。

```json
{
  "behaviorTrees": {
    "combatTree": {
      "rootNode": "CombatRootSelector",
      "nodes": {
        "CombatRootSelector": {
          "type": "Selector",
          "children": ["EmergencyFleeSequence", "AggressiveCombatSequence"]
        },
        "EmergencyFleeSequence": {
          "type": "Sequence",
          "children": ["CheckCriticalHealth", "ExecuteFlee"]
        }
      }
    }
  }
}
```

### Q學習配置 (qlearning_config.json)

配置強化學習參數，包括網路架構、學習率、探索策略等。

```json
{
  "qLearningConfig": {
    "algorithmSettings": {
      "learningRate": 0.1,
      "discountFactor": 0.95,
      "explorationRate": 0.1
    },
    "networkArchitecture": {
      "hiddenLayers": [256, 128, 64],
      "activationFunction": "relu"
    }
  }
}
```

---

## 🎯 實際應用範例

### 範例1: 戰鬥AI

```cpp
void setupCombatAI()
{
    auto combatBrain = std::make_unique<JyAI::AIPlayerBrain>("combat_ai");
    
    // 設置攻擊型AI配置
    combatBrain->setDecisionStrategy(JyAI::DecisionStrategyType::UTILITY_BASED);
    
    // 連接戰鬥行為
    QObject::connect(combatBrain.get(), &JyAI::AIPlayerBrain::attackRequested,
                     [](const QString &playerId, const QString &targetId) {
        // 執行攻擊邏輯
        executeCombatAttack(playerId, targetId);
    });
    
    QObject::connect(combatBrain.get(), &JyAI::AIPlayerBrain::skillUseRequested,
                     [](const QString &playerId, const QString &skillId, const QString &targetId) {
        // 執行技能邏輯
        executeSkillAttack(playerId, skillId, targetId);
    });
    
    combatBrain->start();
}
```

### 範例2: 支援AI

```cpp
void setupSupportAI()
{
    auto supportBrain = std::make_unique<JyAI::AIPlayerBrain>("support_ai");
    
    // 設置支援型AI配置
    supportBrain->setDecisionStrategy(JyAI::DecisionStrategyType::HIERARCHICAL);
    
    // 連接支援行為
    QObject::connect(supportBrain.get(), &JyAI::AIPlayerBrain::teamSupportNeeded,
                     [](const QString &playerId, const QString &allyId) {
        // 執行支援邏輯
        provideSupportToAlly(playerId, allyId);
    });
    
    supportBrain->start();
}
```

### 範例3: 學習型AI

```cpp
void setupLearningAI()
{
    auto learningBrain = std::make_unique<JyAI::AIPlayerBrain>("learning_ai");
    
    // 設置學習型AI配置
    learningBrain->setDecisionStrategy(JyAI::DecisionStrategyType::Q_LEARNING);
    
    // 監控學習進度
    QObject::connect(learningBrain.get(), &JyAI::AIPlayerBrain::actionExecuted,
                     [learningBrain = learningBrain.get()](const QString &playerId, const JyAI::AIAction &action) {
        // 記錄行為效果並提供回饋
        float reward = calculateActionReward(action);
        // learningBrain->provideFeedback(reward); // 如果有此方法
    });
    
    learningBrain->start();
}
```

---

## 📊 性能優化

### 優化建議

1. **更新頻率調整**
```cpp
// 根據AI重要性調整更新頻率
brain->setUpdateFrequency(100);  // 重要AI: 100ms
brain->setUpdateFrequency(200);  // 一般AI: 200ms
brain->setUpdateFrequency(500);  // 背景AI: 500ms
```

2. **感知範圍限制**
```cpp
// 在配置中限制感知範圍以提升性能
"environmentPerception": {
  "perceptionRadius": 30.0,      // 減少感知半徑
  "updateFrequency": 100,        // 增加更新間隔
  "enablePrediction": false      // 關閉預測功能
}
```

3. **記憶體管理**
```cpp
// 限制歷史記錄大小
"globalSettings": {
  "maxDecisionHistorySize": 500,  // 減少歷史記錄
  "enableGarbageCollection": true
}
```

### 性能監控

```cpp
// 獲取性能統計
auto stats = brain->getPerformanceStats();
qDebug() << "Average decision time:" << stats.averageDecisionTime << "ms";
qDebug() << "Total updates:" << stats.totalUpdates;

// 管理器統計
auto managerStats = manager.getManagerStats();
qDebug() << "Active players:" << managerStats.activePlayers;
qDebug() << "Average decision time:" << managerStats.averageDecisionTime << "ms";
```

---

## 🔧 故障排除

### 常見問題

1. **AI決策速度緩慢**
   - 檢查更新頻率設置
   - 減少感知範圍
   - 關閉不必要的調試功能

2. **AI行為不合理**
   - 檢查配置文件是否正確載入
   - 驗證感知數據是否準確
   - 調整效用函數權重

3. **記憶體使用過高**
   - 限制決策歷史大小
   - 減少AI實體數量
   - 啟用記憶體優化選項

4. **學習效果不佳**
   - 調整學習率和探索率
   - 檢查獎勵函數設計
   - 增加訓練數據量

### 調試技巧

```cpp
// 啟用調試模式
brain->enableDebug(true);

// 獲取調試資訊
QString debugInfo = brain->getDebugInfo();
qDebug() << debugInfo;

// 查看行為歷史
auto history = brain->getActionHistory(20);
for (const auto &record : history) {
    qDebug() << "Action:" << static_cast<int>(record.action.type)
             << "Time:" << record.timestamp.toString();
}
```

---

## 👨‍💻 開發指南

### 擴展自定義策略

```cpp
// 1. 實現自定義決策策略
class CustomStrategy : public IDecisionStrategy {
public:
    AIAction makeDecision(const PerceptionData &perception) override {
        AIAction action;
        // 實現自定義決策邏輯
        action.type = ActionType::MOVE;
        action.confidence = 0.8f;
        action.isValid = true;
        return action;
    }
    
    void learn(const PerceptionData &perception, 
               const AIAction &action, 
               float reward) override {
        // 實現學習邏輯
    }
};

// 2. 註冊自定義策略
auto customStrategy = std::make_unique<CustomStrategy>();
decisionCore->setCustomStrategy(std::move(customStrategy));
decisionCore->setStrategy(DecisionStrategyType::CUSTOM);
```

### 添加自定義行為

```cpp
// 擴展動作類型
enum class CustomActionType {
    CAST_SPELL = 100,
    TRADE_ITEM = 101,
    BUILD_STRUCTURE = 102
};

// 在AIPlayerBrain中處理自定義動作
void AIPlayerBrain::executeCustomAction(const AIAction &action) {
    switch (static_cast<CustomActionType>(action.type)) {
        case CustomActionType::CAST_SPELL:
            executeMagicSpell(action);
            break;
        case CustomActionType::TRADE_ITEM:
            executeTrading(action);
            break;
        default:
            break;
    }
}
```

### 集成現有系統

```cpp
// 與RANOnline現有AI系統集成
class RANOnlineAIBridge {
public:
    void integrateWithExistingAI(AIPlayerBrain *brain) {
        // 連接到現有的AI系統
        connect(brain, &AIPlayerBrain::moveRequested,
                this, &RANOnlineAIBridge::handleMoveRequest);
        
        connect(brain, &AIPlayerBrain::attackRequested,
                this, &RANOnlineAIBridge::handleAttackRequest);
    }
    
private:
    void handleMoveRequest(const QString &playerId, const QVector3D &position) {
        // 轉發到RANOnline的移動系統
        RANOnline::AI::movePlayer(playerId, position);
    }
    
    void handleAttackRequest(const QString &playerId, const QString &targetId) {
        // 轉發到RANOnline的戰鬥系統
        RANOnline::AI::attackTarget(playerId, targetId);
    }
};
```

---

## 📈 版本歷史

### v4.0.0 (當前版本)
- ✅ 完整的AI決策核心系統
- ✅ 六種決策策略實現
- ✅ AI玩家大腦和管理器
- ✅ 配置文件熱更新
- ✅ 性能監控和調試工具

### 即將推出的功能
- 🔄 深度強化學習支援
- 🔄 多智能體協作算法
- 🔄 可視化調試界面
- 🔄 雲端學習服務集成

---

## 📞 技術支援

如需技術支援或有任何問題，請聯繫：

- **Email**: ai-support@ranonline.com
- **文檔**: https://docs.ranonline.com/ai-decision-core
- **GitHub**: https://github.com/ranonline/ai-decision-core

---

## 📄 許可證

Copyright © 2025 Jy技術團隊. All rights reserved.

本軟件按照 MIT 許可證分發。詳細信息請參閱 LICENSE 文件。

---

**🎯 RANOnline AI決策核心系統 - 讓您的遊戲AI更智能、更真實！**
