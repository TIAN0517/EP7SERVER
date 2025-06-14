#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RAN Online AI 四部門提示詞導出工具
自動生成 Copilot、Ollama、ChatGPT 等 AI 助手的完整四部門提示詞模板

作者: Jy技術團隊
版本: 1.0.0
建立時間: 2025-06-14
"""

import json
import os
import sys
from datetime import datetime
from pathlib import Path

def load_config(config_path):
    """載入 LLM 配置文件"""
    try:
        with open(config_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"❌ 錯誤：找不到配置文件 {config_path}")
        return None
    except json.JSONDecodeError as e:
        print(f"❌ 錯誤：配置文件格式錯誤 - {e}")
        return None

def generate_prompt_export():
    """生成完整的提示詞導出文件"""
    
    # 獲取腳本所在目錄
    base_path = Path(__file__).parent.parent
    config_path = base_path / 'Config' / 'llm.json'
    
    print("🔄 正在載入 LLM 配置...")
    llm_config = load_config(config_path)
    
    if not llm_config:
        print("❌ 無法載入配置文件，導出失敗！")
        return False
    
    # 提取配置數據
    config = llm_config['llm_config']
    departments = config['departments']
    academies = config.get('academies', ['聖門', '懸岩', '鳳凰'])
    prompts = config['prompt_templates']
    
    # 生成導出內容
    export_content = f"""# RAN Online AI 四部門 - 完整提示詞模板
## 生成時間：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
## 版本：1.0.0
## 開發團隊：Jy技術團隊

---

## 🏫 學院系統
- **聖門學院**：{academies[0]} - 神聖系，注重防守和治療
- **懸岩學院**：{academies[1]} - 元素系，精通法術和控制  
- **鳳凰學院**：{academies[2]} - 戰鬥系，專精攻擊和爆發

## 🥋 四大部門
- **{departments[0]}系**：近戰突擊專家，高爆發單體攻擊
- **{departments[1]}系**：遠程狙擊專家，精準射擊和控制
- **{departments[2]}系**：格鬥搏擊專家，近身纏鬥和反擊
- **{departments[3]}系**：治療支援專家，團隊治療和增益

---

## 📝 核心提示詞模板

### 1. 🤖 角色批量生成
```
{prompts['advanced_prompts']['character_generation']['batch_generation']}
```

**使用範例**：
```
你是一個《RAN Online》自動AI生成器。請根據三大學院（聖門、懸岩、鳳凰）與四大部門（劍術、弓術、格鬥、氣功）隨機產生5名AI角色，每個角色須有：學院、部門、姓名（6字以內）、性格特徵、戰鬥風格、專屬台詞。輸出格式：JSON陣列。
```

### 2. 🎯 各部門專屬戰術

#### 🗡️ 劍術系戰術
```
{prompts['advanced_prompts']['department_specialization']['sword_master']}
```

#### 🏹 弓術系戰術  
```
{prompts['advanced_prompts']['department_specialization']['archer_specialist']}
```

#### 👊 格鬥系戰術
```
{prompts['advanced_prompts']['department_specialization']['combat_fighter']}
```

#### ⚡ 氣功系戰術
```
{prompts['advanced_prompts']['department_specialization']['qigong_healer']}
```

### 3. 🛡️ 戰術指揮系統

#### 隊長指揮
```
{prompts['advanced_prompts']['tactical_leadership']['squad_leader_tactics']}
```

#### 副隊長協調
```
{prompts['advanced_prompts']['tactical_leadership']['deputy_coordination']}
```

#### 氣功師支援
```
{prompts['advanced_prompts']['tactical_leadership']['qigong_support']}
```

### 4. ⚔️ PK互動嗆聲
```
{prompts['advanced_prompts']['interactive_combat']['pk_taunting']}
```

**使用範例**：
```
你是鳳凰/劍術AI，在PK中遇到敵方挑釁時，請根據你的學院和職業特性，以充滿個性的繁體中文嗆聲回應。僅包含：劍術、弓術、格鬥、氣功四部門。不得與現實語言重複，需貼近RAN Online風格。敵方挑釁：「你們鳳凰學院都是莽夫，只會蠻力攻擊！」
```

