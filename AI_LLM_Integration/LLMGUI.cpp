#include "LLMGUI.h"
#include "OllamaLLMManager.h"
#include <QScreen>
#include <QGuiApplication>
#include <QStyleFactory>
#include <QGraphicsBlurEffect>
#include <QPainterPath>
#include <QRegularExpression>
#include <QClipboard>
#include <QMimeData>
#include <QDrag>
#include <QRandomGenerator>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>
#include <QPropertyAnimation>
#include <memory>

// ====================================================================
// 全局常量定義
// ====================================================================

static const QMap<QString, AcademyTheme> ACADEMY_THEMES = {
    {"戰士", {
        "戰士學院",
        QColor("#FF6B35"), // 橙紅色
        QColor("#FF8E53"), 
        QColor("#FFD23F"),
        QColor("#FFFFFF"),
        "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #FF6B35, stop:1 #FF8E53)"
    }},
    {"法師", {
        "法師學院", 
        QColor("#4A90E2"), // 藍色
        QColor("#5BA0F2"),
        QColor("#7BB3F0"),
        QColor("#FFFFFF"),
        "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4A90E2, stop:1 #5BA0F2)"
    }},
    {"弓手", {
        "弓手學院",
        QColor("#50C878"), // 綠色
        QColor("#66D989"),
        QColor("#85E99B"),
        QColor("#FFFFFF"),
        "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #50C878, stop:1 #66D989)"
    }},
    {"盜賊", {
        "盜賊學院",
        QColor("#9B59B6"), // 紫色
        QColor("#A569BD"),
        QColor("#BB8FCE"),
        QColor("#FFFFFF"),
        "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #9B59B6, stop:1 #A569BD)"
    }},
    {"牧師", {
        "牧師學院",
        QColor("#F1C40F"), // 金色
        QColor("#F4D03F"),
        QColor("#F7DC6F"),
        QColor("#2C3E50"),
        "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #F1C40F, stop:1 #F4D03F)"
    }}
};

// ====================================================================
// FramelessWindow Implementation
// ====================================================================

FramelessWindow::FramelessWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupWindowFlags();
    createShadowEffect();
}

void FramelessWindow::setupWindowFlags()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void FramelessWindow::createShadowEffect()
{
    auto *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 5);
    setGraphicsEffect(shadowEffect);
}

void FramelessWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 繪制圓角背景
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(10, 10, -10, -10), 15, 15);
    
    // 背景漸變
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(20, 25, 35, 240));
    gradient.setColorAt(1, QColor(15, 20, 30, 240));
    
    painter.fillPath(path, gradient);
    
    // 邊框發光效果
    QPen pen(QColor(100, 150, 255, 150), 2);
    painter.setPen(pen);
    painter.drawPath(path);
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPosition = event->globalPos();
        m_windowPositionAsDrag = pos();
    }
    QMainWindow::mousePressEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && event->buttons() & Qt::LeftButton) {
        QPoint diff = event->globalPos() - m_dragStartPosition;
        move(m_windowPositionAsDrag + diff);
    }
    QMainWindow::mouseMoveEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    QMainWindow::mouseReleaseEvent(event);
}

void FramelessWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    
    // 更新遮罩
    QRegion region(10, 10, width() - 20, height() - 20, QRegion::Rectangle);
    setMask(region);
}

// ====================================================================
// CyberProgressBar Implementation
// ====================================================================

CyberProgressBar::CyberProgressBar(QWidget *parent)
    : QProgressBar(parent)
    , m_pulseTimer(new QTimer(this))
{
    setMinimumHeight(25);
    setupPulseAnimation();
}

void CyberProgressBar::setAcademyTheme(const AcademyTheme &theme)
{
    m_theme = theme;
    update();
}

void CyberProgressBar::setGlowEffect(bool enabled)
{
    m_glowEnabled = enabled;
    update();
}

void CyberProgressBar::setAnimationSpeed(int speed)
{
    m_animationSpeed = speed;
    m_pulseTimer->setInterval(speed / 10);
}

