#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RAN Online AI 四部門提示詞導出工具
"""

import json
import os
from datetime import datetime
from pathlib import Path

def main():
    print("=" * 60)
    print("    RAN Online AI 四部門提示詞導出工具")
    print("    開發團隊：Jy技術團隊")
    print("    版本：1.0.0")
    print("=" * 60)
    print()
    
    # 獲取腳本所在目錄
    base_path = Path(__file__).parent.parent
    config_path = base_path / 'Config' / 'llm.json'
    
    print("🔄 正在載入 LLM 配置...")
    
    try:
        with open(config_path, 'r', encoding='utf-8') as f:
            llm_config = json.load(f)
        print("✅ 配置文件載入成功")
    except Exception as e:
        print(f"❌ 錯誤：{e}")
        return False
    
    # 提取配置數據
    config = llm_config['llm_config']
    departments = config['departments']
    academies = config.get('academies', ['聖門', '懸岩', '鳳凰'])
    prompts = config['prompt_templates']
    
    # 生成導出內容
    export_content = f"""# RAN Online AI 四部門 - 完整提示詞模板
## 生成時間：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

## 🏫 三大學院
- 聖門學院：神聖系，注重防守和治療
- 懸岩學院：元素系，精通法術和控制  
- 鳳凰學院：戰鬥系，專精攻擊和爆發

## 🥋 四大部門
- 劍術系：近戰突擊專家，高爆發單體攻擊
- 弓術系：遠程狙擊專家，精準射擊和控制
- 格鬥系：格鬥搏擊專家，近身纏鬥和反擊
- 氣功系：治療支援專家，團隊治療和增益

## 📝 核心提示詞

### 1. 角色批量生成
```
{prompts['advanced_prompts']['character_generation']['batch_generation']}
```

### 2. 劍術系戰術
```
{prompts['advanced_prompts']['department_specialization']['sword_master']}
```

### 3. 弓術系戰術  
```
{prompts['advanced_prompts']['department_specialization']['archer_specialist']}
```

### 4. 格鬥系戰術
```
{prompts['advanced_prompts']['department_specialization']['combat_fighter']}
```

### 5. 氣功系戰術
```
{prompts['advanced_prompts']['department_specialization']['qigong_healer']}
```

### 6. PK互動嗆聲
```
{prompts['advanced_prompts']['interactive_combat']['pk_taunting']}
```

### 7. 技能自動決策
```
{prompts['advanced_prompts']['skill_automation']['skill_decision']}
```

## 🗄️ SQL建表範例
```sql
CREATE TABLE [dbo].[AiStates] (
    [AIId] NVARCHAR(32) PRIMARY KEY,
    [Name] NVARCHAR(16) NOT NULL,
    [Academy] NVARCHAR(8) NOT NULL CHECK ([Academy] IN ('聖門','懸岩','鳳凰')),
    [Department] NVARCHAR(8) NOT NULL CHECK ([Department] IN ('劍術','弓術','格鬥','氣功')),
    [Status] NVARCHAR(8),
    [LastUpdate] DATETIME DEFAULT GETDATE(),
    [IsActive] BIT DEFAULT 1
);
```

## 🎮 使用範例

### 生成鳳凰學院戰隊
```
你是《RAN Online》自動AI生成器。請根據三大學院（聖門、懸岩、鳳凰）與四大部門（劍術、弓術、格鬥、氣功）隨機產生5名AI角色，每個角色須有：學院、部門、姓名（6字以內）、性格特徵、戰鬥風格、專屬台詞。輸出格式：JSON陣列。
```

### PK嗆聲範例
```
你是鳳凰/劍術AI，在PK中遇到敵方挑釁：「你們鳳凰學院都是莽夫！」，請用充滿個性的繁體中文嗆聲回應。
```

---

**開發團隊**: Jy技術團隊 | **版本**: 1.0.0
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

if __name__ == "__main__":
    main()
