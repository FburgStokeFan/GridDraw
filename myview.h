#ifndef MY_VIEW_H
#define MY_VIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

class MyView : public QGraphicsView
{

public:

    MyView(QWidget *parent = nullptr);
    Qt::MouseButton m_panButton;
    int m_mouseDirection;
    int m_zoomKey;
    qreal getScale() const;
    qreal m_panSpeed;
    qreal m_xdir = 1, m_ydir = 1;
    qreal m_zoomDelta;
    void pan(QPointF delta);
    void zoom(qreal scaleFactor);
    void zoomIn();
    void zoomOut();
    void zoomReset();

protected:
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:

    QPoint m_lastMousePos;
    bool m_doMousePanning;
    qreal m_scale;
};

#endif // MY_VIEW_H
