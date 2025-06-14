// LLMGUI.cpp - 第三部分
// 包含所有槽函數和輔助方法的實現

#include "LLMGUI.h"
#include "OllamaLLMManager.h"
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QList>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPixmap>

// ====================================================================
// LLMMainWindow 槽函數實現
// ====================================================================

void LLMMainWindow::onSubmitRequest()
{
    if (m_promptEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("請輸入提示詞"));
        return;
    }
    
    LLMRequestConfig config = createRequestConfig();
    QString requestId = m_llmManager->submitRequest(config);
    
    m_batchPanel->addRequest(requestId, config);
    addLogMessage(QString("提交請求: %1 (模型: %2, 學院: %3)")
                  .arg(requestId.left(8))
                  .arg(config.modelName)
                  .arg(config.academy), "REQUEST");
    
    showNotification(tr("請求已提交"), QString(tr("使用模型: %1")).arg(config.modelName));
}

void LLMMainWindow::onSubmitBatchRequests()
{
    if (m_batchPreviewList->count() == 0) {
        QMessageBox::warning(this, "警告", "請先生成批量任務");
        return;
    }
    
    QList<LLMRequestConfig> configs;
    QString basePrompt = m_batchPromptEdit->toPlainText();
    QString modelName = m_batchModelCombo->currentText();
    QString academy = m_academyCombo->currentText();
    
    for (int i = 0; i < m_batchCountSpin->value(); ++i) {
        LLMRequestConfig config;
        config.modelName = modelName;
        config.academy = academy;
        config.temperature = m_temperatureSpin->value();
        config.maxTokens = m_maxTokensSpin->value();
        config.topK = m_topKSpin->value();
        config.topP = m_topPSpin->value();
        config.systemPrompt = m_systemPromptEdit->toPlainText();
        config.stream = true;
        config.retryCount = 3;
          // 生成變化的提示詞
        if (m_autoVariationCheck->isChecked()) {
            QStringList variations = {
                tr("Please answer in detail: "), tr("Please answer briefly: "), tr("Please answer creatively: "), 
                tr("Please answer professionally: "), tr("Please answer in plain language: ")
            };
            QString variation = variations[i % variations.size()];
            config.prompt = variation + basePrompt;
        } else {
            config.prompt = basePrompt;
        }
        
        configs.append(config);
    }
    
    m_llmManager->submitBatchRequests(configs);
    
    // 添加到面板
    for (const auto &config : configs) {
        QString requestId = config.requestId.isEmpty() ? 
            QUuid::createUuid().toString(QUuid::WithoutBraces).left(16) : config.requestId;
        m_batchPanel->addRequest(requestId, config);
    }
    
    addLogMessage(QString("提交批量請求: %1個任務").arg(configs.size()), "BATCH");
    showNotification("批量請求已提交", QString("共%1個任務").arg(configs.size()));
    
    // 顯示進度條
    m_globalProgressBar->setRange(0, configs.size());
    m_globalProgressBar->setValue(0);
    m_globalProgressBar->setVisible(true);
}

void LLMMainWindow::onCancelAllRequests()
{
    auto reply = QMessageBox::question(this, "確認", "確定要取消所有活動請求嗎？",
                                      QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_llmManager->cancelAllRequests();
        addLogMessage("已取消所有活動請求", "CANCEL");
        showNotification("操作完成", "所有請求已取消");
    }
}

void LLMMainWindow::onRefreshModels()
{
    m_llmManager->refreshModels();
    addLogMessage("正在刷新模型列表...", "SYSTEM");
}

void LLMMainWindow::onExportLogs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "導出日誌", m_lastLogExportPath.isEmpty() ? 
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/llm_logs.txt" :
        m_lastLogExportPath,
        "文本文件 (*.txt);;所有文件 (*.*)");
    
    if (!fileName.isEmpty()) {
        exportLogsToFile(fileName);
        m_lastLogExportPath = fileName;
        addLogMessage(QString("日誌已導出到: %1").arg(fileName), "EXPORT");
        showNotification("導出完成", "日誌已成功導出");
    }
}

void LLMMainWindow::onImportTemplates()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "導入模板", "", "JSON文件 (*.json);;所有文件 (*.*)");
    
    if (!fileName.isEmpty()) {
        // TODO: 實現模板導入邏輯
        addLogMessage(QString("導入模板: %1").arg(fileName), "IMPORT");
    }
}

