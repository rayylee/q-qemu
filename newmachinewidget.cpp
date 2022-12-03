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

    ui->vmNameLineEdit->setValidator(
                new QRegExpValidator(QRegExp("^[A-Za-z]+[a-zA-Z0-9_-]+$")));

    connect(this, &NewMachineWidget::finished, this,
            &NewMachineWidget::_on_finished);
}

void NewMachineWidget::_on_finished(int result)
{
    if (!result)
        return;

    QString name = ui->vmNameLineEdit->text();
    QString accelerator = ui->acceleratorComboBox->currentText();
    QString memory = ui->memorySpinBox->text();
    QString cpu_sockets = ui->cpuSocketSpinBox->text();
    QString cpu_cores = ui->cpuCoreSpinBox->text();
    QString cpu_threads = ui->cpuThreadSpinBox->text();
    QString os_type = ui->osTypeComboBox->currentText();
    QString disk1_path = ui->disk1Path->text();
    QString disk1_type = ui->disk1ComboBox->currentText();
    QString disk2_path = ui->disk2Path->text();
    QString disk2_type = ui->disk2ComboBox->currentText();
    QString cdrom_path = ui->cdromPath->text();    

    QStringList orders;
    for (int i = 0; i < ui->bootOrderListWidget->count(); i++) {
        orders.append(ui->bootOrderListWidget->item(i)->text());
    }

    QString home_local = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation);

    auto vm = new VirtualMachine();

    vm->set_domain_id(QString("%1").arg(m_domain_id));
    vm->set_accelerator(accelerator);
    vm->set_qemu_bin_path(m_qemu_binary_path);
    vm->set_name(name);
    vm->set_current_memory(memory);
    vm->set_cpu_sockets(cpu_sockets);
    vm->set_cpu_cores(cpu_cores);
    vm->set_cpu_threads(cpu_threads);
    vm->set_os_type(os_type);
    vm->set_disk(0, disk1_path, disk1_type);
    vm->set_disk(1, disk2_path, disk2_type);
    vm->set_cdrom(0, cdrom_path);
    vm->set_boot_order(orders);

    qDebug().noquote() << vm->to_xml_string();

    if (!m_app_dir.isEmpty() && vm->name().length() > 0) {
        QDir dir_app(m_app_dir);
        QString xml_path = QDir::toNativeSeparators(
                    dir_app.absoluteFilePath(vm->name() + ".xml"));
        QFile file(xml_path);

        if(file.open(QIODevice::WriteOnly)){
            file.write(vm->to_xml_string().toUtf8());
            file.close();

            emit send_new_machine(vm->name());
        }
    }

    delete vm;
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

void NewMachineWidget::on_bootOrderUpButton_clicked()
{
    auto cur_item = ui->bootOrderListWidget->currentItem();
    if (cur_item) {
        int cur_row = ui->bootOrderListWidget->currentRow();
        QString cur_str = cur_item->text();
        if (cur_row > 0) {
            delete ui->bootOrderListWidget->takeItem(cur_row);
            ui->bootOrderListWidget->insertItem(cur_row - 1, cur_str);
        }
        ui->bootOrderListWidget->setCurrentRow(cur_row - 1);
    }
}

void NewMachineWidget::on_bootOrderDownButton_clicked()
{
    auto cur_item = ui->bootOrderListWidget->currentItem();
    if (cur_item) {
        int cur_row = ui->bootOrderListWidget->currentRow();
        QString cur_str = cur_item->text();
        int row_count = ui->bootOrderListWidget->count();
        if ((row_count - cur_row) > 0) {
            delete ui->bootOrderListWidget->takeItem(cur_row);
            ui->bootOrderListWidget->insertItem(cur_row + 1, cur_str);
        }
        ui->bootOrderListWidget->setCurrentRow(cur_row + 1);
    }
}
