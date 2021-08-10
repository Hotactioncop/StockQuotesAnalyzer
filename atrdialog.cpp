#include "atrdialog.h"
#include "ui_atrdialog.h"

ATRdialog::ATRdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ATRdialog)
{
    ui->setupUi(this);
}

ATRdialog::~ATRdialog()
{
    delete ui;
}
