#include "dealline.h"

DealLine::DealLine(int ID, int lX, int lY, int lW, int lZ, int H, int L, int TP,
                   int SL, bool direct, QObject *parent)
    : ID(ID), lineX(lX), lineY(lY), lineW(lW), lineZ(lZ), hight(H), low(L),
      takeProfit(TP), stopLoss(SL), direction(direct), QObject(parent),
      QGraphicsLineItem() {
  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
  vPoint.push_back(QPointF(lineX, lineY));
  vPoint.push_back(QPointF(lineW, lineZ));
}

// QRectF DealLine::boundingRect() const
//{
//    return QRectF(lineX-1,lineY,lineW,lineZ);
//}

void DealLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
  QPen pen;
  pen.setStyle(Qt::DashLine);
  if (hovered) {
    pen.setColor(Qt::blue);
  } else {
    if (direction) {
      pen.setColor(QColor("#5CCCCC"));
    } else {
      pen.setColor(QColor("#FFB273"));
    }
  }
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(pen);
  painter->drawPolyline(QPolygonF(vPoint));
  pen.setWidth(0); //лучше -1, тогда линии не видно. Но в дебаге выдает
                   //информацию, что значение не может быть отрицательным.
  painter->setPen(pen);
  QBrush brush(Qt::blue);
  painter->setBrush(brush);
  QPolygonF triangleStart;
  triangleStart.append(QPointF(lineX, lineY));
  triangleStart.append(QPointF(lineX - 7, lineY - 7));
  triangleStart.append(QPointF(lineX - 7, lineY + 7));
  painter->drawConvexPolygon(triangleStart);

  QBrush brush1("#EDC60C");
  painter->setBrush(brush1);
  QPolygonF triangleEnd;
  triangleEnd.append(QPointF(lineW, lineZ));
  triangleEnd.append(QPointF(lineW + 7, lineZ - 7));
  triangleEnd.append(QPointF(lineW + 7, lineZ + 7));
  //    triangle.append(QPointF(lineX,lineY));
  painter->drawConvexPolygon(triangleEnd);
  QPen pen2;
  pen2.setWidth(0);
  pen2.setColor(QColor("#FFB273"));
  pen2.setStyle(Qt::DashDotLine);
  painter->setPen(pen2);
  painter->drawLine(lineX, stopLoss, lineW, stopLoss);
  pen2.setColor(QColor("#5CCCCC"));
  painter->setPen(pen2);
  painter->drawLine(lineX, takeProfit, lineW, takeProfit);
  if (takeProfit > stopLoss) {
    QBrush brush(Qt::cyan);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    QPolygonF arrow;
    int highest = (stopLoss < hight) ? stopLoss : hight;
    arrow.append(QPointF(lineX + 3, highest - 10 + 3));
    arrow.append(QPointF(lineX + 3, highest - 10 - 6));
    arrow.append(QPointF(lineX, highest - 10 - 3));
    arrow.append(QPointF(lineX - 6, highest - 10 - 9));
    arrow.append(QPointF(lineX - 9, highest - 10 - 6));
    arrow.append(QPointF(lineX - 3, highest - 10));
    arrow.append(QPointF(lineX - 6, highest - 10 + 3));
    painter->drawConvexPolygon(arrow);
  } else {
    QBrush brush(Qt::cyan);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    QPolygonF arrow;
    int lowest = (stopLoss > low) ? stopLoss : low;
    arrow.append(QPointF(lineX + 3, lowest + 10 - 3));
    arrow.append(QPointF(lineX + 3, lowest + 10 + 6));
    arrow.append(QPointF(lineX, lowest + 10 + 3));
    arrow.append(QPointF(lineX - 6, lowest + 10 + 9));
    arrow.append(QPointF(lineX - 9, lowest + 10 + 6));
    arrow.append(QPointF(lineX - 3, lowest + 10));
    arrow.append(QPointF(lineX - 6, lowest + 10 - 3));
    painter->drawConvexPolygon(arrow);
  }
}

void DealLine::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  hovered = true;
  update();
}

void DealLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  hovered = false;
  update();
}

QPainterPath DealLine::shape() const {
  QPainterPath path;
  path.addPolygon(QPolygonF(QPolygonF(vPoint)));
  return path;
}