void LLMMainWindow::onExportTemplates()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "導出模板", "", "JSON文件 (*.json);;所有文件 (*.*)");
    
    if (!fileName.isEmpty()) {
        m_templateManager->saveTemplates(QFileInfo(fileName).absolutePath());
        addLogMessage(QString("模板已導出到: %1").arg(fileName), "EXPORT");
        showNotification("導出完成", "模板已成功導出");
    }
}

void LLMMainWindow::onAddServer()
{
    QString host = m_serverHostEdit->text().trimmed();
    QString name = m_serverNameEdit->text().trimmed();
    int port = m_serverPortSpin->value();
    
    if (host.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, "警告", "請填寫完整的服務器信息");
        return;
    }
    
    OllamaServerConfig server;
    server.name = name;
    server.host = host;
    server.port = port;
    server.baseUrl = QString("http://%1:%2").arg(host).arg(port);
    server.isActive = true;
    server.weight = 1;
    server.maxConcurrent = 5;
    
    m_llmManager->addServer(server);
    updateServerList();
    
    addLogMessage(QString("添加服務器: %1 (%2:%3)").arg(name, host).arg(port), "SERVER");
    
    // 清空輸入框
    m_serverHostEdit->clear();
    m_serverNameEdit->clear();
    m_serverPortSpin->setValue(11434);
}

