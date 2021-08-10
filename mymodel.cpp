#include "mymodel.h"

MyModel::MyModel(int row, int column, QObject *parent) : QAbstractTableModel(parent),rows(row),columns(column)
{

}
int MyModel::rowCount(const QModelIndex &parent) const
{
    return rows;
}

int MyModel::columnCount(const QModelIndex &parent) const
{
    return columns;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
//    if (role == Qt::TextColorRole && index.column()==7&&index.row()<iList.size()) {
//        double stock = iList.at(index.row()).profit;
//            if(stock>0){
//                return QBrush(QColor("#04a863"));
//            }
//            else {
//                return QBrush(QColor("#ee1c25"));
//            }
//    }
    if(role == Qt::BackgroundColorRole){
            if(iList.at(index.row()).profit>0){
                return QColor("#98FB98");

            }
            else{
                return QColor("#FFC0CB");
            }
    }
    if(role==Qt::DecorationRole && index.column()==0){
        if(iList.at(index.row()).direction){
            return QPixmap(":/Images/Up.ico");
        }
        else{
            return QPixmap(":/Images/Down.ico");
        }
    }
    if((role==Qt::DisplayRole || role==Qt::EditRole)&&index.row()<iList.size()){
        switch(index.column()){
        case 0:
            return iList.at(index.row()).direction?"Buy":"Sell";
            break;
        case 1:
            return QDate(iList.at(index.row()).timeOpen).toString("yyyy MMM dd ");
            break;
        case 2:
            return iList.at(index.row()).open;
            break;
        case 3:
            return iList.at(index.row()).stopLoss;
            break;
        case 4:
            return iList.at(index.row()).takeProfit;
            break;
        case 5:
            return QDate(iList.at(index.row()).timeClose).toString("yyyy MMM dd ");
            break;
        case 6:
            return iList.at(index.row()).close;
            break;
        case 7:
            return iList.at(index.row()).profit;
            break;
        case 8:
            return iList.at(index.row()).modelLot;
            break;
        case 9:
            return iList.at(index.row()).modelDeposit;
            break;
        }
    }
    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole){
        return QVariant();
    }
    if(orientation == Qt::Horizontal){
        switch (section)
        {
        case 0:
            return QVariant(QObject::tr("Buy/Sell"));
            break;
        case 1:
            return QVariant(QObject::tr("Open Time"));
            break;
        case 2:
            return QVariant(QObject::tr("Open"));
            break;
        case 3:
            return QVariant(QObject::tr("Stop Loss"));
            break;
        case 4:
            return QVariant(QObject::tr("Take Profit"));
            break;
        case 5:
            return QVariant(QObject::tr("Close Time"));
            break;
        case 6:
            return QVariant(QObject::tr("Close"));
            break;
        case 7:
            return QVariant(QObject::tr("Profit"));
            break;
        case 8:
            return QVariant(QObject::tr("Lot size"));
            break;
        case 9:
            return QVariant(QObject::tr("Deposit"));
            break;
        default:
            return QVariant();
        }
    }
    if(orientation == Qt::Vertical){
        return section+1;
    }
    return QVariant();
}

bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if(role==Qt::DisplayRole || role==Qt::EditRole){
        iList.push_back(BuySell());
        switch(index.column()){
        case 0:{
            iList[index.row()].direction=value.toBool();
            break;}
        case 1:{
            iList[index.row()].timeOpen=value.toDate();
            break;}
        case 2:{
            iList[index.row()].open=value.toDouble();
            break;}
        case 3:{
            iList[index.row()].stopLoss=value.toDouble();
            break;}
        case 4:{
            iList[index.row()].takeProfit=value.toDouble();
            break;}
        case 5:{
            iList[index.row()].timeClose=value.toDate();
            break;}
        case 6:{
            iList[index.row()].close=value.toDouble();
            break;}
        case 7:{
            iList[index.row()].profit=value.toDouble();
            break;}
        case 8:{
            iList[index.row()].modelLot=value.toString();
            break;}
        case 9:{
            iList[index.row()].modelDeposit=value.toInt();
            break;}
        }
        emit dataChanged(index,index);
        return true;
    }
    return false;
}

void MyModel::clearModel()
{
    iList.clear();
    rows=0;
    columns=0;
}