### 5. 🎯 技能自動決策
```
{prompts['advanced_prompts']['skill_automation']['skill_decision']}
```

**使用範例**：
```
你是AI氣功（如劍術/氣功），根據當前遊戲狀態（敵人血量：30%、友方狀態：劍術系重傷、BUFF剩餘時間：10秒），動態選擇最適合的技能，並用專業簡短語句說明原因和預期效果。
```

### 6. 🔄 緊急撤退策略
```
{prompts['advanced_prompts']['interactive_combat']['emergency_retreat']}
```

### 7. 🎨 角色命名專家
```
{prompts['advanced_prompts']['character_generation']['naming_specialist']}
```

**使用範例**：
```
你是一名AI命名師，請根據聖門/氣功/慈悲自動生成10個角色名稱，全部不超過6字，且富有東方玄幻風格。
```

---

## 🗄️ SQL建表範例（四部門限定）

```sql
-- AI角色狀態表（嚴格限制四部門）
CREATE TABLE [dbo].[AiStates] (
    [AIId] NVARCHAR(32) PRIMARY KEY,
    [Name] NVARCHAR(16) NOT NULL,
    [Academy] NVARCHAR(8) NOT NULL CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [Department] NVARCHAR(8) NOT NULL CHECK ([Department] IN ('劍術','弓術','格鬥','氣功')),
    [Level] INT DEFAULT 1 CHECK ([Level] >= 1 AND [Level] <= 999),
    [BehaviorType] INT,
    [PositionX] INT,
    [PositionY] INT,
    [Status] NVARCHAR(8) DEFAULT '待命',
    [HP] INT DEFAULT 100,
    [MP] INT DEFAULT 50,
    [LastUpdate] DATETIME DEFAULT GETDATE(),
    [IsActive] BIT DEFAULT 1
);

-- AI團隊編組表
CREATE TABLE [dbo].[AiTeams] (
    [TeamId] NVARCHAR(32) PRIMARY KEY,
    [Academy] NVARCHAR(8) NOT NULL,
    [LeaderId] NVARCHAR(32),
    [SwordMember] NVARCHAR(32), -- 劍術系
    [ArcherMember] NVARCHAR(32), -- 弓術系  
    [FighterMember] NVARCHAR(32), -- 格鬥系
    [QigongMember] NVARCHAR(32), -- 氣功系
    [Formation] NVARCHAR(16) DEFAULT '標準陣型',
    [CreatedAt] DATETIME DEFAULT GETDATE(),
    [IsActive] BIT DEFAULT 1
);
```

---

## 🎮 實用範例集

### 範例1：生成鳳凰學院戰隊
```
你是一個《RAN Online》自動AI生成器。請根據三大學院（聖門、懸岩、鳳凰）與四大部門（劍術、弓術、格鬥、氣功）隨機產生4名鳳凰學院AI角色，每個角色須有：學院、部門、姓名（6字以內）、性格特徵、戰鬥風格、專屬台詞。輸出格式：JSON陣列。
```

### 範例2：懸岩學院弓術師戰術
```
你是弓術系AI，擅長遠程狙擊和風箏戰術。戰鬥風格：保持距離、精準射擊、控制節奏。請根據敵方有3名近戰職業逼近制定弓術系專屬戰術。技能特色：精準射擊、範圍箭雨、減速陷阱。
```

### 範例3：聖門隊長指揮
```
你是RAN Online裡的AI隊長，帶領聖門分隊進行PVP活動。請根據當前局勢（如敵人數量：5、地形：橋樑狹道、剩餘資源：MP充足）下達最優行動指令（攻擊、防守、集火、撤退等），並以隊長語氣直接下達命令給隊員。隊員職業僅包含：劍術、弓術、格鬥、氣功。繁體中文輸出。
```

