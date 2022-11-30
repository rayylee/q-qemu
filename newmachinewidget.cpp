#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>

#include "ui_newmachinewidget.h"
#include "newmachinewidget.h"
#include "virtualmachine.h"

NewMachineWidget::NewMachineWidget(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::NewMachineWidget)
{
    ui->setupUi(this);

    connect(this, &NewMachineWidget::finished, this,
            &NewMachineWidget::_on_finished);
}

void NewMachineWidget::_on_finished(int result)
{
    if (!result)
        return;

    QString name = ui->vmNameLineEdit->text();
    QString os_type = ui->osTypeComboBox->currentText();
    QString disk1_path = ui->disk1Path->text();
    QString disk1_type = ui->disk1ComboBox->currentText();
    QString disk2_path = ui->disk2Path->text();
    QString disk2_type = ui->disk2ComboBox->currentText();
    QString cdrom_path = ui->cdromPath->text();

    QString home_local = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation);

    VirtualMachine vm = VirtualMachine();

    vm.set_domain_id(QString("%1").arg(m_domain_id));
    vm.set_qemu_bin_path(m_qemu_binary_path);
    vm.set_name(name);
    vm.set_os_type(os_type);
    vm.set_disk(0, disk1_path, disk1_type);
    vm.set_disk(1, disk2_path, disk2_type);
    vm.set_cdrom(0, cdrom_path);

    qDebug().noquote() << vm.to_xml_string();

    if (!m_app_dir.isEmpty() && vm.name().length() > 0) {
        QDir dir_app(m_app_dir);
        QString xml_path = QDir::toNativeSeparators(
                    dir_app.absoluteFilePath(vm.name() + ".xml"));
        QFile file(xml_path);

        if(file.open(QIODevice::WriteOnly)){
            file.write(vm.to_xml_string().toUtf8());

            emit send_new_machine(vm.name());
        }
        file.close();
    }
}

NewMachineWidget::~NewMachineWidget()
{
    delete ui;
}

void NewMachineWidget::on_choseDisk1Button_clicked()
{
   ui->disk1Path->setText(QFileDialog::getOpenFileName(this));
}

void NewMachineWidget::on_choseDisk2Button_clicked()
{
    ui->disk2Path->setText(QFileDialog::getOpenFileName(this));
}

void NewMachineWidget::on_choseCdromButton_clicked()
{
    ui->cdromPath->setText(QFileDialog::getOpenFileName(this));
}
