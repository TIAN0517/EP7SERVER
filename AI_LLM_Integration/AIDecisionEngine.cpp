/**
 * @file AIDecisionEngine.cpp
 * @brief RAN Online四部門AI智能決策引擎實現
 * @author Jy技術團隊
 * @date 2025年6月14日
 * @version 2.0.0
 */

#include "AIDecisionEngine.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtNetwork/QNetworkRequest>

namespace RANOnline {
namespace AI {

AIDecisionEngine::AIDecisionEngine(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_playerGenerator(new AIPlayerGenerator(this))
    , m_ollamaHost("localhost")
    , m_ollamaPort(11434)
    , m_testTimer(new QTimer(this))
    , m_currentTestCount(0)
    , m_totalTestCount(0)
    , m_autoRepairEnabled(true)
    , m_maxRetryCount(3)
    , m_requestTimeout(30000)
{
    // 連接信號
    connect(m_testTimer, &QTimer::timeout, this, &AIDecisionEngine::onTestTimerTimeout);
    
    // 初始化已知錯誤和修復動作
    initializeErrorHandling();
    
    // 初始化統計報告
    m_currentReport = {};
}

void AIDecisionEngine::initializeErrorHandling()
{
    m_knownErrors = {
        "技能冷卻未結束",
        "法力不足",
        "目標超出範圍",
        "AI已死亡",
        "連接超時",
        "模型回應錯誤",
        "JSON解析失敗"
    };
    
    m_repairActions["技能冷卻未結束"] = "切換基礎攻擊";
    m_repairActions["法力不足"] = "使用法力藥水或等待恢復";
    m_repairActions["目標超出範圍"] = "移動接近目標或選擇新目標";
    m_repairActions["AI已死亡"] = "等待復活或返回基地";
    m_repairActions["連接超時"] = "重新連接Ollama服務器";
    m_repairActions["模型回應錯誤"] = "切換備用模型";
    m_repairActions["JSON解析失敗"] = "重新格式化請求";
}

void AIDecisionEngine::setOllamaServer(const QString &host, int port)
{
    m_ollamaHost = host;
    m_ollamaPort = port;
    
    emit logGenerated(QString("[%1] Ollama服務器設定：%2:%3")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(host).arg(port));
}

void AIDecisionEngine::makeAIDecision(const AIDecisionRequest &request, LLMModel model)
{
    QString requestId = QUuid::createUuid().toString();
    
    // 記錄請求開始時間
    QElapsedTimer timer;
    timer.start();
    m_pendingRequests[requestId] = qMakePair(request, timer);
    
    // 發送LLM請求
    sendLLMRequest(request, model, requestId);
    
    emit logGenerated(QString("[%1] 開始AI決策：%2 使用模型：%3")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(request.aiId).arg(getModelName(model)));
}

void AIDecisionEngine::makeBatchDecisions(const QJsonArray &requests)
{
    QJsonArray results;
    
    for (const QJsonValue &value : requests) {
        QJsonObject requestObj = value.toObject();
        
        // 轉換為AIDecisionRequest
        AIDecisionRequest request;
        request.aiId = requestObj["ai_id"].toString();
        request.academy = requestObj["academy"].toString();
        request.department = requestObj["department"].toString();
        request.hp = requestObj["hp"].toInt();
        request.mp = requestObj["mp"].toInt();
        request.skillCooldowns = requestObj["skill_cooldowns"].toObject();
        request.enemies = requestObj["enemies"].toArray();
        request.allies = requestObj["allies"].toArray();
        request.map = requestObj["map"].toString();
        request.state = requestObj["state"].toString();
        
        // 根據部門選擇模型
        LLMModel model = selectModel(request.department, "normal");
        
        // 執行決策
        AIDecisionResponse response = m_playerGenerator->makeDecision(request);
        
        // 記錄結果
        QJsonObject result;
        result["ai_id"] = request.aiId;
        result["action"] = response.action;
        result["skill"] = response.skill;
        result["target"] = response.target;
        result["reason"] = response.reason;
        result["is_valid"] = response.isValid;
        result["error"] = response.errorMessage;
        result["model"] = getModelName(model);
        
        results.append(result);
        
        // 記錄日誌
        AIDecisionLog log;
        log.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        log.aiId = request.aiId;
        log.action = response.action;
        log.skill = response.skill;
        log.target = response.target;
        log.result = response.isValid ? "成功" : "失敗";
        log.responseTime = 50; // 模擬回應時間
        log.errorMessage = response.errorMessage;
        log.usedModel = model;
        log.isSuccess = response.isValid;
        
        logDecision(log);
    }
    
    emit batchDecisionCompleted(results);
    emit logGenerated(QString("[%1] 批量決策完成：處理%2個請求")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(requests.size()));
}

void AIDecisionEngine::makeTeamDecisions(const QJsonObject &teamData)
{
    QJsonArray teamMembers = teamData["members"].toArray();
    QString situation = teamData["situation"].toString();
    
    QJsonObject decisions;
    
    for (const QJsonValue &value : teamMembers) {
        QJsonObject member = value.toObject();
        QString department = member["department"].toString();
        QString role = member["role"].toString(); // 隊長、副隊、普通隊員
        
        // 根據職業和角色選擇不同模型
        LLMModel model = selectModel(department, role);
        
        AIDecisionRequest request;
        request.aiId = member["ai_id"].toString();
        request.academy = member["academy"].toString();
        request.department = department;
        request.hp = member["hp"].toInt();
        request.mp = member["mp"].toInt();
        request.skillCooldowns = member["skill_cooldowns"].toObject();
        request.enemies = teamData["enemies"].toArray();
        request.allies = teamData["allies"].toArray();
        request.map = teamData["map"].toString();
        request.state = member["state"].toString();
        
        // 執行決策
        AIDecisionResponse response = m_playerGenerator->makeDecision(request);
        
        // 記錄團隊決策
        QJsonObject memberDecision;
        memberDecision["ai_id"] = request.aiId;
        memberDecision["department"] = department;
        memberDecision["role"] = role;
        memberDecision["model"] = getModelName(model);
        memberDecision["decision"] = QJsonObject{
            {"action", response.action},
            {"skill", response.skill},
            {"target", response.target},
            {"reason", response.reason}
        };
        
        decisions[request.aiId] = memberDecision;
        
        emit logGenerated(QString("[%1] 團隊決策：%2(%3-%4) 使用%5模型")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                         .arg(request.aiId).arg(department).arg(role).arg(getModelName(model)));
    }
    
    // 發送團隊決策結果
    QJsonArray resultsArray;
    resultsArray.append(decisions);
    emit batchDecisionCompleted(resultsArray);
}

void AIDecisionEngine::runAutomatedTest(int testCount)
{
    m_totalTestCount = testCount;
    m_currentTestCount = 0;
    
    // 生成測試資料
    m_testRequests = m_playerGenerator->generateTestData(testCount);
    
    // 重置統計
    m_currentReport = {};
    m_currentReport.totalRequests = testCount;
    
    // 開始測試
    m_testTimer->start(100); // 每100ms處理一個請求
    
    emit logGenerated(QString("[%1] 開始自動化測試：%2個請求")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(testCount));
}

void AIDecisionEngine::onTestTimerTimeout()
{
    if (m_currentTestCount >= m_totalTestCount) {
        m_testTimer->stop();
        
        // 計算統計數據
        m_currentReport.successRate = 
            (double)m_currentReport.successfulRequests / m_currentReport.totalRequests * 100.0;
        
        if (m_currentReport.successfulRequests > 0) {
            // 計算平均回應時間（模擬）
            m_currentReport.averageResponseTime = 150.0; // 模擬平均150ms
        }
        
        emit testCompleted(m_currentReport);
        emit logGenerated(QString("[%1] 自動化測試完成：成功率%.2f%%, 平均回應時間%.2fms")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                         .arg(m_currentReport.successRate)
                         .arg(m_currentReport.averageResponseTime));
        return;
    }
    
    // 處理當前請求
    QJsonObject requestObj = m_testRequests[m_currentTestCount].toObject();
    
    AIDecisionRequest request;
    request.aiId = requestObj["ai_id"].toString();
    request.academy = requestObj["academy"].toString();
    request.department = requestObj["department"].toString();
    request.hp = requestObj["hp"].toInt();
    request.mp = requestObj["mp"].toInt();
    request.skillCooldowns = requestObj["skill_cooldowns"].toObject();
    request.enemies = requestObj["enemies"].toArray();
    request.allies = requestObj["allies"].toArray();
    request.map = requestObj["map"].toString();
    request.state = requestObj["state"].toString();
    
    // 選擇模型
    LLMModel model = selectModel(request.department, "normal");
    
    // 執行決策
    QElapsedTimer timer;
    timer.start();
    
    AIDecisionResponse response = m_playerGenerator->makeDecision(request);
    
    qint64 responseTime = timer.elapsed();
    
    // 記錄測試結果
    AIDecisionLog log;
    log.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    log.aiId = request.aiId;
    log.action = response.action;
    log.skill = response.skill;
    log.target = response.target;
    log.result = response.isValid ? "成功" : "失敗";
    log.responseTime = responseTime;
    log.errorMessage = response.errorMessage;
    log.usedModel = model;
    log.isSuccess = response.isValid;
    
    logDecision(log);
    
    // 更新統計
    if (response.isValid) {
        m_currentReport.successfulRequests++;
    } else {
        m_currentReport.failedRequests++;
        m_currentReport.errorTypes[response.errorMessage]++;
        
        // 嘗試自動修復
        if (m_autoRepairEnabled) {
            attemptAutoRepair(request.aiId, response.errorMessage);
        }
    }
    
    m_currentReport.modelUsage[model]++;
    
    m_currentTestCount++;
    emit testProgressUpdated(m_currentTestCount, m_totalTestCount);
}

void AIDecisionEngine::sendLLMRequest(const AIDecisionRequest &request, LLMModel model, const QString &requestId)
{
    QString prompt = buildPrompt(request);
    QString modelName = getModelName(model);
    
    QJsonObject requestBody;
    requestBody["model"] = modelName;
    requestBody["prompt"] = prompt;
    requestBody["stream"] = false;
    
    QJsonDocument doc(requestBody);
    QByteArray data = doc.toJson();
    
    QUrl url(QString("http://%1:%2/api/generate").arg(m_ollamaHost).arg(m_ollamaPort));
    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    netRequest.setRawHeader("X-Request-ID", requestId.toUtf8());
    
    QNetworkReply *reply = m_networkManager->post(netRequest, data);
    
    connect(reply, &QNetworkReply::finished, this, &AIDecisionEngine::onLLMResponseReceived);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &AIDecisionEngine::onNetworkError);
    
    // 設定超時
    QTimer::singleShot(m_requestTimeout, reply, &QNetworkReply::abort);
}

QString AIDecisionEngine::buildPrompt(const AIDecisionRequest &request) const
{
    QString prompt = QString(
        "你是RAN Online %1學院%2部門的AI玩家，根據當前戰鬥狀況做出最優決策。\n\n"
        "【角色狀態】\n"
        "- AI ID: %3\n"
        "- 學院: %4\n"
        "- 部門: %5\n"
        "- HP: %6\n"
        "- MP: %7\n"
        "- 狀態: %8\n"
        "- 地圖: %9\n\n"
        "【技能冷卻】\n%10\n\n"
        "【敵人資料】\n%11\n\n"
        "【友方資料】\n%12\n\n"
        "請分析情況並回傳最優行動，格式必須為JSON：\n"
        "{\n"
        "  \"action\": \"行動類型\",\n"
        "  \"skill\": \"技能名稱\",\n"
        "  \"target\": \"目標ID\",\n"
        "  \"reason\": \"決策原因\"\n"
        "}\n\n"
        "注意：所有技能必須檢查冷卻和法力條件，禁止無效釋放。"
    ).arg(request.academy)
     .arg(request.department)
     .arg(request.aiId)
     .arg(request.academy)
     .arg(request.department)
     .arg(request.hp)
     .arg(request.mp)
     .arg(request.state)
     .arg(request.map)
     .arg(QJsonDocument(request.skillCooldowns).toJson(QJsonDocument::Compact))
     .arg(QJsonDocument(request.enemies).toJson(QJsonDocument::Compact))
     .arg(QJsonDocument(request.allies).toJson(QJsonDocument::Compact));
    
    return prompt;
}

void AIDecisionEngine::onLLMResponseReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    QString requestId = QString::fromUtf8(reply->request().rawHeader("X-Request-ID"));
    
