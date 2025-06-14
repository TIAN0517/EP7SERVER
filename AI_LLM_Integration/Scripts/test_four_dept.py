#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RANOnline AI 四部門實時測試工具
實時測試四大部門AI的響應效果
"""

import json
import time
import random
from datetime import datetime

class FourDepartmentTester:
    def __init__(self):
        self.departments = ["劍術", "弓術", "格鬥", "氣功"]
        self.academies = ["聖門", "懸岩", "鳳凰"]
        self.test_scenarios = self.load_test_scenarios()
    
    def load_test_scenarios(self):
        """載入測試場景"""
        return {
            "character_generation": [
                "生成5名鳳凰學院AI角色",
                "創建聖門學院完整戰隊",
                "隨機生成懸岩學院混合隊伍"
            ],
            "combat_tactics": [
                "面對敵方法師群的戰術",
                "橋樑狹道的防守策略", 
                "敵眾我寡的突圍戰術"
            ],
            "pk_interaction": [
                "回應敵方的挑釁嗆聲",
                "勝利後的慶祝台詞",
                "失敗時的不服氣回應"
            ],
            "skill_automation": [
                "緊急治療決策",
                "爆發技能時機選擇",
                "控制技能優先級判斷"
            ]
        }
    
    def generate_test_prompt(self, category, department=None, academy=None):
        """生成測試提示詞"""
        if not department:
            department = random.choice(self.departments)
        if not academy:
            academy = random.choice(self.academies)
            
        base_prompts = {
            "character_generation": f"""
你是《RAN Online》自動AI生成器。請根據三大學院（聖門、懸岩、鳳凰）與四大部門（劍術、弓術、格鬥、氣功）隨機產生3名{academy}學院AI角色，每個角色須有：學院、部門、姓名（6字以內）、性格特徵、戰鬥風格、專屬台詞。輸出格式：JSON陣列。
""",
            "combat_tactics": f"""
你是{department}系AI，隸屬{academy}學院。當前戰況：敵方有5名不同職業的玩家，地形為山地高低起伏。請制定專屬戰術，考慮你的職業特色和學院風格。
""",
            "pk_interaction": f"""
你是{academy}/{department}AI，在PK中遇到敵方挑釁：「你們{academy}學院的{department}系都很弱！根本不是我們的對手！」請用充滿個性的繁體中文嗆聲回應。
""",
            "skill_automation": f"""
