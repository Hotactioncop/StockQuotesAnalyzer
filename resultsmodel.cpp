#include "resultsmodel.h"

ResultsModel::ResultsModel(int row, int column, QObject *parent) : QAbstractTableModel(parent),rows(row),columns(column)
{
    checked =QPixmap(":/Images/checked.ico");
    unchecked =QPixmap(":/Images/unchecked.ico");
    checked.scaled(10,10,Qt::KeepAspectRatio);
    unchecked.scaled(10,10,Qt::KeepAspectRatio);

}
int ResultsModel::rowCount(const QModelIndex &parent) const
{
    return rows;
}

int ResultsModel::columnCount(const QModelIndex &parent) const
{
    return columns;
}

QVariant ResultsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    if(role==Qt::DecorationRole && (index.column()==2)){
        if(iList.at(index.row()).FixedSL>0){
            return checked;
        }else{
            return unchecked;
        }
    }
    if(role==Qt::DecorationRole && (index.column()==3)){
        if(iList.at(index.row()).currHigh){
            return checked;
        }else{
            return unchecked;
        }
    }
    if(role==Qt::DecorationRole && (index.column()==4)){
        if(iList.at(index.row()).highOfBoth){
            return checked;
        }else{
            return unchecked;
        }
    }
    if(role==Qt::DecorationRole && (index.column()==6)){
        if(iList.at(index.row()).FixedTP>0){
            return checked;
        }else{
            return unchecked;
        }
    }
    if(role==Qt::DecorationRole && (index.column()==7)){
        if(iList.at(index.row()).rate>0){
            return checked;
        }else{
            return unchecked;
        }
    }
    if((role==Qt::DisplayRole || role==Qt::EditRole)&&index.row()<iList.size()){
        switch(index.column()){
        case 0:
            return iList.at(index.row()).totalIncrease;
            break;
        case 1:
            return iList.at(index.row()).pointsEarned;
            break;
        case 2:
            if(iList.at(index.row()).FixedSL>0){
                return iList.at(index.row()).FixedSL;
            }
            else{
                return QVariant();
            }
            break;
        case 3:
            if(iList.at(index.row()).currHigh){
                return true;
            }
            else{
                return false;
            }
            break;
        case 4:
            if(iList.at(index.row()).highOfBoth){
                return true;
            }
            else{
                return false;
            }
            break;
        case 5:
            if(iList.at(index.row()).extraSL>0){
                return iList.at(index.row()).extraSL;
            }
            else{
                return QVariant();
            }
            break;
        case 6:
            if(iList.at(index.row()).FixedTP>0){
                return iList.at(index.row()).FixedTP;
            }
            else{
                return QVariant();
            }
            break;
        case 7:
            if(iList.at(index.row()).rate>0){
                return iList.at(index.row()).rate;
            }
            else{
                return QVariant();
            }
            break;
        case 8:
            return iList.at(index.row()).barBeforeTurn;
            break;
        case 9:
            return iList.at(index.row()).turnBarSize;
            break;
        }
    }
    return QVariant();
}

QVariant ResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole){
        return QVariant();
    }
    if(orientation == Qt::Horizontal){
        switch (section)
        {
        case 0:
            return QVariant(QObject::tr("Total\nincrease"));
            break;
        case 1:
            return QVariant(QObject::tr("Points\nearned"));
            break;
        case 2:
            return QVariant(QObject::tr("Fixed SL"));
            break;
        case 3:
            return QVariant(QObject::tr("SL to last\nbar high"));
            break;
        case 4:
            return QVariant(QObject::tr("SL to both\nbars high"));
            break;
        case 5:
            return QVariant(QObject::tr("Extra SL"));
            break;
        case 6:
            return QVariant(QObject::tr("Fixed TP"));
            break;
        case 7:
            return QVariant(QObject::tr("Rated TP"));
            break;
        case 8:
            return QVariant(QObject::tr("Bar before\nturn"));
            break;
        case 9:
            return QVariant(QObject::tr("Turn bar\nsize"));
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

bool ResultsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if(role==Qt::DisplayRole || role==Qt::EditRole){
        iList.push_back(AdvancedSearch());
        switch(index.column()){
        case 0:
            return iList[index.row()].totalIncrease=value.toInt();
            break;
        case 1:
            return iList[index.row()].pointsEarned=value.toDouble();
            break;
        case 2:
            return iList[index.row()].FixedSL=value.toInt();
            break;
        case 3:
            return iList[index.row()].currHigh = value.toBool();
            break;
        case 4:
            return iList[index.row()].highOfBoth=value.toBool();
            break;
        case 5:
            return iList[index.row()].extraSL=value.toInt();
            break;
        case 6:
            return iList[index.row()].FixedTP=value.toInt();
            break;
        case 7:
            return iList[index.row()].rate=value.toDouble();
            break;
        case 8:
            return iList[index.row()].barBeforeTurn=value.toInt();
            break;
        case 9:
            return iList[index.row()].turnBarSize=value.toInt();
            break;
        }
        emit dataChanged(index,index);
        qDebug() << "setData";
        return true;
    }
    return false;
}


bool ResultsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(),row,count);
    iList.push_back(AdvancedSearch());
    rows++;
    endInsertRows();
    return true;

}

void ResultsModel::clearModel()
{
    if(rows){
        qDebug() << "clearModel";
        iList.clear();
        qDebug() << "clearModel1";
        beginRemoveRows(QModelIndex(),0,rows-1);
        qDebug() << "clearModel2";
        rows=0;
        endRemoveRows();
        qDebug() << "clearModel3";
    }
}


bool ResultsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row<iList.size()){
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        iList.removeAt(row);
        endRemoveRows();
    }
    return true;
}
