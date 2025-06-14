#include "PromptTemplateManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths>

namespace RANOnline {
namespace AI {

// ====================================================================
// PromptTemplate Implementation
// ====================================================================

PromptTemplate::PromptTemplate(const QString &name, const QString &content)
    : m_name(name)
    , m_content(content)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
    m_requiredVariables = extractVariables();
}

QString PromptTemplate::render(const QVariantMap &variables) const
{
    QString result = m_content;
    
    // 合併預設值和提供的變量
    QVariantMap allVariables = m_defaultValues;
    for (auto it = variables.begin(); it != variables.end(); ++it) {
        allVariables[it.key()] = it.value();
    }
    
    // 替換變量（支持 {{variable}} 和 {variable} 格式）
    QRegularExpression regex(R"(\{\{?(\w+)\}?\})");
    QRegularExpressionMatchIterator matches = regex.globalMatch(result);
    
    // 從後往前替換，避免位置偏移問題
    QList<QRegularExpressionMatch> matchList;
    while (matches.hasNext()) {
        matchList.prepend(matches.next());
    }
    
    for (const QRegularExpressionMatch &match : matchList) {
        QString variableName = match.captured(1);
        if (allVariables.contains(variableName)) {
            QString value = allVariables[variableName].toString();
            result = result.replace(match.capturedStart(), match.capturedLength(), value);
        }
    }
    
    return result;
}

bool PromptTemplate::validate() const
{
    if (m_name.isEmpty() || m_content.isEmpty()) {
        return false;
    }
    
    // 檢查是否有未定義的必需變量
    QStringList variables = extractVariables();
    for (const QString &var : variables) {
        if (m_requiredVariables.contains(var) && !m_defaultValues.contains(var)) {
            // 有必需變量但沒有預設值，需要在渲染時提供
            continue;
        }
    }
    
    return true;
}

QStringList PromptTemplate::extractVariables() const
{
    QStringList variables;
    QRegularExpression regex(R"(\{\{?(\w+)\}?\})");
    QRegularExpressionMatchIterator matches = regex.globalMatch(m_content);
    
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString variableName = match.captured(1);
        if (!variables.contains(variableName)) {
            variables.append(variableName);
        }
    }
    
    return variables;
}

QJsonObject PromptTemplate::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["content"] = m_content;
    json["description"] = m_description;
    json["category"] = m_category;
    json["language"] = m_language;
    json["version"] = m_version;
    json["tags"] = QJsonArray::fromStringList(m_tags);
    json["createdAt"] = m_createdAt.toString(Qt::ISODate);
    json["updatedAt"] = m_updatedAt.toString(Qt::ISODate);
    json["requiredVariables"] = QJsonArray::fromStringList(m_requiredVariables);
    
    // 轉換預設值
    QJsonObject defaultValuesJson;
    for (auto it = m_defaultValues.begin(); it != m_defaultValues.end(); ++it) {
        defaultValuesJson[it.key()] = QJsonValue::fromVariant(it.value());
    }
    json["defaultValues"] = defaultValuesJson;
    
    return json;
}

void PromptTemplate::fromJson(const QJsonObject &json)
{
    m_name = json["name"].toString();
    m_content = json["content"].toString();
    m_description = json["description"].toString();
    m_category = json["category"].toString();
    m_language = json["language"].toString("zh-CN");
    m_version = json["version"].toString("1.0.0");
    
    // 轉換標籤
    QJsonArray tagsArray = json["tags"].toArray();
    m_tags.clear();
    for (const QJsonValue &value : tagsArray) {
        m_tags.append(value.toString());
    }
    
    // 轉換日期時間
    m_createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    m_updatedAt = QDateTime::fromString(json["updatedAt"].toString(), Qt::ISODate);
    
    // 轉換必需變量
    QJsonArray varsArray = json["requiredVariables"].toArray();
    m_requiredVariables.clear();
    for (const QJsonValue &value : varsArray) {
        m_requiredVariables.append(value.toString());
    }
    
    // 轉換預設值
    QJsonObject defaultValuesJson = json["defaultValues"].toObject();
    m_defaultValues.clear();
    for (auto it = defaultValuesJson.begin(); it != defaultValuesJson.end(); ++it) {
        m_defaultValues[it.key()] = it.value().toVariant();
    }
    
    // 如果沒有必需變量信息，自動提取
    if (m_requiredVariables.isEmpty()) {
        m_requiredVariables = extractVariables();
    }
}

