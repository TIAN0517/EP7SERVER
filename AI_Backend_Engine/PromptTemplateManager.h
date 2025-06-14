#ifndef PROMPTTEMPLATE_MANAGER_H
#define PROMPTTEMPLATE_MANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QVariantMap>
#include <QRegularExpression>
#include <QDateTime>
#include <memory>

/**
 * @brief 提示詞模板管理器
 * 
 * 功能特性:
 * - 參數化提示詞模板管理
 * - 多語言模板支持
 * - 動態變量替換
 * - 模板版本控制
 * - 場景分類管理
 * - 模板繼承和組合
 * - 自動校驗和格式化
 * 
 * @author Jy技術團隊
 * @date 2025-06-14
 * @version 2.0.0
 */

namespace RANOnline {
namespace AI {

/**
 * @brief 提示詞模板類
 */
class PromptTemplate
{
public:
    PromptTemplate() = default;
    PromptTemplate(const QString &name, const QString &content);
    
    // 基本屬性
    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }
    
    QString content() const { return m_content; }
    void setContent(const QString &content) { m_content = content; }
    
    QString description() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }
    
    QString category() const { return m_category; }
    void setCategory(const QString &category) { m_category = category; }
    
    QString language() const { return m_language; }
    void setLanguage(const QString &language) { m_language = language; }
    
    QString version() const { return m_version; }
    void setVersion(const QString &version) { m_version = version; }
    
    QStringList tags() const { return m_tags; }
    void setTags(const QStringList &tags) { m_tags = tags; }
    void addTag(const QString &tag) { if (!m_tags.contains(tag)) m_tags.append(tag); }
    
    QDateTime createdAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &time) { m_createdAt = time; }
    
    QDateTime updatedAt() const { return m_updatedAt; }
    void setUpdatedAt(const QDateTime &time) { m_updatedAt = time; }
    
    // 變量管理
    QStringList requiredVariables() const { return m_requiredVariables; }
    void setRequiredVariables(const QStringList &variables) { m_requiredVariables = variables; }
    
    QVariantMap defaultValues() const { return m_defaultValues; }
    void setDefaultValues(const QVariantMap &values) { m_defaultValues = values; }
    
    // 模板處理
    QString render(const QVariantMap &variables = QVariantMap()) const;
    bool validate() const;
    QStringList extractVariables() const;
    
    // 序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
    
    // 運算符重載
    bool operator==(const PromptTemplate &other) const;
    bool operator!=(const PromptTemplate &other) const { return !(*this == other); }

private:
    QString m_name;
    QString m_content;
    QString m_description;
    QString m_category;
    QString m_language = "zh-CN";
    QString m_version = "1.0.0";
    QStringList m_tags;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QStringList m_requiredVariables;
    QVariantMap m_defaultValues;
};

/**
 * @brief 提示詞模板管理器主類
 */
class PromptTemplateManager : public QObject
{
    Q_OBJECT

public:
    explicit PromptTemplateManager(QObject *parent = nullptr);
    ~PromptTemplateManager();

    // ============================================================
    // 🔧 初始化和配置
    // ============================================================
    
    /**
     * @brief 初始化模板管理器
     * @param templateDir 模板目錄路徑
     * @return 是否初始化成功
     */
    bool initialize(const QString &templateDir = QString());
    
    /**
     * @brief 載入默認模板
     */
    void loadDefaultTemplates();
    
    /**
     * @brief 從目錄載入模板
     * @param directory 目錄路徑
     * @return 載入的模板數量
     */
    int loadTemplatesFromDirectory(const QString &directory);
    
    /**
     * @brief 保存所有模板到目錄
     * @param directory 目錄路徑
     * @return 是否成功保存
     */
    bool saveTemplatesToDirectory(const QString &directory);

    // ============================================================
    // 📋 模板管理
    // ============================================================
    
    /**
     * @brief 添加模板
     * @param template 模板對象
     * @return 是否成功添加
     */
    bool addTemplate(const PromptTemplate &template);
    
    /**
     * @brief 更新模板
     * @param name 模板名稱
     * @param template 新模板對象
     * @return 是否成功更新
     */
    bool updateTemplate(const QString &name, const PromptTemplate &template);
    
    /**
     * @brief 刪除模板
     * @param name 模板名稱
     * @return 是否成功刪除
     */
    bool removeTemplate(const QString &name);
    
    /**
     * @brief 獲取模板
     * @param name 模板名稱
     * @return 模板對象
     */
    PromptTemplate getTemplate(const QString &name) const;
    
    /**
     * @brief 檢查模板是否存在
     * @param name 模板名稱
     * @return 是否存在
     */
    bool hasTemplate(const QString &name) const;
    
    /**
     * @brief 獲取所有模板名稱
     * @return 模板名稱列表
     */
    QStringList getTemplateNames() const;
    
    /**
     * @brief 按分類獲取模板
     * @param category 分類名稱
     * @return 模板列表
     */
    QList<PromptTemplate> getTemplatesByCategory(const QString &category) const;
    
