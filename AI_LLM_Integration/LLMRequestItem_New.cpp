#include "LLMRequestItem_New.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QCoreApplication>

LLMRequestItem::LLMRequestItem(const QString &requestId, const LLMRequestConfig &config, QWidget *parent)
    : QFrame(parent)
    , m_requestId(requestId)
    , m_config(config)
    , m_isCompleted(false)
{
    setupUI();
    setupAnimations();
    applyTheme();
}

void LLMRequestItem::setupUI()
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setObjectName("LLMRequestItem");
    
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(15, 15, 15, 15);
    m_layout->setSpacing(10);
    
    // Header section
    m_headerLabel = new QLabel(tr("請求 ID: %1").arg(m_requestId.left(8) + "..."));
    m_headerLabel->setObjectName("requestHeader");
    m_headerLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #00ffff;");
    m_layout->addWidget(m_headerLabel);
    
    // Model and Academy info
    auto infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(20);
    
    m_modelLabel = new QLabel(tr("模型: %1").arg(m_config.modelName.isEmpty() ? m_config.model : m_config.modelName));
    m_modelLabel->setStyleSheet("color: #ffffff; font-size: 12px;");
    
    m_academyLabel = new QLabel(tr("學院: %1").arg(m_config.academy));
    m_academyLabel->setStyleSheet("color: #ffaa00; font-size: 12px;");
    
    infoLayout->addWidget(m_modelLabel);
    infoLayout->addWidget(m_academyLabel);
    infoLayout->addStretch();
    m_layout->addLayout(infoLayout);
    
    // Progress bar
    m_progressBar = new CyberProgressBar(this);
    m_progressBar->setMinimumHeight(25);
    m_layout->addWidget(m_progressBar);
    
    // Response area
    m_responseEdit = new QTextEdit(this);
    m_responseEdit->setMaximumHeight(200);
    m_responseEdit->setMinimumHeight(100);
    m_responseEdit->setReadOnly(true);
    m_responseEdit->setStyleSheet(
        "QTextEdit {"
        "    background-color: rgba(20, 20, 30, 180);"
        "    border: 2px solid #00aaff;"
        "    border-radius: 8px;"
        "    color: #ffffff;"
        "    font-family: 'Consolas', monospace;"
        "    font-size: 11px;"
        "    padding: 8px;"
        "}"
    );
    m_layout->addWidget(m_responseEdit);
    
    // Buttons and status
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    
    m_cancelButton = new QPushButton(tr("取消"), this);
    m_cancelButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff6b6b, stop:1 #ff4757);"
        "    border: 2px solid #ff3742;"
        "    border-radius: 6px;"
        "    color: white;"
        "    font-weight: bold;"
        "    padding: 8px 16px;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff7979, stop:1 #ff6b6b);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff4757, stop:1 #ff3742);"
        "}"
    );
    
    m_retryButton = new QPushButton(tr("重試"), this);
    m_retryButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffa502, stop:1 #ff6348);"
        "    border: 2px solid #ff5722;"
        "    border-radius: 6px;"
        "    color: white;"
        "    font-weight: bold;"
        "    padding: 8px 16px;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffb142, stop:1 #ffa502);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff6348, stop:1 #ff5722);"
        "}"
    );
    m_retryButton->hide(); // Initially hidden
    
    connect(m_cancelButton, &QPushButton::clicked, this, &LLMRequestItem::onCancelClicked);
    connect(m_retryButton, &QPushButton::clicked, this, &LLMRequestItem::onRetryClicked);
    
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_retryButton);
    m_buttonLayout->addStretch();
    
    // Status label
    m_statusLabel = new QLabel(tr("處理中..."), this);
    m_statusLabel->setStyleSheet("color: #00ffaa; font-size: 12px; font-weight: bold;");
    m_buttonLayout->addWidget(m_statusLabel);
    
    m_layout->addLayout(m_buttonLayout);
}