bool PromptTemplate::operator==(const PromptTemplate &other) const
{
    return m_name == other.m_name && 
           m_content == other.m_content &&
           m_version == other.m_version;
}

// ====================================================================
// PromptTemplateManager Implementation
// ====================================================================

PromptTemplateManager::PromptTemplateManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
    , m_initialized(false)
{
}

PromptTemplateManager::~PromptTemplateManager()
{
    if (m_initialized) {
        saveTemplatesToDirectory(m_templateDirectory);
    }
}

bool PromptTemplateManager::initialize(const QString &templateDir)
{
    // 設置模板目錄
    if (templateDir.isEmpty()) {
        m_templateDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
    } else {
        m_templateDirectory = templateDir;
    }
    
    setupDirectories();
    
    // 載入內建模板
    loadBuiltinTemplates();
    
    // 載入用戶模板
    loadTemplatesFromDirectory(m_templateDirectory);
    
    m_initialized = true;
    
    qDebug() << "PromptTemplateManager initialized with" << m_templates.size() << "templates";
    return true;
}

void PromptTemplateManager::setupDirectories()
{
    QDir dir;
    if (!dir.exists(m_templateDirectory)) {
        dir.mkpath(m_templateDirectory);
    }
    
    // 創建分類子目錄
    QStringList categories = {"general", "coding", "technical", "game", "analysis", "translation", "creative"};
    for (const QString &category : categories) {
        QString categoryDir = m_templateDirectory + "/" + category;
        if (!dir.exists(categoryDir)) {
            dir.mkpath(categoryDir);
        }
    }
}

void PromptTemplateManager::loadDefaultTemplates()
{
    loadBuiltinTemplates();
}

