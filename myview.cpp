#include "myview.h"
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsItem>

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

MyView::MyView(QWidget *parent)
{
    Q_UNUSED(parent);

    centerOn(0, 0);

    m_zoomDelta = 0.1;
    m_panSpeed = 1;
    m_doMousePanning = false;
    m_scale = 1.0;
    m_panButton = Qt::LeftButton;
    m_zoomKey = Qt::Key_Z;
    m_mouseDirection = 0;
}

qreal MyView::getScale() const
{
    return m_scale;
}

void MyView::keyPressEvent(QKeyEvent * event)
{
    qint32 key = event->key();

    if (key == Qt::Key_Plus || key == Qt::Key_Equal)
        zoomIn();
    else if (key == Qt::Key_Minus || key == Qt::Key_Underscore)
         zoomOut();
    else if (key == Qt::Key_Escape)
         zoomReset();
    else
        QGraphicsView::keyPressEvent(event);
}

void MyView::keyReleaseEvent(QKeyEvent * event)
{
    QGraphicsView::keyReleaseEvent(event);
}

void MyView::mouseMoveEvent(QMouseEvent * event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (m_doMousePanning){
        QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
        qreal tmp;
        switch(m_mouseDirection) {
        case 1:
        {
            tmp = mouseDelta.x();
            mouseDelta.setX(mouseDelta.y());
            mouseDelta.setY(-tmp);
            break;
        }
        case 2:
        {
            mouseDelta.setX(-mouseDelta.x());
            mouseDelta.setY(-mouseDelta.y());
            break;
        }
        case 3:
        {
            tmp = mouseDelta.x();
            mouseDelta.setX(-mouseDelta.y());
            mouseDelta.setY(tmp);
            break;
        }
        default:
                break;
        }
        pan(mouseDelta);
    }

    m_lastMousePos = event->pos();
}

void MyView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->button() == m_panButton && event->modifiers() != Qt::CTRL){
        m_lastMousePos = event->pos();
        m_doMousePanning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void MyView::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == m_panButton){
        m_doMousePanning = false;
    }
    setCursor(Qt::ArrowCursor);
    QGraphicsView::mouseReleaseEvent(event);
}

void MyView::wheelEvent(QWheelEvent *event)
{
    QPoint scrollAmount = event->angleDelta();
    scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

void MyView::contextMenuEvent(QContextMenuEvent *event)
{
    QGraphicsView::contextMenuEvent(event);
}

void MyView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    zoomReset();
}

void MyView::zoom(qreal scaleFactor)
{
    scale(scaleFactor, scaleFactor);
    m_scale *= scaleFactor;
}

void MyView::zoomReset()
{
    m_scale = 1/m_scale;
    scale(m_scale,m_scale);
    m_scale = 1;
}

void MyView::zoomIn()
{
    zoom(1 + m_zoomDelta);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void MyView::zoomOut()
{
    zoom (1 - m_zoomDelta);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void MyView::pan(QPointF delta)
{
    // Scale the pan amount by the current zoom.
    delta *= m_scale;
    delta *= m_panSpeed;

    // Have panning be anchored from the mouse.
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(static_cast<int>(VIEW_WIDTH) / 2 -
                     static_cast<int>(delta.x()),
                     static_cast<int>(VIEW_HEIGHT) / 2 -
                     static_cast<int>(delta.y()));
    centerOn(mapToScene(newCenter));
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}
