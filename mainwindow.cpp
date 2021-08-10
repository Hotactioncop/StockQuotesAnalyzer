#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBitmap>
#include <QCursor>
#include <QDesktopWidget>
#include <QPalette>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QtOpenGL/QGL>
#include <QtOpenGL/QGLFormat>
#include <QtOpenGL/QGLWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  pair = nullptr;
  crossLine1 = nullptr;
  crossLine2 = nullptr;
  rectDown = nullptr;
  rectRight = nullptr;
  textDown = nullptr;
  textRight = nullptr;
  signalForStart = false;
  advancedModel = nullptr;
  crossLine3 = nullptr;
  crossLine4 = nullptr;
  textDown2 = nullptr;
  textRight2 = nullptr;
  lineItem = nullptr;
  crossRect = nullptr;
  crossText = nullptr;

  ui->pushButtonATR->hide();

  setWindowTitle("Advanced Trade");
  setWindowIcon(QIcon(":/Images/market.ico"));
  connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(calculate()));
  connect(ui->buttonCount, SIGNAL(clicked(bool)), this, SLOT(preCalculation()));
  //    ui->frameAdvanced->hide();
  this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        this->size(),
                                        qApp->desktop()->availableGeometry()));
  ui->pushAdvanced->hide();
  //  ui->frame->setFixedWidth(1054);
  //  ui->frame_2->setFixedWidth(179);
  ui->pushAdvanced->setCheckable(true);
  ui->tableView->resizeColumnsToContents();
  connect(ui->pushAdvanced, &QPushButton::clicked, [this] {
    if (ui->pushAdvanced->isChecked()) {
      ui->frameAdvanced->show();
      ui->pushAdvanced->setCheckable(false);
      adjustSize();
    } else {
      this->adjustSize();
      ui->frameAdvanced->hide();
      ui->pushAdvanced->setCheckable(true);
      //            ui->centralWidget->resize(1362,400);
    }
  });
  //----------------Управление основного меню---------------------------
  {

    QRegExp spreadReg("[0-9]");
    QRegExpValidator *rSpread = new QRegExpValidator(spreadReg, this);
    ui->lineSpread->setValidator(rSpread);
    ui->lineSpread->setPlaceholderText("3");

    QRegExp StopLossReg("[0-9]{4}");
    QRegExpValidator *rStopLoss = new QRegExpValidator(StopLossReg, this);
    ui->lineStopLoss->setValidator(rStopLoss);
    ui->lineStopLoss->setPlaceholderText("85");

    QRegExp TakeProfitReg("[0-9]{4}");
    QRegExpValidator *rTakeProfit = new QRegExpValidator(TakeProfitReg, this);
    ui->lineTakeProfit->setValidator(rTakeProfit);
    ui->lineTakeProfit->setPlaceholderText("260");

    QRegExp Deposit("[0-9]{15}");
    QRegExpValidator *rDeposit = new QRegExpValidator(Deposit, this);
    ui->lineDeposit->setValidator(rDeposit);
    ui->lineDeposit->setPlaceholderText("10000");

    QRegExp Bet("^[1-9]$|^[1-9][0-9]$|^(100)$");
    QRegExpValidator *rBet = new QRegExpValidator(Bet, this);
    ui->lineBet->setValidator(rBet);
    ui->lineBet->setPlaceholderText("5%");

    QRegExp Shoulder("^[1-9]$|^[1-9][0-9]$|^(100)$");
    QRegExpValidator *rShoulder = new QRegExpValidator(Shoulder, this);
    ui->lineShoulder->setValidator(rShoulder);
    ui->lineShoulder->setPlaceholderText("100");

    QRegExp Barwith("^[0-9]{2}");
    QRegExpValidator *rBarwith = new QRegExpValidator(Barwith, this);
    ui->lineEditbarBeforeTurn->setValidator(rBarwith);
    ui->lineEditbarBeforeTurn->setPlaceholderText("2");

    QRegExp CloseToC("^[0-9]{3}");
    QRegExpValidator *rCloseToC = new QRegExpValidator(CloseToC, this);
    ui->TurnBarSize->setValidator(rCloseToC);
    ui->TurnBarSize->setPlaceholderText("40");

    QRegExp ExtraStop("^[0-9]{3}");
    QRegExpValidator *rExtraStop = new QRegExpValidator(ExtraStop, this);
    ui->lineEditExtraStopLoss->setValidator(rExtraStop);
    ui->lineEditExtraStopLoss->setPlaceholderText("10");

    connect(ui->Slider, SIGNAL(valueChanged(int)), this, SLOT(setSlider(int)));
    ui->lineEditRated->setText("2.0");

    connect(ui->comboBoxStopLoss,
            static_cast<void (QComboBox::*)(const QString &)>(
                &QComboBox::currentIndexChanged),
            [this] {
              if (ui->comboBoxStopLoss->currentIndex() == 0) {
                ui->lineStopLoss->setEnabled(true);
                ui->lineEditExtraStopLoss->setEnabled(false);
              } else {
                ui->lineStopLoss->setEnabled(false);
                ui->lineEditExtraStopLoss->setEnabled(true);
              }
            });

    connect(ui->comboBoxTakeProfit,
            static_cast<void (QComboBox::*)(const QString &)>(
                &QComboBox::currentIndexChanged),
            [this] {
              if (ui->comboBoxTakeProfit->currentIndex() == 0) {
                takeProfitType = fix;
                ui->lineTakeProfit->setEnabled(true);
                ui->lineEditRated->setEnabled(false);
                ui->Slider->setEnabled(false);
              } else {
                takeProfitType = rated;
                ui->lineTakeProfit->setEnabled(false);
                ui->lineEditRated->setEnabled(true);
                ui->Slider->setEnabled(true);
              }
            });
    takeProfitType = fix;
  }
  //----------------Управление расширенного меню---------------------------
  {

    QRegExp TakeProfitReg("[0-9]{,4}-[0-9]{,4}");
    QRegExpValidator *rTakeProfit = new QRegExpValidator(TakeProfitReg, this);
    ui->lineTakeProfitAdvanced->setValidator(rTakeProfit);
    ui->lineTakeProfitAdvanced->setPlaceholderText("0-9999");

    QRegExp StopLossReg("[0-9]{,4}-[0-9]{,4}");
    QRegExpValidator *rStopLoss = new QRegExpValidator(StopLossReg, this);
    ui->lineStopLossAdvanced->setValidator(rStopLoss);
    ui->lineStopLossAdvanced->setPlaceholderText("0-9999");

    QRegExp TPrated("([1-9]{1})\(\.[0-9]{1})?-([1-9]{1})\(\.[0-9]{1})?");
    QRegExpValidator *rTPrated = new QRegExpValidator(TPrated, this);
    ui->lineEditRated->setValidator(rTPrated);
    ui->lineEditRated->setPlaceholderText("1.0-10.0");
    ;

    QRegExp BarsBeforeRoundReg("[2-9][0-9]{,2}-[0-9]{,3}");
    QRegExpValidator *rBarsBeforeRound =
        new QRegExpValidator(BarsBeforeRoundReg, this);
    ui->barBeforeTurnAdvanced->setValidator(rBarsBeforeRound);
    ui->barBeforeTurnAdvanced->setPlaceholderText("2-20");

    QRegExp TurnBarSizeAdvancedReg("[1-9][0-9]{,3}-[0-9]{,4}");
    QRegExpValidator *rTurnBarSizeAdvanced =
        new QRegExpValidator(TurnBarSizeAdvancedReg, this);
    ui->TurnBarSizeAdvanced->setValidator(rTurnBarSizeAdvanced);
    ui->TurnBarSizeAdvanced->setPlaceholderText("0-100");

    QRegExp ExtraAdvancedReg("[0-9]{,4}-[0-9]{,4}");
    QRegExpValidator *rExtraAdvanced =
        new QRegExpValidator(ExtraAdvancedReg, this);
    ui->lineEditExtraAdvanced->setValidator(rExtraAdvanced);
    ui->lineEditExtraAdvanced->setPlaceholderText("0-100");

    QRegExp lineEditSLStepReg("[0-9]{,3}");
    QRegExpValidator *rlineEditSLStep =
        new QRegExpValidator(lineEditSLStepReg, this);
    ui->lineEditSLStep->setValidator(rlineEditSLStep);
    ui->lineEditSLStep->setPlaceholderText("10");

    QRegExp lineEditTPStepReg("[0-9]{,3}");
    QRegExpValidator *rlineEditTPStep =
        new QRegExpValidator(lineEditTPStepReg, this);
    ui->lineEditTPStep->setValidator(rlineEditTPStep);
    ui->lineEditTPStep->setPlaceholderText("10");

    QRegExp lineEditRatedAdvancedReg(
        "([1-9]{1})\(\.[0-9]{1})?-([1-9]{1})\(\.[0-9]{1})?");
    QRegExpValidator *rlineEditRatedAdvanced =
        new QRegExpValidator(lineEditRatedAdvancedReg, this);
    ui->lineEditRatedAdvanced->setValidator(rlineEditRatedAdvanced);
    ui->lineEditRatedAdvanced->setPlaceholderText("2");

    connect(ui->checkBoxSLfix, &QCheckBox::clicked, [this] {
      if (ui->checkBoxSLfix->isChecked()) {
        ui->lineStopLossAdvanced->setEnabled(true);
        ui->lineEditSLStep->setEnabled(true);
        if (!ui->checkBoxSLcurr->isChecked() &&
            !ui->checkBoxSLboth->isChecked()) {
          ui->lineEditExtraAdvanced->setEnabled(false);
        }
      } else {
        ui->lineStopLossAdvanced->setEnabled(false);
        ui->lineEditSLStep->setEnabled(false);
        if (ui->checkBoxSLcurr->isChecked() ||
            ui->checkBoxSLboth->isChecked()) {
          ui->lineEditExtraAdvanced->setEnabled(true);
        }
      }
    });

    connect(ui->checkBoxTPfix, &QCheckBox::clicked, [this] {
      if (ui->checkBoxTPfix->isChecked()) {
        ui->lineTakeProfitAdvanced->setEnabled(true);
        ui->lineEditTPStep->setEnabled(true);
      } else {
        ui->lineTakeProfitAdvanced->setEnabled(false);
        ui->lineEditTPStep->setEnabled(false);
      }
    });

    connect(ui->checkBoxSLcurr, &QCheckBox::clicked, [this] {
      if (ui->checkBoxSLcurr->isChecked()) {
        ui->lineEditExtraAdvanced->setEnabled(true);
      } else if (!ui->checkBoxSLboth->isChecked() &&
                 !ui->checkBoxSLcurr->isChecked()) {
        ui->lineEditExtraAdvanced->setEnabled(false);
      }
    });

    connect(ui->checkBoxSLboth, &QCheckBox::clicked, [this] {
      if (ui->checkBoxSLboth->isChecked()) {
        ui->lineEditExtraAdvanced->setEnabled(true);
      } else if (!ui->checkBoxSLboth->isChecked() &&
                 !ui->checkBoxSLcurr->isChecked()) {
        ui->lineEditExtraAdvanced->setEnabled(false);
      }
    });

    connect(ui->checkBoxTPrate, &QCheckBox::clicked, [this] {
      if (ui->checkBoxTPrate->isChecked()) {
        ui->lineEditRatedAdvanced->setEnabled(true);
      } else if (!ui->checkBoxTPrate->isChecked()) {
        ui->lineEditRatedAdvanced->setEnabled(false);
      }
    });
  };

  //  ui->tableViewAdvanced->setMinimumWidth(400);
  QPalette p;
  p.setColor(QPalette::Background, Qt::lightGray);
  ui->splitter->setPalette(p);
  ui->splitter_2->setPalette(p);
  ui->splitter_3->setPalette(p);
  connect(ui->tableViewAdvanced, SIGNAL(doubleClicked(QModelIndex)), this,
          SLOT(showMore(QModelIndex)));
  //  ui->splitter->setStretchFactor(0, 1);
  //  ui->splitter->setStretchFactor(1, 1);
  //  ui->splitter_2->setStretchFactor(0, 1);
  //  ui->splitter_2->setStretchFactor(1, 1);
  ui->comboBoxPair->addItem("AUD/USD");
  ui->comboBoxPair->addItem("EUR/USD");
  ui->comboBoxPair->addItem("GBP/USD");
  ui->comboBoxPair->addItem("NZD/USD");
  ui->comboBoxPair->addItem("GBP/JPY");
  ui->comboBoxPair->addItem("USD/JPY");
  ui->comboBoxPair->setCurrentIndex(2);

  ui->comboBoxYear->setCurrentIndex(6);
  ui->comboBoxYear_2->setCurrentIndex(6);
  QStandardItemModel *model = (QStandardItemModel *)ui->comboBoxYear_2->model();
  for (int i = 0; i < ui->comboBoxYear->currentIndex(); i++) {
    model->item(i)->setEnabled(false);
  }

  connect(ui->comboBoxYear, SIGNAL(currentIndexChanged(int)), this,
          SLOT(checkYear(int)));

  QBrush brush(QColor("#313233")); // QColor("#313233")
  scene = new NewScene(this);
  scene->setBackgroundBrush(brush);

  ui->graphicsView->setScene(scene);
  ui->graphicsView->adjustSize();
  connect(ui->splitter_3, SIGNAL(splitterMoved(int, int)), this,
          SLOT(splitterMoved(int, int)));

  //    connect(ui->splitter_3,&QSplitter::splitterMoved,[=]{
  //        adjustSize();
  //    });
  connect(
      ui->pushButton_2, &QPushButton::clicked,
      [this] { // Переключение между разными типами графиков(свечной и линейный)
        if (!signalForStart) {
          return;
        }
        if (!type) {
          ui->pushButton_2->setText("Candlestick");
          type = 1;
          sceneWidth = ui->widget->width() - 70;
          sceneheight = ui->widget->height() - 2;
          scene->setSceneRect(0, 0, sceneWidth, sceneheight);

          if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
            graphEnd = true;
            barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
          }
          if (graphEnd) {
            barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
          }
          //            if(type){
          if (barPos < 1) {
            barPos = 1;
          }
          drawSceneLine(3 + barWidth / 2 + barSpace);
          //            }else{
          //                drawSceneItems(3+barWidth/2+barSpace);
          //            }

        } else {
          ui->pushButton_2->setText("Line");
          type = 0;
          sceneWidth = ui->widget->width() - 70;
          sceneheight = ui->widget->height() - 2;
          scene->setSceneRect(0, 0, sceneWidth, sceneheight);

          if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
            graphEnd = true;
            barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
          }
          if (graphEnd) {
            barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
          }
          //            if(type){
          //                drawSceneLine(3+barWidth/2+barSpace);
          //            }else{
          drawSceneItems(3 + barWidth / 2 + barSpace);
          //            }
        }
      });
  //  ui->splitter_3->setStretchFactor(0, 1);
  //  ui->splitter_3->setStretchFactor(1, 1);

  this->setGeometry(QRect(Qt::AlignCenter, Qt::AlignCenter, 1280, 800));
  D = new QLabel(this);
  D->setFixedWidth(60);
  ui->statusBar->addPermanentWidget(D);
  O = new QLabel(this);
  O->setFixedWidth(50);
  ui->statusBar->addPermanentWidget(O);
  H = new QLabel(this);
  H->setFixedWidth(50);
  ui->statusBar->addPermanentWidget(H);
  L = new QLabel(this);
  L->setFixedWidth(50);
  ui->statusBar->addPermanentWidget(L);
  C = new QLabel(this);
  C->setFixedWidth(50);
  ui->statusBar->addPermanentWidget(C);

  QPixmap curs(":/Images/cross.ico");
  curs.scaled(100, 100, Qt::KeepAspectRatio);
  QBitmap bmap(":/Images/cross.ico");
  curs.setMask(bmap);
  QCursor cursorCross = QCursor(curs);

  connect(scene, SIGNAL(mousePos(QPointF &)), this,
          SLOT(fillStatusBar(QPointF &)));
  connect(scene, &NewScene::mousePress, [=] {
    prevMousePoint = mousePoint;
    drawLineInCrosses = true;
    fillStatusBar(mousePoint);
  });
  connect(scene, &NewScene::mouseRelease, [=] {
    drawLineInCrosses = false;
    //        fillStatusBar(mousePoint);
  });

  connect(ui->pushButtonArrow, &QPushButton::clicked, [=] {
    ui->pushButtonCursor->setChecked(false);
    ui->pushButtonArrow->setChecked(true);
    setCursor(Qt::ArrowCursor);
    if (signalForStart) {
      if (type) {
        drawSceneLine(3 + barWidth / 2 + barSpace);
      } else {
        drawSceneItems(3 + barWidth / 2 + barSpace);
      }
    }
  });
  connect(ui->pushButtonCursor, &QPushButton::clicked, [=] {
    ui->pushButtonArrow->setChecked(false);
    ui->pushButtonCursor->setChecked(true);
    setCursor(Qt::CrossCursor);
    if (signalForStart) {
      if (type) {
        drawSceneLine(3 + barWidth / 2 + barSpace);
      } else {
        drawSceneItems(3 + barWidth / 2 + barSpace);
      }
    }
  });
  connect(ui->checkBoxNet, &QCheckBox::toggled, [=] {
    if (ui->checkBoxNet->isChecked()) {
      hideNet = true;
      if (!signalForStart) {
        return;
      }
      if (type) {
        drawSceneLine(3 + barWidth / 2 + barSpace);
      } else {
        drawSceneItems(3 + barWidth / 2 + barSpace);
      }
    } else {
      hideNet = false;
      if (!signalForStart) {
        return;
      }
      if (type) {
        drawSceneLine(3 + barWidth / 2 + barSpace);
      } else {
        drawSceneItems(3 + barWidth / 2 + barSpace);
      }
    }
  });

  connect(ui->pushButtonATR, &QPushButton::clicked, [=] {
    ATRdialog *dialog = new ATRdialog();
    dialog->show();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
  });
  mousePoint = QPointF(0, 0);
  //    ui->graphicsView->setViewport( new
  //    QGLWidget(QGLFormat(QGL::SampleBuffers)));       //OpenGL
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::calculate() {
  QElapsedTimer timmm;
  timmm.start();
  barList.clear();
  BuySellList.clear();
  DealMap.clear();
  OpenList.clear();
  inDeal = false;
  spread = 0;
  pointToSetStop = 0;
  Deposit = 0;
  Bet = 0;
  Shoulder = 0;
  /*Кол-во баров в одном направлении:
   * Показатель что сделка открыта
   */
  if (model != nullptr) {
    model->clearModel();
  }

  if (ui->lineSpread->text().isEmpty()) {
    spread = 0.0003;
    ui->lineSpread->setText("3");
  } else {
    if (ui->lineSpread->text() == 0) {
      spread = 0.0;
    } else {
      spread = QString(ui->lineSpread->text()).toDouble() / 10000;
    }
  }

  if (ui->lineStopLoss->text().isEmpty()) {
    pointToSetStop = 0.0085;
    ui->lineStopLoss->setText("85");
  } else {
    pointToSetStop = QString(ui->lineStopLoss->text()).toDouble() / 10000;
  }

  if (ui->lineEditExtraStopLoss->text().isEmpty()) {
    extraSL = 0.001;
    ui->lineEditExtraStopLoss->setText("10");
  } else {
    extraSL = QString(ui->lineEditExtraStopLoss->text()).toDouble() / 10000;
  }

  if (takeProfitType == fix && ui->lineTakeProfit->text().isEmpty()) {
    pointToSetProfit = 0.0260;
    ui->lineTakeProfit->setText("260");
  } else {
    pointToSetProfit = QString(ui->lineTakeProfit->text()).toDouble() / 10000;
  }

  int beginDeposit;
  if (ui->lineDeposit->text().isEmpty()) {
    Deposit = 10000;
    ui->lineDeposit->setText("10000");
  } else {
    Deposit = ui->lineDeposit->text().toInt();
  }
  beginDeposit = Deposit;

  if (ui->lineBet->text().isEmpty()) {
    Bet = 0.05;
    ui->lineBet->setText("5");
  } else {
    Bet = QString(ui->lineBet->text()).toDouble() / 100;
  }

  if (ui->lineShoulder->text().isEmpty()) {
    Shoulder = 100;
    ui->lineShoulder->setText("100");
  } else {
    Shoulder = ui->lineShoulder->text().toInt();
  }

  if (ui->TurnBarSize->text().isEmpty()) {
    turnBarSize = 0.004;
    ui->TurnBarSize->setText("40");
  } else {
    turnBarSize = ui->TurnBarSize->text().toDouble() / 10000;
  }

  switch (ui->comboBoxStopLoss->currentIndex()) {
  case 0:
    ui->comboBoxStopLoss->setCurrentIndex(0);
    stopLossType = fixed;
    break;
  case 1:
    ui->comboBoxStopLoss->setCurrentIndex(1);
    stopLossType = currentHigh;
    break;
  case 2:
    ui->comboBoxStopLoss->setCurrentIndex(2);
    stopLossType = highOfBoth;
    break;
  }
  rate = ui->lineEditRated->text().toDouble();

  int barBeforeTurnCount =
      1; //Количество баров в одном направлении, до сигнала на разворот
  if (ui->lineEditbarBeforeTurn->text().toInt() < 2) {
    ui->lineEditbarBeforeTurn->setText("2");
    barBeforeTurn = 2;
  } else {
    barBeforeTurn = ui->lineEditbarBeforeTurn->text().toInt();
  }

  int year = ui->comboBoxYear->currentData(Qt::DisplayRole).toInt();
  int year_end = ui->comboBoxYear_2->currentData(Qt::DisplayRole).toInt();
  int day = 0;

  for (int i = year; i <= year_end; i++) {
    QFile Cash;
    if (ui->comboBoxPair->currentIndex() == -1) {
      ui->comboBoxPair->setCurrentIndex(2);
      Cash.setFileName(QString(":/Docs/GBPUSD_%1.txt").arg(i));
    } else if (ui->comboBoxPair->currentIndex() == 0) {
      Cash.setFileName(QString(":/Docs/AUDUSD_%1.txt").arg(i));
    } else if (ui->comboBoxPair->currentIndex() == 1) {
      Cash.setFileName(QString(":/Docs/EURUSD_%1.txt").arg(i));
    } else if (ui->comboBoxPair->currentIndex() == 2) {
      Cash.setFileName(QString(":/Docs/GBPUSD_%1.txt").arg(i));
    } else if (ui->comboBoxPair->currentIndex() == 3) {
      Cash.setFileName(QString(":/Docs/NZDUSD_%1.txt").arg(i));
    } else if (ui->comboBoxPair->currentIndex() == 4) {
      Cash.setFileName(QString(":/Docs/GBPJPY_%1.txt").arg(i));
    } else if (ui->comboBoxPair->currentIndex() == 5) {
      Cash.setFileName(QString(":/Docs/USDJPY_%1.txt").arg(i));
    }
    if (Cash.open(QIODevice::ReadOnly | QIODevice::Text)) {
      while (!Cash.atEnd()) {
        ++day;
        QString word = Cash.readLine();
        if (word.startsWith("<")) {
          continue;
        }
        QStringList separate = word.split("\t");

        QString dateT = separate.at(2);
        QDate xTime = QDate::fromString(dateT, "dd/MM/yy");
        xTime = xTime.addYears(100);
        if (xTime.dayOfWeek() == 6 || xTime.dayOfWeek() == 7) {
          continue;
        }
        Bar *barX = new Bar(xTime, QString(separate.at(4)).toDouble(),
                            QString(separate.at(5)).toDouble(),
                            QString(separate.at(6)).toDouble(),
                            QString(separate.at(7)).toDouble());
        barList.push_back(*barX);
        delete barX;
      }
    } else {
    }
    Cash.close();
  }

  inDeal = false;

  for (int i = 0; i < barList.size(); i++) { //Открываем позиции
    double barDifference = barList.at(i).close - barList.at(i).open;
    double barHigh = barList.at(i).high;
    double barLow = barList.at(i).low;
    double barClose = barList.at(i).close;
    Bar *currentBar = &barList[i];
    if (i - 2 >= 0) {
      if (barList[i - 1].open > barList[i - 1].close &&
          barList[i - 2].open > barList[i - 2].close) {
        barBeforeTurnCount++;
      } else if (barList[i - 1].open < barList[i - 1].close &&
                 barList[i - 2].open < barList[i - 2].close) {
        barBeforeTurnCount++;
      } else {
        barBeforeTurnCount = 1;
      }
      for (int j = 0; j < OpenList.size(); j++) {
        BuySellPos *dealBar = &OpenList[j];
        if (ui->checkBoxCloseLater->isChecked() &&
                (dealBar->direction &&
                 barHigh >= dealBar->takeProfit - spread) ||
            (!dealBar->direction && barLow <= dealBar->takeProfit + spread)) {
          OpenList[j].TPpassed = true;
          qDebug() << "OpenList[j].TPpassed=true;";
        }
        if (dealBar->direction &&
            barLow <=
                dealBar->stopLoss +
                    spread) { //Лоу бара пересек стоплосс-закрытие по стопу
          dealBar->close = dealBar->stopLoss;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (ui->checkBoxCloseEarlier->isChecked() &&
                   dealBar->direction && barClose < dealBar->open) {
          dealBar->close = barClose;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;

        } else if (ui->checkBoxCloseLater->isChecked() && dealBar->direction &&
                   OpenList.at(j).TPpassed &&
                   barList[i - 1].high >
                       barHigh) { //Хай бара пересек тэйкпрофит-закрытие по
                                  //тэйку
          dealBar->close = barClose; // dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (!ui->checkBoxCloseLater->isChecked() && dealBar->direction &&
                   barHigh >= dealBar->takeProfit -
                                  spread) { //Хай бара пересек
                                            //тэйкпрофит-закрытие по тэйку
          dealBar->close = dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (!dealBar->direction &&
                   barHigh >= dealBar->stopLoss -
                                  spread) { //Лоу бара пересек стоплосс-закрытие
                                            //по стопу
          dealBar->close = dealBar->stopLoss;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (ui->checkBoxCloseEarlier->isChecked() &&
                   !dealBar->direction && barClose > dealBar->open) {
          dealBar->close = barClose;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (ui->checkBoxCloseLater->isChecked() && !dealBar->direction &&
                   OpenList.at(j).TPpassed &&
                   barList[i - 1].low <
                       barLow) { //Хай бара пересек тэйкпрофит-закрытие по тэйку
          dealBar->close = barClose; // dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (!ui->checkBoxCloseLater->isChecked() &&
                   !dealBar->direction &&
                   barLow <= dealBar->takeProfit +
                                 spread) { //Хай бара пересек
                                           //тэйкпрофит-закрытие по тэйку
          dealBar->close = dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (j == barList.size() - 1) { //Если конец года - закрываем
          dealBar->close = currentBar->close;
          dealBar->timeClose = currentBar->time;
          if (dealBar->close - dealBar->open > 0)
            dealBar->profit = dealBar->close - dealBar->open - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        }
      }
      if (barBeforeTurnCount >= barBeforeTurn &&
          barList[i - 1].open > barList[i - 1].close &&
          barList[i].close >=
              (barList[i - 1].close +
               turnBarSize) /* && barList[i-1].low>barList[i].low*/) {
        double closeIt;
        if (stopLossType == fixed) {
          closeIt = barClose - pointToSetStop - extraSL - spread;
        } else if (stopLossType == currentHigh) {
          closeIt = barLow - extraSL - spread;
        } else if (stopLossType == highOfBoth) {
          closeIt = (barLow < barList[i - 1].low)
                        ? barLow - extraSL
                        : barList[i - 1].low - extraSL - spread;
        }
        double profitIt;
        if (takeProfitType == fix) {
          profitIt = barClose + pointToSetProfit + spread;
        } else if (takeProfitType == rated) {
          profitIt =
              barClose + abs(barClose - closeIt - spread) * rate + spread;
        }
        BuySellPos *buySell =
            new BuySellPos(currentBar->time, barClose, closeIt, profitIt, 1);
        buySell->TPpassed = false;
        buySell->openBar = i;
        OpenList.push_back(*buySell);
        inDeal = true;
        recountLot(barClose);
      } else if (barBeforeTurnCount >= barBeforeTurn &&
                 barList[i - 1].open < barList[i - 1].close &&
                 barList[i].close <=
                     (barList[i - 1].close -
                      turnBarSize) /* && barList[i-1].high<barList[i].high*/) {
        double closeIt;
        if (stopLossType == fixed) {
          closeIt = barClose + pointToSetStop + extraSL + spread;
        } else if (stopLossType == currentHigh) {
          closeIt = barHigh + extraSL + spread;
        } else if (stopLossType == highOfBoth) {
          closeIt = (barHigh > barList[i - 1].high)
                        ? barHigh + extraSL
                        : barList[i - 1].high + extraSL + spread;
        }
        double profitIt;
        if (takeProfitType == fix) {
          profitIt = barClose - pointToSetProfit - spread;
        } else if (takeProfitType == rated) {
          profitIt =
              barClose - abs(barClose - closeIt + spread) * rate - spread;
        }
        BuySellPos *buySell =
            new BuySellPos(currentBar->time, barClose, closeIt, profitIt, 0);
        buySell->TPpassed = false;
        buySell->openBar = i;
        OpenList.push_back(*buySell);
        inDeal = true;
        recountLot(barClose);
      }
    }
  }
  double Total = 0;
  for (int i = 0; i < BuySellList.size(); i++) {
    Total += BuySellList.at(i).profit;
  }

  ui->label_3->setText(QString::number(Total * 10000));
  ui->label_5->setText(QString::number(BuySellList.size()));
  model = new MyModel(BuySellList.size(), 10);
  ui->tableView->verticalHeader()->show();
  CustomProxyModel *proxyModel = new CustomProxyModel(this);
  proxyModel->setSourceModel(model);
  ui->tableView->setModel(proxyModel);
  ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableView->setSortingEnabled(true);
  ui->tableView->sortByColumn(1, Qt::AscendingOrder);
  for (int i = 0; i < BuySellList.size(); i++) {
    model->setData(model->index(i, 0), BuySellList[i].direction, Qt::EditRole);
    model->setData(model->index(i, 1), BuySellList[i].timeOpen, Qt::EditRole);
    model->setData(model->index(i, 2), BuySellList[i].open, Qt::EditRole);
    model->setData(model->index(i, 3), BuySellList[i].stopLoss, Qt::EditRole);
    model->setData(model->index(i, 4), BuySellList[i].takeProfit, Qt::EditRole);
    model->setData(model->index(i, 5), BuySellList[i].timeClose, Qt::EditRole);
    model->setData(model->index(i, 6), BuySellList[i].close, Qt::EditRole);
    model->setData(model->index(i, 7), BuySellList[i].profit, Qt::EditRole);
    model->setData(model->index(i, 8), BuySellList[i].dealLotSize,
                   Qt::EditRole);
    model->setData(model->index(i, 9), BuySellList[i].dealDeposit,
                   Qt::EditRole);
  }
  ui->labelProfitShow->setText(QString::number(Deposit - beginDeposit) + "$");
  ui->labelProfitShow_2->setText(
      QString::number((Deposit - beginDeposit) / 100.0) + "%");

  //----------------------------------------------------Рисуем
  //график-------------------------------------------------
  pen.setColor(Qt::white);
  pen.setWidth(0.1);
  brush1.setStyle(Qt::SolidPattern);
  penLine.setStyle(Qt::DashLine);
  penLine.setColor(QColor(183, 219, 255));
  penLine.setWidth(1);
  ui->graphicsView->setAlignment(
      Qt::AlignTop | Qt::AlignLeft); // Делаем привязку содержимого к центру
  ui->graphicsView->setSizePolicy(
      QSizePolicy::Expanding,
      QSizePolicy::Expanding); // Растягиваем содержимое по виджету
  this->setFocus();

  font.setPointSize(6); //Устанавливаем размер шрифта для оси У
  fontInfo.setPointSize(7);
  penCandel.setWidth(0);
  netPen.setColor(QColor("#4F4D4B")); //Ручка для сетки #836033
  netPen.setWidth(0);
  netPen.setStyle(Qt::DashLine); //Пунктиром
  QBrush burhInfo(Qt::white);
  boxinfo.setBrush(burhInfo);

  barWidth = barWidthList[barIndex]; //Устанавливаем толщину свечи согласно
                                     //дефолтному индексу
  barSpace = barSpaceList[barIndex]; //Устанавливаем пробел между свечами
                                     //согласно дефолтному индексу

  this->setAttribute(Qt::WA_NoMousePropagation);
  signalForStart = true;
  sceneWidth = ui->widget->width() - 70;
  sceneheight = ui->widget->height() - 2;
  barPos = 1;
  if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
    graphEnd = true;
    barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
  }
  if (graphEnd) {
    barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
  }
  if (type) {
    drawSceneLine(3 + barWidth / 2 + barSpace);
  } else {
    drawSceneItems(3 + barWidth / 2 + barSpace);
  }

  //----------------------------------------------------------------------------------------------------------------------------
}

void MainWindow::advancedCalculation() {
  BuySellList.clear();
  OpenList.clear();
  DealMap.clear();
  inDeal = false;
  int barBeforeTurnCount = 1;

  for (int i = 0; i < barList.size(); i++) { //Открываем позиции
    double barDifference = barList.at(i).close - barList.at(i).open;
    double barHigh = barList.at(i).high;
    double barLow = barList.at(i).low;
    double barClose = barList.at(i).close;
    Bar *currentBar = &barList[i];
    if (i - 2 >= 0) {
      if (barList[i - 1].open > barList[i - 1].close &&
          barList[i - 2].open > barList[i - 2].close) {
        barBeforeTurnCount++;
      } else if (barList[i - 1].open < barList[i - 1].close &&
                 barList[i - 2].open < barList[i - 2].close) {
        barBeforeTurnCount++;
      } else {
        barBeforeTurnCount = 1;
      }
      for (int j = 0; j < OpenList.size(); j++) {
        BuySellPos *dealBar = &OpenList[j];
        if (ui->checkBoxCloseLater->isChecked() &&
                (dealBar->direction &&
                 barHigh >= dealBar->takeProfit - spread) ||
            (!dealBar->direction && barLow <= dealBar->takeProfit + spread)) {
          OpenList[j].TPpassed = true;
          qDebug() << "OpenList[j].TPpassed=true;";
        }
        if (dealBar->direction &&
            barLow <=
                dealBar->stopLoss +
                    spread) { //Лоу бара пересек стоплосс-закрытие по стопу
          dealBar->close = dealBar->stopLoss;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (ui->checkBoxCloseEarlier->isChecked() &&
                   dealBar->direction && barClose < dealBar->open) {
          dealBar->close = barClose;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;

        } else if (ui->checkBoxCloseLater->isChecked() && dealBar->direction &&
                   OpenList.at(j).TPpassed &&
                   barList[i - 1].high >
                       barHigh) { //Хай бара пересек тэйкпрофит-закрытие по
                                  //тэйку
          dealBar->close = barClose; // dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (!ui->checkBoxCloseLater->isChecked() && dealBar->direction &&
                   barHigh >= dealBar->takeProfit -
                                  spread) { //Хай бара пересек
                                            //тэйкпрофит-закрытие по тэйку
          dealBar->close = dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->close - dealBar->open - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (!dealBar->direction &&
                   barHigh >= dealBar->stopLoss -
                                  spread) { //Лоу бара пересек стоплосс-закрытие
                                            //по стопу
          dealBar->close = dealBar->stopLoss;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (ui->checkBoxCloseEarlier->isChecked() &&
                   !dealBar->direction && barClose > dealBar->open) {
          dealBar->close = barClose;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (ui->checkBoxCloseLater->isChecked() && !dealBar->direction &&
                   OpenList.at(j).TPpassed &&
                   barList[i - 1].low <
                       barLow) { //Хай бара пересек тэйкпрофит-закрытие по тэйку
          dealBar->close = barClose; // dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (!ui->checkBoxCloseLater->isChecked() &&
                   !dealBar->direction &&
                   barLow <= dealBar->takeProfit +
                                 spread) { //Хай бара пересек
                                           //тэйкпрофит-закрытие по тэйку
          dealBar->close = dealBar->takeProfit;
          dealBar->timeClose = currentBar->time;
          dealBar->profit = dealBar->open - dealBar->close - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        } else if (j == barList.size() - 1) { //Если конец года - закрываем
          dealBar->close = currentBar->close;
          dealBar->timeClose = currentBar->time;
          if (dealBar->close - dealBar->open > 0)
            dealBar->profit = dealBar->close - dealBar->open - spread;
          inDeal = false;
          BuySellList.push_back(*dealBar);
          DealMap.insert(i, *dealBar);
          OpenList.removeAt(j);
          recountDeposit();
          --j;
        }
      }
      if (barBeforeTurnCount >= barBeforeTurn &&
          barList[i - 1].open > barList[i - 1].close &&
          barList[i].close >=
              (barList[i - 1].close +
               turnBarSize) /* && barList[i-1].low>barList[i].low*/) {
        double closeIt;
        if (stopLossType == fixed) {
          closeIt = barClose - pointToSetStop - extraSL - spread;
        } else if (stopLossType == currentHigh) {
          closeIt = barLow - extraSL - spread;
        } else if (stopLossType == highOfBoth) {
          closeIt = (barLow < barList[i - 1].low)
                        ? barLow - extraSL
                        : barList[i - 1].low - extraSL - spread;
        }
        double profitIt;
        if (takeProfitType == fix) {
          profitIt = barClose + pointToSetProfit + spread;
        } else if (takeProfitType == rated) {
          profitIt =
              barClose + abs(barClose - closeIt - spread) * rate + spread;
        }
        BuySellPos *buySell =
            new BuySellPos(currentBar->time, barClose, closeIt, profitIt, 1);
        buySell->TPpassed = false;
        buySell->openBar = i;
        OpenList.push_back(*buySell);
        inDeal = true;
        recountLot(barClose);
      } else if (barBeforeTurnCount >= barBeforeTurn &&
                 barList[i - 1].open < barList[i - 1].close &&
                 barList[i].close <=
                     (barList[i - 1].close -
                      turnBarSize) /* && barList[i-1].high<barList[i].high*/) {
        double closeIt;
        if (stopLossType == fixed) {
          closeIt = barClose + pointToSetStop + extraSL + spread;
        } else if (stopLossType == currentHigh) {
          closeIt = barHigh + extraSL + spread;
        } else if (stopLossType == highOfBoth) {
          closeIt = (barHigh > barList[i - 1].high)
                        ? barHigh + extraSL
                        : barList[i - 1].high + extraSL + spread;
        }
        double profitIt;
        if (takeProfitType == fix) {
          profitIt = barClose - pointToSetProfit - spread;
        } else if (takeProfitType == rated) {
          profitIt =
              barClose - abs(barClose - closeIt + spread) * rate - spread;
        }
        BuySellPos *buySell =
            new BuySellPos(currentBar->time, barClose, closeIt, profitIt, 0);
        buySell->TPpassed = false;
        buySell->openBar = i;
        OpenList.push_back(*buySell);
        inDeal = true;
        recountLot(barClose);
      }
    }
  }
  totalPips = 0;
  for (int i = 0; i < BuySellList.size(); i++) {
    totalPips += BuySellList.at(i).profit;
  }
}

void MainWindow::preCalculation() {
  if (advancedModel != nullptr) {
    delete advancedModel;
    advancedModel = nullptr;
    //        advancedModel->removeRows(0,advancedModel->rowCount(QModelIndex()),QModelIndex());
  }
  barList.clear();
  totalPips = 0;

  int beginDeposit;
  if (ui->lineDeposit->text().isEmpty()) {
    Deposit = 10000;
    ui->lineDeposit->setText("10000");
  } else {
    Deposit = ui->lineDeposit->text().toInt();
  }
  beginDeposit = Deposit;

  if (ui->lineBet->text().isEmpty()) {
    Bet = 0.05;
    ui->lineBet->setText("5");
  } else {
    switch (ui->lineBet->text().size()) {
    case 1:
      Bet = QString("0.0" + ui->lineBet->text()).toDouble();
      break;
    case 2:
      Bet = QString("0." + ui->lineBet->text()).toDouble();
      break;
    }
  }

  if (ui->lineShoulder->text().isEmpty()) {
    Shoulder = 100;
    ui->lineShoulder->setText("100");
  } else {
    Shoulder = ui->lineShoulder->text().toInt();
  }

  if (ui->lineSpread->text().isEmpty()) {
    spread = 0.0003;
    ui->lineSpread->setText("3");
  } else {
    if (ui->lineSpread->text() == 0) {
      spread = 0.0;
    } else {
      spread = QString(ui->lineSpread->text()).toDouble() / 10000;
    }
  }

  int iterations = 0;
  double pointToSetProfitAdvanced;
  if (ui->lineTakeProfitAdvanced->text().isEmpty()) {
    pointToSetProfit = 0.0260;
    pointToSetProfitAdvanced = 0.0260;
    ui->lineTakeProfitAdvanced->setText("260");
  } else {
    QStringList totalPoints = ui->lineTakeProfitAdvanced->text().split("-");
    QString(totalPoints.at(totalPoints.size() - 1)).remove("\n");
    switch (totalPoints.size()) {
    case 1:
      pointToSetProfit = QString(totalPoints.at(0)).toDouble() / 10000;
      pointToSetProfitAdvanced = pointToSetProfit;
      break;
    case 2:
      if (QString(totalPoints.at(1)).isEmpty()) {
        pointToSetProfit = QString(totalPoints.at(0)).toDouble() / 10000;
        pointToSetProfitAdvanced = pointToSetProfit;
      } else if (QString(totalPoints.at(1)).toDouble() <
                 QString(totalPoints.at(0)).toDouble()) {
        QMessageBox::information(
            this, "Ошибка",
            "Поле \"akeProfit max pips\" некорректно. Второе значение "
            "промежутка должно быть больше первого.");
        return;
      }
      pointToSetProfit = QString(totalPoints.at(0)).toDouble() / 10000;
      pointToSetProfitAdvanced = QString(totalPoints.at(1)).toDouble() / 10000;
      break;
    default:
      break;
    }
  }

  double pointToSetStopAdvanced;
  if (ui->lineStopLossAdvanced->text().isEmpty()) {
    pointToSetStop = 0.0085;
    pointToSetStopAdvanced = 0.0085;
    ui->lineStopLossAdvanced->setText("85");
  } else {
    QStringList totalPoints = ui->lineStopLossAdvanced->text().split("-");
    QString(totalPoints.at(totalPoints.size() - 1)).remove("\n");
    switch (totalPoints.size()) {
    case 1:
      pointToSetStop = QString(totalPoints.at(0)).toDouble() / 10000;
      pointToSetStopAdvanced = pointToSetStop;
      break;
    case 2:
      if (QString(totalPoints.at(1)).isEmpty()) {
        pointToSetStop = QString(totalPoints.at(0)).toDouble() / 10000;
        pointToSetStopAdvanced = pointToSetStop;
      } else if (QString(totalPoints.at(1)).toDouble() <
                 QString(totalPoints.at(0)).toDouble()) {
        QMessageBox::information(
            this, "Ошибка",
            "Поле \"StopLoss max pips\" некорректно. Второе значение "
            "промежутка должно быть больше первого.");
        return;
      }
      pointToSetStop = QString(totalPoints.at(0)).toDouble() / 10000;
      pointToSetStopAdvanced = QString(totalPoints.at(1)).toDouble() / 10000;
      break;
    default:
      break;
    }
  }

  double extraSLAdvanced;
  if (ui->lineEditExtraAdvanced->text().isEmpty()) {
    extraSL = 0.001;
    extraSLAdvanced = 0.001;
    ui->lineEditExtraAdvanced->setText("10");
  } else {
    QStringList totalPoints = ui->lineEditExtraAdvanced->text().split("-");
    QString(totalPoints.at(totalPoints.size() - 1)).remove("\n");
    switch (totalPoints.size()) {
    case 1:
      extraSL = QString(totalPoints.at(0)).toDouble() / 10000;
      extraSLAdvanced = extraSL;
      break;
    case 2:
      if (QString(totalPoints.at(1)).isEmpty()) {
        extraSL = QString(totalPoints.at(0)).toDouble() / 10000;
        extraSLAdvanced = extraSL;
      } else if (QString(totalPoints.at(1)).toDouble() <
                 QString(totalPoints.at(0)).toDouble()) {
        QMessageBox::information(
            this, "Ошибка",
            "Поле \"StopLoss max pips\" некорректно. Второе значение "
            "промежутка должно быть больше первого.");
        return;
      } else {
        extraSL = QString(totalPoints.at(0)).toDouble() / 10000;
        extraSLAdvanced = QString(totalPoints.at(1)).toDouble() / 10000;
      }
      break;
    default:
      break;
    }
  }

  int barBeforeTurnAdvanced;
  if (ui->barBeforeTurnAdvanced->text().isEmpty()) {
    barBeforeTurn = 2;
    barBeforeTurnAdvanced = 2;
    ui->barBeforeTurnAdvanced->setText("2");
  } else {
    QStringList totalPoints = ui->barBeforeTurnAdvanced->text().split("-");
    QString(totalPoints.at(totalPoints.size() - 1)).remove("\n");
    switch (totalPoints.size()) {
    case 1:
      barBeforeTurn = QString(totalPoints.at(0)).toInt();
      barBeforeTurnAdvanced = barBeforeTurn;
      break;
    case 2:
      if (QString(totalPoints.at(1)).isEmpty()) {
        barBeforeTurn = QString(totalPoints.at(0)).toInt();
        barBeforeTurnAdvanced = barBeforeTurn;
      } else if (QString(totalPoints.at(1)).toInt() <
                 QString(totalPoints.at(0)).toInt()) {
        QMessageBox::information(
            this, "Ошибка",
            "Поле \"Бары до разворота\" некорректно. Второе значение "
            "промежутка должно быть больше первого.");
        return;
      } else {
        barBeforeTurn = QString(totalPoints.at(0)).toInt();
        barBeforeTurnAdvanced = QString(totalPoints.at(1)).toInt();
      }
      break;
    default:
      break;
    }
  }

  double turnBarSizeAdvanced;
  if (ui->TurnBarSizeAdvanced->text().isEmpty()) {
    turnBarSize = 0.004;
    turnBarSizeAdvanced = turnBarSize;
    ui->TurnBarSizeAdvanced->setText("40");
  } else {
    QStringList totalPoints = ui->TurnBarSizeAdvanced->text().split("-");
    QString(totalPoints.at(totalPoints.size() - 1)).remove("\n");
    switch (totalPoints.size()) {
    case 1:
      turnBarSize = QString(totalPoints.at(0)).toDouble() / 10000;
      turnBarSizeAdvanced = turnBarSize;
      break;
    case 2:
      if (QString(totalPoints.at(1)).isEmpty()) {
        turnBarSize = QString(totalPoints.at(0)).toDouble() / 10000;
        turnBarSizeAdvanced = turnBarSize;
      } else if (QString(totalPoints.at(1)).toDouble() <
                 QString(totalPoints.at(0)).toDouble()) {
        QMessageBox::information(
            this, "Ошибка",
            "Поле \"Бары до разворота\" некорректно. Второе значение "
            "промежутка должно быть больше первого.");
        return;
      } else {
        turnBarSize = QString(totalPoints.at(0)).toDouble() / 10000;
        turnBarSizeAdvanced = QString(totalPoints.at(1)).toDouble() / 10000;
      }
      break;
    default:
      break;
    }
  }

  double rateAdvanced;
  if (ui->lineEditRatedAdvanced->text().isEmpty()) {
    rate = 2.0;
    rateAdvanced = rate;
    ui->lineEditRatedAdvanced->setText("2");
  } else {
    QStringList totalPoints = ui->lineEditRatedAdvanced->text().split("-");
    QString(totalPoints.at(totalPoints.size() - 1)).remove("\n");
    switch (totalPoints.size()) {
    case 1:
      rate = QString(totalPoints.at(0)).toDouble();
      rateAdvanced = rate;
      break;
    case 2:
      if (QString(totalPoints.at(1)).isEmpty()) {
        rate = QString(totalPoints.at(0)).toDouble();
        rateAdvanced = rate;
      } else if (QString(totalPoints.at(1)).toDouble() <
                 QString(totalPoints.at(0)).toDouble()) {
        QMessageBox::information(
            this, "Ошибка",
            "Поле \"StopLoss max pips\" некорректно. Второе значение "
            "промежутка должно быть больше первого.");
        return;
      } else {
        rate = QString(totalPoints.at(0)).toDouble();
        rateAdvanced = QString(totalPoints.at(1)).toDouble();
      }
      break;
    default:
      break;
    }
  }

  double SLstep;
  if (ui->lineEditSLStep->text().isEmpty()) {
    SLstep = 0.001;
    ui->lineEditSLStep->setText("10");
  } else if (ui->lineEditSLStep->text().toDouble() == 0) {
    SLstep = 0.0001;
    ui->lineEditSLStep->setText("1");
  } else {
    SLstep = ui->lineEditSLStep->text().toDouble() / 10000;
  }

  double TPstep;
  if (ui->lineEditTPStep->text().isEmpty()) {
    TPstep = 0.001;
    ui->lineEditTPStep->setText("10");
  } else if (ui->lineEditTPStep->text().toDouble() == 0) {
    TPstep = 0.0001;
    ui->lineEditTPStep->setText("1");
  } else {
    TPstep = ui->lineEditTPStep->text().toDouble() / 10000;
  }
  bool checkBoxSLfix;
  if (ui->checkBoxSLfix->isChecked()) {
    checkBoxSLfix = true;
  } else {
    checkBoxSLfix = false;
  }
  bool checkBoxTPfix;
  if (ui->checkBoxTPfix->isChecked()) {
    checkBoxTPfix = true;
  } else {
    checkBoxTPfix = false;
  }
  bool checkBoxSLboth;
  if (ui->checkBoxSLboth->isChecked()) {
    checkBoxSLboth = true;
  } else {
    checkBoxSLboth = false;
  }
  bool checkBoxTPrate;
  if (ui->checkBoxTPrate->isChecked()) {
    checkBoxTPrate = true;
  } else {
    checkBoxTPrate = false;
  }
  bool checkBoxSLcurr;
  if (ui->checkBoxSLcurr->isChecked()) {
    checkBoxSLcurr = true;
  } else {
    checkBoxSLcurr = false;
  }

  iterations =
      (checkBoxSLfix)
          ? (pointToSetStopAdvanced - pointToSetStop + 1) / SLstep
          : 0 * (checkBoxTPfix)
                ? (pointToSetProfitAdvanced - pointToSetProfit + 1) / TPstep
                : 0 * (barBeforeTurnAdvanced - barBeforeTurn + 1) *
                      (turnBarSizeAdvanced - turnBarSize + 1);
  iterations +=
      (checkBoxSLcurr)
          ? 1
          : 0 * (checkBoxTPfix)
                ? (pointToSetProfitAdvanced - pointToSetProfit + 1) / TPstep
                : 0 * (extraSLAdvanced - extraSL + 1) *
                      (barBeforeTurnAdvanced - barBeforeTurn + 1) *
                      (turnBarSizeAdvanced - turnBarSize + 1);
  iterations +=
      (checkBoxSLboth)
          ? 1
          : 0 * (checkBoxTPfix)
                ? (pointToSetProfitAdvanced - pointToSetProfit + 1) / TPstep
                : 0 * (extraSLAdvanced - extraSL + 1) *
                      (barBeforeTurnAdvanced - barBeforeTurn + 1) *
                      (turnBarSizeAdvanced - turnBarSize + 1);
  iterations += (checkBoxSLfix)
                    ? (pointToSetStopAdvanced - pointToSetStop + 1) / SLstep
                    : 0 * (checkBoxTPrate)
                          ? 1
                          : 0 * (rateAdvanced - rate + 1) *
                                (barBeforeTurnAdvanced - barBeforeTurn + 1) *
                                (turnBarSizeAdvanced - turnBarSize + 1);
  iterations += (checkBoxSLcurr)
                    ? 1
                    : 0 * (checkBoxTPrate)
                          ? 1
                          : 0 * (rateAdvanced - rate + 1) *
                                (barBeforeTurnAdvanced - barBeforeTurn + 1) *
                                (turnBarSizeAdvanced - turnBarSize + 1);
  iterations += (checkBoxSLboth)
                    ? 1
                    : 0 * (checkBoxTPrate)
                          ? 1
                          : 0 * (rateAdvanced - rate + 1) *
                                (barBeforeTurnAdvanced - barBeforeTurn + 1) *
                                (turnBarSizeAdvanced - turnBarSize + 1);

  int seconds = iterations / 8000;
  int minutes = seconds / 60;
  seconds = seconds % 60;
  int hours = minutes / 60;
  minutes = minutes % 60;
  int days = hours / 24;
  hours = hours / 24;

  QMessageBox mBox;
  if (days == hours && days == minutes && days == seconds && days == 0) {
    mBox.setText("Приблизительное время для расчета всех возможных вариантов "
                 "займет меньше секунды.");
  } else {
    if (iterations > 100000) {
      mBox.setText(
          QString(
              "Приблизительное время для расчета %0 возможных вариантов займет "
              "не более %1 дней, %2 часов, %3 минут и %4 секунд. "
              "При таком количестве итераций, возможно зависание. Необходимо "
              "дождаться окончания расчета, либо сократите выборку.")
              .arg(iterations)
              .arg(days)
              .arg(hours)
              .arg(minutes)
              .arg(seconds));
    } else {
      mBox.setText(
          QString("Приблизительное время для расчета %0 возможных вариантов "
                  "займет не более %1 дней, %2 часов, %3 минут и %4 секунд.")
              .arg(iterations)
              .arg(days)
              .arg(hours)
              .arg(minutes)
              .arg(seconds));
    }
  }
  QPushButton *connectButton =
      mBox.addButton(tr("Продолжить"), QMessageBox::ActionRole);
  QPushButton *abortButton =
      mBox.addButton(tr("Изменить"), QMessageBox::ActionRole);
  mBox.exec();
  if (mBox.clickedButton() == connectButton) {
    int year = ui->comboBoxYear->currentData(Qt::DisplayRole).toInt();
    int year_end = ui->comboBoxYear_2->currentData(Qt::DisplayRole).toInt();
    int day = 0;
    for (int i = year; i <= year_end; i++) {
      QFile Cash;
      if (ui->comboBoxPair->currentIndex() == -1) {
        ui->comboBoxPair->setCurrentIndex(2);
        Cash.setFileName(QString(":/Docs/GBPUSD_%1.txt").arg(i));
      } else if (ui->comboBoxPair->currentIndex() == 0) {
        Cash.setFileName(QString(":/Docs/AUDUSD_%1.txt").arg(i));
      } else if (ui->comboBoxPair->currentIndex() == 1) {
        Cash.setFileName(QString(":/Docs/EURUSD_%1.txt").arg(i));
      } else if (ui->comboBoxPair->currentIndex() == 2) {
        Cash.setFileName(QString(":/Docs/GBPUSD_%1.txt").arg(i));
      } else if (ui->comboBoxPair->currentIndex() == 3) {
        Cash.setFileName(QString(":/Docs/NZDUSD_%1.txt").arg(i));
      } else if (ui->comboBoxPair->currentIndex() == 4) {
        Cash.setFileName(QString(":/Docs/GBPJPY_%1.txt").arg(i));
      } else if (ui->comboBoxPair->currentIndex() == 5) {
        Cash.setFileName(QString(":/Docs/USDJPY_%1.txt").arg(i));
      }
      if (Cash.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!Cash.atEnd()) {
          ++day;
          QString word = Cash.readLine();
          if (word.startsWith("<")) {
            continue;
          }
          QStringList separate = word.split("\t");

          QString dateT = separate.at(2);
          QDate xTime = QDate::fromString(dateT, "dd/MM/yy");
          xTime = xTime.addYears(100);
          if (xTime.dayOfWeek() == 6 || xTime.dayOfWeek() == 7) {
            continue;
          }
          Bar *barX = new Bar(xTime, QString(separate.at(4)).toDouble(),
                              QString(separate.at(5)).toDouble(),
                              QString(separate.at(6)).toDouble(),
                              QString(separate.at(7)).toDouble());
          barList.push_back(*barX);
          delete barX;
        }
      } else {
      }
      Cash.close();
    }
    int progressCount = 0;
    advancedModel = new ResultsModel(1, 10);
    QSortFilterProxyModel *m = new QSortFilterProxyModel(this);
    m->setDynamicSortFilter(true);
    m->setSourceModel(advancedModel);
    ui->tableViewAdvanced->setModel(m);
    ui->tableViewAdvanced->setSortingEnabled(true);
    ui->tableViewAdvanced->sortByColumn(0, Qt::AscendingOrder);
    ui->tableViewAdvanced->setSelectionMode(
        QAbstractItemView::ExtendedSelection);
    ui->tableViewAdvanced->setSelectionBehavior(QAbstractItemView::SelectRows);

    double ptsP = pointToSetProfit;
    double ptsS = pointToSetStop;
    double exSL = extraSL;
    int bbT = barBeforeTurn;
    double tbS = turnBarSize;
    double dRate = rate;
    if (checkBoxSLcurr && checkBoxTPrate) {
      stopLossType = currentHigh;
      takeProfitType = rated;
      for (extraSL = exSL; extraSL <= extraSLAdvanced; extraSL += 0.0001) {
        for (barBeforeTurn = bbT; barBeforeTurn <= barBeforeTurnAdvanced;
             barBeforeTurn++) {
          for (turnBarSize = tbS; turnBarSize <= turnBarSizeAdvanced;
               turnBarSize += 0.0001) {
            for (rate = dRate; rate <= rateAdvanced; rate += 0.1) {
              Deposit = beginDeposit;
              totalPips = 0;
              advancedCalculation();
              advancedModel->insertRow(1);
              advancedModel->setData(advancedModel->index(progressCount, 0),
                                     (Deposit - beginDeposit), Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 1),
                                     totalPips * 10000, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 2), -1,
                                     Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 3),
                                     true, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 4),
                                     false, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 5),
                                     extraSL * 10000, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 6), -1,
                                     Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 7),
                                     rate, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 8),
                                     barBeforeTurn, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 9),
                                     turnBarSize * 10000, Qt::EditRole);
              ui->tableViewAdvanced->reset();
              progressCount++;
              ui->progressBar->setValue(progressCount * 100 / iterations);
            }
          }
        }
      }
      ui->tableViewAdvanced->reset();
    }
    int j = 0;
    if (checkBoxSLboth && checkBoxTPrate) {
      stopLossType = highOfBoth;
      takeProfitType = rated;
      for (extraSL = exSL; extraSL <= extraSLAdvanced; extraSL += 0.0001) {
        for (barBeforeTurn = bbT; barBeforeTurn <= barBeforeTurnAdvanced;
             barBeforeTurn++) {
          for (turnBarSize = tbS; turnBarSize <= turnBarSizeAdvanced;
               turnBarSize += 0.0001) {
            for (rate = dRate; rate <= rateAdvanced; rate += 0.1) {
              Deposit = beginDeposit;
              totalPips = 0;
              advancedCalculation();
              advancedModel->insertRow(1);
              advancedModel->setData(advancedModel->index(progressCount, 0),
                                     (Deposit - beginDeposit), Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 1),
                                     totalPips * 10000, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 2), -1,
                                     Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 3),
                                     false, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 4),
                                     true, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 5),
                                     extraSL * 10000, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 6), -1,
                                     Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 7),
                                     rate, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 8),
                                     barBeforeTurn, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 9),
                                     turnBarSize * 10000, Qt::EditRole);
              ui->tableViewAdvanced->reset();
              progressCount++;
              ui->progressBar->setValue(progressCount * 100 / iterations);
            }
          }
        }
      }
      ui->tableViewAdvanced->reset();
    }
    int a = 0;
    if (checkBoxSLfix && checkBoxTPrate) {
      stopLossType = fixed;
      takeProfitType = rated;
      for (pointToSetStop = ptsS; pointToSetStop <= pointToSetStopAdvanced;
           pointToSetStop += SLstep) {
        for (barBeforeTurn = bbT; barBeforeTurn <= barBeforeTurnAdvanced;
             barBeforeTurn++) {
          for (turnBarSize = tbS; turnBarSize <= turnBarSizeAdvanced;
               turnBarSize += 0.0001) {
            for (rate = dRate; rate <= rateAdvanced; rate += 0.1) {
              Deposit = beginDeposit;
              totalPips = 0;
              advancedCalculation();
              advancedModel->insertRow(1);
              advancedModel->setData(advancedModel->index(progressCount, 0),
                                     (Deposit - beginDeposit), Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 1),
                                     totalPips * 10000, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 2),
                                     pointToSetStop * 10000, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 3),
                                     false, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 4),
                                     false, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 5), -1,
                                     Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 6), -1,
                                     Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 7),
                                     rate, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 8),
                                     barBeforeTurn, Qt::EditRole);
              advancedModel->setData(advancedModel->index(progressCount, 9),
                                     turnBarSize * 10000, Qt::EditRole);
              progressCount++;
              ui->progressBar->setValue(progressCount * 100 / iterations);
            }
          }
        }
      }
      ui->tableViewAdvanced->reset();
    }
    int b = 0;
    if (checkBoxSLfix && checkBoxTPfix) {
      stopLossType = fixed;
      takeProfitType = fix;
      for (pointToSetStop = ptsS; pointToSetStop <= pointToSetStopAdvanced;
           pointToSetStop += SLstep) {
        for (pointToSetProfit = ptsP;
             pointToSetProfit <= pointToSetProfitAdvanced;
             pointToSetProfit += TPstep) {
          for (barBeforeTurn = bbT; barBeforeTurn <= barBeforeTurnAdvanced;
               barBeforeTurn++) {
            for (turnBarSize = tbS; turnBarSize <= turnBarSizeAdvanced;
                 turnBarSize += 0.0001) {
              for (rate = dRate; rate <= rateAdvanced; rate += 0.1) {
                Deposit = beginDeposit;
                totalPips = 0;
                advancedCalculation();
                advancedModel->insertRow(1);
                advancedModel->setData(advancedModel->index(progressCount, 0),
                                       (Deposit - beginDeposit), Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 1),
                                       totalPips * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 2),
                                       pointToSetStop * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 3),
                                       false, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 4),
                                       false, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 5),
                                       -1, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 6),
                                       pointToSetProfit * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 7),
                                       -1, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 8),
                                       barBeforeTurn, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 9),
                                       turnBarSize * 10000, Qt::EditRole);
                ui->tableViewAdvanced->reset();
                progressCount++;
                ui->progressBar->setValue(progressCount * 100 / iterations);
              }
            }
          }
        }
      }
      ui->tableViewAdvanced->reset();
    }
    int c = 0;
    if (checkBoxSLcurr && checkBoxTPfix) {
      stopLossType = currentHigh;
      takeProfitType = fix;
      for (pointToSetProfit = ptsP;
           pointToSetProfit <= pointToSetProfitAdvanced;
           pointToSetProfit += TPstep) {
        for (extraSL = exSL; extraSL <= extraSLAdvanced; extraSL += 0.0001) {
          for (barBeforeTurn = bbT; barBeforeTurn <= barBeforeTurnAdvanced;
               barBeforeTurn++) {
            for (turnBarSize = tbS; turnBarSize <= turnBarSizeAdvanced;
                 turnBarSize += 0.0001) {
              for (rate = dRate; rate <= rateAdvanced; rate += 0.1) {
                Deposit = beginDeposit;
                totalPips = 0;
                advancedCalculation();
                advancedModel->insertRow(1);
                advancedModel->setData(advancedModel->index(progressCount, 0),
                                       (Deposit - beginDeposit), Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 1),
                                       totalPips * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 2),
                                       -1, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 3),
                                       true, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 4),
                                       false, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 5),
                                       extraSL * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 6),
                                       pointToSetProfit * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 7),
                                       -1, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 8),
                                       barBeforeTurn, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 9),
                                       turnBarSize * 10000, Qt::EditRole);
                ui->tableViewAdvanced->reset();
                progressCount++;
                ui->progressBar->setValue(progressCount * 100 / iterations);
              }
            }
          }
        }
      }
      ui->tableViewAdvanced->reset();
    }
    int d = 0;
    if (checkBoxSLboth && checkBoxTPfix) {
      stopLossType = highOfBoth;
      takeProfitType = fix;
      for (pointToSetProfit = ptsP;
           pointToSetProfit <= pointToSetProfitAdvanced;
           pointToSetProfit += TPstep) {
        for (extraSL = exSL; extraSL <= extraSLAdvanced; extraSL += 0.0001) {
          for (barBeforeTurn = bbT; barBeforeTurn <= barBeforeTurnAdvanced;
               barBeforeTurn++) {
            for (turnBarSize = tbS; turnBarSize <= turnBarSizeAdvanced;
                 turnBarSize += 0.0001) {
              for (rate = dRate; rate <= rateAdvanced; rate += 0.1) {
                Deposit = beginDeposit;
                totalPips = 0;
                advancedCalculation();
                advancedModel->insertRow(1);
                advancedModel->setData(advancedModel->index(progressCount, 0),
                                       (Deposit - beginDeposit), Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 1),
                                       totalPips * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 2),
                                       -1, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 3),
                                       false, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 4),
                                       true, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 5),
                                       extraSL * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 6),
                                       pointToSetProfit * 10000, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 7),
                                       -1, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 8),
                                       barBeforeTurn, Qt::EditRole);
                advancedModel->setData(advancedModel->index(progressCount, 9),
                                       turnBarSize * 10000, Qt::EditRole);
                ui->tableViewAdvanced->reset();
                progressCount++;
                ui->progressBar->setValue(progressCount * 100 / iterations);
              }
            }
          }
        }
      }
      ui->tableViewAdvanced->reset();
    }
  }
}