void PromptTemplateManager::loadBuiltinTemplates()
{
    // ============================================================
    // 一般對話模板
    // ============================================================
    
    PromptTemplate generalChat("general_chat", 
        "System: You are a helpful AI assistant in the RANOnline game system. "
        "Please provide helpful, accurate, and {{tone}} responses in {{language}}.\n\n"
        "User: {{user_message}}");
    generalChat.setDescription("通用對話模板");
    generalChat.setCategory("general");
    generalChat.addTag("chat");
    generalChat.addTag("general");
    generalChat.setRequiredVariables({"user_message"});
    generalChat.setDefaultValues({
        {"tone", "friendly"},
        {"language", "Chinese"}
    });
    addTemplate(generalChat);
    
    // ============================================================
    // 代碼生成模板
    // ============================================================
    
    PromptTemplate codeGeneration("code_generation",
        "System: You are an expert {{language}} programmer. Generate clean, well-commented, "
        "and efficient code following best practices for {{language}} development.\n\n"
        "Requirements:\n"
        "- Language: {{language}}\n"
        "- Task: {{task_description}}\n"
        "{{#context}}- Context: {{context}}{{/context}}\n"
        "{{#constraints}}- Constraints: {{constraints}}{{/constraints}}\n\n"
        "Please provide the code with detailed comments explaining the logic.");
    codeGeneration.setDescription("代碼生成模板");
    codeGeneration.setCategory("coding");
    codeGeneration.addTag("code");
    codeGeneration.addTag("programming");
    codeGeneration.setRequiredVariables({"language", "task_description"});
    codeGeneration.setDefaultValues({
        {"language", "C++"},
        {"context", ""},
        {"constraints", ""}
    });
    addTemplate(codeGeneration);
    
    // ============================================================
    // 技術支援模板
    // ============================================================
    
    PromptTemplate technicalSupport("technical_support",
        "System: You are a technical expert assistant specializing in {{domain}}. "
        "Provide detailed, accurate, and practical solutions to technical problems. "
        "Include step-by-step instructions and examples when appropriate.\n\n"
        "Problem: {{problem_description}}\n"
        "{{#context}}Context: {{context}}{{/context}}\n"
        "{{#environment}}Environment: {{environment}}{{/environment}}\n\n"
        "Please provide a comprehensive solution with troubleshooting steps.");
    technicalSupport.setDescription("技術支援問題解答模板");
    technicalSupport.setCategory("technical");
    technicalSupport.addTag("support");
    technicalSupport.addTag("troubleshooting");
    technicalSupport.setRequiredVariables({"domain", "problem_description"});
    technicalSupport.setDefaultValues({
        {"domain", "software development"},
        {"context", ""},
        {"environment", ""}
    });
    addTemplate(technicalSupport);
    
    // ============================================================
    // 遊戲劇情模板
    // ============================================================
    
    PromptTemplate gameNarrative("game_narrative",
        "System: You are a creative game narrative designer for RANOnline. "
        "Create engaging, immersive storylines that fit the game's {{genre}} theme. "
        "The tone should be {{tone}} and appropriate for {{target_audience}}.\n\n"
        "Scenario: {{scenario}}\n"
        "Characters: {{characters}}\n"
        "Setting: {{setting}}\n"
        "{{#plot_points}}Key Plot Points: {{plot_points}}{{/plot_points}}\n\n"
        "Generate an engaging narrative that advances the story and provides meaningful choices for players.");
    gameNarrative.setDescription("遊戲劇情生成模板");
    gameNarrative.setCategory("game");
    gameNarrative.addTag("narrative");
    gameNarrative.addTag("storytelling");
    gameNarrative.addTag("game");
    gameNarrative.setRequiredVariables({"scenario", "characters", "setting"});
    gameNarrative.setDefaultValues({
        {"genre", "fantasy RPG"},
        {"tone", "epic and adventurous"},
        {"target_audience", "young adults"},
        {"plot_points", ""}
    });
    addTemplate(gameNarrative);
    
    // ============================================================
    // 數據分析模板
    // ============================================================
    
    PromptTemplate dataAnalysis("data_analysis",
        "System: You are a data analyst expert. Analyze the provided data and generate "
        "{{analysis_type}} insights. Focus on {{focus_areas}} and provide actionable recommendations.\n\n"
        "Data Description: {{data_description}}\n"
        "Analysis Goal: {{analysis_goal}}\n"
        "{{#metrics}}Key Metrics: {{metrics}}{{/metrics}}\n"
        "{{#constraints}}Constraints: {{constraints}}{{/constraints}}\n\n"
        "Please provide:\n"
        "1. Data summary and key findings\n"
        "2. Detailed analysis with supporting evidence\n"
        "3. Actionable recommendations\n"
        "4. Potential limitations and considerations");
    dataAnalysis.setDescription("數據分析報告模板");
    dataAnalysis.setCategory("analysis");
    dataAnalysis.addTag("data");
    dataAnalysis.addTag("analysis");
    dataAnalysis.addTag("insights");
    dataAnalysis.setRequiredVariables({"data_description", "analysis_goal"});
    dataAnalysis.setDefaultValues({
        {"analysis_type", "comprehensive"},
        {"focus_areas", "trends and patterns"},
        {"metrics", ""},
        {"constraints", ""}
    });
    addTemplate(dataAnalysis);
    
    // ============================================================
    // 翻譯模板
    // ============================================================
    
    PromptTemplate translation("translation",
        "System: You are a professional translator specializing in {{domain}} translation. "
        "Translate the text from {{source_language}} to {{target_language}} while maintaining "
        "the original meaning, tone, and context. Consider cultural nuances and technical terminology.\n\n"
        "Source Language: {{source_language}}\n"
        "Target Language: {{target_language}}\n"
        "{{#context}}Context: {{context}}{{/context}}\n"
        "{{#style}}Style: {{style}}{{/style}}\n\n"
        "Text to translate:\n{{source_text}}\n\n"
        "Please provide an accurate and natural translation.");
    translation.setDescription("多語言翻譯模板");
    translation.setCategory("translation");
    translation.addTag("translation");
    translation.addTag("language");
    translation.setRequiredVariables({"source_language", "target_language", "source_text"});
    translation.setDefaultValues({
        {"domain", "general"},
        {"context", ""},
        {"style", "formal"}
    });
    addTemplate(translation);
    
    // ============================================================
    // 摘要模板
    // ============================================================
    
    PromptTemplate summarization("summarization",
        "System: You are an expert at creating clear, concise summaries. "
        "Create a {{summary_type}} summary of the provided content, highlighting {{focus}} "
        "and maintaining {{length}} length.\n\n"
        "Content Type: {{content_type}}\n"
        "Target Audience: {{target_audience}}\n"
        "{{#key_points}}Key Points to Include: {{key_points}}{{/key_points}}\n\n"
        "Content to summarize:\n{{content}}\n\n"
        "Please provide a well-structured summary with clear main points.");
    summarization.setDescription("內容摘要生成模板");
    summarization.setCategory("analysis");
    summarization.addTag("summary");
    summarization.addTag("analysis");
    summarization.setRequiredVariables({"content"});
    summarization.setDefaultValues({
        {"summary_type", "comprehensive"},
        {"focus", "key insights and main points"},
        {"length", "medium"},
        {"content_type", "general"},
        {"target_audience", "general audience"},
        {"key_points", ""}
    });
    addTemplate(summarization);
    
    // ============================================================
    // 創意寫作模板
    // ============================================================
    
    PromptTemplate creativeWriting("creative_writing",
        "System: You are a creative writer with expertise in {{genre}} writing. "
        "Create engaging, original content with {{style}} style and {{tone}} tone. "
        "Focus on {{focus_elements}} to create compelling narratives.\n\n"
        "Writing Type: {{writing_type}}\n"
        "Theme: {{theme}}\n"
        "{{#characters}}Characters: {{characters}}{{/characters}}\n"
        "{{#setting}}Setting: {{setting}}{{/setting}}\n"
        "{{#constraints}}Constraints: {{constraints}}{{/constraints}}\n\n"
        "Prompt: {{writing_prompt}}\n\n"
        "Please create an engaging piece that captures the reader's imagination.");
    creativeWriting.setDescription("創意寫作模板");
    creativeWriting.setCategory("creative");
    creativeWriting.addTag("writing");
    creativeWriting.addTag("creative");
    creativeWriting.addTag("storytelling");
    creativeWriting.setRequiredVariables({"writing_prompt"});
    creativeWriting.setDefaultValues({
        {"genre", "fantasy"},
        {"style", "descriptive"},
        {"tone", "engaging"},
        {"focus_elements", "character development and world-building"},
        {"writing_type", "short story"},
        {"theme", "adventure"},
        {"characters", ""},
        {"setting", ""},
        {"constraints", ""}
    });
    addTemplate(creativeWriting);
    
    // ============================================================
    // 調試輔助模板
    // ============================================================
    
    PromptTemplate debugging("debugging_assistant",
        "System: You are an expert debugging assistant for {{language}} development. "
        "Analyze the provided code/error and provide step-by-step debugging guidance. "
        "Focus on identifying root causes and suggesting practical solutions.\n\n"
        "Programming Language: {{language}}\n"
        "{{#error_message}}Error Message: {{error_message}}{{/error_message}}\n"
        "{{#expected_behavior}}Expected Behavior: {{expected_behavior}}{{/expected_behavior}}\n"
        "{{#actual_behavior}}Actual Behavior: {{actual_behavior}}{{/actual_behavior}}\n\n"
        "Code:\n{{code}}\n\n"
        "Please provide:\n"
        "1. Analysis of the problem\n"
        "2. Possible root causes\n"
        "3. Step-by-step debugging approach\n"
        "4. Suggested fixes with explanations");
    debugging.setDescription("代碼調試輔助模板");
    debugging.setCategory("coding");
    debugging.addTag("debugging");
    debugging.addTag("troubleshooting");
    debugging.addTag("code");
    debugging.setRequiredVariables({"language", "code"});
    debugging.setDefaultValues({
        {"language", "C++"},
        {"error_message", ""},
        {"expected_behavior", ""},
        {"actual_behavior", ""}
    });
    addTemplate(debugging);
}

