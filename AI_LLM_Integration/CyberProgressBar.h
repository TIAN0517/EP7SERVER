#pragma once

#include <QProgressBar>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include "AcademyTheme.h"

/**
 * @brief 科技感進度條組件
 */
class CyberProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit CyberProgressBar(QWidget *parent = nullptr);
    
    void setAcademyTheme(const AcademyTheme &theme);
    void setGlowEffect(bool enabled);
    void setAnimationSpeed(int speed);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updatePulse();

private:
    AcademyTheme m_theme;
    bool m_glowEnabled = true;
    int m_animationSpeed = 1000;
    QTimer *m_pulseTimer;
    double m_pulseOpacity = 1.0;
    
    void setupPulseAnimation();
};