你是AI{department}師，當前狀況：友方劍術系HP剩20%且被石化，弓術系MP耗盡，格鬥系正在與敵方纏鬥，敵方法師正在吟唱大招。請選擇最適合的技能並說明原因。
"""
        }
        
        return base_prompts.get(category, "")
    
    def run_interactive_test(self):
        """運行互動式測試"""
        print("🎮 RANOnline AI 四部門實時測試工具")
        print("=" * 60)
        print("💡 輸入 'help' 查看指令，輸入 'quit' 退出")
        print()
        
        while True:
            try:
                user_input = input("🎯 請選擇測試類型 (char/combat/pk/skill) 或輸入自定義提示詞: ").strip()
                
                if user_input.lower() == 'quit':
                    break
                elif user_input.lower() == 'help':
                    self.show_help()
                    continue
                elif user_input.lower() == 'char':
                    category = "character_generation"
                elif user_input.lower() == 'combat':
                    category = "combat_tactics"
                elif user_input.lower() == 'pk':
                    category = "pk_interaction"
                elif user_input.lower() == 'skill':
                    category = "skill_automation"
                else:
                    # 自定義提示詞
                    self.process_custom_prompt(user_input)
                    continue
                
                # 生成並顯示測試提示詞
                department = input(f"🏹 選擇部門 ({'/'.join(self.departments)}) [隨機]: ").strip()
                if not department or department not in self.departments:
                    department = random.choice(self.departments)
                
                academy = input(f"🏫 選擇學院 ({'/'.join(self.academies)}) [隨機]: ").strip()
                if not academy or academy not in self.academies:
                    academy = random.choice(self.academies)
                
                prompt = self.generate_test_prompt(category, department, academy)
                self.display_test_result(prompt, department, academy, category)
                
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f"❌ 錯誤: {e}")
    
    def process_custom_prompt(self, prompt):
        """處理自定義提示詞"""
        print("\n" + "="*60)
        print("📝 自定義提示詞測試")
        print("="*60)
        
        # 檢查是否包含四部門關鍵詞
        dept_mentions = [dept for dept in self.departments if dept in prompt]
        academy_mentions = [academy for academy in self.academies if academy in prompt]
        
        print(f"🎯 提示詞: {prompt}")
        print(f"🏹 檢測到的部門: {', '.join(dept_mentions) if dept_mentions else '無'}")
        print(f"🏫 檢測到的學院: {', '.join(academy_mentions) if academy_mentions else '無'}")
        
        if not dept_mentions:
            print("⚠️  警告：提示詞中未檢測到四大部門關鍵詞")
        if not academy_mentions:
            print("⚠️  警告：提示詞中未檢測到三大學院關鍵詞")
        
        print("\n✅ 這個提示詞可以直接複製到以下AI平台:")
        print("   • GitHub Copilot")
        print("   • ChatGPT")
        print("   • Ollama")
        print("   • Claude")
        print("\n" + "="*60 + "\n")
    
    def display_test_result(self, prompt, department, academy, category):
        """顯示測試結果"""
        print("\n" + "="*60)
        print(f"🧪 測試結果 - {category.upper()}")
        print("="*60)
        print(f"🏫 學院: {academy}")
        print(f"🏹 部門: {department}")
        print(f"⏰ 時間: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print()
        print("📝 生成的提示詞:")
        print("-" * 40)
        print(prompt.strip())
        print("-" * 40)
        print()
        print("✅ 使用說明:")
        print("   1. 複製上方提示詞")
        print("   2. 貼到 AI 平台（ChatGPT/Copilot/Ollama）")
        print("   3. 等待 AI 回應")
        print("   4. 檢查回應是否符合四部門規範")
        print()
        print("🎯 預期回應特徵:")
        self.show_expected_features(department, academy, category)
        print("\n" + "="*60 + "\n")
    
    def show_expected_features(self, department, academy, category):
        """顯示預期的回應特徵"""
        dept_features = {
            "劍術": "近戰攻擊、連擊、突進技能",
            "弓術": "遠程攻擊、精準射擊、控制技能", 
            "格鬥": "格鬥技巧、擒拿技、反擊",
            "氣功": "治療技能、BUFF、狀態管理"
        }
        
        academy_features = {
            "聖門": "神聖、防守、治療、莊重語氣",
            "懸岩": "元素、控制、智慧、深沉語氣",
            "鳳凰": "戰鬥、爆發、勇猛、豪邁語氣"
        }
        
        print(f"   • 部門特徵: {dept_features.get(department, '未知')}")
        print(f"   • 學院特徵: {academy_features.get(academy, '未知')}")
        print(f"   • 語言: 繁體中文")
        print(f"   • 風格: RAN Online 遊戲風格")
    
    def show_help(self):
        """顯示幫助信息"""
        help_text = """
🆘 指令說明:
   char   - 角色生成測試
   combat - 戰術策略測試  
   pk     - PK互動測試
   skill  - 技能決策測試
   help   - 顯示此幫助
   quit   - 退出程式

🏫 三大學院:
   聖門 - 神聖系，注重防守和治療
   懸岩 - 元素系，精通法術和控制
   鳳凰 - 戰鬥系，專精攻擊和爆發

🏹 四大部門:
   劍術 - 近戰突擊專家
   弓術 - 遠程狙擊專家
   格鬥 - 格鬥搏擊專家
   氣功 - 治療支援專家

💡 使用技巧:
   • 可以輸入完整的自定義提示詞
   • 選擇部門/學院時可按 Enter 使用隨機
   • 生成的提示詞可直接用於各AI平台
"""
        print(help_text)
    
    def run_batch_test(self):
        """運行批量測試"""
        print("🔄 執行批量測試...")
        
        test_results = []
        for category in self.test_scenarios:
            for department in self.departments:
                for academy in self.academies:
                    prompt = self.generate_test_prompt(category, department, academy)
                    result = {
                        "category": category,
                        "department": department,
                        "academy": academy,
                        "prompt": prompt.strip(),
                        "timestamp": datetime.now().isoformat()
                    }
                    test_results.append(result)
        
        # 保存測試結果
        with open("test_results.json", "w", encoding="utf-8") as f:
            json.dump(test_results, f, ensure_ascii=False, indent=2)
        
        print(f"✅ 批量測試完成！生成了 {len(test_results)} 個測試案例")
        print("📁 結果已保存到 test_results.json")

def main():
    tester = FourDepartmentTester()
    
    print("🎮 RANOnline AI 四部門測試工具")
    print("開發團隊：Jy技術團隊")
    print("版本：1.0.0")
    print()
    
    mode = input("選擇模式 - 互動測試(i) 或 批量測試(b): ").strip().lower()
    
    if mode == 'b':
        tester.run_batch_test()
    else:
        tester.run_interactive_test()
    
    print("👋 感謝使用！")

if __name__ == "__main__":
    main()