    /**
     * @brief 按標籤搜索模板
     * @param tag 標籤名稱
     * @return 模板列表
     */
    QList<PromptTemplate> getTemplatesByTag(const QString &tag) const;
    
    /**
     * @brief 搜索模板
     * @param keyword 關鍵字
     * @return 模板列表
     */
    QList<PromptTemplate> searchTemplates(const QString &keyword) const;

    // ============================================================
    // 🎯 模板處理
    // ============================================================
    
    /**
     * @brief 渲染模板
     * @param templateName 模板名稱
     * @param variables 變量映射
     * @return 渲染後的文本
     */
    QString renderTemplate(const QString &templateName, const QVariantMap &variables = QVariantMap()) const;
    
    /**
     * @brief 驗證模板變量
     * @param templateName 模板名稱
     * @param variables 變量映射
     * @return 驗證結果和錯誤信息
     */
    QPair<bool, QString> validateTemplateVariables(const QString &templateName, const QVariantMap &variables) const;
    
    /**
     * @brief 獲取模板所需變量
     * @param templateName 模板名稱
     * @return 必需變量列表
     */
    QStringList getRequiredVariables(const QString &templateName) const;
    
    /**
     * @brief 獲取模板預設值
     * @param templateName 模板名稱
     * @return 預設值映射
     */
    QVariantMap getDefaultValues(const QString &templateName) const;

    // ============================================================
    // 🔧 實用工具
    // ============================================================
    
    /**
     * @brief 創建快速模板
     * @param name 模板名稱
     * @param content 模板內容
     * @param category 分類
     * @return 是否成功創建
     */
    bool createQuickTemplate(const QString &name, const QString &content, const QString &category = "custom");
    
    /**
     * @brief 複製模板
     * @param sourceName 源模板名稱
     * @param targetName 目標模板名稱
     * @return 是否成功複製
     */
    bool duplicateTemplate(const QString &sourceName, const QString &targetName);
    
    /**
     * @brief 導入模板文件
     * @param filePath 文件路徑
     * @return 導入的模板數量
     */
    int importTemplateFile(const QString &filePath);
    
    /**
     * @brief 導出模板到文件
     * @param templateName 模板名稱
     * @param filePath 文件路徑
     * @return 是否成功導出
     */
    bool exportTemplate(const QString &templateName, const QString &filePath);
    
    /**
     * @brief 獲取統計信息
     * @return 統計信息
     */
    QJsonObject getStatistics() const;

signals:
    /**
     * @brief 模板添加信號
     * @param templateName 模板名稱
     */
    void templateAdded(const QString &templateName);
    
    /**
     * @brief 模板更新信號
     * @param templateName 模板名稱
     */
    void templateUpdated(const QString &templateName);
    
    /**
     * @brief 模板刪除信號
     * @param templateName 模板名稱
     */
    void templateRemoved(const QString &templateName);

private:
    // ============================================================
    // 🔧 內部方法
    // ============================================================
    
    void setupDirectories();
    void loadBuiltinTemplates();
    QString replaceVariables(const QString &text, const QVariantMap &variables) const;
    QStringList parseVariables(const QString &text) const;
    bool validateTemplate(const PromptTemplate &template) const;
    QString getTemplateFilePath(const QString &templateName) const;
    
    // ============================================================
    // 📊 成員變量
    // ============================================================
    
    QString m_templateDirectory;
    QMap<QString, PromptTemplate> m_templates;
    QSettings *m_settings;
    bool m_initialized;
};

/**
 * @brief 提示詞構建器輔助類
 */
class PromptBuilder
{
public:
    PromptBuilder();
    
    // 基本構建方法
    PromptBuilder& setSystemPrompt(const QString &prompt);
    PromptBuilder& setUserPrompt(const QString &prompt);
    PromptBuilder& addContext(const QString &context);
    PromptBuilder& addExample(const QString &example);
    PromptBuilder& addConstraint(const QString &constraint);
    PromptBuilder& setOutputFormat(const QString &format);
    PromptBuilder& setLanguage(const QString &language);
    PromptBuilder& setTone(const QString &tone);
    PromptBuilder& addVariable(const QString &key, const QVariant &value);
    
    // 預設構建器
    static PromptBuilder forCodeGeneration(const QString &language = "cpp");
    static PromptBuilder forTechnicalSupport();
    static PromptBuilder forGameNarrative();
    static PromptBuilder forDataAnalysis();
    static PromptBuilder forTranslation(const QString &fromLang, const QString &toLang);
    static PromptBuilder forSummarization();
    static PromptBuilder forCreativeWriting();
    
    // 構建最終提示詞
    QString build() const;
    QVariantMap getVariables() const;
    
    // 重置構建器
    void reset();

private:
    QString m_systemPrompt;
    QString m_userPrompt;
    QStringList m_contexts;
    QStringList m_examples;
    QStringList m_constraints;
    QString m_outputFormat;
    QString m_language;
    QString m_tone;
    QVariantMap m_variables;
};

} // namespace AI
} // namespace RANOnline

Q_DECLARE_METATYPE(RANOnline::AI::PromptTemplate)

#endif // PROMPTTEMPLATE_MANAGER_H