int PromptTemplateManager::loadTemplatesFromDirectory(const QString &directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        return 0;
    }
    
    int loadedCount = 0;
    QStringList filters;
    filters << "*.json" << "*.template";
    
    // 遞歸搜索所有子目錄
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files | QDir::Readable, QDir::Name);
    
    for (const QFileInfo &fileInfo : files) {
        if (loadTemplateFromFile(fileInfo.absoluteFilePath())) {
            loadedCount++;
        }
    }
    
    // 搜索子目錄
    QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &subdirInfo : subdirs) {
        loadedCount += loadTemplatesFromDirectory(subdirInfo.absoluteFilePath());
    }
    
    return loadedCount;
}

bool PromptTemplateManager::loadTemplateFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open template file:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error in template file:" << filePath << error.errorString();
        return false;
    }
    
    if (doc.isObject()) {
        // 單個模板
        PromptTemplate template;
        template.fromJson(doc.object());
        return addTemplate(template);
    } else if (doc.isArray()) {
        // 多個模板
        QJsonArray array = doc.array();
        bool success = true;
        for (const QJsonValue &value : array) {
            if (value.isObject()) {
                PromptTemplate template;
                template.fromJson(value.toObject());
                if (!addTemplate(template)) {
                    success = false;
                }
            }
        }
        return success;
    }
    
    return false;
}