void CyberProgressBar::setupPulseAnimation()
{
    m_pulseTimer->setInterval(100);
    connect(m_pulseTimer, &QTimer::timeout, [this]() {
        static double direction = 0.05;
        m_pulseOpacity += direction;
        
        if (m_pulseOpacity >= 1.0) {
            m_pulseOpacity = 1.0;
            direction = -0.05;
        } else if (m_pulseOpacity <= 0.3) {
            m_pulseOpacity = 0.3;
            direction = 0.05;
        }
        
        update();
    });
    
    m_pulseTimer->start();
}

void CyberProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect rect = this->rect().adjusted(2, 2, -2, -2);
    
    // 背景
    painter.fillRect(rect, QColor(30, 35, 45, 200));
    
    // 進度條
    if (value() > 0) {
        int progressWidth = static_cast<int>((static_cast<double>(value()) / maximum()) * rect.width());
        QRect progressRect(rect.x(), rect.y(), progressWidth, rect.height());
        
        // 創建進度漸變
        QLinearGradient gradient(progressRect.topLeft(), progressRect.bottomRight());
        if (m_theme.name.isEmpty()) {
            gradient.setColorAt(0, QColor(100, 150, 255, static_cast<int>(255 * m_pulseOpacity)));
            gradient.setColorAt(1, QColor(50, 100, 200, static_cast<int>(255 * m_pulseOpacity)));
        } else {
            QColor primary = m_theme.primaryColor;
            QColor secondary = m_theme.secondaryColor;
            primary.setAlpha(static_cast<int>(255 * m_pulseOpacity));
            secondary.setAlpha(static_cast<int>(255 * m_pulseOpacity));
            gradient.setColorAt(0, primary);
            gradient.setColorAt(1, secondary);
        }
        
        painter.fillRect(progressRect, gradient);
        
        // 發光效果
        if (m_glowEnabled) {
            QPen glowPen(m_theme.accentColor.isValid() ? m_theme.accentColor : QColor(150, 200, 255), 1);
            painter.setPen(glowPen);
            painter.drawRect(progressRect);
        }
    }
    
    // 邊框
    QPen borderPen(QColor(80, 120, 160), 1);
    painter.setPen(borderPen);
    painter.drawRect(rect);
    
    // 文字
    if (!text().isEmpty()) {
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(rect, Qt::AlignCenter, text());
    }
}

// ====================================================================
// LLMRequestItem Implementation
// ====================================================================

LLMRequestItem::LLMRequestItem(const QString &requestId, const LLMRequestConfig &config, QWidget *parent)
    : QFrame(parent)
    , m_requestId(requestId)
    , m_config(config)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setMinimumHeight(200);
    setMaximumHeight(300);
    
    setupUI();
    setupAnimations();
    
    // 設置學院主題
    if (ACADEMY_THEMES.contains(config.academy)) {
        setAcademyTheme(ACADEMY_THEMES[config.academy]);
    }
}

void LLMRequestItem::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(8);
    m_layout->setContentsMargins(12, 12, 12, 12);
    
    // 頭部信息
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    m_headerLabel = new QLabel(QString("請求 #%1").arg(m_requestId.left(8)), this);
    m_headerLabel->setFont(QFont("Arial", 10, QFont::Bold));
    headerLayout->addWidget(m_headerLabel);
    
    headerLayout->addStretch();
    
    m_modelLabel = new QLabel(QString("模型: %1").arg(m_config.modelName), this);
    m_modelLabel->setFont(QFont("Arial", 9));
    headerLayout->addWidget(m_modelLabel);
    
    m_academyLabel = new QLabel(QString("學院: %1").arg(m_config.academy), this);
    m_academyLabel->setFont(QFont("Arial", 9, QFont::Bold));
    headerLayout->addWidget(m_academyLabel);
    
    m_layout->addLayout(headerLayout);
    
    // 進度條
    m_progressBar = new CyberProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_layout->addWidget(m_progressBar);
    
    // 回應內容
    m_responseEdit = new QTextEdit(this);
    m_responseEdit->setMaximumHeight(100);
    m_responseEdit->setPlaceholderText("等待AI回應...");
    m_responseEdit->setReadOnly(true);
    m_layout->addWidget(m_responseEdit);
    
    // 按鈕區域
    m_buttonLayout = new QHBoxLayout();
    
    m_cancelButton = new QPushButton("取消", this);
    m_cancelButton->setMaximumWidth(80);
    connect(m_cancelButton, &QPushButton::clicked, this, &LLMRequestItem::onCancelClicked);
    m_buttonLayout->addWidget(m_cancelButton);
    
    m_retryButton = new QPushButton("重試", this);
    m_retryButton->setMaximumWidth(80);
    m_retryButton->setVisible(false);
    connect(m_retryButton, &QPushButton::clicked, this, &LLMRequestItem::onRetryClicked);
    m_buttonLayout->addWidget(m_retryButton);
    
    m_buttonLayout->addStretch();
    
    m_statusLabel = new QLabel("處理中...", this);
    m_statusLabel->setFont(QFont("Arial", 9));
    m_buttonLayout->addWidget(m_statusLabel);
    
    m_layout->addLayout(m_buttonLayout);
}

