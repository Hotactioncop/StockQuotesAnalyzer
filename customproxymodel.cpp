#include "customproxymodel.h"

CustomProxyModel::CustomProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}


bool CustomProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (source_left.column()==1 || source_left.column()==5) {
        QString left = source_left.data(Qt::DisplayRole).toString();
        QString right = source_right.data(Qt::DisplayRole).toString();
        QDate leftData = QDate::fromString(left,"yyyy MMM dd ");
        QDate rightData = QDate::fromString(right,"yyyy MMM dd ");
            return leftData< rightData;
        }else{
        return QSortFilterProxyModel::lessThan(source_left,source_right);
    }
}