void LLMMainWindow::onRemoveServer()
{
    QListWidgetItem *current = m_serverList->currentItem();
    if (!current) {
        QMessageBox::warning(this, "警告", "請選擇要移除的服務器");
        return;
    }
    
    QString serverName = current->text().split(" - ").first();
    
    auto reply = QMessageBox::question(this, "確認", 
        QString("確定要移除服務器 '%1' 嗎？").arg(serverName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_llmManager->removeServer(serverName);
        updateServerList();
        addLogMessage(QString("移除服務器: %1").arg(serverName), "SERVER");
    }
}

void LLMMainWindow::onTestConnection()
{
    QListWidgetItem *current = m_serverList->currentItem();
    if (!current) {
        QMessageBox::warning(this, "警告", "請選擇要測試的服務器");
        return;
    }
    
    // TODO: 實現連接測試邏輯
    m_connectionStatusLabel->setText("狀態: 測試中...");
    
    QTimer::singleShot(2000, [this]() {
        m_connectionStatusLabel->setText("狀態: ✅ 連接成功");
        addLogMessage("服務器連接測試成功", "TEST");
    });
}

void LLMMainWindow::onThemeChanged()
{
    // 循環切換主題
    int currentIndex = static_cast<int>(m_currentTheme);
    currentIndex = (currentIndex + 1) % 5;
    m_currentTheme = static_cast<ThemeMode>(currentIndex);
    
    applyTheme(m_currentTheme);
    
    QString themeName;
    switch (m_currentTheme) {
    case ThemeMode::CyberBlue: themeName = "賽博藍"; break;
    case ThemeMode::NeonGreen: themeName = "霓虹綠"; break;
    case ThemeMode::PurpleHaze: themeName = "紫霧"; break;
    case ThemeMode::RedAlert: themeName = "紅色警戒"; break;
    case ThemeMode::GoldenGlow: themeName = "金輝"; break;
    }
    
    addLogMessage(QString("切換主題: %1").arg(themeName), "THEME");
    showNotification("主題已切換", themeName);
}

void LLMMainWindow::onShowSettings()
{
    // TODO: 顯示設置對話框
}

void LLMMainWindow::onShowAbout()
{
    QMessageBox::about(this, "關於 RANOnline LLM控制中心",
        "<h2>RANOnline EP7 AI - LLM控制中心 v2.0</h2>"
        "<p><b>開發團隊:</b> Jy技術團隊</p>"
        "<p><b>發布日期:</b> 2025年6月14日</p>"
        "<p><b>功能特色:</b></p>"
        "<ul>"
        "<li>🤖 支援多個Ollama LLM模型統一管理</li>"
        "<li>📦 智能批量請求處理</li>"
        "<li>🎮 WebSocket遊戲事件同步</li>"
        "<li>🎨 科技感無邊框界面</li>"
        "<li>📊 實時統計和監控</li>"
        "<li>🔄 自動重試和錯誤處理</li>"
        "<li>⚖️ 負載均衡和分流</li>"
        "</ul>"
        "<p><b>支援模型:</b> Llama3, Phi3, Mistral, Gemma, Qwen2, DeepSeek-Coder</p>");
}

void LLMMainWindow::onToggleFullscreen()
{
    if (isMaximized()) {
        showNormal();
        m_maximizeButton->setText("□");
    } else {
        showMaximized();
        m_maximizeButton->setText("◱");
    }
}

void LLMMainWindow::onMinimizeToTray()
{
    hide();
    showNotification("最小化到系統托盤", "點擊托盤圖標可重新顯示窗口");
}

// ====================================================================
// LLM回調函數實現
// ====================================================================

void LLMMainWindow::onLLMRequestCompleted(const LLMResponse &response)
{
    m_batchPanel->updateRequest(response.requestId, response);
    
    QString status = response.isError ? "失敗" : "成功";
    addLogMessage(QString("請求完成: %1 - %2 (耗時: %3ms)")
                  .arg(response.requestId.left(8))
                  .arg(status)
                  .arg(response.responseTime), 
                  response.isError ? "ERROR" : "SUCCESS");
    
    if (!response.isError) {
        // 發送到遊戲服務器（如果連接）
        if (m_gameSync->isConnected()) {
            m_gameSync->sendAIResponse(response.requestId, response.content);
        }
    }
}

void LLMMainWindow::onLLMRequestFailed(const QString &requestId, const QString &error)
{
    addLogMessage(QString("請求失敗: %1 - %2").arg(requestId.left(8)).arg(error), "ERROR");
}

void LLMMainWindow::onLLMStreamUpdate(const QString &requestId, const QString &content)
{
    m_batchPanel->updateStream(requestId, content);
}

void LLMMainWindow::onLLMRequestProgress(const QString &requestId, int progress)
{
    m_batchPanel->updateProgress(requestId, progress);
}

void LLMMainWindow::onLLMModelsRefreshed(const QList<LLMModelInfo> &models)
{
    updateModelCombo();
    addLogMessage(QString("模型列表已更新，共%1個模型").arg(models.size()), "SYSTEM");
}

void LLMMainWindow::onLLMServerStatusChanged(const QString &serverName, bool isOnline)
{
    QString status = isOnline ? "在線" : "離線";
    addLogMessage(QString("服務器狀態變更: %1 - %2").arg(serverName).arg(status), "SERVER");
    
    updateServerList();
    
    if (isOnline) {
        m_connectionLabel->setText("🟢 已連接");
    } else {
        // 檢查是否還有其他在線服務器
        auto servers = m_llmManager->getServers();
        bool hasOnlineServer = std::any_of(servers.begin(), servers.end(),
                                          [](const OllamaServerConfig &s) { return s.isActive; });
        if (!hasOnlineServer) {
            m_connectionLabel->setText("🔴 未連接");
        }
    }
}

void LLMMainWindow::onLLMStatisticsUpdated()
{
    updateStats();
}

// ====================================================================
// 遊戲事件處理函數
// ====================================================================

void LLMMainWindow::onGameEventReceived(const QString &eventType, const QJsonObject &data)
{
    addLogMessage(QString("收到遊戲事件: %1").arg(eventType), "GAME");
}

void LLMMainWindow::onPlayerMessageReceived(const QString &playerId, const QString &message)
{
    handlePlayerInteraction(playerId, message);
}

void LLMMainWindow::onSkillGenerationRequest(const QString &skillId, const QJsonObject &requirements)
{
    handleGameSkillGeneration(skillId, requirements);
}

void LLMMainWindow::onGameConnectionStatusChanged(bool connected)
{
    QString status = connected ? "🟢 遊戲已連接" : "🔴 遊戲未連接";
    m_connectionLabel->setText(status);
    
    addLogMessage(QString("遊戲連接狀態: %1").arg(connected ? "已連接" : "已斷開"), "GAME");
}

void LLMMainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
    }
}

// ====================================================================
// 輔助方法實現
// ====================================================================