void MainWindow::recountDeposit() {
  Deposit += BuySellList[BuySellList.size() - 1].profit * 100000 * lotSize;
  BuySellList[BuySellList.size() - 1].dealDeposit = Deposit;
}

void MainWindow::recountLot(double &closePrice) {
  lotSize = round(Deposit * Bet * Shoulder / closePrice / 100000 * 100) / 100;
  OpenList[OpenList.size() - 1].dealLotSize = QString::number(lotSize, 'd', 2);
}

void MainWindow::showMore(QModelIndex index) {
  if (index.model()->data(index.model()->index(index.row(), 2)).toInt() > 0) {
    ui->lineStopLoss->setEnabled(true);
    ui->lineStopLoss->setText(QString::number(
        index.model()->data(index.model()->index(index.row(), 2)).toInt()));
    ui->lineEditExtraStopLoss->setEnabled(false);
    ui->comboBoxStopLoss->setCurrentIndex(0);
  }
  if (index.model()->data(index.model()->index(index.row(), 3)).toBool()) {
    ui->lineStopLoss->setEnabled(false);
    ui->lineEditExtraStopLoss->setEnabled(true);
    ui->lineEditExtraStopLoss->setText(QString::number(
        index.model()->data(index.model()->index(index.row(), 5)).toInt()));
    ui->comboBoxStopLoss->setCurrentIndex(1);
  }
  if (index.model()->data(index.model()->index(index.row(), 4)).toBool()) {
    ui->lineStopLoss->setEnabled(false);
    ui->lineEditExtraStopLoss->setEnabled(true);
    ui->lineEditExtraStopLoss->setText(QString::number(
        index.model()->data(index.model()->index(index.row(), 5)).toInt()));
    ui->comboBoxStopLoss->setCurrentIndex(2);
  }
  if (index.model()->data(index.model()->index(index.row(), 6)).toInt() > 0) {
    ui->lineTakeProfit->setEnabled(true);
    ui->lineTakeProfit->setText(QString::number(
        index.model()->data(index.model()->index(index.row(), 6)).toInt()));
    ui->comboBoxTakeProfit->setCurrentIndex(0);
    ui->Slider->setEnabled(false);
    ui->lineEditRated->setEnabled(false);
  } else if (index.model()->data(index.model()->index(index.row(), 7)).toInt() >
             0) {
    ui->lineTakeProfit->setEnabled(false);
    ui->comboBoxTakeProfit->setCurrentIndex(1);
    ui->Slider->setEnabled(true);
    ui->lineEditRated->setEnabled(true);
    ui->Slider->setSliderPosition(
        index.model()->data(index.model()->index(index.row(), 7)).toDouble() *
        10);
  }
  ui->lineEditbarBeforeTurn->setText(QString::number(
      index.model()->data(index.model()->index(index.row(), 8)).toInt()));
  ui->TurnBarSize->setText(QString::number(
      index.model()->data(index.model()->index(index.row(), 9)).toInt()));
  calculate();
}