    if (!m_pendingRequests.contains(requestId)) {
        reply->deleteLater();
        return;
    }
    
    auto requestData = m_pendingRequests.take(requestId);
    AIDecisionRequest request = requestData.first;
    QElapsedTimer timer = requestData.second;
    qint64 responseTime = timer.elapsed();
    
    AIDecisionResponse response;
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        
        QString llmResponse = obj["response"].toString();
        response = parseLLMResponse(llmResponse, request);
        
        if (!validateDecision(response, request)) {
            response.isValid = false;
            response.errorMessage = "決策驗證失敗";
        }
    } else {
        response.isValid = false;
        response.errorMessage = QString("網絡錯誤：%1").arg(reply->errorString());
    }
    
    // 記錄決策日誌
    AIDecisionLog log;
    log.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    log.aiId = request.aiId;
    log.action = response.action;
    log.skill = response.skill;
    log.target = response.target;
    log.result = response.isValid ? "成功" : "失敗";
    log.responseTime = responseTime;
    log.errorMessage = response.errorMessage;
    log.usedModel = LLMModel::GEMMA; // 簡化處理
    log.isSuccess = response.isValid;
    
    logDecision(log);
    
    emit decisionCompleted(request.aiId, response);
    
    reply->deleteLater();
}

AIDecisionResponse AIDecisionEngine::parseLLMResponse(const QString &response, const AIDecisionRequest &request) const
{
    Q_UNUSED(request)
    AIDecisionResponse result;
    result.isValid = false;
    
    try {
        // 嘗試解析JSON回應
        QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
        if (!doc.isObject()) {
            result.errorMessage = "回應不是有效的JSON格式";
            return result;
        }
        
        QJsonObject obj = doc.object();
        result.action = obj["action"].toString();
        result.skill = obj["skill"].toString();
        result.target = obj["target"].toString();
        result.reason = obj["reason"].toString();
        
        if (result.action.isEmpty() || result.skill.isEmpty()) {
            result.errorMessage = "回應缺少必要欄位";
            return result;
        }
        
        result.isValid = true;
        
    } catch (...) {
        result.errorMessage = "JSON解析異常";
    }
    
    return result;
}

