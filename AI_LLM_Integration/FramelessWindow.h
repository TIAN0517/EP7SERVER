#pragma once

#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPoint>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

/**
 * @brief 自定義無邊框窗口基類
 */
class FramelessWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool m_isDragging = false;
    QPoint m_dragStartPosition;
    QPoint m_windowPositionAsDrag;
    
    void setupWindowFlags();
    void createShadowEffect();
};