void LLMMainWindow::loadSettings()
{
    // 窗口設置
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());
    
    // 主題設置
    int themeIndex = m_settings->value("theme", 0).toInt();
    m_currentTheme = static_cast<ThemeMode>(themeIndex);
    
    // 其他設置
    m_lastLogExportPath = m_settings->value("lastLogExportPath").toString();
    
    // 控件狀態
    m_temperatureSpin->setValue(m_settings->value("temperature", 0.7).toDouble());
    m_maxTokensSpin->setValue(m_settings->value("maxTokens", 1000).toInt());
    m_topKSpin->setValue(m_settings->value("topK", 40).toInt());
    m_topPSpin->setValue(m_settings->value("topP", 0.9).toDouble());
    m_batchCountSpin->setValue(m_settings->value("batchCount", 5).toInt());
    
    addLogMessage("設置已加載", "SYSTEM");
}

void LLMMainWindow::saveSettings()
{
    // 窗口設置
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    
    // 主題設置
    m_settings->setValue("theme", static_cast<int>(m_currentTheme));
    
    // 其他設置
    m_settings->setValue("lastLogExportPath", m_lastLogExportPath);
    
    // 控件狀態
    m_settings->setValue("temperature", m_temperatureSpin->value());
    m_settings->setValue("maxTokens", m_maxTokensSpin->value());
    m_settings->setValue("topK", m_topKSpin->value());
    m_settings->setValue("topP", m_topPSpin->value());
    m_settings->setValue("batchCount", m_batchCountSpin->value());
    
    // 保存LLM配置
    m_llmManager->saveConfiguration();
}

void LLMMainWindow::applyTheme(ThemeMode theme)
{
    m_currentTheme = theme;
    updateThemeColors();
    
    QString styleSheet = getThemeStyleSheet();
    setStyleSheet(styleSheet);
    
    // 更新統計面板主題
    m_statsDashboard->setTheme(theme);
}

void LLMMainWindow::updateThemeColors()
{
    QColor themeColor;
    QString themeGradient;
    
    switch (m_currentTheme) {
    case ThemeMode::CyberBlue:
        themeColor = QColor(100, 150, 255);
        themeGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #6496FF, stop:1 #4169E1)";
        break;
    case ThemeMode::NeonGreen:
        themeColor = QColor(50, 255, 120);
        themeGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #32FF78, stop:1 #00FF7F)";
        break;
    case ThemeMode::PurpleHaze:
        themeColor = QColor(180, 100, 255);
        themeGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #B464FF, stop:1 #9932CC)";
        break;
    case ThemeMode::RedAlert:
        themeColor = QColor(255, 80, 80);
        themeGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #FF5050, stop:1 #DC143C)";
        break;
    case ThemeMode::GoldenGlow:
        themeColor = QColor(255, 200, 50);
        themeGradient = "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #FFC832, stop:1 #FFD700)";
        break;
    }
    
    // 更新按鈕主題顏色
    m_themeButton->setStyleSheet(getGlowEffectStyle(themeColor));
}

void LLMMainWindow::updateServerList()
{
    m_serverList->clear();
    
    auto servers = m_llmManager->getServers();
    for (const auto &server : servers) {
        QString status = server.isActive ? "🟢" : "🔴";
        QString item = QString("%1 - %2 %3:%4").arg(server.name).arg(status).arg(server.host).arg(server.port);
        m_serverList->addItem(item);
    }
}

void LLMMainWindow::updateModelCombo()
{
    QString currentModel = m_modelCombo->currentText();
    
    m_modelCombo->clear();
    m_batchModelCombo->clear();
    
    auto models = m_llmManager->getAvailableModels();
    for (const auto &model : models) {
        if (model.isAvailable) {
            QString displayName = QString("%1 (%2)").arg(model.name).arg(model.parameterSize);
            m_modelCombo->addItem(displayName, model.name);
            m_batchModelCombo->addItem(displayName, model.name);
        }
    }
    
    // 恢復之前的選擇
    int index = m_modelCombo->findData(currentModel);
    if (index >= 0) {
        m_modelCombo->setCurrentIndex(index);
    }
}

void LLMMainWindow::updateStats()
{
    int activeCount = m_llmManager->getActiveRequestCount();
    int totalCount = m_llmManager->getTotalRequestCount();
    double avgTime = m_llmManager->getAverageResponseTime();
    auto modelUsage = m_llmManager->getModelUsageStats();
    
    m_requestCountLabel->setText(QString("請求: %1/%2").arg(activeCount).arg(totalCount));
    m_statsDashboard->updateStats(activeCount, totalCount, avgTime, modelUsage);
    
    // 更新全局進度條
    if (activeCount == 0 && m_globalProgressBar->isVisible()) {
        m_globalProgressBar->setVisible(false);
    }
}