void LLMRequestItem::setupAnimations()
{
    // Shadow effect
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(25);
    m_shadowEffect->setOffset(0, 8);
    m_shadowEffect->setColor(QColor(0, 170, 255, 120));
    setGraphicsEffect(m_shadowEffect);
    
    // Glow animation
    m_glowAnimation = new QPropertyAnimation(m_shadowEffect, "color", this);
    m_glowAnimation->setDuration(3000);
    m_glowAnimation->setLoopCount(-1);
    m_glowAnimation->setStartValue(QColor(0, 170, 255, 80));
    m_glowAnimation->setEndValue(QColor(0, 255, 170, 160));
    m_glowAnimation->start();
}

void LLMRequestItem::applyTheme()
{
    // Apply academy-specific styling
    QString baseStyle = 
        "QFrame#LLMRequestItem {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 rgba(20, 25, 40, 220), "
        "        stop:0.5 rgba(30, 35, 50, 200), "
        "        stop:1 rgba(25, 30, 45, 230));"
        "    border: 2px solid %1;"
        "    border-radius: 12px;"
        "    margin: 5px;"
        "}";
    
    QColor borderColor = m_theme.primaryColor.isValid() ? m_theme.primaryColor : QColor(0, 170, 255);
    setStyleSheet(baseStyle.arg(borderColor.name()));
}

void LLMRequestItem::updateProgress(int progress)
{
    m_progressBar->setValue(progress);
    
    if (progress >= 100) {
        setCompleted(true, tr("請求完成"));
    } else {
        m_statusLabel->setText(tr("處理中... %1%").arg(progress));
    }
}

void LLMRequestItem::updateResponse(const QString &content)
{
    // Append content with proper formatting
    if (!content.trimmed().isEmpty()) {
        QString currentText = m_responseEdit->toPlainText();
        if (!currentText.isEmpty()) {
            m_responseEdit->setPlainText(currentText + content);
        } else {
            m_responseEdit->setPlainText(content);
        }
        
        // Auto-scroll to bottom
        QTextCursor cursor = m_responseEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_responseEdit->setTextCursor(cursor);
    }
}

void LLMRequestItem::setCompleted(bool success, const QString &message)
{
    m_isCompleted = true;
    m_progressBar->setValue(100);
    
    if (success) {
        m_statusLabel->setText(message.isEmpty() ? tr("✅ 完成") : message);
        m_statusLabel->setStyleSheet("color: #00ff88; font-size: 12px; font-weight: bold;");
        m_cancelButton->hide();
        
        // Stop glow animation
        if (m_glowAnimation) {
            m_glowAnimation->stop();
            m_shadowEffect->setColor(QColor(0, 255, 100, 100));
        }
    } else {
        m_statusLabel->setText(message.isEmpty() ? tr("❌ 失敗") : message);
        m_statusLabel->setStyleSheet("color: #ff4757; font-size: 12px; font-weight: bold;");
        m_cancelButton->hide();
        m_retryButton->show();
        
        // Change to error glow
        if (m_glowAnimation) {
            m_glowAnimation->stop();
            m_shadowEffect->setColor(QColor(255, 70, 70, 120));
        }
    }
}

void LLMRequestItem::setAcademyTheme(const AcademyTheme &theme)
{
    m_theme = theme;
    applyTheme();
    
    // Update academy label color
    if (m_academyLabel && theme.primaryColor.isValid()) {
        m_academyLabel->setStyleSheet(QString("color: %1; font-size: 12px; font-weight: bold;")
                                     .arg(theme.primaryColor.name()));
    }
}

void LLMRequestItem::onCancelClicked()
{
    emit cancelRequested(m_requestId);
}

void LLMRequestItem::onRetryClicked()
{
    // Reset state for retry
    m_isCompleted = false;
    m_progressBar->setValue(0);
    m_statusLabel->setText(tr("重新處理中..."));
    m_statusLabel->setStyleSheet("color: #00ffaa; font-size: 12px; font-weight: bold;");
    m_retryButton->hide();
    m_cancelButton->show();
    
    // Restart glow animation
    if (m_glowAnimation) {
        m_glowAnimation->start();
    }
    
    emit retryRequested(m_requestId);
}