bool PromptTemplateManager::saveTemplatesToDirectory(const QString &directory)
{
    QDir dir;
    if (!dir.exists(directory)) {
        dir.mkpath(directory);
    }
    
    bool allSuccess = true;
    
    // 按分類保存模板
    QMap<QString, QList<PromptTemplate>> categorizedTemplates;
    for (const PromptTemplate &template : m_templates) {
        QString category = template.category().isEmpty() ? "general" : template.category();
        categorizedTemplates[category].append(template);
    }
    
    for (auto it = categorizedTemplates.begin(); it != categorizedTemplates.end(); ++it) {
        QString categoryDir = directory + "/" + it.key();
        QDir().mkpath(categoryDir);
        
        QString fileName = categoryDir + "/" + it.key() + "_templates.json";
        
        QJsonArray templatesArray;
        for (const PromptTemplate &template : it.value()) {
            templatesArray.append(template.toJson());
        }
        
        QJsonDocument doc(templatesArray);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
        } else {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool PromptTemplateManager::addTemplate(const PromptTemplate &template)
{
    if (template.name().isEmpty() || !validateTemplate(template)) {
        return false;
    }
    
    m_templates[template.name()] = template;
    emit templateAdded(template.name());
    
    return true;
}

bool PromptTemplateManager::updateTemplate(const QString &name, const PromptTemplate &template)
{
    if (!m_templates.contains(name) || !validateTemplate(template)) {
        return false;
    }
    
    PromptTemplate updatedTemplate = template;
    updatedTemplate.setUpdatedAt(QDateTime::currentDateTime());
    
    m_templates[name] = updatedTemplate;
    emit templateUpdated(name);
    
    return true;
}

bool PromptTemplateManager::removeTemplate(const QString &name)
{
    if (!m_templates.contains(name)) {
        return false;
    }
    
    m_templates.remove(name);
    emit templateRemoved(name);
    
    return true;
}

PromptTemplate PromptTemplateManager::getTemplate(const QString &name) const
{
    return m_templates.value(name);
}

bool PromptTemplateManager::hasTemplate(const QString &name) const
{
    return m_templates.contains(name);
}

QStringList PromptTemplateManager::getTemplateNames() const
{
    return m_templates.keys();
}

QList<PromptTemplate> PromptTemplateManager::getTemplatesByCategory(const QString &category) const
{
    QList<PromptTemplate> result;
    for (const PromptTemplate &template : m_templates) {
        if (template.category() == category) {
            result.append(template);
        }
    }
    return result;
}

QList<PromptTemplate> PromptTemplateManager::getTemplatesByTag(const QString &tag) const
{
    QList<PromptTemplate> result;
    for (const PromptTemplate &template : m_templates) {
        if (template.tags().contains(tag)) {
            result.append(template);
        }
    }
    return result;
}

QList<PromptTemplate> PromptTemplateManager::searchTemplates(const QString &keyword) const
{
    QList<PromptTemplate> result;
    QString lowercaseKeyword = keyword.toLower();
    
    for (const PromptTemplate &template : m_templates) {
        bool matches = template.name().toLower().contains(lowercaseKeyword) ||
                      template.description().toLower().contains(lowercaseKeyword) ||
                      template.content().toLower().contains(lowercaseKeyword) ||
                      template.tags().join(" ").toLower().contains(lowercaseKeyword);
        
        if (matches) {
            result.append(template);
        }
    }
    
    return result;
}

QString PromptTemplateManager::renderTemplate(const QString &templateName, const QVariantMap &variables) const
{
    if (!m_templates.contains(templateName)) {
        return QString("Error: Template '%1' not found").arg(templateName);
    }
    
    return m_templates[templateName].render(variables);
}

QPair<bool, QString> PromptTemplateManager::validateTemplateVariables(const QString &templateName, const QVariantMap &variables) const
{
    if (!m_templates.contains(templateName)) {
        return {false, QString("Template '%1' not found").arg(templateName)};
    }
    
    const PromptTemplate &template = m_templates[templateName];
    QStringList requiredVars = template.requiredVariables();
    QStringList missingVars;
    
    for (const QString &var : requiredVars) {
        if (!variables.contains(var) && !template.defaultValues().contains(var)) {
            missingVars.append(var);
        }
    }
    
    if (!missingVars.isEmpty()) {
        return {false, QString("Missing required variables: %1").arg(missingVars.join(", "))};
    }
    
    return {true, "Validation successful"};
}

QStringList PromptTemplateManager::getRequiredVariables(const QString &templateName) const
{
    if (!m_templates.contains(templateName)) {
        return QStringList();
    }
    
    return m_templates[templateName].requiredVariables();
}

QVariantMap PromptTemplateManager::getDefaultValues(const QString &templateName) const
{
    if (!m_templates.contains(templateName)) {
        return QVariantMap();
    }
    
    return m_templates[templateName].defaultValues();
}

bool PromptTemplateManager::createQuickTemplate(const QString &name, const QString &content, const QString &category)
{
    PromptTemplate template(name, content);
    template.setCategory(category);
    template.setDescription(QString("Quick template: %1").arg(name));
    
    return addTemplate(template);
}

bool PromptTemplateManager::duplicateTemplate(const QString &sourceName, const QString &targetName)
{
    if (!m_templates.contains(sourceName) || m_templates.contains(targetName)) {
        return false;
    }
    
    PromptTemplate sourceTemplate = m_templates[sourceName];
    sourceTemplate.setName(targetName);
    sourceTemplate.setCreatedAt(QDateTime::currentDateTime());
    sourceTemplate.setUpdatedAt(QDateTime::currentDateTime());
    
    return addTemplate(sourceTemplate);
}

int PromptTemplateManager::importTemplateFile(const QString &filePath)
{
    return loadTemplateFromFile(filePath) ? 1 : 0;
}

bool PromptTemplateManager::exportTemplate(const QString &templateName, const QString &filePath)
{
    if (!m_templates.contains(templateName)) {
        return false;
    }
    
    QJsonDocument doc(m_templates[templateName].toJson());
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        return true;
    }
    
    return false;
}

QJsonObject PromptTemplateManager::getStatistics() const
{
    QJsonObject stats;
    stats["totalTemplates"] = m_templates.size();
    
    // 按分類統計
    QMap<QString, int> categoryCount;
    QMap<QString, int> tagCount;
    
    for (const PromptTemplate &template : m_templates) {
        QString category = template.category().isEmpty() ? "uncategorized" : template.category();
        categoryCount[category]++;
        
        for (const QString &tag : template.tags()) {
            tagCount[tag]++;
        }
    }
    
    QJsonObject categoriesJson;
    for (auto it = categoryCount.begin(); it != categoryCount.end(); ++it) {
        categoriesJson[it.key()] = it.value();
    }
    stats["categoryCounts"] = categoriesJson;
    
    QJsonObject tagsJson;
    for (auto it = tagCount.begin(); it != tagCount.end(); ++it) {
        tagsJson[it.key()] = it.value();
    }
    stats["tagCounts"] = tagsJson;
    
    return stats;
}

bool PromptTemplateManager::validateTemplate(const PromptTemplate &template) const
{
    return !template.name().isEmpty() && 
           !template.content().isEmpty() && 
           template.validate();
}

QString PromptTemplateManager::getTemplateFilePath(const QString &templateName) const
{
    if (!m_templates.contains(templateName)) {
        return QString();
    }
    
    const PromptTemplate &template = m_templates[templateName];
    QString category = template.category().isEmpty() ? "general" : template.category();
    
    return m_templateDirectory + "/" + category + "/" + templateName + ".json";
}

// ====================================================================
// PromptBuilder Implementation
// ====================================================================

PromptBuilder::PromptBuilder()
{
    reset();
}

PromptBuilder& PromptBuilder::setSystemPrompt(const QString &prompt)
{
    m_systemPrompt = prompt;
    return *this;
}

PromptBuilder& PromptBuilder::setUserPrompt(const QString &prompt)
{
    m_userPrompt = prompt;
    return *this;
}

PromptBuilder& PromptBuilder::addContext(const QString &context)
{
    if (!context.isEmpty()) {
        m_contexts.append(context);
    }
    return *this;
}

PromptBuilder& PromptBuilder::addExample(const QString &example)
{
    if (!example.isEmpty()) {
        m_examples.append(example);
    }
    return *this;
}

PromptBuilder& PromptBuilder::addConstraint(const QString &constraint)
{
    if (!constraint.isEmpty()) {
        m_constraints.append(constraint);
    }
    return *this;
}

PromptBuilder& PromptBuilder::setOutputFormat(const QString &format)
{
    m_outputFormat = format;
    return *this;
}

PromptBuilder& PromptBuilder::setLanguage(const QString &language)
{
    m_language = language;
    return *this;
}

PromptBuilder& PromptBuilder::setTone(const QString &tone)
{
    m_tone = tone;
    return *this;
}

PromptBuilder& PromptBuilder::addVariable(const QString &key, const QVariant &value)
{
    m_variables[key] = value;
    return *this;
}

PromptBuilder PromptBuilder::forCodeGeneration(const QString &language)
{
    PromptBuilder builder;
    builder.setSystemPrompt(QString(
        "You are an expert %1 programmer. Generate clean, well-commented, "
        "and efficient code following best practices for %1 development."
    ).arg(language))
    .setLanguage("English")
    .setTone("professional")
    .addVariable("programming_language", language);
    
    return builder;
}

PromptBuilder PromptBuilder::forTechnicalSupport()
{
    PromptBuilder builder;
    builder.setSystemPrompt(
        "You are a technical expert assistant. Provide detailed, accurate, "
        "and practical solutions to technical problems. Include step-by-step "
        "instructions and examples when appropriate."
    )
    .setLanguage("English")
    .setTone("helpful and professional");
    
    return builder;
}

PromptBuilder PromptBuilder::forGameNarrative()
{
    PromptBuilder builder;
    builder.setSystemPrompt(
        "You are a creative game narrative designer. Create engaging, "
        "immersive storylines that captivate players and enhance their "
        "gaming experience."
    )
    .setLanguage("English")
    .setTone("engaging and dramatic");
    
    return builder;
}

PromptBuilder PromptBuilder::forDataAnalysis()
{
    PromptBuilder builder;
    builder.setSystemPrompt(
        "You are a data analyst expert. Analyze the provided data and generate "
        "actionable insights with clear explanations and recommendations."
    )
    .setLanguage("English")
    .setTone("analytical and objective");
    
    return builder;
}

PromptBuilder PromptBuilder::forTranslation(const QString &fromLang, const QString &toLang)
{
    PromptBuilder builder;
    builder.setSystemPrompt(QString(
        "You are a professional translator. Translate accurately from %1 to %2 "
        "while maintaining the original meaning, tone, and cultural context."
    ).arg(fromLang, toLang))
    .addVariable("source_language", fromLang)
    .addVariable("target_language", toLang);
    
    return builder;
}

PromptBuilder PromptBuilder::forSummarization()
{
    PromptBuilder builder;
    builder.setSystemPrompt(
        "You are an expert at creating clear, concise summaries. Extract key "
        "information and present it in a well-structured, easy-to-understand format."
    )
    .setLanguage("English")
    .setTone("clear and concise");
    
    return builder;
}

PromptBuilder PromptBuilder::forCreativeWriting()
{
    PromptBuilder builder;
    builder.setSystemPrompt(
        "You are a creative writer. Create engaging, original content that "
        "captures the reader's imagination with vivid descriptions and compelling narratives."
    )
    .setLanguage("English")
    .setTone("creative and engaging");
    
    return builder;
}

QString PromptBuilder::build() const
{
    QString result;
    
    // 系統提示詞
    if (!m_systemPrompt.isEmpty()) {
        result += "System: " + m_systemPrompt + "\n";
        
        // 添加語言和語調
        if (!m_language.isEmpty()) {
            result += "Language: " + m_language + "\n";
        }
        if (!m_tone.isEmpty()) {
            result += "Tone: " + m_tone + "\n";
        }
        
        result += "\n";
    }
    
    // 上下文信息
    if (!m_contexts.isEmpty()) {
        result += "Context:\n";
        for (const QString &context : m_contexts) {
            result += "- " + context + "\n";
        }
        result += "\n";
    }
    
    // 示例
    if (!m_examples.isEmpty()) {
        result += "Examples:\n";
        for (const QString &example : m_examples) {
            result += "- " + example + "\n";
        }
        result += "\n";
    }
    
    // 約束條件
    if (!m_constraints.isEmpty()) {
        result += "Constraints:\n";
        for (const QString &constraint : m_constraints) {
            result += "- " + constraint + "\n";
        }
        result += "\n";
    }
    
    // 輸出格式
    if (!m_outputFormat.isEmpty()) {
        result += "Output Format: " + m_outputFormat + "\n\n";
    }
    
    // 用戶提示詞
    if (!m_userPrompt.isEmpty()) {
        result += "User: " + m_userPrompt;
    }
    
    return result;
}

QVariantMap PromptBuilder::getVariables() const
{
    return m_variables;
}

void PromptBuilder::reset()
{
    m_systemPrompt.clear();
    m_userPrompt.clear();
    m_contexts.clear();
    m_examples.clear();
    m_constraints.clear();
    m_outputFormat.clear();
    m_language.clear();
    m_tone.clear();
    m_variables.clear();
}

} // namespace AI
} // namespace RANOnline