void MainWindow::checkYear(int index) {
  QStandardItemModel *model = (QStandardItemModel *)ui->comboBoxYear_2->model();
  int i = 0;
  for (; i < index; i++) {
    model->item(i)->setEnabled(false);
  }
  for (; i < ui->comboBoxYear->count(); i++) {
    model->item(i)->setEnabled(true);
  }
  if (ui->comboBoxYear_2->currentIndex() < index) {
    ui->comboBoxYear_2->setCurrentIndex(index);
  }
}
void MainWindow::wheelEvent(QWheelEvent *event) {
  if (!signalForStart) {
    return;
  }
  if (!resizeMark) { //Поскольку wheelevent вызывается дважды - используем бул,
                     //чтобы отсекать один лишний вызов.
    resizeMark = true;
    return;
  }
  if (drawLineInCrosses) {
    return;
  }
  resizeMark = false;
  if (event->delta() > 0) { //Если промотка вправо
    if (ctrlPressed) { //Зум увеличиваем, если зажат ctrl
      if (barIndex + 1 <
          barWidthList
              .size()) { //Если инкрементированный индекс меньше размера
                         //контейнера, в котором хранятся величины свечей
        barIndex++;
        barWidth = barWidthList[barIndex];
        barSpace = barSpaceList[barIndex];
        if (barPos + (sceneWidth) / (barWidth + barSpace) >
            barList.size()) { //Если при этом первый бар на рисунке+все
                              //остальные бары больше размера контейнера, в
                              //котором хранятся все бары:
          if (barList.size() - (sceneWidth) / (barWidth + barSpace) < 0) {
            barIndex--;
            barWidth = barWidthList[barIndex];
            barSpace = barSpaceList[barIndex];
            return;
          }
          graphEnd = true; //Устанавливаем метку для ресайза, что достигли конца
          barPos =
              barList.size() -
              (sceneWidth) /
                  (barWidth +
                   barSpace); //Устанавливаем так, чтобы на экране выводились
                              //все бары включая последний в контейнере.
        } else {
          graphEnd =
              false; //Устанавливаем метку для ресайза, что покинули конец
        }
      } else {
        return;
      }
    } else {
      if (barPos + barScrollList[barIndex] +
              (sceneWidth) / (barWidth + barSpace) >
          barList.size()) { //Если при прокрутке вперед, последний бар выходит
                            //за границу контейнера
        graphEnd = true; //Устанавливаем метку для ресайза, что достигли конца
        barPos = barList.size() -
                 (sceneWidth) /
                     (barWidth +
                      barSpace); //Устанавливаем так, чтобы на экране выводились
                                 //все бары включая последний в контейнере.
      } else {
        barPos += barScrollList[barIndex]; //Промотка по бару
      }
    }
    if (type) { //Выводим на экран тот график, чей индекс стоит
      drawSceneLine(3 + barWidth / 2 + barSpace);
    } else {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    }
    if (ui->pushButtonCursor
            ->isChecked()) { //Вывести курсор крест, если активировано меню
      fillStatusBar(mousePoint);
    }
  } else {             //Если промотка влево
    if (ctrlPressed) { //Зум уменьшаем, если зажат ctrl
      if (barIndex - 1 >= 0) {
        barIndex--;
        barWidth = barWidthList[barIndex];
        barSpace = barSpaceList[barIndex];
        if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
          if (barList.size() - (sceneWidth) / (barWidth + barSpace) < 0) {
            barIndex++;
            barWidth = barWidthList[barIndex];
            barSpace = barSpaceList[barIndex];
            return;
          }
          graphEnd = true;
          barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
        } else if (barPos < 1) { // barPos-(sceneWidth)/(barWidth+barSpace)<1
          barPos = 1;
        } else {
          graphEnd = false;
        }
      } else {
        return;
      }
    } else {
      if (barPos - barScrollList[barIndex] < 1) {
        barPos = 1; //Если вышел за левую границу
      } else {
        graphEnd = false;
        barPos -= barScrollList[barIndex]; //Промотка по бару
      }
    }
    if (type) {
      drawSceneLine(3 + barWidth / 2 + barSpace);
    } else {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    }
    if (ui->pushButtonCursor
            ->isChecked()) { //Вывести курсор крест, если активировано меню
      fillStatusBar(mousePoint);
    }
  }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  if (signalForStart) {
    sceneWidth = ui->widget->width() - 70;
    sceneheight = ui->widget->height() - 2;
    scene->setSceneRect(0, 0, sceneWidth, sceneheight);

    if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
      graphEnd = true;
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    }
    if (graphEnd) {
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    }
    if (type) {
      drawSceneLine(3 + barWidth / 2 + barSpace);
    } else {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    }
  }
}