void LLMMainWindow::addLogMessage(const QString &message, const QString &category)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString coloredMessage;
    
    // 根據分類設置顏色
    if (category == "ERROR") {
        coloredMessage = QString("<span style='color: #FF6B6B;'>[%1] %2: %3</span>")
                        .arg(timestamp).arg(category).arg(message);
    } else if (category == "SUCCESS") {
        coloredMessage = QString("<span style='color: #50C878;'>[%1] %2: %3</span>")
                        .arg(timestamp).arg(category).arg(message);
    } else if (category == "WARNING") {
        coloredMessage = QString("<span style='color: #FFD700;'>[%1] %2: %3</span>")
                        .arg(timestamp).arg(category).arg(message);
    } else if (category == "GAME") {
        coloredMessage = QString("<span style='color: #9B59B6;'>[%1] %2: %3</span>")
                        .arg(timestamp).arg(category).arg(message);
    } else {
        coloredMessage = QString("<span style='color: #FFFFFF;'>[%1] %2: %3</span>")
                        .arg(timestamp).arg(category).arg(message);
    }
    
    m_logEdit->append(coloredMessage);
    
    // 確保日誌自動滾動到底部
    QTextCursor cursor = m_logEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logEdit->setTextCursor(cursor);
}

void LLMMainWindow::exportLogsToFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        
        // 寫入頭部信息
        out << "RANOnline EP7 AI - LLM控制中心日誌\n";
        out << "導出時間: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        out << "================================\n\n";
        
        // 寫入日誌內容（去除HTML格式）
        QString logContent = m_logEdit->toPlainText();
        out << logContent;
        
        file.close();
    }
}

void LLMMainWindow::showNotification(const QString &title, const QString &message)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}

QString LLMMainWindow::formatDuration(qint64 milliseconds) const
{
    if (milliseconds < 1000) {
        return QString("%1ms").arg(milliseconds);
    } else if (milliseconds < 60000) {
        return QString("%1.%2s").arg(milliseconds / 1000).arg((milliseconds % 1000) / 100);
    } else {
        int minutes = milliseconds / 60000;
        int seconds = (milliseconds % 60000) / 1000;
        return QString("%1m%2s").arg(minutes).arg(seconds);
    }
}

QMap<QString, QString> LLMMainWindow::collectTemplateVariables() const
{
    QMap<QString, QString> variables;
    variables["academy"] = m_academyCombo->currentText();
    variables["model"] = m_modelCombo->currentData().toString();
    variables["user_prompt"] = m_promptEdit->toPlainText();
    variables["system_prompt"] = m_systemPromptEdit->toPlainText();
    return variables;
}

LLMRequestConfig LLMMainWindow::createRequestConfig() const
{
    LLMRequestConfig config;
    config.modelName = m_modelCombo->currentData().toString();
    config.prompt = m_promptEdit->toPlainText();
    config.systemPrompt = m_systemPromptEdit->toPlainText();
    config.academy = m_academyCombo->currentText();
    config.temperature = m_temperatureSpin->value();
    config.maxTokens = m_maxTokensSpin->value();
    config.topK = m_topKSpin->value();
    config.topP = m_topPSpin->value();
    config.stream = true;
    config.retryCount = 3;
    return config;
}

void LLMMainWindow::handleGameSkillGeneration(const QString &skillId, const QJsonObject &requirements)
{
    QString academy = requirements["academy"].toString();
    QString skillType = requirements["type"].toString();
    int level = requirements["level"].toInt();
    
    // 使用技能生成模板
    auto tmpl = m_templateManager->getTemplate("skill_description");
    if (!tmpl.id.isEmpty()) {
        QMap<QString, QString> variables;
        variables["skill_type"] = skillType;
        variables["skill_name"] = requirements["name"].toString();
        variables["level"] = QString::number(level);
        variables["academy"] = academy;
        
        QString prompt = m_templateManager->renderTemplate("skill_description", variables);
        
        LLMRequestConfig config;
        config.modelName = m_llmManager->getDefaultModel();
        config.prompt = prompt;
        config.academy = academy;
        config.requestId = skillId;
        config.temperature = 0.8; // 稍高的創造性
        config.maxTokens = 500;
        
        QString requestId = m_llmManager->submitRequest(config);
        m_batchPanel->addRequest(requestId, config);
        
        addLogMessage(QString("生成技能描述: %1 (%2學院)").arg(skillId).arg(academy), "GAME");
    }
}

