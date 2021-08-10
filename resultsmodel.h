#ifndef RESULTSMODEL_H
#define RESULTSMODEL_H

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

struct AdvancedSearch{
public:
    int FixedSL, FixedTP, barBeforeTurn, turnBarSize, extraSL;
    bool currHigh, highOfBoth;
    double rate;
    double pointsEarned;
    int totalIncrease;
};
class ResultsModel : public QAbstractTableModel
{
    Q_OBJECT
    int rows=0;
    int columns=0;
    QList<AdvancedSearch> iList;
    QPixmap checked;
    QPixmap unchecked;
public:
    explicit ResultsModel(int row=0, int column=10, QObject *parent = nullptr);

signals:

public slots:
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent) override;
    void clearModel();
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;
};

#endif // RESULTSMODEL_H
