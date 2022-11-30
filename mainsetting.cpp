#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <utility>

#include "mainsetting.h"
#include "ui_mainsetting.h"
#include "config/global_setting.h"

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

void MainSetting::set_app_dir(QString app_dir)
{  
    m_app_dir = std::move(app_dir);

    GlobalSetting s = GlobalSetting(m_app_dir);
    m_qemu_dir = s.qemu_dir();
    m_qemu_binary_path = s.qemu_binary_path();

    ui->qemuDirLineEdit->setText(m_qemu_dir);
    ui->qemuBinLabel->setText(m_qemu_binary_path);
}

void MainSetting::on_choseQemuDirButton_clicked()
{
    QString qemu_dir = QFileDialog::getExistingDirectory(this);
    QString qemu_binary_path = "-";

    ui->qemuDirLineEdit->setText(
                QDir::toNativeSeparators(qemu_dir));

    QDirIterator it(qemu_dir, QStringList() << "qemu-system-*",
                    QDir::NoFilter, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        if (it.fileName().contains("x86_64w")) {
            QDir dir_path(qemu_dir);
            qemu_binary_path = QDir::toNativeSeparators(
                        dir_path.absoluteFilePath(it.fileName()));
        }
    }

    ui->qemuBinLabel->setText(qemu_binary_path);
}

void MainSetting::on_confirmButtonBox_accepted()
{
    QString qemu_dir = ui->qemuDirLineEdit->text();

    QString qemu_dir_native = QDir::toNativeSeparators(qemu_dir);

    if (qemu_dir_native.compare(m_qemu_dir) != 0) {
        m_qemu_dir = qemu_dir_native;
    }

    GlobalSetting s = GlobalSetting(m_app_dir);
    s.set_qemu_dir(m_qemu_dir);
    s.search_qemu_binary();
    s.save_config();

    emit send_qemu_dir(m_qemu_dir, m_qemu_binary_path);
}
