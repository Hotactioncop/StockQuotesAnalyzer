#ifndef DEALLINE_H
#define DEALLINE_H

#include <QObject>
#include <QGraphicsItem>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QFont>
#include <QPoint>
#include <QLinearGradient>
#include <QPolygonF>
#include <QPointF>
#include <QGraphicsLineItem>

class DealLine : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
    QFont font;
    int lineX,lineY,lineW,lineZ;
    int hight,low,takeProfit,stopLoss;
    bool direction;
    int ID;
    QVector<QPointF> vPoint;
    bool hovered = false;
public:
    explicit DealLine(int,int,int,int,int,int,int,int,int,bool,QObject *parent = nullptr);

signals:

public slots:

    // QGraphicsItem interface
public:
//    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // QGraphicsItem interface
public:
    virtual QPainterPath shape() const override;
};

#endif // DEALLINE_H
