#ifndef CUSTOMPROXYMODEL_H
#define CUSTOMPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QDate>
#include <QDebug>
#include <QString>

class CustomProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CustomProxyModel(QObject *parent = nullptr);

signals:

public slots:

    // QSortFilterProxyModel interface
protected:
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // CUSTOMPROXYMODEL_H
