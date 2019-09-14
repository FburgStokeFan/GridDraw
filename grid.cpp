#include "grid.h"
#include <QtMath>
#include <QPainter>
#include <QRectF>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QDebug>
#include <QColorDialog>

Grid::Grid(QWidget *parent)
{
    m_parent = parent;
    m_radius = 1;
    m_lineWidth = 1;
    m_oldWidth = m_lineWidth;
    m_lineColor = Qt::black;
    m_backgroundColor = Qt::white;
    m_orientation = Flat_Top;
    m_antiAliasing = false;
    m_rotation = 0;
}

bool Grid::antiAliasing() const
{
    return m_antiAliasing;
}

QColor Grid::backgroundColor() const
{
    return m_backgroundColor;
}

QColor Grid::centerPointColor() const
{
    return m_centerPointColor;
}

QRectF Grid::boundingRect() const
{
    if(m_angle < 1) {
        // Flat top/bottom
        if(style() == HexagonGrid) {
            return QRectF(-radius(),
                          -radius() * sin(60 * M_PI/180.0),
                          2*radius(),
                          2*radius()*sin(60 * M_PI/180.0));
        } else {
            return QRectF(-m_radius * cos(45 * M_PI/180.0),
                          -m_radius * cos(45 * M_PI/180.0),
                          sqrt(m_radius*m_radius + m_radius*m_radius),
                          sqrt(m_radius*m_radius + m_radius*m_radius));
        }
    } else {
        // Pointed top/bottom
        if(style() == HexagonGrid) {
            return QRectF(-radius() * sin(60 * M_PI/180.0),
                          -radius(),
                          2*radius()*sin(60 * M_PI/180.0),
                          2*radius());
        } else {
            return QRectF(-m_radius,
                          -m_radius,
                          2*m_radius,
                          2*m_radius);
        }
    }
}

int Grid::style() const
{
    return m_style;
}

void Grid::setStyle(int style)
{
    m_style = style;
}

QPainterPath Grid::shape() const
{
    QPainterPath path;
    path.addPolygon(m_poly);
    return path;
}

int Grid::rotation() const
{
    return m_rotation;
}

void Grid::setRotation(int rotation)
{
    m_rotation = rotation;
}

QString Grid::label() const
{
    return m_label;
}

int Grid::centerPointSymbol() const
{
    return m_centerPointSymbol;
}

QColor Grid::labelColor() const
{
    return m_labelColor;
}

int Grid::labelLocation() const
{
    return m_labelLocation;
}

int Grid::labelSize() const
{
    return m_labelSize;
}

QColor Grid::lineColor() const
{
    return m_lineColor;
}

qreal Grid::lineWidth() const
{
    return m_lineWidth;
}

int Grid::orientation() const
{
    return m_orientation;
}

void Grid::drawLabel(QPainter *painter)
{
    qreal r;
    int ah = Qt::AlignHCenter;;
    int av = Qt::AlignAbsolute;
    QFont theFont = m_labelFont;

    if(!m_label.isEmpty()) {

        painter->setRenderHint(QPainter::TextAntialiasing,antiAliasing());
        setTransformOriginPoint(0,0);
        painter->rotate(m_rotation);
        painter->setPen(QPen(m_labelColor));
        painter->setFont(theFont);

        switch (m_labelLocation) {
        case Bottom:
        {
            av = Qt::AlignBottom;
            break;
        }
        case Center:
        {
            av = Qt::AlignVCenter;
            break;
        }
        case Top:
        {
            av = Qt::AlignTop;
            break;
        }
        default:
            break;
        }

        if(m_style == HexagonGrid) {
            r = radius()*sin(60 * M_PI/180.0) - (3 * lineWidth()/10.0);
        } else {
            r = radius()*sin(45 * M_PI/180.0) - (3 * lineWidth()/10.0);
        }

        painter->drawText(QRectF(-r,-r,2*r,2*r),av|ah,m_label);
    }
}

void Grid::drawCenterPoint(QPainter *painter)
{
    m_pen.setWidthF(0.5);
    m_pen.setCapStyle(Qt::SquareCap);
    m_pen.setJoinStyle(Qt::MiterJoin);

    switch (centerPointSymbol()) {
    case Dot:
        painter->drawEllipse(QPointF(0,0),2,2);
        break;
    case Square:
        painter->drawRect(-2,-2,4,4);
        break;
    case Cross:
    {
        painter->drawLine(QPointF(-3,0),QPointF(3,0));
        painter->drawLine(QPointF(0,3),QPointF(0,-3));
        break;
    }
    case Star:
    {
        for(int a = 0; a < 360; a+=60)
            painter->drawLine(QPointF(0,0),QPointF(3*cos(a*M_PI/180.0),3*sin(a*M_PI/180.0)));
        break;
    }
    case X_Mark:
    {
        for(int a = 45; a < 360; a+=90)
            painter->drawLine(QPointF(0,0),QPointF(3*cos(a*M_PI/180.0),3*sin(a*M_PI/180.0)));
        break;
    }
    default:
        break;
    }
}