void MainWindow::drawSceneItems(int put) //получаем позицию(по горизонтали) с
                                         //которой начинают добавляться фигуры
{
  if (pair != nullptr) {
    delete pair;
  }
  for (auto it : candleVector) {
    delete it;
  }
  candleVector.clear();
  if (crossLine1 != nullptr) {
    scene->removeItem(crossLine1);
    scene->removeItem(crossLine2);
    delete crossLine1;
    delete crossLine2;
    crossLine1 = nullptr;
    crossLine2 = nullptr;
    scene->update();
  }
  if (rectDown != nullptr) {
    scene->removeItem(rectDown);
    scene->removeItem(rectRight);
    scene->removeItem(textDown);
    scene->removeItem(textRight);
    delete rectDown;
    delete rectRight;
    delete textDown;
    delete textRight;
    rectDown = nullptr;
    rectRight = nullptr;
    textDown = nullptr;
    textRight = nullptr;
    scene->update();
  }

  scene->clear();
  //Очищаем форму от предыдущих элементов
  if (!hideNet) {
    drawSceneNet();
  }
  std::multiset<double> iSet;
  for (int i = barPos; i < (barPos + (sceneWidth) / (barWidth + barSpace));
       i++) { // Добавляем в упорядоченный контейнер все показатели (хай, лоу,
              // опен, клоуз) для опеределения максимума и минимума для подгона
              // окна
    iSet.insert(barList.at(i).high);
    iSet.insert(barList.at(i).low);
  }
  maxSet = *iSet.rbegin();
  minSet = *iSet.begin();

  screenSize =
      (sceneheight - 100) /
      (maxSet -
       minSet); //-100, чтобы оставить 30 пикселей снизу и 70 сверху от бара

  pair = scene->addText(
      ui->comboBoxPair->currentData(Qt::DisplayRole).toString(), fontInfo);
  pair->setDefaultTextColor(Qt::white);
  pair->moveBy(0, 0);
  for (int i = barPos; i < (barPos + (sceneWidth) / (barWidth + barSpace));
       i++) { //Пробегаем от индекса бара, содержащегося в контейнере barList
    double size = abs(barList[i].close -
                      barList[i].open); //Разница между открытием и закрытием
                                        //для изображения бара на экране
    if (barList[i].close >= barList[i].open) { //Если в рост
      if (i && barList[i - 1].time.month() !=
                   barList[i].time.month()) { //Если начало дня, то добавляем
                                              //вертикальный разделитель(линию)
        scene->addLine(put + barWidth / 2, 2, put + barWidth / 2,
                       sceneheight - paddingBottom, penLine);
      }
      CandleBar *bbbar = new CandleBar(
          i, put, bonusSpace + (maxSet - barList[i].close) * screenSize,
          barWidth, size * screenSize, put + barWidth / 2,
          bonusSpace + (maxSet - barList[i].high) * screenSize,
          put + barWidth / 2,
          bonusSpace + (maxSet - barList[i].low) * screenSize, 1, scene);
      scene->addItem(bbbar);
      candleVector.push_back(bbbar);
      connect(bbbar, SIGNAL(sendIDinfo(int &)), this, SLOT(showBarInfo(int &)));
      connect(bbbar, &CandleBar::clearIDinfo,
              [this] { scene->removeItem(textInfo); });
      bbbar = nullptr;
    } else {
      if (i && barList[i - 1].time.month() !=
                   barList[i].time.month()) { //Если начало дня, то добавляем
                                              //вертикальный разделитель(линию)
        scene->addLine(put + barWidth / 2, 2, put + barWidth / 2,
                       sceneheight - paddingBottom, penLine);
      }
      CandleBar *bbbar = new CandleBar(
          i, put, bonusSpace + (maxSet - barList[i].open) * screenSize,
          barWidth, size * screenSize, put + barWidth / 2,
          bonusSpace + (maxSet - barList[i].high) * screenSize,
          put + barWidth / 2,
          bonusSpace + (maxSet - barList[i].low) * screenSize, 0, scene);
      scene->addItem(bbbar);
      candleVector.push_back(bbbar);
      connect(bbbar, SIGNAL(sendIDinfo(int &)), this, SLOT(showBarInfo(int &)));
      connect(bbbar, &CandleBar::clearIDinfo,
              [this] { scene->removeItem(textInfo); });
      bbbar = nullptr;
    }
    put += (barWidth + barSpace);
  }
  scene->addRect(2 + sceneWidth, 2, ui->widget->width() - 2 - sceneWidth,
                 sceneheight - paddingBottom, QPen(QColor("#313233")),
                 QBrush(QColor("#313233"))); //Закрашиваем правую сторону-если
                                             //фигура выходит за рамку
  scene->addRect(2, 2, sceneWidth, sceneheight - paddingBottom,
                 pen); //Каждый раз рисуем рамку экрана

  put = 3;
  for (auto it = DealMap.begin(); it != DealMap.end();
       it++) { //Отображаем все сделки
    if (it.key() < (barPos + (sceneWidth) / (barWidth + barSpace)) &&
        it.key() >= barPos) {
      BuySellPos curPos = static_cast<BuySellPos>(it.value());
      if (curPos.openBar < (barPos + (sceneWidth) / (barWidth + barSpace)) &&
          curPos.openBar >= barPos) {
        bool result = (curPos.profit > 0) ? 1 : 0;
        DealLine *deal = new DealLine(
            it.key(),
            put + (curPos.openBar - barPos + 1) * (barWidth + barSpace),
            bonusSpace + (maxSet - curPos.open) * screenSize,
            put + (it.key() - barPos + 1) * (barWidth + barSpace),
            bonusSpace + (maxSet - curPos.close) * screenSize,
            bonusSpace + (maxSet - barList[curPos.openBar].high) * screenSize,
            bonusSpace + (maxSet - barList[curPos.openBar].low) * screenSize,
            bonusSpace + (maxSet - curPos.takeProfit) * screenSize,
            bonusSpace + (maxSet - curPos.stopLoss) * screenSize, result,
            scene);
        scene->addItem(deal);
        deal = nullptr;
      }
    }
  }

  pixelsize = (maxSet - minSet) / double(sceneheight - paddingBottom - 70);
  double begin = minSet - pixelsize * 43;
  for (int i = sceneheight - 28; i > 10; i -= 32) {
    scene->addLine(sceneWidth + 2, i, sceneWidth + 5, i, pen);
    QGraphicsTextItem *textitem =
        scene->addText(QString::number(begin, 'f', 5), font);
    textitem->setDefaultTextColor(Qt::white);
    textitem->moveBy(sceneWidth + 6, i - 8);
    begin += pixelsize * 32;
    textitem = nullptr;
  }
  bool whenToPutDate = true;
  for (int i = 2; i < sceneWidth; i += 32) {
    if (whenToPutDate) {
      scene->addLine(i, sceneheight - 27, i, sceneheight - 23, pen);
      int whichBar = i / (barWidthList[barIndex] + barSpaceList[barIndex]) - 1;
      if (barPos + whichBar < 0) {
        QGraphicsTextItem *textitem = scene->addText(
            barList[0].time.addDays(-1).toString("yyyy MMM dd"), font);
        textitem->setDefaultTextColor(Qt::white);
        textitem->moveBy(i, sceneheight - 27);
        whenToPutDate = false;
      } else {
        QGraphicsTextItem *textitem = scene->addText(
            barList[barPos + whichBar].time.toString("yyyy MMM dd"), font);
        textitem->setDefaultTextColor(Qt::white);
        textitem->moveBy(i, sceneheight - 27);
        whenToPutDate = false;
      }
    } else {
      scene->addLine(i, sceneheight - 27, i, sceneheight - 25, pen);
      whenToPutDate = true;
    }
  }

  ui->graphicsView->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier) {
    ctrlPressed = true;
  } else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_F12) {
    if (barPos + 1 + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
      graphEnd = true;
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    } else {
      barPos++;
    }
    if (!type) {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    } else {
      drawSceneLine(3 + barWidth / 2 + barSpace);
    }
  } else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_F11) {

    if (graphEnd) {
      graphEnd = false;
    }
    if (barPos - 1 < 0) {
      return;
    }
    barPos--;
    if (!type) {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    } else {
      if (barPos < 1) {
        return;
      }
      drawSceneLine(3 + barWidth / 2 + barSpace);
    }
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) { ctrlPressed = false; }

