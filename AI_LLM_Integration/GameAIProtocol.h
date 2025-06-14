/**
 * @file GameAIProtocol.h
 * @brief RANOnline EP7 四部門AI遊戲通訊協議定義
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QDateTime>

namespace RANOnline {
namespace AI {

/**
 * @brief 學院枚舉
 */
enum class Academy {
    SEONGMUN = 1,    // 聖門
    HYONRYON = 2,    // 懸岩  
    BONGCHEON = 3    // 鳳凰
};

/**
 * @brief 部門枚舉
 */
enum class Department {
    SWORD = 1,      // 劍術
    BOW = 2,        // 弓術
    MARTIAL = 3,    // 格鬥
    QIGONG = 4      // 氣功
};

/**
 * @brief AI狀態枚舉
 */
enum class AIState {
    IDLE = 0,          // 空閒
    MOVING = 1,        // 移動中
    FIGHTING = 2,      // 戰鬥中
    USING_SKILL = 3,   // 使用技能
    DEAD = 4,          // 死亡
    RETURNING = 5      // 返回基地
};

/**
 * @brief 座標結構
 */
struct Position {
    int x = 0;
    int y = 0;
    int z = 0;
    
    QJsonObject toJson() const {
        return QJsonObject{
            {"x", x},
            {"y", y}, 
            {"z", z}
        };
    }
    
    static Position fromJson(const QJsonObject &obj) {
        return Position{
            obj["x"].toInt(),
            obj["y"].toInt(),
            obj["z"].toInt()
        };
    }
};

/**
 * @brief AI角色信息
 */
struct AICharacterInfo {
    QString aiId;
    Academy academy;
    Department department;
    int teamId = 0;
    QString name;
    int level = 1;
    int hp = 1000;
    int maxHp = 1000;
    int mp = 500;
    int maxMp = 500;
    Position position;
    AIState state = AIState::IDLE;
    QDateTime lastUpdate;
    
    QJsonObject toJson() const {
        return QJsonObject{
            {"ai_id", aiId},
            {"academy", static_cast<int>(academy)},
            {"department", static_cast<int>(department)},
            {"team_id", teamId},
            {"name", name},
            {"level", level},
            {"hp", hp},
            {"max_hp", maxHp},
            {"mp", mp},
            {"max_mp", maxMp},
            {"position", position.toJson()},
            {"state", static_cast<int>(state)},
            {"last_update", lastUpdate.toString(Qt::ISODate)}
        };
    }
    
    static AICharacterInfo fromJson(const QJsonObject &obj) {
        AICharacterInfo info;
        info.aiId = obj["ai_id"].toString();
        info.academy = static_cast<Academy>(obj["academy"].toInt());
        info.department = static_cast<Department>(obj["department"].toInt());
        info.teamId = obj["team_id"].toInt();
        info.name = obj["name"].toString();
        info.level = obj["level"].toInt();
        info.hp = obj["hp"].toInt();
        info.maxHp = obj["max_hp"].toInt();
        info.mp = obj["mp"].toInt();
        info.maxMp = obj["max_mp"].toInt();
        info.position = Position::fromJson(obj["position"].toObject());
        info.state = static_cast<AIState>(obj["state"].toInt());
        info.lastUpdate = QDateTime::fromString(obj["last_update"].toString(), Qt::ISODate);
        return info;
    }
};

/**
 * @brief 遊戲通訊協議命令
 */
namespace Commands {
    // AI批量生成
    constexpr const char* SPAWN_AI = "spawn_ai";
    
    // AI動作指令
    constexpr const char* AI_COMMAND = "ai_command";
    
    // AI隊伍分配
    constexpr const char* ASSIGN_TEAM = "assign_team";
    
    // 查詢AI狀態
    constexpr const char* GET_STATUS = "get_status";
    
    // AI刪除
    constexpr const char* DELETE_AI = "delete_ai";
    
    // 批量操作
    constexpr const char* BATCH_OPERATION = "batch_operation";
    
    // 系統控制
    constexpr const char* SYSTEM_CONTROL = "system_control";
}

/**
 * @brief 動作類型
 */
namespace Actions {
    constexpr const char* MOVE = "move";
    constexpr const char* ATTACK = "attack";
    constexpr const char* USE_SKILL = "use_skill";
    constexpr const char* PATROL = "patrol";
    constexpr const char* RETURN_BASE = "return_base";
    constexpr const char* FOLLOW = "follow";
    constexpr const char* GUARD = "guard";
}

/**
 * @brief 工具函數
 */
class ProtocolUtils {
public:
    static QString academyToString(Academy academy);
    static Academy stringToAcademy(const QString &str);
    
    static QString departmentToString(Department dept);
    static Department stringToDepartment(const QString &str);
    
    static QString stateToString(AIState state);
    static AIState stringToState(const QString &str);
    
    static QJsonObject createSpawnRequest(Academy academy, Department department, 
                                        int count, int teamId = 0);
    
    static QJsonObject createCommandRequest(const QString &aiId, const QString &action, 
                                          const QJsonObject &params = QJsonObject());
    
    static QJsonObject createTeamAssignRequest(const QStringList &aiIds, int teamId);
    
    static QJsonObject createStatusRequest(const QStringList &aiIds = QStringList());
    
    static QJsonObject createBatchRequest(const QJsonArray &operations);
    
    static QJsonObject createSuccessResponse(const QJsonObject &data = QJsonObject());
    
    static QJsonObject createErrorResponse(const QString &error, int code = 1);
};

} // namespace AI
} // namespace RANOnline