void Grid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    m_brush = QBrush(backgroundColor(),Qt::SolidPattern);
    m_pen = QPen(lineColor(),lineWidth(),Qt::SolidLine);
    m_pen.setJoinStyle(Qt::RoundJoin);
    m_pen.setCapStyle(Qt::RoundCap);
    painter->setRenderHint(QPainter::Antialiasing,antiAliasing());
    painter->setBrush(m_brush);
    painter->setPen(m_pen);
    painter->drawPolygon(m_poly);
    setScale(m_relativeScale);
    painter->setPen(QPen(m_centerPointColor));
    painter->setBrush(QBrush(m_centerPointColor));

    drawCenterPoint(painter);
    drawLabel(painter);

}

QPolygonF Grid::poly() const
{
    return m_poly;
}

int Grid::radius() const
{
    return m_radius;
}

void Grid::setAntiAliasing(bool onOff)
{
    m_antiAliasing = onOff;
    update();
}

void Grid::setBackgroundColor(const QColor &backgroundColor)
{
    m_backgroundColor = backgroundColor;
    update();
}

void Grid::setLabel(const QString &label)
{
    m_label = label;
    update();
}

void Grid::setLabelColor(const QColor &labelColor)
{
    m_labelColor = labelColor;
    update();
}

void Grid::setLabelFont(const QFont &labelFont)
{
    m_labelFont = labelFont;
    update();
}

void Grid::setLabelLocation(int labelLocation)
{
    m_labelLocation = labelLocation;
    update();
}

void Grid::setLabelSize(int labelSize)
{
    m_labelSize = labelSize;
    update();
}

void Grid::setLineColor(const QColor &lineColor)
{
    m_lineColor = lineColor;
    update();
}

void Grid::setLineWidth(const qreal &lineWidth)
{
    m_lineWidth = lineWidth;
    update();
}

void Grid::setOrientation(int orientation)
{
    if(style() == HexagonGrid) {
        m_angle = 30.0 * static_cast<int>(orientation);
    } else {
        m_angle = 45.0 * static_cast<int>(orientation);
    }
    m_orientation = orientation;
    setRadius(m_radius);
    update();
}

void Grid::setPoly(const QPolygonF &polyHex)
{
    m_poly = polyHex;
}

void Grid::setRadius(int radius)
{
    m_radius = radius;

    // Define the polygon nodes
    if(m_style == HexagonGrid) {
        m_poly = QPolygonF() <<
            QPointF(radius * cos((  0+m_angle) * M_PI/180.0),radius * sin((  0+m_angle) * M_PI/180.0)) <<
            QPointF(radius * cos(( 60+m_angle) * M_PI/180.0),radius * sin(( 60+m_angle) * M_PI/180.0)) <<
            QPointF(radius * cos((120+m_angle) * M_PI/180.0),radius * sin((120+m_angle) * M_PI/180.0)) <<
            QPointF(radius * cos((180+m_angle) * M_PI/180.0),radius * sin((180+m_angle) * M_PI/180.0)) <<
            QPointF(radius * cos((240+m_angle) * M_PI/180.0),radius * sin((240+m_angle) * M_PI/180.0)) <<
            QPointF(radius * cos((300+m_angle) * M_PI/180.0),radius * sin((300+m_angle) * M_PI/180.0));

        if(m_orientation == Flat_Top) {
            m_xOffset = radius + radius * cos(60 * M_PI/180.0);
            m_yOffset = 2 * radius * sin(60 * M_PI/180.0);
        } else {
            m_xOffset = 2 * radius * sin(60 * M_PI/180.0);
            m_yOffset = radius + radius * cos(60 * M_PI/180.0);
        }
    } else {
        if(m_orientation == Flat_Top) {
            m_poly = QPolygonF() <<
                QPointF(-m_radius * cos(45 * M_PI/180.0),-m_radius * cos(45 * M_PI/180.0)) <<
                QPointF( m_radius * cos(45 * M_PI/180.0),-m_radius * cos(45 * M_PI/180.0)) <<
                QPointF( m_radius * cos(45 * M_PI/180.0), m_radius * cos(45 * M_PI/180.0)) <<
                QPointF(-m_radius * cos(45 * M_PI/180.0), m_radius * cos(45 * M_PI/180.0));

            m_xOffset = radius/cos(45 * M_PI/180.0);
            m_yOffset = radius/cos(45 * M_PI/180.0);

        } else {
            m_poly = QPolygonF() <<
                QPointF(-m_radius,0) <<
                QPointF(0,-m_radius) <<
                QPointF(m_radius,0) <<
                QPointF(0,m_radius);

            m_xOffset = m_radius;
            m_yOffset = m_radius;
        }

    }

    update();
}

void Grid::setRelativeScale(const qreal &relativeScale)
{
    m_relativeScale = relativeScale;
}

void Grid::setCenterPointColor(const QColor &centerPointColor)
{
    m_centerPointColor = centerPointColor;
    update();
}

void Grid::setCenterPointSymbol(const int symbol)
{
    m_centerPointSymbol = symbol;
    update();
}

qreal Grid::xOffset() const
{
    return m_xOffset;
}

qreal Grid::yOffset() const
{
    return m_yOffset;
}