void LLMRequestItem::setupAnimations()
{
    // 陰影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(10);
    m_shadowEffect->setColor(QColor(0, 0, 0, 60));
    m_shadowEffect->setOffset(0, 2);
    setGraphicsEffect(m_shadowEffect);
    
    // 發光動畫
    m_glowAnimation = new QPropertyAnimation(m_shadowEffect, "color", this);
    m_glowAnimation->setDuration(2000);
    m_glowAnimation->setLoopCount(-1);
    
    // 更新動畫定時器
    QTimer *updateTimer = new QTimer(this);
    updateTimer->setInterval(100);
    connect(updateTimer, &QTimer::timeout, this, &LLMRequestItem::updateAnimation);
    updateTimer->start();
}

void LLMRequestItem::updateProgress(int progress)
{
    m_progressBar->setValue(progress);
    
    if (progress >= 100) {
        m_statusLabel->setText("完成");
        m_cancelButton->setVisible(false);
    } else {
        m_statusLabel->setText(QString("進度: %1%").arg(progress));
    }
}

void LLMRequestItem::updateResponse(const QString &content)
{
    QString currentText = m_responseEdit->toPlainText();
    m_responseEdit->setPlainText(currentText + content);
    
    // 滾動到底部
    QTextCursor cursor = m_responseEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_responseEdit->setTextCursor(cursor);
}

void LLMRequestItem::setCompleted(bool success, const QString &message)
{
    m_isCompleted = true;
    m_progressBar->setValue(100);
    m_cancelButton->setVisible(false);
    
    if (success) {
        m_statusLabel->setText("✅ 成功");
        m_statusLabel->setStyleSheet("color: #50C878;");
    } else {
        m_statusLabel->setText("❌ 失敗");
        m_statusLabel->setStyleSheet("color: #FF6B6B;");
        m_retryButton->setVisible(true);
        
        if (!message.isEmpty()) {
            m_responseEdit->setPlainText(message);
        }
    }
    
    applyTheme();
}

void LLMRequestItem::setAcademyTheme(const AcademyTheme &theme)
{
    m_theme = theme;
    m_progressBar->setAcademyTheme(theme);
    applyTheme();
}

void LLMRequestItem::applyTheme()
{
    QString styleSheet = QString(
        "LLMRequestItem {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 rgba(%1, %2, %3, 30), "
        "        stop:1 rgba(%4, %5, %6, 20));"
        "    border: 2px solid rgba(%7, %8, %9, 100);"
        "    border-radius: 8px;"
        "}"
    ).arg(m_theme.primaryColor.red()).arg(m_theme.primaryColor.green()).arg(m_theme.primaryColor.blue())
     .arg(m_theme.secondaryColor.red()).arg(m_theme.secondaryColor.green()).arg(m_theme.secondaryColor.blue())
     .arg(m_theme.accentColor.red()).arg(m_theme.accentColor.green()).arg(m_theme.accentColor.blue());
    
    setStyleSheet(styleSheet);
    
    m_academyLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(m_theme.accentColor.name()));
}

void LLMRequestItem::onCancelClicked()
{
    emit cancelRequested(m_requestId);
}

void LLMRequestItem::onRetryClicked()
{
    emit retryRequested(m_requestId);
    m_retryButton->setVisible(false);
    m_cancelButton->setVisible(true);
    m_statusLabel->setText("重新處理中...");
    m_statusLabel->setStyleSheet("");
    m_progressBar->setValue(0);
    m_isCompleted = false;
}