### 範例4：氣功師緊急治療
```
你是AI氣功師，專責團隊治療和BUFF。每當有隊員掉血、被控制、或需加強防禦時，請自動發動對應技能，並用專業語氣簡短播報行動（例如：『立即為弓術系補充護盾』、『團隊回復技能已啟動』）。當前狀況：劍術系HP剩30%，格鬥系被石化，弓術系MP不足
```

---

## 🔧 AI模型推薦配置

### 最佳模型組合
- **角色生成**：`llama3:latest`, `deepseek-coder:latest`
- **戰術規劃**：`phi3:latest`, `gemma:latest`  
- **角色扮演**：`mistral:latest`, `qwen2:latest`
- **技能自動化**：`deepseek-coder:latest`, `llama3:latest`

### 品質過濾器
1. ✅ 部門驗證：確保只包含劍術、弓術、格鬥、氣功
2. ✅ 學院驗證：確保只包含聖門、懸岩、鳳凰
3. ✅ 語言一致性：繁體中文，台灣遊戲風格
4. ✅ 內容適當性：避免現實政治敏感話題

---

## 📋 快速使用清單

### ✅ 複製任一提示詞到以下AI平台
- **GitHub Copilot**：直接貼上，立即可用
- **ChatGPT**：複製提示詞，替換參數變數
- **Ollama**：適用於本地部署的LLM
- **Claude**：支援長文本對話
- **Gemini**：Google的AI助手

### ✅ 參數替換指南
- `{{count}}` → 替換為具體數字（如：5）
- `{{academy}}` → 替換為學院名稱（聖門/懸岩/鳳凰）
- `{{department}}` → 替換為部門名稱（劍術/弓術/格鬥/氣功）
- `{{combat_situation}}` → 替換為具體戰況描述

### ✅ 輸出格式控制
所有AI回應將自動：
- 使用繁體中文
- 限定四大部門
- 符合RAN Online遊戲風格
- 提供結構化JSON格式（如需要）

---

<div align="center">

**🎮 為 RANOnline EP7 提供最專業的 AI 四部門提示詞方案**

*讓每個 AI 角色都擁有獨特的學院特色和部門專精*

**生成時間**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}  
**開發團隊**: Jy技術團隊  

</div>
"""

    # 確保導出目錄存在
    export_dir = base_path / 'exports'
    export_dir.mkdir(exist_ok=True)
    
    # 寫入導出文件
    output_file = export_dir / 'ai_prompts_export_four_dept.md'
    
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(export_content)
        
        print(f"✅ 四部門提示詞導出成功！")
        print(f"📁 文件位置：{output_file.absolute()}")
        print(f"📝 文件大小：{output_file.stat().st_size / 1024:.1f} KB")
        print(f"🎯 包含部門：{', '.join(departments)}")
        print(f"🏫 包含學院：{', '.join(academies)}")
        
        return True
        
    except Exception as e:
        print(f"❌ 導出失敗：{e}")
        return False

def generate_copilot_snippets():
    """生成 GitHub Copilot 專用的程式碼片段"""
    
    base_path = Path(__file__).parent.parent
    
    copilot_snippets = """// RANOnline AI 四部門 - GitHub Copilot 程式碼片段
// 使用方法：複製以下程式碼到你的IDE中，Copilot會自動建議相關程式碼

// 1. 角色生成函數
function generateRANCharacter(academy: string, department: string): AICharacter {
    // 學院：聖門、懸岩、鳳凰
    // 部門：劍術、弓術、格鬥、氣功
    const validAcademies = ['聖門', '懸岩', '鳳凰'];
    const validDepartments = ['劍術', '弓術', '格鬥', '氣功'];
    
    if (!validAcademies.includes(academy) || !validDepartments.includes(department)) {
        throw new Error('Invalid academy or department');
    }
    
    return {
        name: generateName(academy, department),
        academy: academy,
        department: department,
        level: Math.floor(Math.random() * 50) + 1,
        stats: generateStats(academy, department),
        skills: getSkillsByDepartment(department),
        personality: generatePersonality(academy, department)
    };
}

// 2. 戰術決策AI
class TacticalAI {
    private academy: string;
    private department: string;
    
