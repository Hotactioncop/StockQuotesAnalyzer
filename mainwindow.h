#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mymodel.h"
#include "resultsmodel.h"
#include <QComboBox>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QModelIndex>
#include <QPair>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSet>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <math.h>

#include <QBrush>
#include <QColor>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHash>
#include <QLinearGradient>
#include <QList>
#include <QPen>
#include <QPoint>
#include <QSet>
#include <QStringList>
#include <QTime>
#include <dealline.h>
#include <set>

#include "atrdialog.h"
#include "candlebar.h"
#include "customproxymodel.h"
#include "graphicslinepolygon.h"
#include "newscene.h"

using namespace std;
namespace Ui {
class MainWindow;
}
namespace Candle {
struct Bar { //Структура для хранения каждого 15минутного участка
  double open, high, low, close;
  QDate time;
  Bar(QDate time, double open, double high, double low, double close)
      : time(time), open(open), high(high), low(low), close(close) {}
};
struct BuySellPos {
  double open, stopLoss, takeProfit, close, profit;
  bool direction; // 0-down, 1- up;
  int dealDeposit;
  int openBar;
  QString dealLotSize;
  QDate timeOpen;
  QDate timeClose;
  bool TPpassed;
  BuySellPos(QDate timeOpen, double open, double stopLoss, double takeProfit,
             bool direction)
      : timeOpen(timeOpen), open(open), stopLoss(stopLoss),
        takeProfit(takeProfit), direction(direction) {}
};
} // namespace Candle
using namespace Candle;
class MainWindow : public QMainWindow {
  Q_OBJECT
  QList<Bar> barList; //храним все дневки в данном контейнере
  QList<BuySellPos> BuySellList; //Храним все операции в контейнере
  QList<BuySellPos> OpenList; //Храним все операции в контейнере
  QMultiMap<int, BuySellPos> DealMap;
  bool inDeal = false; //статус
  MyModel *model = nullptr;
  QLabel *O, *H, *L, *C, *D;

  double pointToSetProfit = 0;
  double pointToSetStop = 0;
  double pointToSetStopLast = 0;
  double spread = 0;
  int insertedStopLoss;
  enum typeStop { fixed = 1, currentHigh, highOfBoth };
  typeStop stopLossType;

  double extraSL;
  double totalPips;

  enum typeTake { fix = 1, rated };
  typeTake takeProfitType;
  double rate = 3.0;

  int barBeforeTurn; //Кол-во баров в одном направлении до разворотной свечи
  double turnBarSize; //Отношение закрытия бара текущего к предыдущему.

  bool signalForStart;

  int Deposit;
  double Bet;
  int Shoulder;
  double lotSize = 0;

  ResultsModel *advancedModel = nullptr;

  QSet<double> height;
  bool resizeMark = true; // WheelEvent вызывается дважды. Когда тру - ресайз и
                          // меняем на фолс. Когда фолс - только замена на тру.
  bool graphEnd =
      false; //Вводим булеву для определения, стоит ли график в конце, если да,
             //то при ресайзе, двигается левая сторона, а не правая. Если нет,
             //то бары добавляются и исчезают справа.
  double screenSize;
  double maxSet;
  double minSet;
  double highLowDiff; //Если все элементы окажутся на одном уровне, то установим
                      //шкалу, как в предыдущий раз.
  int pos = 3;
  int sceneWidth = 600;
  int sceneheight = 480;
  int paddingTop =
      70; //Отступ от края сцены сверху - от него рисуются элементы графика
  int paddingBottom =
      30; //Отступ от края сцены снизу - до него рисуются элементы графика
  int paddingVolume = 0; //Отступ снизу для изображения графика объемов
  int barWidth = 13;
  int barSpace = 3; //Место между барами
  int barPos = 1;
  int bonusSpace = 30;
  int bonusSpaceUp = 90;
  int bonusSpaceDown = 10;
  QBrush brush1;
  QPen pen;       //Ручка для свечного графика
  QPen penLine;   //Ручка для линейного графика
  QPen penCandel; //Ручка для обрамления свечей = 0.
  QPen netPen;    //Ручка для прорисовки сетки
  QPen boxinfo;
  GraphicsLinePolygon *polygon;

  QGraphicsItemGroup *group1;
  QPen crossLine;
  bool crossArrow = false;
  QPointF prevMousePoint;
  QPointF mousePoint;

  QGraphicsRectItem *rectDown;
  QGraphicsTextItem *textDown;
  double pixelsize;
  QGraphicsRectItem *rectRight;
  QGraphicsTextItem *textRight;

  QGraphicsRectItem *rectDown2;
  QGraphicsRectItem *rectRight2;
  QGraphicsTextItem *textDown2;
  QGraphicsTextItem *textRight2;

  QGraphicsRectItem *crossRect;
  QGraphicsTextItem *crossText;

  QList<QGraphicsTextItem *> axisY;

  QVector<int> barWidthList{27, 13 /*,11*/, 5, 3, 1, 1}; //Размер бара
  QVector<int> barSpaceList{5, 3 /*,5*/, 3, 1, 1, 0}; //Расстояние между барами
  QVector<int> barScrollList{4,  8,  16,
                             32, 64, 128}; //Количество баров для скролла
  int barIndex = 1; //Индекс какой размер бара и расстояние сейчас актуален (13
                    //размер и 2 пикселя расстояние)

  bool ctrlPressed = false;
  double barResize =
      0; /*Поскольку прокрутка колеса мыши вызывает wheelevent дважды, то ввожу
         barResize, который на каждую прокрутку будет увеличиваться на 0.5 и
         операция ресайза будет выполняться один раз за прокрутку колеса мыши.
         При значении 1, barSize обнуляется.*/

  QFont font;
  QFont fontInfo; //Размер информационного текста в левом верхнем углу

  NewScene *scene;
  QVector<QGraphicsTextItem *> listItem;
  QVector<CandleBar *> candleVector;
  QGraphicsTextItem *textInfo;
  QGraphicsTextItem *pair;
  int type = 0; // 0-свеча, 1-линия.

  QGraphicsLineItem *crossLine1;
  QGraphicsLineItem *crossLine2;
  QGraphicsLineItem *crossLine3;
  QGraphicsLineItem *crossLine4;
  QGraphicsLineItem *lineItem;
  bool drawLineInCrosses = false;

  bool hideNet = false; //Индикатор- рисовать ли сетку на сцене

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
public slots:
  void calculate();
  void advancedCalculation();
  void preCalculation();
  void recountDeposit();
  void recountLot(double &);
  void showMore(QModelIndex);
  void checkYear(int);

protected:
  virtual void wheelEvent(QWheelEvent *event) override;
  virtual void resizeEvent(QResizeEvent *event) override;
  virtual void keyPressEvent(QKeyEvent *event) override;
  virtual void keyReleaseEvent(QKeyEvent *event) override;
  void drawSceneItems(int);
  void drawSceneLine(int);
  void drawSceneNet();
public slots:
  void showBarInfo(int &);
  void splitterMoved(int, int);
  void fillStatusBar(QPointF &);
  void setSlider(int);
};

#endif // MAINWINDOW_H