void MainWindow::drawSceneLine(int put) //получаем позицию(по горизонтали) с
                                        //которой начинают добавляться фигуры
{
  if (pair != nullptr) {
    delete pair;
  }
  for (auto it : candleVector) {
    delete it;
  }
  candleVector.clear();
  if (crossLine1 != nullptr) {
    scene->removeItem(crossLine1);
    scene->removeItem(crossLine2);
    delete crossLine1;
    delete crossLine2;
    crossLine1 = nullptr;
    crossLine2 = nullptr;
    scene->update();
  }
  if (rectDown != nullptr) {
    scene->removeItem(rectDown);
    scene->removeItem(rectRight);
    scene->removeItem(textDown);
    scene->removeItem(textRight);
    delete rectDown;
    delete rectRight;
    delete textDown;
    delete textRight;
    rectDown = nullptr;
    rectRight = nullptr;
    textDown = nullptr;
    textRight = nullptr;
    scene->update();
  }
  scene->clear(); //Очищаем форму от предыдущих элементов
  if (!hideNet) {
    drawSceneNet();
  }
  std::multiset<double> iSet;
  for (int i = barPos; i < (barPos + (sceneWidth) / (barWidth + barSpace));
       i++) { // Добавляем в упорядоченный контейнер все показатели (хай, лоу,
              // опен, клоуз) для опеределения максимума и минимума для подгона
              // окна
    iSet.insert(barList.at(i).high);
    iSet.insert(barList.at(i).low);
  }
  maxSet = *iSet.rbegin();
  minSet = *iSet.begin();
  screenSize =
      (sceneheight - 100) /
      (*iSet.rbegin() -
       *iSet.begin()); //-60, чтобы оставить 30 пикселей снизу и сверху от бара
  int i = barPos;

  put -= (barWidth / 2 + barSpace + 1);
  //    scene->addLine(put-(barWidth+barSpace),bonusSpace+(maxSet-barList[i-1].close)*screenSize,put,bonusSpace+(maxSet-barList[i].close)*screenSize,penLine);
  //    //Вертикальная линия(хай->лоу)
  polygon = new GraphicsLinePolygon(
      (barPos + (sceneWidth) / (barWidth + barSpace)) - i, sceneWidth,
      sceneheight, scene);
  polygon->addPoint(put, sceneheight - paddingBottom);
  polygon->addPoint(put,
                    bonusSpace + (maxSet - barList[i - 1].close) * screenSize);
  pair = scene->addText(
      ui->comboBoxPair->currentData(Qt::DisplayRole).toString(), fontInfo);
  pair->setDefaultTextColor(Qt::white);
  pair->moveBy(0, 0);
  for (; i < (barPos + (sceneWidth) / (barWidth + barSpace));
       i++) { //Пробегаем от индекса бара, содержащегося в контейнере barList
    //        double size = abs(barList[i].close-barList[i].open);    //Разница
    //        между открытием и закрытием для изображения бара на экране
    //        scene->addLine(put,bonusSpace+(maxSet-barList[i-1].close)*screenSize,put+(barWidth+barSpace),bonusSpace+(maxSet-barList[i].close)*screenSize,penLine);
    //        //Вертикальная линия(хай->лоу)
    if (i && barList[i - 1].time.month() !=
                 barList[i].time.month()) { //Если начало дня, то добавляем
                                            //вертикальный разделитель(линию)
      scene->addLine(put + barWidth + barSpace, 2, put + barWidth + barSpace,
                     sceneheight - paddingBottom, penLine);
    }
    polygon->addPoint(put + (barWidth + barSpace),
                      bonusSpace + (maxSet - barList[i].close) * screenSize);
    put += (barWidth + barSpace);
  }
  polygon->addPoint(put, sceneheight - paddingBottom);
  scene->addItem(polygon);
  polygon = nullptr;
  scene->addRect(2 + sceneWidth, 2, ui->widget->width() - 2 - sceneWidth,
                 sceneheight - paddingBottom, QPen(QColor("#313233")),
                 QBrush(QColor("#313233"))); //Закрашиваем правую сторону-если
                                             //фигура выходит за рамку
  scene->addRect(2, 2, sceneWidth, sceneheight - paddingBottom,
                 pen); //Каждый раз рисуем рамку экрана

  put = 3;
  for (auto it = DealMap.begin(); it != DealMap.end(); it++) {
    if (it.key() < (barPos + (sceneWidth) / (barWidth + barSpace)) &&
        it.key() >= barPos) {
      BuySellPos curPos = static_cast<BuySellPos>(it.value());
      if (curPos.openBar < (barPos + (sceneWidth) / (barWidth + barSpace)) &&
          curPos.openBar >= barPos) {
        bool result = (curPos.profit > 0) ? 1 : 0;
        DealLine *deal = new DealLine(
            it.key(),
            put + (curPos.openBar - barPos + 1) * (barWidth + barSpace),
            bonusSpace + (maxSet - curPos.open) * screenSize,
            put + (it.key() - barPos + 1) * (barWidth + barSpace),
            bonusSpace + (maxSet - curPos.close) * screenSize,
            bonusSpace + (maxSet - barList[it.key()].high) * screenSize,
            bonusSpace + (maxSet - barList[it.key()].low) * screenSize,
            bonusSpace + (maxSet - curPos.takeProfit) * screenSize,
            bonusSpace + (maxSet - curPos.stopLoss) * screenSize, result,
            scene);
        scene->addItem(deal);
        deal = nullptr;
      }
    }
  }

  //    scene->addLine(put+barWidth/2,bonusSpace+(maxSet-barList[i].close)*screenSize,put+(barWidth+barSpace)+barWidth/2,bonusSpace+(maxSet-barList[i+1].close)*screenSize,penLine);
  //    //Вертикальная линия(хай->лоу)
  pixelsize = (maxSet - minSet) / double(sceneheight - paddingBottom - 70);
  double begin = minSet - pixelsize * 43;
  for (int i = sceneheight - 28; i > 10; i -= 32) {
    scene->addLine(sceneWidth + 2, i, sceneWidth + 5, i, pen);
    QGraphicsTextItem *textitem =
        scene->addText(QString::number(begin, 'f', 5), font);
    textitem->setDefaultTextColor(Qt::white);
    textitem->moveBy(sceneWidth + 6, i - 8);
    begin += pixelsize * 32;
  }
  bool whenToPutDate = true;
  for (int i = 2; i < sceneWidth; i += 32) {
    if (whenToPutDate) {
      scene->addLine(i, sceneheight - 27, i, sceneheight - 23, pen);
      int whichBar = i / (barWidthList[barIndex] + barSpaceList[barIndex]) - 1;
      QGraphicsTextItem *textitem = scene->addText(
          barList[barPos + whichBar].time.toString("yyyy MMM dd"), font);
      textitem->setDefaultTextColor(Qt::white);
      textitem->moveBy(i, sceneheight - 27);
      whenToPutDate = false;
    } else {
      scene->addLine(i, sceneheight - 27, i, sceneheight - 25, pen);
      whenToPutDate = true;
    }
  }

  ui->graphicsView->update();
}