void LLMRequestItem::updateAnimation()
{
    if (!m_isCompleted) {
        // 處理中的動畫效果
        static int animationStep = 0;
        animationStep = (animationStep + 1) % 60;
        
        double opacity = 0.5 + 0.5 * qSin(animationStep * M_PI / 30.0);
        QColor glowColor = m_theme.primaryColor.isValid() ? m_theme.primaryColor : QColor(100, 150, 255);
        glowColor.setAlphaF(opacity);
        
        m_shadowEffect->setColor(glowColor);
    }
}

// ====================================================================
// BatchRequestPanel Implementation
// ====================================================================

BatchRequestPanel::BatchRequestPanel(QWidget *parent)
    : QScrollArea(parent)
{
    setupUI();
}

void BatchRequestPanel::setupUI()
{
    m_contentWidget = new QWidget();
    m_layout = new QVBoxLayout(m_contentWidget);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->addStretch();
    
    setWidget(m_contentWidget);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // 自定義滾動條樣式
    setStyleSheet(
        "QScrollArea {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: rgba(50, 60, 70, 100);"
        "    width: 12px;"
        "    border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: rgba(100, 150, 255, 150);"
        "    border-radius: 6px;"
        "    min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: rgba(120, 170, 255, 200);"
        "}"
    );
}

void BatchRequestPanel::addRequest(const QString &requestId, const LLMRequestConfig &config)
{
    if (m_requestItems.contains(requestId)) {
        return;
    }
    
    auto *item = new LLMRequestItem(requestId, config, this);
    connect(item, &LLMRequestItem::cancelRequested, this, &BatchRequestPanel::onRequestCancel);
    connect(item, &LLMRequestItem::retryRequested, this, &BatchRequestPanel::onRequestRetry);
    
    m_requestItems[requestId] = item;
    
    // 插入到最後一個stretch之前
    m_layout->insertWidget(m_layout->count() - 1, item);
    
    // 滾動到新項目
    QTimer::singleShot(100, [this, item]() {
        ensureWidgetVisible(item);
    });
}

void BatchRequestPanel::updateRequest(const QString &requestId, const LLMResponse &response)
{
    if (!m_requestItems.contains(requestId)) {
        return;
    }
    
    LLMRequestItem *item = m_requestItems[requestId];
    
    if (response.isDone) {
        item->setCompleted(!response.isError, response.isError ? response.errorMessage : "");
        if (!response.content.isEmpty()) {
            item->updateResponse(response.content);
        }
    } else if (!response.content.isEmpty()) {
        item->updateResponse(response.content);
    }
}

void BatchRequestPanel::updateProgress(const QString &requestId, int progress)
{
    if (m_requestItems.contains(requestId)) {
        m_requestItems[requestId]->updateProgress(progress);
    }
}

void BatchRequestPanel::updateStream(const QString &requestId, const QString &content)
{
    if (m_requestItems.contains(requestId)) {
        m_requestItems[requestId]->updateResponse(content);
    }
}

void BatchRequestPanel::removeRequest(const QString &requestId)
{
    if (m_requestItems.contains(requestId)) {
        LLMRequestItem *item = m_requestItems.take(requestId);
        m_layout->removeWidget(item);
        item->deleteLater();
    }
}

void BatchRequestPanel::clearAll()
{
    for (auto it = m_requestItems.begin(); it != m_requestItems.end(); ++it) {
        m_layout->removeWidget(it.value());
        it.value()->deleteLater();
    }
    m_requestItems.clear();
}

int BatchRequestPanel::getActiveCount() const
{
    int count = 0;
    for (const auto *item : m_requestItems) {
        if (!item->isCompleted()) {
            count++;
        }
    }
    return count;
}

int BatchRequestPanel::getCompletedCount() const
{
    int count = 0;
    for (const auto *item : m_requestItems) {
        if (item->isCompleted()) {
            count++;
        }
    }
    return count;
}

int BatchRequestPanel::getTotalCount() const
{
    return m_requestItems.size();
}

void BatchRequestPanel::onRequestCancel(const QString &requestId)
{
    emit requestCancelled(requestId);
}

