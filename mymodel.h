#ifndef MYMODEL_H
#define MYMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QFont>
#include <QDebug>
#include <QApplication>
#include <QMap>
#include <QList>
#include <QColor>
#include <QBrush>
#include <QMimeData>
#include <QDateTime>
#include <QPixmap>

struct BuySell{
public:
    double open,stopLoss,takeProfit,close,profit;
    bool direction; //0-down, 1- up;
    int modelDeposit;
    QString modelLot;
    QDate timeOpen;
    QDate timeClose;
};
class MyModel : public QAbstractTableModel
{
    Q_OBJECT
    int rows=0;
    int columns=0;
protected:
    QList<BuySell> iList;
    QMap<QModelIndex,QVariant> celldata;

public:
    explicit MyModel(int row, int column, QObject *parent = nullptr);

signals:

public slots:
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    void clearModel();
};

#endif // MYMODEL_H