void MainWindow::drawSceneNet() {
  for (int i = 34; i < sceneWidth; i += 32) {
    scene->addLine(i, 3, i, sceneheight - paddingBottom, netPen);
  }

  for (int i = sceneheight - 60; i > 2; i -= 32) {
    scene->addLine(3, i, sceneWidth - 2, i, netPen);
  }
}

void MainWindow::showBarInfo(int &ID) {
  textInfo =
      scene->addText(QString("High: %1\tOpen: %3\tDate: %5\nLow: %2\tClose: %4")
                         .arg(barList[ID].high)
                         .arg(barList[ID].low)
                         .arg(barList[ID].open)
                         .arg(barList[ID].close)
                         .arg(barList[ID].time.toString("yyyy MMM dd dddd")),
                     fontInfo);
  textInfo->setDefaultTextColor(Qt::white);
  textInfo->moveBy(50, 0);
}

void MainWindow::splitterMoved(int X, int Y) {
  if (!signalForStart) {
    return;
  }
  if (!type) {
    sceneWidth = ui->widget->width() - 70;
    sceneheight = ui->widget->height() - 2;
    scene->setSceneRect(0, 0, sceneWidth, sceneheight);

    if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
      graphEnd = true;
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    }
    if (graphEnd) {
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    }
    if (type) {
      drawSceneLine(3 + barWidth / 2 + barSpace);
    } else {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    }

  } else {
    sceneWidth = ui->widget->width() - 70;
    sceneheight = ui->widget->height() - 2;
    scene->setSceneRect(0, 0, sceneWidth, sceneheight);

    if (barPos + (sceneWidth) / (barWidth + barSpace) > barList.size()) {
      graphEnd = true;
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    }
    if (graphEnd) {
      barPos = barList.size() - (sceneWidth) / (barWidth + barSpace);
    }
    if (type) {
      drawSceneLine(3 + barWidth / 2 + barSpace);
    } else {
      drawSceneItems(3 + barWidth / 2 + barSpace);
    }
  }
}

