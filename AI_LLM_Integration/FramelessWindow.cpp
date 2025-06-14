#include "FramelessWindow.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

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
    
    // 繪製圓角背景
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(10, 10, -10, -10), 12, 12);
    
    // 背景漸變
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(26, 35, 53));
    gradient.setColorAt(1, QColor(15, 20, 31));
    
    painter.fillPath(path, gradient);
    
    // 邊框
    QPen pen(QColor(70, 130, 255, 100), 2);
    painter.setPen(pen);
    painter.drawPath(path);
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPosition = event->globalPosition().toPoint();
        m_windowPositionAsDrag = pos();
    }
    QMainWindow::mousePressEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint offset = event->globalPosition().toPoint() - m_dragStartPosition;
        move(m_windowPositionAsDrag + offset);
    }
    QMainWindow::mouseMoveEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

void FramelessWindow::resizeEvent(QResizeEvent *event)
{    QMainWindow::resizeEvent(event);
    update();
}
