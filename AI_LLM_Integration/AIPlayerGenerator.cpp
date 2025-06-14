/**
 * @file AIPlayerGenerator.cpp
 * @brief RAN Online四部門AI玩家批量生成器實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "AIPlayerGenerator.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QUuid>

namespace RANOnline {
namespace AI {

AIPlayerGenerator::AIPlayerGenerator(QObject *parent)
    : QObject(parent)
    , m_random(QRandomGenerator::global())
{
    initializeDataTables();
}

void AIPlayerGenerator::initializeDataTables()
{
    // 學院和部門初始化
    m_academies = {"聖門", "懸岩", "鳳凰"};
    m_departments = {"劍道", "弓箭", "格鬥", "氣功"};
      // 姓名詞庫初始化 - 根據學院特色主題設計
    // 聖門學院：God系列主題，威嚴神聖風格（6字符限制）
    m_nameComponents["聖門"] = {
        "God食", "God餐", "God醒", "God戰", "God光", "God力", "God威", "God聖",
        "God護", "God劍", "God箭", "God拳", "God愛", "God慈", "God仁", "God義"
    };
    
    // 懸岩學院：水墨風格主題，詩意文雅風格（6字符限制）
    m_nameComponents["懸岩"] = {
        "水墨風", "水墨畫", "水墨詩", "水墨夢", "水墨雲", "水墨月", "水墨雨", "水墨雪",
        "水墨山", "水墨竹", "水墨蘭", "水墨梅", "水墨菊", "水墨松", "水墨石", "水墨泉"
    };
    
    // 鳳凰學院：心情感系列主題，情感豐富風格（6字符限制）
    m_nameComponents["鳳凰"] = {
        "心被傷害", "心被偷走", "心被感動", "心被震撼", "心被溫暖", "心被觸動",
        "心中有愛", "心中有夢", "心中有火", "心中有光", "心如烈火", "心如鳳凰",
        "心懷夢想", "心懷熱情", "心懷勇氣", "心懷希望"
    };
      // 性格特徵詞庫 - 符合學院主題
    m_personalities["聖門"] = {
        "神聖威嚴", "正義守護", "光明庇佑", "仁慈威嚴", "聖潔高貴",
        "威武神聖", "神威凜然", "光明正大", "正義之心", "神聖使命"
    };
    
    m_personalities["懸岩"] = {
        "水墨雅致", "詩意飄逸", "文雅內斂", "寧靜致遠", "淡泊明志",
        "雲淡風輕", "如水靈動", "墨香書韻", "文人風骨", "詩心畫意"
    };
    
    m_personalities["鳳凰"] = {
        "情感豐富", "熱情似火", "心懷大愛", "感情真摯", "情深意重",
        "心靈敏感", "情緒飽滿", "愛恨分明", "真情流露", "情意綿綿"
    };
    
    // 戰鬥風格詞庫
    m_combatStyles["劍道"] = {
        "近戰主坦", "劍術精通", "攻守兼備", "快劍如風", "重劍無鋒",
        "劍氣縱橫", "一劍封喉", "劍舞天下", "劍意無雙", "劍心通明"
    };
    
    m_combatStyles["弓箭"] = {
        "遠程狙擊", "精準射擊", "快速連射", "爆發輸出", "風箭如虹",
        "百步穿楊", "箭如流星", "穿雲裂石", "神箭手", "箭雨漫天"
    };
    
    m_combatStyles["格鬥"] = {
        "近身格鬥", "連擊專家", "暴擊流", "體術大師", "鐵拳無敵",
        "拳拳到肉", "格鬥之王", "身手敏捷", "拳法精湛", "徒手奪白刃"
    };
    
    m_combatStyles["氣功"] = {
        "治療輔助", "Buff專家", "群體增益", "精神控制", "氣息調理",
        "內功深厚", "氣貫長虹", "回復大師", "輔助之神", "氣功宗師"
    };
      // 專屬台詞詞庫 - 符合學院主題風格
    m_signatureLines["聖門_劍道"] = {
        "God之劍威！", "神聖審判！", "God力斬邪！", "聖劍護世！", "God威無敵！"
    };
    
    m_signatureLines["聖門_弓箭"] = {
        "God之神箭！", "聖光引導！", "God力射擊！", "神聖一擊！", "God箭無虛發！"
    };
    
    m_signatureLines["聖門_格鬥"] = {
        "God之鐵拳！", "聖拳制裁！", "God力無窮！", "神聖格鬥！", "God拳破邪！"
    };
    
    m_signatureLines["聖門_氣功"] = {
        "God之恩惠！", "聖光庇護！", "God力治癒！", "神聖祝福！", "God恩浩蕩！"
    };
    
    m_signatureLines["懸岩_劍道"] = {
        "水墨劍舞！", "詩劍飄逸！", "墨劍如虹！", "雅劍無聲！", "文劍致遠！"
    };
    
    m_signatureLines["懸岩_弓箭"] = {
        "水墨之箭！", "詩意射擊！", "墨箭如詩！", "雅射無聲！", "文箭飄逸！"
    };
    
    m_signatureLines["懸岩_格鬥"] = {
        "水墨拳意！", "詩拳如畫！", "墨拳飄逸！", "雅拳無聲！", "文武雙全！"
    };
    
    m_signatureLines["懸岩_氣功"] = {
        "水墨療癒！", "詩意回復！", "墨氣如雲！", "雅氣飄逸！", "文氣致遠！"
    };
    
    m_signatureLines["鳳凰_劍道"] = {
        "心劍合一！", "情劍飛舞！", "愛劍無敵！", "心中烈火！", "情深劍意！"
    };
    
    m_signatureLines["鳳凰_弓箭"] = {
        "心箭如虹！", "情箭穿心！", "愛箭無敵！", "心中火箭！", "情深箭意！"
    };
    
    m_signatureLines["鳳凰_格鬥"] = {
        "心拳如火！", "情拳炸裂！", "愛拳無敵！", "心中烈拳！", "情深拳意！"
    };
    
    m_signatureLines["鳳凰_氣功"] = {
        "心靈治癒！", "情感回復！", "愛的力量！", "心中溫暖！", "情深氣厚！"
    };
    
    // 技能資料庫初始化
    initializeSkillDatabase();
}

void AIPlayerGenerator::initializeSkillDatabase()
{
    // 劍道技能
    m_skillDatabase["斬擊"] = QJsonObject{
        {"cooldown", 0}, {"mpCost", 30}, {"damage", 120}, {"range", 1}
    };
    m_skillDatabase["破空"] = QJsonObject{
        {"cooldown", 5}, {"mpCost", 50}, {"damage", 200}, {"range", 2}
    };
    m_skillDatabase["聖光斬"] = QJsonObject{
        {"cooldown", 15}, {"mpCost", 100}, {"damage", 350}, {"range", 3}
    };
    
    // 弓箭技能
    m_skillDatabase["連射"] = QJsonObject{
        {"cooldown", 0}, {"mpCost", 25}, {"damage", 80}, {"range", 8}
    };
    m_skillDatabase["穿透射擊"] = QJsonObject{
        {"cooldown", 8}, {"mpCost", 60}, {"damage", 180}, {"range", 10}
    };
    m_skillDatabase["箭雨"] = QJsonObject{
        {"cooldown", 20}, {"mpCost", 120}, {"damage", 250}, {"range", 12}
    };
    
    // 格鬥技能
    m_skillDatabase["連擊"] = QJsonObject{
        {"cooldown", 0}, {"mpCost", 20}, {"damage", 90}, {"range", 1}
    };
    m_skillDatabase["重拳"] = QJsonObject{
        {"cooldown", 6}, {"mpCost", 45}, {"damage", 160}, {"range", 1}
    };
    m_skillDatabase["狂暴"] = QJsonObject{
        {"cooldown", 25}, {"mpCost", 80}, {"damage", 300}, {"range", 1}
    };
    
    // 氣功技能
    m_skillDatabase["治癒"] = QJsonObject{
        {"cooldown", 3}, {"mpCost", 40}, {"healing", 150}, {"range", 5}
    };
    m_skillDatabase["群體治癒"] = QJsonObject{
        {"cooldown", 12}, {"mpCost", 80}, {"healing", 120}, {"range", 8}
    };
    m_skillDatabase["聖光庇護"] = QJsonObject{
        {"cooldown", 30}, {"mpCost", 100}, {"shield", 300}, {"range", 6}
    };
}

QJsonArray AIPlayerGenerator::generateAIPlayers(int count)
{
    QJsonArray players;
    
    for (int i = 0; i < count; ++i) {
        AIPlayerData player = generateSingleAI();
        
        QJsonObject playerJson{
            {"academy", player.academy},
            {"department", player.department},
            {"name", player.name},
            {"personality", player.personality},
            {"combat_style", player.combatStyle},
            {"signature_line", player.signatureLine},
            {"ai_id", player.aiId},
            {"hp", player.hp},
            {"mp", player.mp},
            {"buffs", QJsonArray::fromStringList(player.buffs)},
            {"state", player.state}
        };
        
        players.append(playerJson);
        
        emit aiPlayerGenerated(player);
        emit logGenerated(QString("[%1] 生成AI玩家: %2 (%3-%4)")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                         .arg(player.name).arg(player.academy).arg(player.department));
    }
    
    return players;
}

AIPlayerData AIPlayerGenerator::generateSingleAI()
{
    AIPlayerData player;
    
    // 隨機選擇學院和部門
    player.academy = m_academies[m_random->bounded(m_academies.size())];
    player.department = m_departments[m_random->bounded(m_departments.size())];
    
    // 生成姓名
    player.name = generateName(player.academy, player.department);
    
    // 生成性格特徵
    player.personality = generatePersonality(player.academy);
    
    // 生成戰鬥風格
    player.combatStyle = generateCombatStyle(player.department);
    
    // 生成專屬台詞
    player.signatureLine = generateSignatureLine(player.academy, player.department);
    
    // 生成AI ID
    player.aiId = generateAIId();
    
    // 生成基礎屬性
    player.hp = m_random->bounded(4000, 8000);
    player.mp = m_random->bounded(1000, 2000);
    
    // 生成Buff狀態
    QStringList possibleBuffs = {"護盾", "加速", "強化", "治癒", "防護"};
    int buffCount = m_random->bounded(0, 3);
    for (int i = 0; i < buffCount; ++i) {
        QString buff = possibleBuffs[m_random->bounded(possibleBuffs.size())];
        if (!player.buffs.contains(buff)) {
            player.buffs.append(buff);
        }
    }
    
    // 設定初始狀態
    player.state = "正常";
    
    return player;
}

QString AIPlayerGenerator::generateName(const QString &academy, const QString &department)
{
    QStringList components = m_nameComponents[academy];
    if (components.isEmpty()) {
        return QString("AI_%1").arg(QDateTime::currentMSecsSinceEpoch() % 10000);
    }
    
    // 新命名系統：直接使用學院主題名字，已符合6字符限制
    QString baseName = components[m_random->bounded(components.size())];
    
    // 根據部門添加簡短後綴以區分職業（保持6字符限制）
    QString suffix;
    if (department == "劍道") {
        QStringList swordSuffixes = {"劍", "斬", "刃", "鋒"};
        suffix = swordSuffixes[m_random->bounded(swordSuffixes.size())];
    } else if (department == "弓箭") {
        QStringList bowSuffixes = {"弓", "矢", "箭", "射"};
        suffix = bowSuffixes[m_random->bounded(bowSuffixes.size())];
    } else if (department == "格鬥") {
        QStringList fightSuffixes = {"拳", "武", "戰", "鬥"};
        suffix = fightSuffixes[m_random->bounded(fightSuffixes.size())];
    } else if (department == "氣功") {
        QStringList qigongSuffixes = {"氣", "功", "法", "靈"};
        suffix = qigongSuffixes[m_random->bounded(qigongSuffixes.size())];
    }
    
    // 為避免超過6字符限制，某些組合需要特殊處理
    QString finalName;
    if (baseName.length() >= 6) {
        // 如果基礎名字已經是6字符，直接使用
        finalName = baseName;
    } else if (baseName.length() + suffix.length() <= 6) {
        // 如果加上後綴不超過6字符，則添加後綴
        finalName = baseName + suffix;
    } else {
        // 如果會超過限制，只使用基礎名字
        finalName = baseName;
    }
    
    return finalName;
}

QString AIPlayerGenerator::generatePersonality(const QString &academy)
{
    QStringList personalities = m_personalities[academy];
    if (personalities.isEmpty()) {
        return "神秘莫測";
    }
    return personalities[m_random->bounded(personalities.size())];
}

QString AIPlayerGenerator::generateCombatStyle(const QString &department)
{
    QStringList styles = m_combatStyles[department];
    if (styles.isEmpty()) {
        return "全能戰士";
    }
    return styles[m_random->bounded(styles.size())];
}

QString AIPlayerGenerator::generateSignatureLine(const QString &academy, const QString &department)
{
    QString key = QString("%1_%2").arg(academy).arg(department);
    QStringList lines = m_signatureLines[key];
    if (lines.isEmpty()) {
        return "戰鬥吧！";
    }
    return lines[m_random->bounded(lines.size())];
}

QString AIPlayerGenerator::generateAIId()
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    QString randomPart = QString::number(m_random->bounded(1000, 9999));
    return QString("AI%1_%2").arg(timestamp.right(8)).arg(randomPart);
}

AIDecisionResponse AIPlayerGenerator::makeDecision(const AIDecisionRequest &request)
{
    AIDecisionResponse response;
    response.isValid = false;
    
    try {
        // 檢查AI狀態
        if (request.hp <= 0) {
            response.errorMessage = "AI已死亡，無法行動";
            return response;
        }
        
        if (request.state == "暈眩" || request.state == "封印") {
            response.errorMessage = QString("AI處於%1狀態，無法行動").arg(request.state);
            return response;
        }
        
        // 根據部門選擇技能
        QStringList availableSkills;
        if (request.department == "劍道") {
            availableSkills = {"斬擊", "破空", "聖光斬"};
        } else if (request.department == "弓箭") {
            availableSkills = {"連射", "穿透射擊", "箭雨"};
        } else if (request.department == "格鬥") {
            availableSkills = {"連擊", "重拳", "狂暴"};
        } else if (request.department == "氣功") {
            availableSkills = {"治癒", "群體治癒", "聖光庇護"};
        }
        
        // 選擇可用技能
        QString selectedSkill;
        for (const QString &skill : availableSkills) {
            if (isSkillAvailable(skill, request.skillCooldowns, request.mp)) {
                selectedSkill = skill;
                break;
            }
        }
        
        if (selectedSkill.isEmpty()) {
            response.errorMessage = "沒有可用技能";
            return response;
        }
        
        // 選擇目標
        QString target;
        if (request.department == "氣功") {
            // 氣功優先治療友方
            if (!request.allies.isEmpty()) {
                QJsonObject ally = request.allies[0].toObject();
                target = ally["id"].toString();
                response.action = "治癒友方";
            }
        } else {
            // 攻擊職業優先攻擊敵方
            if (!request.enemies.isEmpty()) {
                target = selectBestTarget(request.enemies, "攻擊");
                response.action = "施放技能";
            }
        }
        
        if (target.isEmpty()) {
            response.errorMessage = "沒有有效目標";
            return response;
        }
        
        // 設定回應
        response.skill = selectedSkill;
        response.target = target;
        response.reason = QString("%1冷卻結束，對%2執行最優策略").arg(selectedSkill).arg(target);
        response.isValid = true;
        
        // 記錄決策日誌
        QString logEntry = QString("[%1] AI:%2 決策：使用%3對%4，原因：%5")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(request.aiId).arg(selectedSkill).arg(target).arg(response.reason);
        emit logGenerated(logEntry);
        
    } catch (const std::exception &e) {
        response.errorMessage = QString("決策引擎錯誤：%1").arg(e.what());
        emit errorOccurred(response.errorMessage);
    }
    
    return response;
}

bool AIPlayerGenerator::isSkillAvailable(const QString &skill, const QJsonObject &cooldowns, int mp)
{
    // 檢查技能冷卻
    if (cooldowns.contains(skill) && cooldowns[skill].toInt() > 0) {
        return false;
    }
    
    // 檢查法力消耗
    if (m_skillDatabase.contains(skill)) {
        QJsonObject skillData = m_skillDatabase[skill];
        int mpCost = skillData["mpCost"].toInt();
        if (mp < mpCost) {
            return false;
        }
    }
    
    return true;
}

QString AIPlayerGenerator::selectBestTarget(const QJsonArray &enemies, const QString &strategy)
{
    Q_UNUSED(strategy)
    if (enemies.isEmpty()) {
        return QString();
    }
    
    // 簡單策略：選擇血量最低的敵人
    QString bestTarget;
    int lowestHp = INT_MAX;
    
    for (const QJsonValue &value : enemies) {
        QJsonObject enemy = value.toObject();
        int hp = enemy["hp"].toInt();
        if (hp < lowestHp) {
            lowestHp = hp;
            bestTarget = enemy["id"].toString();
        }
    }
    
    return bestTarget;
}

QJsonObject AIPlayerGenerator::generateTeamStrategy(const QJsonObject &battleSituation)
{
    QJsonObject strategy;
    
    try {
        int enemyCount = battleSituation["enemy_count"].toInt();
        QString terrain = battleSituation["terrain"].toString();
        QJsonArray team = battleSituation["team"].toArray();
        QJsonObject resources = battleSituation["resources"].toObject();
        
        // 基於敵人數量調整策略
        if (enemyCount > 3) {
            strategy["swordman"] = "集中防守，保護後排";
            strategy["archer"] = "優先消滅敵方治療";
            strategy["fighter"] = "分散敵方注意力";
            strategy["qigong"] = "持續群體治療";
        } else {
            strategy["swordman"] = "快速接近敵方主力，優先控制遠程";
            strategy["archer"] = "保持距離狙擊敵方氣功";
            strategy["fighter"] = "貼身牽制敵方副坦";
            strategy["qigong"] = "優先群體治療與Buff全隊";
        }
        
        // 根據地形調整
        if (terrain.contains("山地")) {
            strategy["archer"] = strategy["archer"].toString() + "，利用地形優勢";
        }
        
        QString logEntry = QString("[%1] 團隊策略制定完成：敵人數量%2，地形%3")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(enemyCount).arg(terrain);
        emit logGenerated(logEntry);
        
    } catch (const std::exception &e) {
        QString error = QString("團隊策略制定錯誤：%1").arg(e.what());
        emit errorOccurred(error);
    }
    
    return strategy;
}

QJsonArray AIPlayerGenerator::generateTestData(int count)
{
    QJsonArray testData;
    
    for (int i = 0; i < count; ++i) {
        QJsonObject data;
        
        // 生成隨機AI狀態
        QString aiId = generateAIId();
        QString academy = m_academies[m_random->bounded(m_academies.size())];
        QString department = m_departments[m_random->bounded(m_departments.size())];
        
        data["ai_id"] = aiId;
        data["academy"] = academy;
        data["department"] = department;
        data["hp"] = m_random->bounded(1000, 8000);
        data["mp"] = m_random->bounded(500, 2000);
        
        // 隨機技能冷卻
        QJsonObject cooldowns;
        cooldowns["基礎攻擊"] = m_random->bounded(0, 3);
        cooldowns["特殊技能"] = m_random->bounded(0, 10);
        cooldowns["終極技能"] = m_random->bounded(0, 30);
        data["skill_cooldowns"] = cooldowns;
        
        // 隨機敵人和友方
        QJsonArray enemies, allies;
        int enemyCount = m_random->bounded(1, 5);
        for (int j = 0; j < enemyCount; ++j) {
            QJsonObject enemy;
            enemy["id"] = QString("P%1").arg(QString::number(j + 1).rightJustified(3, '0'));
            enemy["hp"] = m_random->bounded(1000, 6000);
            enemy["distance"] = m_random->bounded(1, 10);
            enemies.append(enemy);
        }
        data["enemies"] = enemies;
        
        int allyCount = m_random->bounded(0, 3);
        for (int j = 0; j < allyCount; ++j) {
            QJsonObject ally;
            ally["id"] = QString("AI%1").arg(QString::number(j + 1).rightJustified(3, '0'));
            ally["hp"] = m_random->bounded(2000, 7000);
            allies.append(ally);
        }
        data["allies"] = allies;
        
        data["state"] = "正常";
        data["map"] = QString("%1之域").arg(academy);
        
        testData.append(data);
    }
    
    emit logGenerated(QString("[%1] 生成%2條測試資料")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(count));
    
    return testData;
}

bool AIPlayerGenerator::exportAILogs(const QString &filePath, const QJsonArray &logs)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred(QString("無法打開檔案：%1").arg(filePath));
        return false;
    }
      QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    
    // 寫入CSV標頭
    stream << "時間,AI_ID,動作,技能,目標,結果,回應時間,錯誤訊息\n";
    
    // 寫入日誌資料
    for (const QJsonValue &value : logs) {
        QJsonObject log = value.toObject();
        stream << QString("%1,%2,%3,%4,%5,%6,%7,%8\n")
                  .arg(log["timestamp"].toString())
                  .arg(log["ai_id"].toString())
                  .arg(log["action"].toString())
                  .arg(log["skill"].toString())
                  .arg(log["target"].toString())
                  .arg(log["result"].toString())
                  .arg(log["response_time"].toInt())
                  .arg(log["error"].toString());
    }
    
    file.close();
    
    emit logGenerated(QString("[%1] AI日誌導出完成：%2")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(filePath));
    
    return true;
}

} // namespace AI
} // namespace RANOnline