    constructor(academy: string, department: string) {
        this.academy = academy;
        this.department = department;
    }
    
    makeDecision(battleSituation: BattleSituation): TacticalDecision {
        // 根據四大部門特性制定戰術
        switch (this.department) {
            case '劍術':
                return this.swordTactics(battleSituation);
            case '弓術':
                return this.archerTactics(battleSituation);
            case '格鬥':
                return this.fighterTactics(battleSituation);
            case '氣功':
                return this.qigongTactics(battleSituation);
            default:
                throw new Error('Unknown department');
        }
    }
}

// 3. SQL 查詢助手
const FourDepartmentQueries = {
    // 查詢指定學院的四部門分布
    getAcademyDistribution: `
        SELECT 
            [Academy] as '學院',
            [Department] as '部門', 
            COUNT(*) as '人數'
        FROM [AiStates] 
        WHERE [Department] IN ('劍術','弓術','格鬥','氣功')
        AND [Academy] IN ('聖門','懸岩','鳳凰')
        GROUP BY [Academy], [Department]
        ORDER BY [Academy], [Department]
    `,
    
    // 建立標準四部門團隊
    createBalancedTeam: `
        WITH DepartmentMembers AS (
            SELECT 
                [AIId], [Name], [Academy], [Department], [Level],
                ROW_NUMBER() OVER (PARTITION BY [Academy], [Department] ORDER BY [Level] DESC) as rn
            FROM [AiStates]
            WHERE [Department] IN ('劍術','弓術','格鬥','氣功')
            AND [IsActive] = 1
        )
        SELECT * FROM DepartmentMembers WHERE rn = 1
    `
};

// 4. LLM 提示詞模板
const PromptTemplates = {
    characterGeneration: \`你是《RAN Online》AI生成器。請根據三大學院（聖門、懸岩、鳳凰）與四大部門（劍術、弓術、格鬥、氣功）生成{count}名角色。\`,
    
    tacticalCommand: \`你是{academy}學院的AI隊長，指揮四大部門（劍術、弓術、格鬥、氣功）進行戰鬥。當前情況：{situation}\`,
    
    skillDecision: \`作為{department}系AI，根據戰況選擇最適合的技能。敵人狀態：{enemyStatus}，友軍狀態：{allyStatus}\`,
    
    pkTaunting: \`你是{academy}/{department}AI，面對敵方挑釁：「{enemyTaunt}」，請用充滿個性的繁體中文回應。\`
};
"""

    # 寫入 Copilot 片段文件
    export_dir = base_path / 'exports'
    copilot_file = export_dir / 'copilot_four_dept_snippets.js'
    
    try:
        with open(copilot_file, 'w', encoding='utf-8') as f:
            f.write(copilot_snippets)
        
        print(f"✅ Copilot 程式碼片段導出成功！")
        print(f"📁 文件位置：{copilot_file.absolute()}")
        
    except Exception as e:
        print(f"❌ Copilot 片段導出失敗：{e}")

if __name__ == "__main__":
    print("=" * 60)
    print("    RAN Online AI 四部門提示詞導出工具")
    print("    開發團隊：Jy技術團隊")
    print("    版本：1.0.0")
    print("=" * 60)
    print()
    
    # 執行主要導出
    success = generate_prompt_export()
    
    if success:
        print()
        print("🔄 正在生成 Copilot 程式碼片段...")
        generate_copilot_snippets()
        
        print()
        print("🎉 所有文件導出完成！")
        print()
        print("📋 使用指南：")
        print("   1. 打開 exports/ai_prompts_export_four_dept.md")
        print("   2. 複製任何提示詞到 AI 平台（Copilot/ChatGPT/Ollama）")
        print("   3. 替換 {參數} 為實際值")
        print("   4. 享受專業的四部門AI回應！")
        print()
        print("🚀 現在就開始使用 RANOnline AI 四部門系統吧！")
    else:
        print("❌ 導出過程中發生錯誤，請檢查配置文件。")
        sys.exit(1)