void BatchRequestPanel::onRequestRetry(const QString &requestId)
{
    emit requestRetry(requestId);
}

AcademyTheme BatchRequestPanel::getAcademyTheme(const QString &academy) const
{
    return ACADEMY_THEMES.value(academy, ACADEMY_THEMES["戰士"]);
}

// ====================================================================
// StatsDashboard Implementation  
// ====================================================================

StatsDashboard::StatsDashboard(QWidget *parent)
    : QFrame(parent)
    , m_updateTimer(new QTimer(this))
{
    setMinimumHeight(150);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    
    setupUI();
    setupAnimations();
}

void StatsDashboard::setupUI()
{
    m_updateTimer->setInterval(1000);
    connect(m_updateTimer, &QTimer::timeout, this, QOverload<>::of(&StatsDashboard::update));
    m_updateTimer->start();
}

void StatsDashboard::setupAnimations()
{
    // 添加發光效果
    auto *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(getThemeColor());
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);
}

void StatsDashboard::updateStats(int active, int total, double avgTime, const QMap<QString, int> &modelUsage)
{
    m_activeRequests = active;
    m_totalRequests = total;
    m_averageTime = avgTime;
    m_modelUsage = modelUsage;
    update();
}

void StatsDashboard::setTheme(ThemeMode theme)
{
    m_theme = theme;
    update();
}

void StatsDashboard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect rect = this->rect().adjusted(5, 5, -5, -5);
    
    // 背景
    QLinearGradient backgroundGradient(rect.topLeft(), rect.bottomRight());
    backgroundGradient.setColorAt(0, QColor(25, 30, 40, 200));
    backgroundGradient.setColorAt(1, QColor(15, 20, 30, 200));
    painter.fillRect(rect, backgroundGradient);
    
    // 邊框
    QPen borderPen(getThemeColor(), 2);
    painter.setPen(borderPen);
    painter.drawRect(rect);
    
    // 繪制統計信息
    painter.setPen(QColor(255, 255, 255));
    QFont font("Arial", 12, QFont::Bold);
    painter.setFont(font);
    
    int y = rect.top() + 25;
    int lineHeight = 25;
    
    painter.drawText(rect.left() + 15, y, "📊 系統統計");
    y += lineHeight * 1.5;
    
    painter.setFont(QFont("Arial", 10));
    painter.drawText(rect.left() + 15, y, QString("活動請求: %1").arg(m_activeRequests));
    y += lineHeight;
    
    painter.drawText(rect.left() + 15, y, QString("總請求數: %1").arg(m_totalRequests));
    y += lineHeight;
    
    painter.drawText(rect.left() + 15, y, QString("平均回應時間: %1ms").arg(QString::number(m_averageTime, 'f', 1)));
    y += lineHeight;
    
    // 模型使用統計
    if (!m_modelUsage.isEmpty()) {
        painter.setFont(QFont("Arial", 9));
        painter.drawText(rect.left() + 15, y, "模型使用統計:");
        y += lineHeight * 0.8;
        
        for (auto it = m_modelUsage.begin(); it != m_modelUsage.end(); ++it) {
            painter.drawText(rect.left() + 25, y, QString("• %1: %2次").arg(it.key()).arg(it.value()));
            y += lineHeight * 0.7;
        }
    }
}

QColor StatsDashboard::getThemeColor() const
{
    switch (m_theme) {
    case ThemeMode::CyberBlue:
        return QColor(100, 150, 255);
    case ThemeMode::NeonGreen:
        return QColor(50, 255, 120);
    case ThemeMode::PurpleHaze:
        return QColor(180, 100, 255);
    case ThemeMode::RedAlert:
        return QColor(255, 80, 80);
    case ThemeMode::GoldenGlow:
        return QColor(255, 200, 50);
    default:
        return QColor(100, 150, 255);
    }
}

QString StatsDashboard::getThemeGradient() const
{
    QColor color = getThemeColor();
    return QString("qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgba(%1, %2, %3, 100), stop:1 rgba(%4, %5, %6, 50))")
           .arg(color.red()).arg(color.green()).arg(color.blue())
           .arg(color.red()).arg(color.green()).arg(color.blue());
}

// 待續...下一部分將包含GameEventSyncer和LLMMainWindow的實現
