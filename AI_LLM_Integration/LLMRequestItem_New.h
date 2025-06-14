#pragma once

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include "AcademyTheme.h"
#include "CyberProgressBar.h"

/**
 * @brief LLM請求項目組件
 */
class LLMRequestItem : public QFrame
{
    Q_OBJECT

public:
    explicit LLMRequestItem(const QString &requestId, const LLMRequestConfig &config, QWidget *parent = nullptr);
    
    void updateProgress(int progress);
    void updateResponse(const QString &content);
    void setCompleted(bool success, const QString &message = "");
    void setAcademyTheme(const AcademyTheme &theme);
    
    QString getRequestId() const { return m_requestId; }
    bool isCompleted() const { return m_isCompleted; }

signals:
    void cancelRequested(const QString &requestId);
    void retryRequested(const QString &requestId);

private slots:
    void onCancelClicked();
    void onRetryClicked();

private:
    QString m_requestId;
    LLMRequestConfig m_config;
    AcademyTheme m_theme;
    bool m_isCompleted = false;
    
    // UI組件
    QVBoxLayout *m_layout;
    QLabel *m_headerLabel;
    QLabel *m_modelLabel;
    QLabel *m_academyLabel;
    CyberProgressBar *m_progressBar;
    QTextEdit *m_responseEdit;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_cancelButton;
    QPushButton *m_retryButton;
    QLabel *m_statusLabel;
    
    // 動畫效果
    QPropertyAnimation *m_glowAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    void setupUI();
    void setupAnimations();
    void applyTheme();
};
