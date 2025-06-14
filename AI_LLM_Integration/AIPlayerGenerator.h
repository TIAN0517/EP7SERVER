/**
 * @file AIPlayerGenerator.h
 * @brief RAN Online四部門AI玩家批量生成器
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDateTime>
#include "GameAIProtocol.h"

namespace RANOnline {
namespace AI {

/**
 * @brief AI玩家資料結構
 */
struct AIPlayerData {
    QString academy;           // 學院
    QString department;        // 部門
    QString name;             // 姓名
    QString personality;      // 性格特徵
    QString combatStyle;      // 戰鬥風格
    QString signatureLine;    // 專屬台詞
    QString aiId;            // AI唯一ID
    int hp;                  // 血量
    int mp;                  // 法力
    QStringList buffs;       // Buff狀態
    QString state;           // 當前狀態
};

/**
 * @brief AI決策請求結構
 */
struct AIDecisionRequest {
    QString aiId;
    QString academy;
    QString department;
    int hp;
    int mp;
    QJsonObject skillCooldowns;
    QStringList buffs;
    QJsonArray enemies;
    QJsonArray allies;
    QString map;
    QString state;
};

/**
 * @brief AI決策回應結構
 */
struct AIDecisionResponse {
    QString action;
    QString skill;
    QString target;
    QString reason;
    bool isValid;
    QString errorMessage;
};

/**
 * @brief AI玩家生成器類
 */
class AIPlayerGenerator : public QObject
{
    Q_OBJECT

public:
    explicit AIPlayerGenerator(QObject *parent = nullptr);
    
    // 批量生成AI玩家
    QJsonArray generateAIPlayers(int count);
    
    // 生成單個AI玩家
    AIPlayerData generateSingleAI();
    
    // AI決策引擎
    AIDecisionResponse makeDecision(const AIDecisionRequest &request);
    
    // 團隊策略制定
    QJsonObject generateTeamStrategy(const QJsonObject &battleSituation);
    
    // 生成測試資料
    QJsonArray generateTestData(int count);
    
    // 導出AI日誌
    bool exportAILogs(const QString &filePath, const QJsonArray &logs);

signals:
    void aiPlayerGenerated(const AIPlayerData &player);
    void decisionMade(const QString &aiId, const AIDecisionResponse &decision);
    void logGenerated(const QString &logEntry);
    void errorOccurred(const QString &error);

private:
    // 姓名生成器
    QString generateName(const QString &academy, const QString &department);
    
    // 性格特徵生成
    QString generatePersonality(const QString &academy);
    
    // 戰鬥風格生成
    QString generateCombatStyle(const QString &department);
    
    // 專屬台詞生成
    QString generateSignatureLine(const QString &academy, const QString &department);
    
    // 技能可用性檢查
    bool isSkillAvailable(const QString &skill, const QJsonObject &cooldowns, int mp);
    
    // 選擇最優目標
    QString selectBestTarget(const QJsonArray &enemies, const QString &strategy);
    
    // 生成AI ID
    QString generateAIId();
    
    // 初始化資料表
    void initializeDataTables();
    void initializeSkillDatabase();

private:
    // 學院列表
    QStringList m_academies;
    
    // 部門列表
    QStringList m_departments;
    
    // 姓名詞庫
    QHash<QString, QStringList> m_nameComponents;
    
    // 性格特徵詞庫
    QHash<QString, QStringList> m_personalities;
    
    // 戰鬥風格詞庫
    QHash<QString, QStringList> m_combatStyles;
    
    // 台詞詞庫
    QHash<QString, QStringList> m_signatureLines;
    
    // 技能資料庫
    QHash<QString, QJsonObject> m_skillDatabase;
    
    // AI決策日誌
    QJsonArray m_decisionLogs;
    
    // 隨機生成器
    QRandomGenerator *m_random;
};

} // namespace AI
} // namespace RANOnline
