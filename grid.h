#ifndef GRID_H
#define GRID_H

#include <QGraphicsObject>
#include <QPen>
#include <QList>
#include <QtMath>
#include <QFont>
#include <QContextMenuEvent>

class Grid : public QGraphicsObject
{
public:

    enum GridStyle {
        HexagonGrid,
        SquareGrid
    };

    enum Location {
        None,
        Top,
        Center,
        Bottom
    };

    enum Orientation {
        Flat_Top,
        Pointed_Top
    };

    enum CenterPoint {
        NoPoint,
        Dot,
        Square,
        Cross,
        Star,
        X_Mark
    };

    Grid(QWidget *parent = nullptr);

    QColor backgroundColor() const;
    QColor centerPointColor() const;
    QColor labelColor() const;
    QColor lineColor() const;
    QPolygonF poly() const;
    QString label() const;

    bool antiAliasing() const;

    int centerPointSymbol() const;
    int labelLocation() const;
    int labelSize() const;
    int orientation() const;
    int radius() const;
    int rotation() const;
    int style() const;

    qreal lineWidth() const;
    qreal xOffset() const;
    qreal yOffset() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void setAntiAliasing(bool onOff);
    void setBackgroundColor(const QColor &backgroundColor);
    void setBold(bool bold);
    void setCenterPointColor(const QColor &centerPointColor);
    void setCenterPointSymbol(const int symbol);
    void setItalic(bool italic);
    void setLabel(const QString &label);
    void setLabelColor(const QColor &labelColor);
    void setLabelFont(const QFont &labelFont);
    void setLabelLocation(int labelLocation);
    void setLabelSize(int labelSize);
    void setLineColor(const QColor &lineColor);
    void setLineWidth(const qreal &lineWidth);
    void setOrientation(int orientation);
    void setPoly(const QPolygonF &poly);
    void setRadius(int radius);
    void setRelativeScale(const qreal &relativeScale);
    void setRotation(int rotation);
    void setStyle(int style);

private:
    QBrush       m_brush;
    QColor       m_backgroundColor;
    QColor       m_centerPointColor;
    QColor       m_labelColor;
    QColor       m_lineColor;
    QFont        m_labelFont;
    QPainterPath shape() const override;
    QPen         m_pen;
    QPolygonF    m_poly;
    QRectF       boundingRect() const override;
    QString      m_label;
    QWidget     *m_parent;
    bool         m_antiAliasing;
    int          m_centerPointSymbol;
    int          m_labelLocation;
    int          m_labelSize;
    int          m_orientation;
    int          m_radius;
    int          m_rotation;
    int          m_style;
    qreal        m_angle;
    qreal        m_lineWidth;
    qreal        m_oldWidth;
    qreal        m_relativeScale;
    qreal        m_xOffset;
    qreal        m_yOffset;
    void drawCenterPoint(QPainter *painter);
    void drawLabel(QPainter *painter);
};

#endif // GRID_H
