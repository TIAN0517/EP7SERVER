/**
 * @file GameAIProtocol.cpp
 * @brief RANOnline EP7 四部門AI遊戲通訊協議實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "GameAIProtocol.h"
#include <QtCore/QUuid>

namespace RANOnline {
namespace AI {

QString ProtocolUtils::academyToString(Academy academy)
{
    switch (academy) {
        case Academy::SEONGMUN: return "聖門";
        case Academy::HYONRYON: return "懸岩";
        case Academy::BONGCHEON: return "鳳凰";
        default: return "未知";
    }
}

Academy ProtocolUtils::stringToAcademy(const QString &str)
{
    if (str == "聖門") return Academy::SEONGMUN;
    if (str == "懸岩") return Academy::HYONRYON;
    if (str == "鳳凰") return Academy::BONGCHEON;
    return Academy::SEONGMUN; // 預設
}

QString ProtocolUtils::departmentToString(Department dept)
{
    switch (dept) {
        case Department::SWORD: return "劍術";
        case Department::BOW: return "弓術";
        case Department::MARTIAL: return "格鬥";
        case Department::QIGONG: return "氣功";
        default: return "未知";
    }
}

Department ProtocolUtils::stringToDepartment(const QString &str)
{
    if (str == "劍術") return Department::SWORD;
    if (str == "弓術") return Department::BOW;
    if (str == "格鬥") return Department::MARTIAL;
    if (str == "氣功") return Department::QIGONG;
    return Department::SWORD; // 預設
}

QString ProtocolUtils::stateToString(AIState state)
{
    switch (state) {
        case AIState::IDLE: return "idle";
        case AIState::MOVING: return "moving";
        case AIState::FIGHTING: return "fighting";
        case AIState::USING_SKILL: return "using_skill";
        case AIState::DEAD: return "dead";
        case AIState::RETURNING: return "returning";
        default: return "unknown";
    }
}

AIState ProtocolUtils::stringToState(const QString &str)
{
    if (str == "idle") return AIState::IDLE;
    if (str == "moving") return AIState::MOVING;
    if (str == "fighting") return AIState::FIGHTING;
    if (str == "using_skill") return AIState::USING_SKILL;
    if (str == "dead") return AIState::DEAD;
    if (str == "returning") return AIState::RETURNING;
    return AIState::IDLE; // 預設
}

QJsonObject ProtocolUtils::createSpawnRequest(Academy academy, Department department, int count, int teamId)
{
    return QJsonObject{
        {"cmd", Commands::SPAWN_AI},
        {"academy", academyToString(academy)},
        {"department", departmentToString(department)},
        {"count", count},
        {"team", teamId},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
}

QJsonObject ProtocolUtils::createCommandRequest(const QString &aiId, const QString &action, const QJsonObject &params)
{
    QJsonObject request{
        {"cmd", Commands::AI_COMMAND},
        {"ai_id", aiId},
        {"action", action},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
    
    if (!params.isEmpty()) {
        request["params"] = params;
    }
    
    return request;
}

QJsonObject ProtocolUtils::createTeamAssignRequest(const QStringList &aiIds, int teamId)
{
    QJsonArray idsArray;
    for (const QString &id : aiIds) {
        idsArray.append(id);
    }
    
    return QJsonObject{
        {"cmd", Commands::ASSIGN_TEAM},
        {"ai_ids", idsArray},
        {"team", teamId},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
}

QJsonObject ProtocolUtils::createStatusRequest(const QStringList &aiIds)
{
    QJsonObject request{
        {"cmd", Commands::GET_STATUS},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
    
    if (!aiIds.isEmpty()) {
        QJsonArray idsArray;
        for (const QString &id : aiIds) {
            idsArray.append(id);
        }
        request["ai_ids"] = idsArray;
    }
    
    return request;
}

QJsonObject ProtocolUtils::createBatchRequest(const QJsonArray &operations)
{
    return QJsonObject{
        {"cmd", Commands::BATCH_OPERATION},
        {"operations", operations},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()},
        {"request_id", QUuid::createUuid().toString(QUuid::WithoutBraces)}
    };
}

QJsonObject ProtocolUtils::createSuccessResponse(const QJsonObject &data)
{
    QJsonObject response{
        {"status", "success"},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
    
    if (!data.isEmpty()) {
        response["data"] = data;
    }
    
    return response;
}

QJsonObject ProtocolUtils::createErrorResponse(const QString &error, int code)
{
    return QJsonObject{
        {"status", "error"},
        {"error", error},
        {"error_code", code},
        {"timestamp", QDateTime::currentMSecsSinceEpoch()}
    };
}

} // namespace AI
} // namespace RANOnline
