#ifndef ATRDIALOG_H
#define ATRDIALOG_H

#include <QDialog>

namespace Ui {
class ATRdialog;
}

class ATRdialog : public QDialog
{
    Q_OBJECT

public:
    explicit ATRdialog(QWidget *parent = nullptr);
    ~ATRdialog();

private:
    Ui::ATRdialog *ui;
};

#endif // ATRDIALOG_H