void MainWindow::fillStatusBar(QPointF &point) {
  if (point.rx() > 2 && point.rx() < sceneWidth && point.ry() > 2 &&
      point.ry() < sceneheight - paddingBottom) {
    int bar = barPos + (point.rx() - (3 + barWidth / 2 + barSpace)) /
                           (barWidth + barSpace);
    bar = (bar >= barList.size()) ? barList.size() - 1 : bar;
    D->setText(barList[bar].time.toString("yyyy.MM.dd"));
    O->setText(QString("O: %1").arg(barList[bar].open));
    H->setText(QString("H: %1").arg(barList[bar].high));
    L->setText(QString("L: %1").arg(barList[bar].low));
    C->setText(QString("C: %1").arg(barList[bar].close));
    scene->update();
    if (ui->pushButtonCursor->isChecked()) {
      QPen crossLine;
      crossLine.setWidth(1);
      crossLine.setColor(Qt::white);
      if (crossLine1 == nullptr) {
        crossLine1 = scene->addLine(point.rx(), 3, point.rx(), sceneheight - 29,
                                    crossLine);
        crossLine2 = scene->addLine(3, point.ry(), sceneWidth + 1, point.ry(),
                                    crossLine);
      } else {
        crossLine1->moveBy(point.rx() - mousePoint.rx(), 0);
        crossLine2->moveBy(0, point.ry() - mousePoint.ry());
      }
      if (rectDown != nullptr) {
        scene->removeItem(rectDown);
        scene->removeItem(rectRight);
        scene->removeItem(textDown);
        scene->removeItem(textRight);
        delete rectDown;
        delete rectRight;
        delete textDown;
        delete textRight;
        rectDown = nullptr;
        rectRight = nullptr;
        textDown = nullptr;
        textRight = nullptr;
        scene->update();
      }
      if (rectDown == nullptr) {
        double currPos = maxSet + (29 - point.ry()) * pixelsize;

        rectDown = scene->addRect((point.rx() - 25 > 3) ? point.rx() - 25 : 3,
                                  sceneheight - 27, 50, 12, boxinfo,
                                  QBrush(Qt::white));
        textDown =
            scene->addText(barList[bar].time.toString("yyyy.MM.dd"), font);
        rectRight = scene->addRect(sceneWidth + 3,
                                   (point.ry() - 6 > 3) ? point.ry() - 6 : 3,
                                   40, 12, boxinfo, QBrush(Qt::white));
        textRight = scene->addText(QString::number(currPos, 'd', 5), font);
        textDown->setDefaultTextColor(Qt::black);
        textRight->setDefaultTextColor(Qt::black);
        textDown->moveBy((point.rx() - 27 > 0) ? point.rx() - 27 : 1,
                         sceneheight - paddingBottom);
        textRight->moveBy(sceneWidth + 2,
                          (point.ry() - 8 > 0) ? point.ry() - 8 : 1);
        rectDown->setBrush(Qt::white);
        rectRight->setBrush(Qt::white);
      }
      if (!drawLineInCrosses && crossLine3 != nullptr) {
        scene->removeItem(crossLine3);
        scene->removeItem(crossLine4);
        scene->removeItem(rectDown2);
        scene->removeItem(rectRight2);
        scene->removeItem(textDown2);
        scene->removeItem(textRight2);
        scene->removeItem(lineItem);
        scene->removeItem(crossRect);
        scene->removeItem(crossText);
        delete crossLine3;
        delete crossLine4;
        delete rectDown2;
        delete rectRight2;
        delete textDown2;
        delete textRight2;
        delete lineItem;
        delete crossRect;
        delete crossText;
        crossLine3 = nullptr;
        crossLine4 = nullptr;
        rectDown2 = nullptr;
        rectRight2 = nullptr;
        textDown2 = nullptr;
        textRight2 = nullptr;
        lineItem = nullptr;
        crossRect = nullptr;
        crossText = nullptr;
        scene->update();
      }
      if (drawLineInCrosses && crossLine3 == nullptr) {
        int crossBar =
            abs(point.rx() - prevMousePoint.rx()) / (barWidth + barSpace);
        int crossPips =
            abs(point.ry() - prevMousePoint.ry()) * pixelsize * 10000;

        double currPos = maxSet + (29 - point.ry()) * pixelsize;
        crossLine3 = scene->addLine(prevMousePoint.rx(), 3, prevMousePoint.rx(),
                                    sceneheight - 29, crossLine);
        crossLine4 = scene->addLine(3, prevMousePoint.ry(), sceneWidth + 1,
                                    prevMousePoint.ry(), crossLine);
        rectDown2 = scene->addRect(
            (prevMousePoint.rx() - 25 > 3) ? prevMousePoint.rx() - 25 : 3,
            sceneheight - 27, 50, 12, boxinfo, QBrush(Qt::white));
        rectRight2 = scene->addRect(
            sceneWidth + 3,
            (prevMousePoint.ry() - 6 > 3) ? prevMousePoint.ry() - 6 : 3, 40, 12,
            boxinfo, QBrush(Qt::white));
        textDown2 =
            scene->addText(barList[bar].time.toString("yyyy.MM.dd"), font);
        textRight2 = scene->addText(QString::number(currPos, 'd', 5), font);
        textDown2->setDefaultTextColor(Qt::black);
        textRight2->setDefaultTextColor(Qt::black);
        textDown2->moveBy(
            (prevMousePoint.rx() - 27 > 0) ? prevMousePoint.rx() - 27 : 1,
            sceneheight - paddingBottom);
        textRight2->moveBy(sceneWidth + 2,
                           (point.ry() - 8 > 0) ? prevMousePoint.ry() - 8 : 1);
        lineItem = scene->addLine(prevMousePoint.rx(), prevMousePoint.ry(),
                                  mousePoint.rx(), mousePoint.ry(), crossLine);
        crossRect =
            scene->addRect(point.rx() + 1, point.ry() + 1, 80, 12,
                           QPen(QColor("#313233")), QBrush(QColor("#313233")));
        crossText = scene->addText(
            QString("bars: %1, pips: %2").arg(crossBar).arg(crossPips), font);
        crossText->setDefaultTextColor(Qt::white);
        crossText->moveBy(point.rx(), point.ry());
        scene->update();
      }
      if (drawLineInCrosses && crossLine3 != nullptr) {
        int crossBar =
            abs(point.rx() - prevMousePoint.rx()) / (barWidth + barSpace);
        int crossPips =
            abs(point.ry() - prevMousePoint.ry()) * pixelsize * 10000;
        scene->removeItem(lineItem);
        delete lineItem;
        lineItem = nullptr;
        scene->removeItem(crossRect);
        delete crossRect;
        crossRect = nullptr;
        scene->removeItem(crossText);
        delete crossText;
        crossText = nullptr;
        lineItem = scene->addLine(prevMousePoint.rx(), prevMousePoint.ry(),
                                  mousePoint.rx(), mousePoint.ry(), crossLine);
        crossRect =
            scene->addRect(point.rx() + 1, point.ry() + 1, 80, 12,
                           QPen(QColor("#313233")), QBrush(QColor("#313233")));
        crossText = scene->addText(
            QString("bars: %1, pips: %2").arg(crossBar).arg(crossPips), font);
        crossText->setDefaultTextColor(Qt::white);
        crossText->moveBy(point.rx(), point.ry());
      }
      mousePoint = point;
    }
  } else {
    if (drawLineInCrosses && crossLine3 != nullptr) {
      drawLineInCrosses = false;
      scene->removeItem(crossLine3);
      scene->removeItem(crossLine4);
      scene->removeItem(rectDown2);
      scene->removeItem(rectRight2);
      scene->removeItem(textDown2);
      scene->removeItem(textRight2);
      scene->removeItem(lineItem);
      scene->removeItem(crossRect);
      scene->removeItem(crossText);
      delete crossLine3;
      delete crossLine4;
      delete rectDown2;
      delete rectRight2;
      delete textDown2;
      delete textRight2;
      delete lineItem;
      delete crossRect;
      delete crossText;
      crossLine3 = nullptr;
      crossLine4 = nullptr;
      rectDown2 = nullptr;
      rectRight2 = nullptr;
      textDown2 = nullptr;
      textRight2 = nullptr;
      lineItem = nullptr;
      crossRect = nullptr;
      crossText = nullptr;
      scene->update();
    }
    if (rectDown != nullptr) {
      scene->removeItem(rectDown);
      scene->removeItem(rectRight);
      scene->removeItem(textDown);
      scene->removeItem(textRight);
      scene->removeItem(crossLine1);
      scene->removeItem(crossLine2);
      delete rectDown;
      delete rectRight;
      delete textDown;
      delete textRight;
      delete crossLine1;
      delete crossLine2;
      rectDown = nullptr;
      rectRight = nullptr;
      textDown = nullptr;
      textRight = nullptr;
      crossLine1 = nullptr;
      crossLine2 = nullptr;
      scene->update();
    }
  }
}

void MainWindow::setSlider(int slide) {
  ui->lineEditRated->setText(QString::number(slide / 10.0, 'f', 1));
}