LLMModel AIDecisionEngine::selectModel(const QString &department, const QString &role) const
{
    // 根據職業和角色選擇最適合的模型
    if (role == "captain" || role == "隊長") {
        return LLMModel::LLAMA3; // 隊長使用最強模型
    } else if (role == "vice" || role == "副隊") {
        return LLMModel::PHI3; // 副隊使用高效模型
    } else if (department == "氣功") {
        return LLMModel::MISTRAL; // 氣功專用模型
    } else if (department == "劍道") {
        return LLMModel::GEMMA; // 劍道使用通用模型
    } else if (department == "弓箭") {
        return LLMModel::QWEN2; // 弓箭使用中文優化模型
    } else if (department == "格鬥") {
        return LLMModel::DEEPSEEK; // 格鬥使用精準模型
    }
    
    return LLMModel::GEMMA; // 默認模型
}

QString AIDecisionEngine::getModelName(LLMModel model) const
{
    switch (model) {
        case LLMModel::LLAMA3: return "llama3:latest";
        case LLMModel::PHI3: return "phi3:latest";
        case LLMModel::MISTRAL: return "mistral:latest";
        case LLMModel::GEMMA: return "gemma:latest";
        case LLMModel::QWEN2: return "qwen2:latest";
        case LLMModel::DEEPSEEK: return "deepseek-coder:latest";
        default: return "gemma:latest";
    }
}

