#include "candlebar.h"

CandleBar::CandleBar(int ID, int X, int Y, int W, int Z, int lX, int lY, int lW,
                     int lZ, bool direct, QObject *parent)
    : ID(ID), rectX(X), rectY(Y), rectW(W), rectZ(Z), lineX(lX), lineY(lY),
      lineW(lW), lineZ(lZ), direction(direct), QObject(parent),
      QGraphicsItem() {
  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
  font.setPointSize(7); //Устанавливаем размер шрифта для оси У
  gradient = QLinearGradient(rectX, rectY, rectX + rectW, rectY + rectZ);
  if (direction) {
    gradient.setColorAt(0, QColor("#23439e"));
    gradient.setColorAt(1, QColor("#02b0b1"));
  } else {
    gradient.setColorAt(0, QColor("#f9a81b"));
    gradient.setColorAt(1, QColor("#ee423e"));
  }
  brush = QBrush(gradient);
}

QRectF CandleBar::boundingRect() const {
  return QRectF(rectX, rectY, rectW, rectZ);
}

void CandleBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget) {
  //    QPen pen;
  //    brush=QBrush(gradient);
  //    if(enter){
  //        painter->drawLine(lineX,lineY,lineW,lineZ);
  //        painter->setBrush(brush);
  //        QPen pen2(QColor(183,219,255));
  //        pen2.setWidth(1);
  //        painter->setPen(pen2);
  //        painter->drawRect(rectX,rectY,rectW,rectZ);
  //    }else{
  //        painter->drawLine(lineX,lineY,lineW,lineZ);
  //        painter->setPen(QPen(Qt::black));
  //        painter->setBrush(brush);
  //        painter->drawRect(rectX,rectY,rectW,rectZ);
  //    }

  QPen pen(QColor(103, 180, 101));
  pen.setWidth(1);
  if (direction) {
    brush = QBrush(QColor(Qt::white));
  } else {
    brush = QBrush(QColor("#313233"));
  }
  painter->setPen(pen);
  if (enter) {
    painter->drawLine(lineX, lineY, lineW, lineZ);
    painter->setBrush(brush);
    QPen pen2(QColor(183, 219, 255));
    pen2.setWidth(1);
    painter->setPen(pen2);
    painter->drawRect(rectX, rectY, rectW, rectZ);
  } else {
    painter->drawLine(lineX, lineY, lineW, lineZ);
    painter->setBrush(brush);
    painter->drawRect(rectX, rectY, rectW, rectZ);
  }
}

void CandleBar::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  enter = true;
  emit sendIDinfo(ID);
  update();
}

void CandleBar::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  enter = false;
  emit clearIDinfo();
  update();
}
