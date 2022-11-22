#include <QFileDialog>

#include "mainsetting.h"
#include "ui_mainsetting.h"

MainSetting::MainSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainSetting)
{    
    ui->setupUi(this);
}

MainSetting::~MainSetting()
{
    delete ui;
}

void MainSetting::set_qemu_dir(QString qemu_dir)
{
    ui->qemuDirLineEdit->setText(qemu_dir);
}

void MainSetting::on_choseQemuDirButton_clicked()
{
    ui->qemuDirLineEdit->setText(
                QFileDialog::getExistingDirectory(this));
}

void MainSetting::on_confirmButtonBox_accepted()
{
    QString qemu_dir = ui->qemuDirLineEdit->text();

    emit send_qemu_dir(qemu_dir);
}