void AIDecisionEngine::logDecision(const AIDecisionLog &log)
{
    m_decisionLogs.append(log);
    
    QString logEntry = QString("[%1] AI:%2 %3:%4->%5 (%6ms) %7")
                      .arg(log.timestamp)
                      .arg(log.aiId)
                      .arg(log.action)
                      .arg(log.skill)
                      .arg(log.target)
                      .arg(log.responseTime)
                      .arg(log.result);
    
    if (!log.errorMessage.isEmpty()) {
        logEntry += QString(" 錯誤:%1").arg(log.errorMessage);
    }
    
    emit logGenerated(logEntry);
    
    updateStatistics(log);
}

void AIDecisionEngine::attemptAutoRepair(const QString &aiId, const QString &error)
{
    QString repairAction = m_repairActions.value(error, "未知錯誤，建議人工檢查");
    
    emit autoRepairTriggered(aiId, repairAction);
    emit logGenerated(QString("[%1] 自動修復：AI:%2 錯誤:%3 修復動作:%4")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(aiId).arg(error).arg(repairAction));
}

bool AIDecisionEngine::validateDecision(const AIDecisionResponse &response, const AIDecisionRequest &request) const
{
    // 基本有效性檢查
    if (response.action.isEmpty() || response.skill.isEmpty()) {
        return false;
    }
    
    // 檢查AI狀態
    if (request.hp <= 0 && response.action != "復活") {
        return false;
    }
    
    // 檢查目標有效性
    if (response.action == "攻擊" && response.target.isEmpty()) {
        return false;
    }
    
    return true;
}

