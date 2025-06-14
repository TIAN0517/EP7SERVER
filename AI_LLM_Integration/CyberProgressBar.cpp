#include "CyberProgressBar.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

CyberProgressBar::CyberProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    setRange(0, 100);
    setTextVisible(false);
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
    if (m_pulseTimer) {
        m_pulseTimer->setInterval(speed / 60); // 60 FPS
    }
}

void CyberProgressBar::setupPulseAnimation()
{
    m_pulseTimer = new QTimer(this);
    connect(m_pulseTimer, &QTimer::timeout, this, &CyberProgressBar::updatePulse);
    m_pulseTimer->start(16); // ~60 FPS
}

void CyberProgressBar::updatePulse()
{
    static qreal phase = 0.0;
    phase += 0.05;
    if (phase > 2.0 * M_PI) phase = 0.0;
    
    m_pulseOpacity = 0.5 + 0.5 * qSin(phase);
    update();
}

void CyberProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect progressRect = rect().adjusted(2, 2, -2, -2);
    
    // 背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(progressRect, 8, 8);
    painter.fillPath(bgPath, QColor(20, 25, 35));
    
    // 進度條
    if (value() > 0) {
        int progressWidth = (progressRect.width() * value()) / maximum();
        QRect fillRect(progressRect.x(), progressRect.y(), progressWidth, progressRect.height());
        
        QPainterPath fillPath;
        fillPath.addRoundedRect(fillRect, 8, 8);
        
        // 漸變填充
        QLinearGradient gradient(fillRect.topLeft(), fillRect.topRight());
        gradient.setColorAt(0, m_theme.primaryColor);
        gradient.setColorAt(0.5, m_theme.accentColor);
        gradient.setColorAt(1, m_theme.secondaryColor);
        
        painter.fillPath(fillPath, gradient);
        
        // 脈沖效果
        if (m_glowEnabled) {
            painter.setOpacity(m_pulseOpacity * 0.3);
            painter.fillPath(fillPath, m_theme.accentColor);
            painter.setOpacity(1.0);
        }
    }
      // 邊框
    QPen pen(m_theme.primaryColor, 1);
    painter.setPen(pen);
    painter.drawPath(bgPath);
}
