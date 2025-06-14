/**
 * @file academy_naming_demo.cpp
 * @brief RAN Online學院命名系統演示程序
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 1.0.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

/**
 * @brief 設置控制台編碼（Windows專用）
 */
void setupConsoleEncoding() {
#ifdef _WIN32
    // 設置控制台代碼頁為UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 設置控制台標題
    SetConsoleTitleA("RANOnline Academy Naming Demo - UTF-8 Console");
    
    // 同步C++流與C流
    std::ios::sync_with_stdio(true);
#endif
}

class AcademyNamingDemo {
private:
    std::vector<std::string> shenmen_names = {
        "God食", "God餐", "God醒", "God戰", "God光", "God力", "God威", "God聖",
        "God護", "God劍", "God箭", "God拳", "God愛", "God慈", "God仁", "God義"
    };
    
    std::vector<std::string> xuanyan_names = {
        "水墨風", "水墨畫", "水墨詩", "水墨夢", "水墨雲", "水墨月", "水墨雨", "水墨雪",
        "水墨山", "水墨竹", "水墨蘭", "水墨梅", "水墨菊", "水墨松", "水墨石", "水墨泉"
    };
    
    std::vector<std::string> fenghuang_names = {
        "心被傷害", "心被偷走", "心被感動", "心被震撼", "心被溫暖", "心被觸動",
        "心中有愛", "心中有夢", "心中有火", "心中有光", "心如烈火", "心如鳳凰",
        "心懷夢想", "心懷熱情", "心懷勇氣", "心懷希望"
    };
    
    std::vector<std::string> departments = {"劍道", "弓箭", "格鬥", "氣功"};
    std::vector<std::string> academies = {"聖門", "懸岩", "鳳凰"};
    
    std::random_device rd;
    std::mt19937 gen;
    
public:
    AcademyNamingDemo() : gen(rd()) {}
    
    std::string generateName(const std::string& academy, const std::string& department) {
        std::vector<std::string>* names_pool = nullptr;
        
        if (academy == "聖門") {
            names_pool = &shenmen_names;
        } else if (academy == "懸岩") {
            names_pool = &xuanyan_names;
        } else if (academy == "鳳凰") {
            names_pool = &fenghuang_names;
        }
        
        if (names_pool && !names_pool->empty()) {
            std::uniform_int_distribution<> dis(0, names_pool->size() - 1);
            std::string base_name = (*names_pool)[dis(gen)];
            
            // 添加部門後綴（如果長度允許）
            std::vector<std::string> suffixes;
            if (department == "劍道") suffixes = {"劍", "斬", "刃", "鋒"};
            else if (department == "弓箭") suffixes = {"弓", "矢", "箭", "射"};
            else if (department == "格鬥") suffixes = {"拳", "武", "戰", "鬥"};
            else if (department == "氣功") suffixes = {"氣", "功", "法", "靈"};
            
            if (!suffixes.empty() && base_name.length() < 6) {
                std::uniform_int_distribution<> suffix_dis(0, suffixes.size() - 1);
                std::string suffix = suffixes[suffix_dis(gen)];
                if (base_name.length() + suffix.length() <= 6) {
                    return base_name + suffix;
                }
            }
            
            return base_name;
        }
        
        return "未知AI";
    }
    
    void runDemo() {
        std::cout << "=== RAN Online 四部門AI學院命名系統演示 ===\n";
        std::cout << "測試新的三學院命名規則：\n";
        std::cout << "• 聖門：God主題（威嚴神聖）\n";
        std::cout << "• 懸岩：水墨風格（詩意文雅）\n";
        std::cout << "• 鳳凰：心情感系列（情感豐富）\n\n";
        
        // 每個學院每個部門生成5個AI展示
        for (const auto& academy : academies) {
            std::cout << "=== " << academy << "學院 ===\n";
            
            for (const auto& department : departments) {
                std::cout << "【" << department << "部門】\n";
                
                for (int i = 0; i < 5; ++i) {
                    std::string name = generateName(academy, department);
                    std::cout << "  " << (i+1) << ". " << name 
                             << " (" << academy << "-" << department << ")\n";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        
        // 統計測試
        std::cout << "=== 命名系統統計測試 ===\n";
        std::map<std::string, int> academy_count;
        std::map<int, int> length_count;
        std::vector<std::string> sample_names;
        
        // 生成1000個AI進行統計
        for (int i = 0; i < 1000; ++i) {
            std::uniform_int_distribution<> academy_dis(0, academies.size() - 1);
            std::uniform_int_distribution<> dept_dis(0, departments.size() - 1);
            
            std::string academy = academies[academy_dis(gen)];
            std::string department = departments[dept_dis(gen)];
            std::string name = generateName(academy, department);
            
            academy_count[academy]++;
            length_count[name.length()]++;
            
            if (sample_names.size() < 10) {
                sample_names.push_back(name + " (" + academy + "-" + department + ")");
            }
        }
        
        std::cout << "學院分佈：\n";
        for (const auto& pair : academy_count) {
            std::cout << "  " << pair.first << ": " << pair.second << "名AI\n";
        }
        
        std::cout << "\n名字長度分佈：\n";
        bool all_valid = true;
        for (const auto& pair : length_count) {
            std::cout << "  " << pair.first << "字符: " << pair.second << "個名字\n";
            if (pair.first > 6) all_valid = false;
        }
        
        if (all_valid) {
            std::cout << "\n✅ 所有名字都符合6字符限制！\n";
        } else {
            std::cout << "\n⚠️ 發現超長名字！\n";
        }
        
        std::cout << "\n隨機樣本：\n";
        for (const auto& name : sample_names) {
            std::cout << "  " << name << "\n";
        }
        
        std::cout << "\n=== 測試完成 ===\n";
    }
};

int main() {
    // 設置控制台輸出編碼
    setupConsoleEncoding();
    
    AcademyNamingDemo demo;
    demo.runDemo();
    
    std::cout << "\n按任意鍵退出...";
    std::cin.get();
    
    return 0;
}
