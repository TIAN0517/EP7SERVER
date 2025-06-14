#pragma once

#include <QString>
#include <QColor>
#include <QMap>
#include <QStringList>
#include <QMetaType>

/**
 * @brief 學院主題結構體
 */
struct AcademyTheme {
    QString name;
    QColor primaryColor;
    QColor secondaryColor;
    QColor accentColor;
    QColor textColor;
    QString gradient;
};

/**
 * @brief LLM請求配置結構體
 */
struct LLMRequestConfig {
    QString requestId;
    QString prompt;
    QString model;
    QString modelName;  // 添加modelName字段
    QString academy;
    double temperature = 0.7;
    int maxTokens = 2048;
    bool useStreaming = true;
    bool stream = true;  // 添加stream字段
    int topK = 40;       // 添加topK字段
    double topP = 0.9;   // 添加topP字段
    double repeatPenalty = 1.1;  // 添加repeatPenalty字段
    QString systemPrompt;        // 添加systemPrompt字段
    QStringList context;         // 添加context字段
    int retryCount = 0;          // 添加retryCount字段
    QMap<QString, QString> parameters;
    
    // 默認構造函數
    LLMRequestConfig() = default;
    
    // 拷貝構造函數
    LLMRequestConfig(const LLMRequestConfig &) = default;
    
    // 賦值操作符
    LLMRequestConfig &operator=(const LLMRequestConfig &) = default;
};

/**
 * @brief LLM響應結構體
 */
struct LLMResponse {
    QString requestId;
    QString content;
    QString model;
    QString modelName;      // 添加modelName字段
    QString academy;        // 添加academy字段
    qint64 timestamp = 0;   // 添加timestamp字段
    bool isDone = false;    // 添加isDone字段
    bool isError = false;   // 添加isError字段
    QString errorMessage;   // 添加errorMessage字段
    int tokensUsed = 0;
    qint64 responseTime = 0;
    bool success = true;
    QString error;
    
    // 默認構造函數
    LLMResponse() = default;
    
    // 拷貝構造函數
    LLMResponse(const LLMResponse &) = default;
    
    // 賦值操作符
    LLMResponse &operator=(const LLMResponse &) = default;
};

/**
 * @brief Ollama服務器配置結構體
 */
struct OllamaServerConfig {
    QString id;
    QString name;
    QString host;
    QString baseUrl;         // 添加baseUrl字段
    int port = 11434;
    bool enabled = true;
    bool isActive = true;    // 添加isActive字段
    int maxConnections = 10;
    int maxConcurrent = 5;   // 添加maxConcurrent字段
    int currentLoad = 0;     // 添加currentLoad字段
    qint64 lastResponseTime = 0;  // 添加lastResponseTime字段
    double weight = 1.0;     // 添加weight字段
    QStringList models;
    QMap<QString, QString> loadBalancer;
    
    // 默認構造函數
    OllamaServerConfig() = default;
    
    // 拷貝構造函數
    OllamaServerConfig(const OllamaServerConfig &) = default;
    
    // 賦值操作符
    OllamaServerConfig &operator=(const OllamaServerConfig &) = default;
};

/**
 * @brief LLM模型信息結構體
 */
struct LLMModelInfo {
    QString name;
    QString description;
    QString size;
    QString digest;          // 添加digest字段
    QString modifiedAt;      // 添加modifiedAt字段
    QString family;          // 添加family字段
    QString parameterSize;   // 添加parameterSize字段
    QString quantization;    // 添加quantization字段
    bool isLoaded = false;
    bool isAvailable = true; // 添加isAvailable字段
    QStringList capabilities;
    QMap<QString, QString> parameters;
};