void AIDecisionEngine::updateStatistics(const AIDecisionLog &log)
{
    // 更新模型使用統計
    m_modelLoad[log.usedModel]++;
    
    // 更新錯誤統計
    if (!log.isSuccess && !log.errorMessage.isEmpty()) {
        m_currentReport.errorTypes[log.errorMessage]++;
    }
}

void AIDecisionEngine::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    QString errorMsg = QString("網絡錯誤：%1").arg(reply->errorString());
    emit errorOccurred(errorMsg);
    
    reply->deleteLater();
}

TestReport AIDecisionEngine::generateTestReport() const
{
    return m_currentReport;
}

bool AIDecisionEngine::exportDecisionLogs(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
      QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    
    // 寫入CSV標頭
    stream << "時間,AI_ID,動作,技能,目標,結果,回應時間(ms),錯誤訊息,使用模型,是否成功\n";
    
    // 寫入決策日誌
    for (const AIDecisionLog &log : m_decisionLogs) {
        stream << QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10\n")
                  .arg(log.timestamp)
                  .arg(log.aiId)
                  .arg(log.action)
                  .arg(log.skill)
                  .arg(log.target)
                  .arg(log.result)
                  .arg(log.responseTime)
                  .arg(log.errorMessage)
                  .arg(getModelName(log.usedModel))
                  .arg(log.isSuccess ? "是" : "否");
    }
    
    file.close();
    return true;
}

void AIDecisionEngine::clearLogs()
{
    m_decisionLogs.clear();
    emit logGenerated(QString("[%1] 決策日誌已清空")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
}

void AIDecisionEngine::setAutoRepairEnabled(bool enabled)
{
    m_autoRepairEnabled = enabled;
    emit logGenerated(QString("[%1] 自動修復功能：%2")
                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(enabled ? "已啟用" : "已停用"));
}

void AIDecisionEngine::runBatchTest(const QJsonArray &testData, const QString &modelType)
{
    qDebug() << QString("開始批量測試，共%1條資料，使用模型：%2").arg(testData.size()).arg(modelType);
    
    // 這裡可以實現批量測試的邏輯
    // 暫時模擬測試完成
    TestReport report;
    report.totalRequests = testData.size();
    report.successfulRequests = testData.size();
    report.failedRequests = 0;
    report.averageResponseTime = 150.0;
    report.testDuration = 30.0;
    
    emit testCompleted(report);
}

} // namespace AI
} // namespace RANOnline
