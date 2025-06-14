#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RANOnline AI 四部門系統 - 最終部署驗證
檢查所有組件是否正確配置和準備就緒
"""

import json
import os
from pathlib import Path
import subprocess
import sys

class DeploymentValidator:
    def __init__(self):
        self.base_path = Path(__file__).parent.parent
        self.errors = []
        self.warnings = []
        self.passed_checks = []
    
    def check_file_exists(self, file_path, description):
        """檢查文件是否存在"""
        full_path = self.base_path / file_path
        if full_path.exists():
            self.passed_checks.append(f"✅ {description}: {file_path}")
            return True
        else:
            self.errors.append(f"❌ 缺少文件 - {description}: {file_path}")
            return False
    
    def check_json_valid(self, file_path, description):
        """檢查JSON文件是否有效"""
        full_path = self.base_path / file_path
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            # 檢查四部門配置
            if 'llm_config' in data and 'departments' in data['llm_config']:
                departments = data['llm_config']['departments']
                expected_depts = ['劍術', '弓術', '格鬥', '氣功']
                if departments == expected_depts:
                    self.passed_checks.append(f"✅ {description}: 四部門配置正確")
                else:
                    self.warnings.append(f"⚠️  {description}: 部門配置可能不完整")
            
            return True
        except Exception as e:
            self.errors.append(f"❌ JSON文件錯誤 - {description}: {e}")
            return False
    
    def check_python_syntax(self, file_path, description):
        """檢查Python文件語法"""
        full_path = self.base_path / file_path
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                compile(f.read(), str(full_path), 'exec')
            self.passed_checks.append(f"✅ {description}: Python語法正確")
            return True
        except SyntaxError as e:
            self.errors.append(f"❌ Python語法錯誤 - {description}: {e}")
            return False
        except Exception as e:
            self.warnings.append(f"⚠️  {description}: {e}")
            return False
    
    def check_environment(self):
        """檢查運行環境"""
        print("🔍 檢查運行環境...")
        
        # 檢查Python版本
        version = sys.version_info
        if version.major >= 3 and version.minor >= 7:
            self.passed_checks.append(f"✅ Python版本: {version.major}.{version.minor}.{version.micro}")
        else:
            self.errors.append(f"❌ Python版本過低: {version.major}.{version.minor}.{version.micro} (需要3.7+)")
        
        # 檢查CMake
        try:
            result = subprocess.run(['cmake', '--version'], capture_output=True, text=True)
            if result.returncode == 0:
                self.passed_checks.append("✅ CMake已安裝")
            else:
                self.warnings.append("⚠️  CMake可能未正確安裝")
        except FileNotFoundError:
            self.warnings.append("⚠️  未找到CMake (構建Qt6應用程式時需要)")
    
    def validate_four_departments_config(self):
        """驗證四部門配置完整性"""
        print("🎯 驗證四部門配置...")
        
        # 檢查核心配置文件
        config_files = [
            ("Config/llm.json", "四部門核心配置"),
            ("Config/four_dept_templates.json", "四部門模板庫"),
            ("Config/academy_themes.json", "學院主題配色"),
            ("Config/four_departments_database.sql", "四部門資料庫建表")
        ]
        
        for file_path, description in config_files:
            if self.check_file_exists(file_path, description):
                if file_path.endswith('.json'):
                    self.check_json_valid(file_path, description)
    
    def validate_automation_tools(self):
        """驗證自動化工具"""
        print("🛠️  驗證自動化工具...")
        
        script_files = [
            ("Scripts/export_prompts.py", "完整提示詞導出工具"),
            ("Scripts/export_prompts_simple.py", "簡化提示詞導出工具"),
            ("Scripts/test_four_dept.py", "四部門實時測試工具"),
            ("Scripts/export_prompts.bat", "一鍵批次執行"),
            ("Scripts/deploy_four_dept.bat", "完整部署工具")
        ]
        
        for file_path, description in script_files:
            if self.check_file_exists(file_path, description):
                if file_path.endswith('.py'):
                    self.check_python_syntax(file_path, description)
    
    def validate_qt6_components(self):
        """驗證Qt6組件"""
        print("🖥️  驗證Qt6組件...")
        
        qt_files = [
            ("AcademyTheme.h", "學院主題和數據結構"),
            ("FramelessWindow.h", "無邊框窗口基類"),
            ("FramelessWindow.cpp", "無邊框窗口實現"),
            ("CyberProgressBar.h", "科技感進度條頭文件"),
            ("CyberProgressBar.cpp", "科技感進度條實現"),
            ("LLMRequestItem.h", "請求項目組件頭文件"),
            ("LLMRequestItem.cpp", "請求項目組件實現"),
            ("LLMMainWindow.h", "主窗口頭文件"),
            ("LLMMainWindow.cpp", "主窗口實現"),
            ("OllamaLLMManager.h", "LLM管理器"),
            ("main.cpp", "應用程式入口點"),
            ("CMakeLists.txt", "CMake構建配置")
        ]
        
        for file_path, description in qt_files:
            self.check_file_exists(file_path, description)
    
    def validate_exported_content(self):
        """驗證導出的內容"""
        print("📝 驗證導出內容...")
        
        export_files = [
            ("exports/ai_prompts_export_four_dept.md", "四部門提示詞模板"),
            ("exports/README.md", "快速使用指南")
        ]
        
        for file_path, description in export_files:
            self.check_file_exists(file_path, description)
    
    def test_export_functionality(self):
        """測試導出功能"""
        print("🧪 測試導出功能...")
        
        try:
            # 嘗試運行簡化版導出工具
            script_path = self.base_path / "Scripts" / "export_prompts_simple.py"
            result = subprocess.run([sys.executable, str(script_path)], 
                                  capture_output=True, text=True, cwd=str(script_path.parent))
            
            if result.returncode == 0:
                self.passed_checks.append("✅ 提示詞導出功能測試通過")
            else:
                self.warnings.append(f"⚠️  提示詞導出測試警告: {result.stderr}")
                
        except Exception as e:
            self.warnings.append(f"⚠️  無法測試導出功能: {e}")
    
    def generate_report(self):
        """生成驗證報告"""
        print("\n" + "="*80)
        print("📊 RANOnline AI 四部門系統 - 部署驗證報告")
        print("="*80)
        
        print(f"\n✅ 通過檢查 ({len(self.passed_checks)}項):")
        for check in self.passed_checks:
            print(f"  {check}")
        
        if self.warnings:
            print(f"\n⚠️  警告 ({len(self.warnings)}項):")
            for warning in self.warnings:
                print(f"  {warning}")
        
        if self.errors:
            print(f"\n❌ 錯誤 ({len(self.errors)}項):")
            for error in self.errors:
                print(f"  {error}")
        
        print("\n" + "="*80)
        
        # 計算總體狀態
        total_checks = len(self.passed_checks) + len(self.warnings) + len(self.errors)
        success_rate = len(self.passed_checks) / total_checks * 100 if total_checks > 0 else 0
        
        if len(self.errors) == 0:
            if len(self.warnings) == 0:
                status = "🎉 完美！系統已準備就緒"
                color = "綠色"
            else:
                status = "✅ 良好！系統基本準備就緒，有輕微警告"
                color = "黃色"
        else:
            status = "❌ 需要修復！發現嚴重錯誤"
            color = "紅色"
        
        print(f"📈 總體狀態: {status}")
        print(f"📊 成功率: {success_rate:.1f}%")
        print(f"🎨 狀態燈號: {color}")
        
        print("\n🎯 四部門配置摘要:")
        print("   🏫 學院: 聖門、懸岩、鳳凰")
        print("   🥋 部門: 劍術、弓術、格鬥、氣功")
        print("   💻 平台: Qt6 + Python + SQL Server + Ollama")
        print("   🎮 目標: RANOnline EP7 AI整合")
        
        print("\n📋 下一步建議:")
        if len(self.errors) == 0:
            print("   1. 執行 Scripts/deploy_four_dept.bat 進行完整部署")
            print("   2. 啟動 Ollama 服務：ollama serve")
            print("   3. 測試 Qt6 應用程式編譯")
            print("   4. 驗證與 RANOnline 遊戲的整合")
        else:
            print("   1. 修復上述錯誤")
            print("   2. 重新運行此驗證腳本")
            print("   3. 確保所有依賴項已正確安裝")
        
        print(f"\n⏰ 驗證完成時間: {self.get_current_time()}")
        print("👥 開發團隊: Jy技術團隊")
        print("📅 目標完成日期: 2025年6月14日")
        print("="*80)
    
    def get_current_time(self):
        """獲取當前時間"""
        from datetime import datetime
        return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    def run_full_validation(self):
        """運行完整驗證"""
        print("🚀 開始 RANOnline AI 四部門系統部署驗證...")
        print()
        
        self.check_environment()
        self.validate_four_departments_config()
        self.validate_automation_tools()
        self.validate_qt6_components()
        self.validate_exported_content()
        self.test_export_functionality()
        
        self.generate_report()
        
        return len(self.errors) == 0

def main():
    validator = DeploymentValidator()
    success = validator.run_full_validation()
    
    if success:
        print("\n🎉 恭喜！RANOnline AI 四部門系統已準備就緒！")
        return 0
    else:
        print("\n❌ 驗證失敗，請修復錯誤後重試。")
        return 1

if __name__ == "__main__":
    sys.exit(main())