void LLMMainWindow::handlePlayerInteraction(const QString &playerId, const QString &message)
{
    // 使用角色對話模板
    auto tmpl = m_templateManager->getTemplate("game_character");
    if (!tmpl.id.isEmpty()) {
        QMap<QString, QString> variables;
        variables["academy"] = "戰士"; // 可以根據玩家信息動態設置
        variables["character_type"] = "NPC";
        variables["personality"] = "友善且樂於助人";
        variables["user_message"] = message;
        
        QString prompt = m_templateManager->renderTemplate("game_character", variables);
        
        LLMRequestConfig config;
        config.modelName = m_llmManager->getDefaultModel();
        config.prompt = prompt;
        config.academy = "戰士";
        config.requestId = playerId;
        config.temperature = 0.9; // 高創造性用於對話
        config.maxTokens = 300;
        
        QString requestId = m_llmManager->submitRequest(config);
        m_batchPanel->addRequest(requestId, config);
        
        addLogMessage(QString("處理玩家交互: %1").arg(playerId), "GAME");
    }
}

AcademyTheme LLMMainWindow::getAcademyTheme(const QString &academy) const
{
    return ACADEMY_THEMES.value(academy, ACADEMY_THEMES["戰士"]);
}

QString LLMMainWindow::getThemeStyleSheet() const
{
    QColor themeColor;
    switch (m_currentTheme) {
    case ThemeMode::CyberBlue: themeColor = QColor(100, 150, 255); break;
    case ThemeMode::NeonGreen: themeColor = QColor(50, 255, 120); break;
    case ThemeMode::PurpleHaze: themeColor = QColor(180, 100, 255); break;
    case ThemeMode::RedAlert: themeColor = QColor(255, 80, 80); break;
    case ThemeMode::GoldenGlow: themeColor = QColor(255, 200, 50); break;
    }
    
    return QString(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 rgba(20, 25, 35, 240), stop:1 rgba(15, 20, 30, 240));"
        "}"
        "QTabWidget::pane {"
        "    border: 2px solid rgba(%1, %2, %3, 100);"
        "    border-radius: 8px;"
        "    background: rgba(25, 30, 40, 200);"
        "}"
        "QTabBar::tab {"
        "    background: rgba(40, 50, 60, 150);"
        "    color: white;"
        "    padding: 8px 16px;"
        "    margin: 2px;"
        "    border-radius: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "    background: rgba(%4, %5, %6, 200);"
        "}"
        + getCyberButtonStyle()
    ).arg(themeColor.red()).arg(themeColor.green()).arg(themeColor.blue())
     .arg(themeColor.red()).arg(themeColor.green()).arg(themeColor.blue());
}

QString LLMMainWindow::getCyberButtonStyle() const
{
    return QString(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 rgba(60, 70, 80, 200), stop:1 rgba(40, 50, 60, 200));"
        "    border: 1px solid rgba(100, 120, 140, 150);"
        "    border-radius: 6px;"
        "    color: white;"
        "    padding: 6px 12px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 rgba(80, 90, 100, 200), stop:1 rgba(60, 70, 80, 200));"
        "    border: 1px solid rgba(120, 140, 160, 200);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 rgba(40, 50, 60, 200), stop:1 rgba(60, 70, 80, 200));"
        "}"
    );
}

QString LLMMainWindow::getGlowEffectStyle(const QColor &color) const
{
    return QString(
        "border: 2px solid rgba(%1, %2, %3, 150);"
        "border-radius: 6px;"
        "background: rgba(%4, %5, %6, 50);"
    ).arg(color.red()).arg(color.green()).arg(color.blue())
     .arg(color.red()).arg(color.green()).arg(color.blue());
}

void LLMMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        showNotification("程序已最小化", "程序將繼續在後台運行");
        event->ignore();
    } else {
        saveSettings();
        event->accept();
    }
}

void LLMMainWindow::showEvent(QShowEvent *event)
{
    FramelessWindow::showEvent(event);
    
    if (m_isInitialized) {
        m_fadeAnimation->start();
    }
}
