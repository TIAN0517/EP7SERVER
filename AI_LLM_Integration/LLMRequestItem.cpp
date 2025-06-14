#include "LLMRequestItem.h"
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
}

void LLMRequestItem::setupUI()
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setObjectName("LLMRequestItem");
    
    m_layout = new QVBoxLayout(this);
    
    // Header section
    m_headerLabel = new QLabel(tr("Request: %1").arg(m_requestId));
    m_headerLabel->setObjectName("requestHeader");
    m_layout->addWidget(m_headerLabel);
    
    // Model and Academy info
    auto infoLayout = new QHBoxLayout();
    m_modelLabel = new QLabel(tr("Model: %1").arg(m_config.model));
    m_academyLabel = new QLabel(tr("Academy: %1").arg(m_config.academy));
    infoLayout->addWidget(m_modelLabel);
    infoLayout->addWidget(m_academyLabel);
    m_layout->addLayout(infoLayout);
    
    // Progress bar
    m_progressBar = new CyberProgressBar(this);
    m_layout->addWidget(m_progressBar);
    
    // Response area
    m_responseEdit = new QTextEdit(this);
    m_responseEdit->setMaximumHeight(150);
    m_responseEdit->setReadOnly(true);
    m_layout->addWidget(m_responseEdit);
    
    // Buttons
    m_buttonLayout = new QHBoxLayout();
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_retryButton = new QPushButton(tr("Retry"), this);
    m_retryButton->hide(); // Initially hidden
    
    connect(m_cancelButton, &QPushButton::clicked, this, &LLMRequestItem::onCancelClicked);
    connect(m_retryButton, &QPushButton::clicked, this, &LLMRequestItem::onRetryClicked);
    
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_retryButton);
    m_buttonLayout->addStretch();
    
    // Status label
    m_statusLabel = new QLabel(tr("Processing..."), this);
    m_buttonLayout->addWidget(m_statusLabel);
    
    m_layout->addLayout(m_buttonLayout);
}

void LLMRequestItem::setupAnimations()
{
    // Shadow effect
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(20);
    m_shadowEffect->setOffset(0, 5);
    m_shadowEffect->setColor(QColor(0, 150, 255, 100));
    setGraphicsEffect(m_shadowEffect);
    
    // Glow animation
    m_glowAnimation = new QPropertyAnimation(m_shadowEffect, "color", this);
    m_glowAnimation->setDuration(2000);
    m_glowAnimation->setLoopCount(-1);
    m_glowAnimation->setStartValue(QColor(0, 150, 255, 50));
    m_glowAnimation->setEndValue(QColor(0, 150, 255, 150));
    m_glowAnimation->start();
}

void LLMRequestItem::updateProgress(int progress)
{
    m_progressBar->setValue(progress);
    
    if (progress >= 100) {
        setCompleted(true);
    }
}

void LLMRequestItem::updateResponse(const QString &content)
{
    m_responseEdit->append(content);
}

void LLMRequestItem::setCompleted(bool success, const QString &message)
{
    m_isCompleted = true;
    m_progressBar->setValue(100);
    
    if (success) {
        m_statusLabel->setText(tr("Completed"));
        m_statusLabel->setStyleSheet("color: green;");
        m_cancelButton->hide();
    } else {
        m_statusLabel->setText(tr("Failed: %1").arg(message));
        m_statusLabel->setStyleSheet("color: red;");
        m_cancelButton->setText(tr("Remove"));
        m_retryButton->show();
    }
    
    // Stop glow animation
    m_glowAnimation->stop();
}

void LLMRequestItem::setAcademyTheme(const AcademyTheme &theme)
{
    m_theme = theme;
    applyTheme();
}

void LLMRequestItem::applyTheme()
{
    QString styleSheet = QString(
        "LLMRequestItem {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 %1, stop:1 %2);"
        "    border: 2px solid %3;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "}"
        "QLabel#requestHeader {"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    color: %4;"
        "}"
    ).arg(m_theme.primaryColor.name())
     .arg(m_theme.secondaryColor.name())
     .arg(m_theme.accentColor.name())
     .arg(m_theme.textColor.name());
    
    setStyleSheet(styleSheet);
    
    if (m_progressBar) {
        m_progressBar->setAcademyTheme(m_theme);
    }
}

void LLMRequestItem::onCancelClicked()
{
    emit cancelRequested(m_requestId);
}

void LLMRequestItem::onRetryClicked()
{    emit retryRequested(m_requestId);
}
